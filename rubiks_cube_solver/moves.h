#pragma once
#define GLM_SWIZZLE 
#define GLM_SWIZZLE_XYZ

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <algorithm>
#include <queue>
#include "model.h"
#include "util.h"
#include <iterator>
#include <functional>

ostream& operator<< (ostream& out, const vec3 v) {
	out << v.x << ' ' << v.y << ' ' << v.z << ' ';
	return out;
}

namespace rubiks {
	using namespace std::placeholders;
	using namespace glm;

	struct Rotation {
		vec3 axis;
		float amout;

		operator mat4() const {
			return rotate(mat4(1), radians(amout), axis);
		}
	};

	class Rotatable {
	public:
		Rotatable(Rotation r) :rotation(r) {}

	public:
		const Rotation rotation;
	};

	class Move : public Rotatable {
	public:
		Move(const string n, Rotation r) :name(n), Rotatable(r) {}

		virtual void applyTo(RubiksCube& cube) = 0;
		virtual bool affects(Cube& cube) const = 0;
		virtual operator mat4() const = 0;

		const string name;
	};

	class FaceMove : public Move {
	public:
		FaceMove(const Face& f, const float ra, const string n) :face(f), Move(n, { f.direction, ra }) {}

		virtual void applyTo(RubiksCube& cube) override {
			applyTo(cube, rotation.axis, rotation.amout);
		}



		virtual void applyTo(RubiksCube& rCube, vec3 axis, float amount) {
			mat4 m = rotate(mat4(1), radians(amount), axis);
			mat3 nm = mat3(m);
			for (int i = 0; i < NUM_CUBES; i++) {
				auto& cube = rCube.cubes[i];
				if (face.contains(cube)) {
					cube.pos = round(m * vec4(cube.pos, 1.0)).xyz;
					cube.fx = round(nm * cube.fx);
					cube.fy = round(nm * cube.fy);
					cube.fz = round(nm * cube.fz);
				}
			}
		}

		virtual bool affects(Cube& cube) const override {
			return face.contains(cube);
		}

		virtual operator mat4() const {
			return rotate(mat4(1), radians(rotation.amout), rotation.axis);
		}

	public:
		const Face& face;
	};

	class DoubleFaceMove : public FaceMove {
	public:
		DoubleFaceMove(const Face& f, const float ra, const string n) :FaceMove(f, ra, n) {}

		virtual void applyTo(RubiksCube& rCube) override {
			mat4 m = rotate(mat4(1), radians(rotation.amout), rotation.axis);
			mat3 nm = mat3(m);

			auto transform = [&](Cube& c) {
				c.pos = round(m * vec4(c.pos, 1.0)).xyz;
				c.fx = round(nm * c.fx);
				c.fy = round(nm * c.fy);
				c.fz = round(nm * c.fz);
			};

			auto cubes = rCube.find([&](Cube& c) { return face.contains(c); });
			vector<reference_wrapper<Cube>> neighbours;
			for_each(cubes.begin(), cubes.end(), [&](Cube& c) {
				auto loc = c.pos - face.direction;
				if (dot(loc, loc) != 0) {
					Cube& nCube = rCube.cubeAt(loc);
					neighbours.push_back(nCube);
				}
			});

			for (int i = 0; i < cubes.size(); i++) {
				transform(cubes[i].get());
				if (i < cubes.size() - 1) {
					transform(neighbours[i].get());
				}
			}
		}

		virtual bool affects(Cube& cube) const override {
			if (face.contains(cube)) {
				return true;
			}
			else {
				vec3 pos = cube.pos + face.direction;
				if ((dot(pos, pos) == 0)) return false;
				auto& neighbour = cube.parent->cubeAt(pos);
				return face.contains(neighbour);
			}
		}

	};

	class Spin : public Move {
	public:
		Spin(const Rotation r, const string n) :Move(n, r) {}

		virtual void applyTo(RubiksCube& cube) override {
			applyTo(cube, rotation.axis, rotation.amout);
		}

		virtual void applyTo(RubiksCube& rCube, vec3 axis, float amount) {
			mat4 m = rotate(mat4(1), radians(amount), axis);
			mat3 nm = mat3(m);
			for (int i = 0; i < NUM_CUBES; i++) {
				auto& cube = rCube.cubes[i];
				cube.pos = round(m * vec4(cube.pos, 1.0)).xyz;
				cube.fx = round(nm * cube.fx);
				cube.fy = round(nm * cube.fy);
				cube.fz = round(nm * cube.fz);
			}
		}

		virtual bool affects(Cube& cube) const override {
			return true;
		}

		virtual operator mat4() const {
			return rotate(mat4(1), radians(rotation.amout), rotation.axis);
		}
	};

	static  FaceMove F = FaceMove(FRONT_FACE, -90.f, "F");
	static  FaceMove R = FaceMove(RIGHT_FACE, -90.f, "R");
	static  FaceMove B = FaceMove(BACK_FACE, -90.f, "B");
	static  FaceMove L = FaceMove(LEFT_FACE, -90.f, "L");
	static  FaceMove U = FaceMove(UP_FACE, -90.f, "U");
	static  FaceMove D = FaceMove(DOWN_FACE, -90.f, "D");

	static  FaceMove _F = FaceMove(FRONT_FACE, 90.f, "-F");
	static  FaceMove _R = FaceMove(RIGHT_FACE, 90.f, "-R");
	static  FaceMove _B = FaceMove(BACK_FACE, 90.f, "-B");
	static  FaceMove _L = FaceMove(LEFT_FACE, 90.f, "-L");
	static  FaceMove _U = FaceMove(UP_FACE, 90.f, "-U");
	static  FaceMove _D = FaceMove(DOWN_FACE, 90.f, "-D");

