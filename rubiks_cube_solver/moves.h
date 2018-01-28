#pragma once
#define GLM_SWIZZLE 
#define GLM_SWIZZLE_XYZ
#include "model.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <ncl/gl/orientation.h>
#include <ncl/gl/util.h>
#include <iostream>
#include <algorithm>

ostream& operator<< (ostream& out, const vec3 v) {
	out << v.x << ' ' << v.y << ' ' << v.z << ' ';
	return out;
}

namespace rubiks {

	using namespace glm;

	struct Rotation {
		vec3 axis;
		float amout;
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

	class FaceMove : public Move{
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
		DoubleFaceMove(const Face& f,  const float ra, const string n) :FaceMove(f, ra, n) {}

		virtual void applyTo(RubiksCube& rCube) override {
			mat4 m = rotate(mat4(1), radians(rotation.amout), rotation.axis);
			mat3 nm = mat3(m);

			auto transform = [&](Cube& c) {
				c.pos = round(m * vec4(c.pos, 1.0)).xyz;
				c.fx = round(nm * c.fx);
				c.fy = round(nm * c.fy);
				c.fz = round(nm * c.fz);
			};

			vector<Cube*> cubes = rCube.find([&](Cube& c) { return face.contains(c); });
			vector<Cube*> neighbours(cubes.size());
			std::transform(cubes.begin(), cubes.end(), neighbours.begin(), [&](Cube* c) {
				const vec3 pos = c->pos - face.direction;
				return rCube.cubeAt(pos);
			});

			for (int i = 0; i < cubes.size(); i++) {
				transform(*cubes[i]);
				transform(*neighbours[i]);
			}
		}

		virtual bool affects(Cube& cube) const override {
			if (face.contains(cube)) {
				return true;
			}
			else {
				vec3 pos = cube.pos + face.direction;
				auto& neighbour = *cube.parent->cubeAt(pos);
				return face.contains(neighbour);
			}
		}

	};

	class Spin : public Move{
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

	static Spin SPIN_RIGHT = Spin({ { 0, 1, 0 }, -90.f }, "spin right");
	static Spin SPIN_LEFT = Spin({ { 0, 1, 0 }, 90.f }, "spin left");
	static Spin SPIN_UP = Spin({ { 1, 0, 0 }, -90.f }, "spin up");
	static Spin SPIN_DOWN = Spin({ { 1, 0, 0 }, 90.f }, "spin down");

	static FaceMove moves[6] = { F, R, B, L, U, D };
	static Move* allMoves[14] = { &F, &R, &B, &L, &U, &D, &_F, &_R, &_B, &_L, &_U, &_D, &SPIN_LEFT, &SPIN_RIGHT };

	Move* moveFor(vec3 direction) {
		for (int i = 0; i < 6; i++) {
			if (direction == moves[i].face.direction) {
				return &moves[i];
			}
		}
		return nullptr;
	}

	queue<Move*>  scramble(int amount) {
		queue<Move*> moves;
		ncl::Random rgn;
		for (int i = 0; i < amount; i++) {
			moves.push(allMoves[rgn._int(13)]);
		}
		return moves;
	}

	vector<Cube*> RubiksCube::edgesAround(const Cube& cube) {
		assert(cube.type == CENTER);
		const Face& f = *faceFor(cube.directionOf(cube.zc));
		return find([&](Cube& c) { return c.type == EDGE && (c.fy == f.direction || c.fz == f.direction); });
	}

}