	static  DoubleFaceMove f = DoubleFaceMove(FRONT_FACE, -90.f, "f");
	static  DoubleFaceMove r = DoubleFaceMove(RIGHT_FACE, -90.f, "r");
	static  DoubleFaceMove b = DoubleFaceMove(BACK_FACE, -90.f, "b");
	static  DoubleFaceMove l = DoubleFaceMove(LEFT_FACE, -90.f, "l");
	static  DoubleFaceMove u = DoubleFaceMove(UP_FACE, -90.f, "u");
	static  DoubleFaceMove d = DoubleFaceMove(DOWN_FACE, -90.f, "d");

	static  DoubleFaceMove _f = DoubleFaceMove(FRONT_FACE, 90.f, "-f");
	static  DoubleFaceMove _r = DoubleFaceMove(RIGHT_FACE, 90.f, "-r");
	static  DoubleFaceMove _b = DoubleFaceMove(BACK_FACE, 90.f, "-b");
	static  DoubleFaceMove _l = DoubleFaceMove(LEFT_FACE, 90.f, "-l");
	static  DoubleFaceMove _u = DoubleFaceMove(UP_FACE, 90.f, "-u");
	static  DoubleFaceMove _d = DoubleFaceMove(DOWN_FACE, 90.f, "-d");

	static Spin SPIN_RIGHT = Spin({ { 0, 1, 0 }, -90.f }, "spin right");
	static Spin SPIN_LEFT = Spin({ { 0, 1, 0 }, 90.f }, "spin left");
	static Spin SPIN_UP = Spin({ { 1, 0, 0 }, -90.f }, "spin up");
	static Spin SPIN_DOWN = Spin({ { 1, 0, 0 }, 90.f }, "spin down");

	static vector<Move*> RArg{ &R, &U, &_R, &_U };
	static vector<Move*> LArg{ &_L, &_U, &L, &U };

	void apply(vector<Move*> moves, RubiksCube& cube, queue<Move*>& appliedMoves, int iterations) {
		for (int i = 0; i < iterations; i++) {
			for_each(moves.begin(), moves.end(), [&](Move* m) {
				m->applyTo(cube);
				appliedMoves.push(m);
			});
		}
	}

	void add(vector<Move*> moves, queue<Move*>& movesOut) {
		foreach(moves, [&](Move* m) { movesOut.push(m); });
	}

	auto R_U_RI_UI = bind(apply, RArg, _1, _2, _3);
	auto LI_UI_L_U = bind(apply, LArg, _1, _2, _3);


	static Move* allMoves[22] = {
		&F, &R, &B, &L, &U, &D, &_F, &_R, &_B, &_L, &_U, &_D, &SPIN_LEFT, &SPIN_RIGHT, &SPIN_UP, &SPIN_DOWN,
		&f, &r, &b, &l, &u, &d
	};


	Move* moveFor(vec3 direction) {
		for (int i = 0; i < 6; i++) {
			FaceMove* move = dynamic_cast<FaceMove*>(allMoves[i]);
			if (move != nullptr && direction == move->face.direction) {
				return move;
			}
		}
		return nullptr;
	}

	queue<Move*>  scramble(int amount) {
		queue<Move*> moves;
		for (int i = 0; i < amount; i++) {
			moves.push(allMoves[nextInt(22)]);
		}
		return moves;
	}

	void scramble(RubiksCube& cube) {
		auto moves = scramble(20);
		do {
			Move& m = *moves.front();
			moves.pop();
			m.applyTo(cube);
		} while (!moves.empty());
	}


	bool isSuperFlip(RubiksCube& cube) {
		bool allInPlaceNonStrict = all_of(begin(cube.cubes), end(cube.cubes), [&](Cube& c) {
			return cube.isInPlace(c, false);
		});
		if (allInPlaceNonStrict) {
			return all_of(begin(ALL_COLORS), end(ALL_COLORS), [&](vec3 color) {
				auto center = cube.center(color);
				auto edges = cube.edgesAround(center);
				return forall(edges, [&](Cube& c) { return center.zc != c.colorFor(*faceFor(center.fz)); });
			});
		}
		else {
			return false;
		}
	}

	queue<Move*> superFlip() {
		queue<Move*> moves;
		add({ &U, &R, &R, &F, &B, &R, &B, &B, &R, &U, &U, &L, &B, &B,
		&R, &_U, &_D, &R, &R, &F, &_R, &L, &B, &B, &U, &U, &F, &F}, moves);
		return moves;
	}

	bool cached(Move* move) {
		return any_of(begin(allMoves), end(allMoves), [&](Move* m) {
			return move == m;
		});
	}

	vector<reference_wrapper<Cube>> RubiksCube::edgesAround(const Cube& cube) {
		assert(cube.type == CENTER);
		const Face& f = *faceFor(cube.directionOf(cube.zc));
		return find([&](Cube& c) { return c.type == EDGE && (c.fy == f.direction || c.fz == f.direction); });
	}

	vector<reference_wrapper<Cube>> RubiksCube::cornersAround(const Cube& cube) {
		assert(cube.type == CENTER);
		const Face& f = *faceFor(cube.directionOf(cube.zc));
		return find([&](Cube& c) { return c.type == CORNER && (c.fx == f.direction || c.fy == f.direction || c.fz == f.direction); });
	}

}