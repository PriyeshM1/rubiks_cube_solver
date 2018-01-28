#pragma once

#include <glm/glm.hpp>
#include <functional>
#include <vector>
using namespace std;
using namespace glm;

using Rotation = Orientation;
const int NUM_CUBES = 27;
const int NUM_FACES = 6;

namespace rubiks {

	struct Face;

	using Type = unsigned;

	//enum Type { CORNER, EDGE, CENTER };
	const unsigned CORNER = 0;
	const unsigned EDGE = 1;
	const unsigned CENTER = 2;
	const vec3 FRONT = { 0, 0, 1 };
	const vec3 BACK = { 0, 0, -1 };
	const vec3 RIGHT = { 1, 0, 0 };
	const vec3 LEFT = { -1, 0, 0 };
	const vec3 UP = { 0, 1, 0 };
	const vec3 DOWN = { 0, -1, 0 };

	const vec3 RED = { 1, 0, 0 };
	const vec3 GREEN = {0.3, 0.6, 0.3};
	const vec3 BLUE = { 0, 0, 1 };
	const vec3 WHITE = { 1, 1, 1 };
	const vec3 YELLOW = { 1, 1, 0};
	const vec3 ORANGE = {0.9, 0.4, 0.2};

	struct Cube {
		vec3 pos;
		vec3 fx, fy, fz;
		vec3 xc, yc, zc;
		Type type;

		void apply(const mat3 r) {
			pos = r * pos;
		}

		vec3 colorFor(const Face& face) const;

		vec3 directionOf(const vec3 color) const {
			if (color == xc) return fx;
			else if (color == yc) return fy;
			else return fz;	 // TODO fix this
		}
	};


	class RubiksCube {
	public:
		Cube cubes[NUM_CUBES];

		RubiksCube() {
			cubes[0] = { {-1, 1, 1}, { -1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, BLUE, YELLOW, RED, CORNER };
			cubes[1] = { {0, 1, 1}, vec3(0), { 0, 1, 0 }, { 0, 0, 1 }, { 0, 0, 0 }, YELLOW, RED, EDGE };
			cubes[2] = { { 1, 1, 1 },{ 1, 0, 0 },{ 0, 1, 0 },{ 0, 0, 1 },GREEN,YELLOW, RED, CORNER };

			cubes[3] = { { -1, 0, 1 }, vec3(0),{ -1, 0, 0 },{ 0, 0, 1 },vec3(0), BLUE, RED, EDGE };
			cubes[4] = { vec3(0, 0, 1), vec3(0), vec3(0), {0, 0, 1}, vec3(0), vec3(0), RED, CENTER };
			cubes[5] = { { 1, 0, 1 }, vec3(0),{ 1, 0, 0 },{ 0, 0, 1 },vec3(0),GREEN, RED, EDGE };

			cubes[7] = { { -1, -1, 1 },{ -1, 0, 0 },{ 0, -1, 0 },{ 0, 0, 1 },BLUE, WHITE, RED, CORNER };
			cubes[8] = { { 0, -1, 1 }, vec3(0),{ 0, -1, 0 },{ 0, 0, 1 },{ 0, 0, 0 }, WHITE, RED, EDGE };
			cubes[9] = { { 1, -1, 1 },{ 1, 0, 0 },{ 0, -1, 0 },{ 0, 0, 1 },GREEN, WHITE, RED, CORNER };

			cubes[10] = { { -1, 1, 0 }, vec3(0),{ -1, 0, 0 },{ 0, 1, 0 }, vec3(0),BLUE, YELLOW, EDGE };
			cubes[11] = { {0, 1, 0}, vec3(0), vec3(0) ,{ 0, 1, 0 }, vec3(0), vec3(0) , YELLOW, CENTER };
			cubes[12] = { { 1, 1, 0}, vec3(0), { 1, 0, 0 },{ 0, 1, 0 }, vec3(0), GREEN, YELLOW, EDGE };

			cubes[13] = { { -1, 0, 0 }, vec3(0), vec3(0), { -1, 0, 0 }, vec3(0), vec3(0), BLUE, CENTER };
			cubes[14] = { { 1, 0, 0 }, vec3(0), vec3(0), { 1, 0, 0 }, vec3(0), vec3(0), GREEN, CENTER };

			cubes[15] = { { -1, -1, 0 }, vec3(0),{ -1, 0, 0 },{ 0, -1, 0 }, vec3(0),BLUE, WHITE, EDGE };
			cubes[16] = { { 0, -1, 0 }, vec3(0), vec3(0) ,{ 0, -1, 0 }, vec3(0), vec3(0) , WHITE, CENTER };
			cubes[17] = { { 1, -1, 0 }, vec3(0),{ 1, 0, 0 },{ 0, -1, 0 }, vec3(0),GREEN, WHITE, EDGE };

			cubes[18] = { { -1, 1, -1 },{ -1, 0, 0 },{ 0, 1, 0 },{ 0, 0, -1 },BLUE, YELLOW, ORANGE, CORNER };
			cubes[19] = { { 0, 1, -1 }, vec3(0),{ 0, 1, 0 },{ 0, 0, -1 },{ 0, 0, 0 }, YELLOW, ORANGE, EDGE };
			cubes[20] = { { 1, 1, -1 },{ 1, 0, 0 },{ 0, 1, 0 },{ 0, 0, -1 },GREEN, YELLOW, ORANGE, CORNER };

			cubes[21] = { { -1, 0, -1 }, vec3(0),{ -1, 0, 0 },{ 0, 0, -1 },vec3(0),BLUE, ORANGE, EDGE };
			cubes[22] = { vec3(0, 0, -1), vec3(0), vec3(0),{ 0, 0, -1 }, vec3(0), vec3(0), ORANGE, CENTER };
			cubes[23] = { { 1, 0, -1 }, vec3(0),{ 1, 0, 0 },{ 0, 0, -1 },vec3(0), GREEN, ORANGE, EDGE };

			cubes[24] = { { -1, -1, -1 },{ -1, 0, 0 },{ 0, -1, 0 },{ 0, 0, -1 }, BLUE,  WHITE, ORANGE, CORNER };
			cubes[25] = { { 0, -1, -1 }, vec3(0),{ 0, -1, 0 },{ 0, 0, -1 },{ 0, 0, 0 }, WHITE, ORANGE, EDGE };
			cubes[26] = { { 1, -1, -1 },{ 1, 0, 0 },{ 0, -1, 0 },{ 0, 0, -1 }, GREEN , WHITE, ORANGE, CORNER };
		}

		RubiksCube(const RubiksCube& original) {
			for (int i = 0; i < NUM_CUBES; i++) {
				cubes[i] = original.cubes[i];
			}
		}

		vector<Cube*> find(function<bool(Cube&)> predicate) {
			vector<Cube*> res;
			for (int i = 0; i < NUM_CUBES; i++) {
				if (predicate(cubes[i])) {
					res.push_back(&cubes[i]);
				}
			}
			return res;
		}

		vector<Cube*> edgesAround(const Cube& cube);

	};

	struct Face {
		vec3 direction;

		bool isSolved() const { return false; };
		
		bool contains(const Cube& cube) const {
			if (cube.fx == direction) {
				return true;
			}
			else if (cube.fy == direction) {
				return true;
			}
			else if (cube.fz == direction) {
				return true;
			}
			return false;
		}

		vector<Cube*> get(RubiksCube& rCube) const {
			vector<Cube*> res;
			for (int i = 0; i < NUM_CUBES; i++) {
				Cube* cube = &rCube.cubes[i];
				if (contains(*cube)) {
					res.push_back(cube);
				}
			}
			return res;
		}

		bool isIn(const vec3& dir) const {
			return direction == dir;
		}
	};

	vec3 Cube::colorFor(const Face& face) const{
		if (fx == face.direction) {
			return xc;
		}
		else if (fy == face.direction) {
			return yc;
		}
		else if (fz == face.direction) {
			return zc;
		}
		return vec3(0);
	}

	const static Face RIGHT_FACE{ { 1, 0, 0 } };
	const static Face LEFT_FACE{ { -1, 0, 0 } };
	const static Face UP_FACE{ { 0, 1, 0 } };
	const static Face DOWN_FACE{ { 0, -1, 0 } };
	const static Face FRONT_FACE{ { 0, 0, 1 } };
	const static Face BACK_FACE{ { 0, 0, -1 } };

	const Face* faceFor(vec3 direction) {
		if (RIGHT_FACE.direction == direction) {
			return &RIGHT_FACE;
		}
		if (LEFT_FACE.direction == direction) {
			return &LEFT_FACE;
		}
		if (UP_FACE.direction == direction) {
			return &UP_FACE;
		}
		if (DOWN_FACE.direction == direction) {
			return &DOWN_FACE;
		}
		if (FRONT_FACE.direction == direction) {
			return &FRONT_FACE;
		}
		if (BACK_FACE.direction == direction) {
			return &BACK_FACE;
		}

		return nullptr;
	}

	function<bool(Cube*, Cube*)> compareBy(const vec3& color) {
		return [&](Cube* a, Cube* b) {
			if (a == b) return false;
			vec3 aDir = a->directionOf(WHITE);
			vec3 bDir = b->directionOf(WHITE);
			if (faceFor(aDir) == faceFor(bDir)) {
				if (faceFor(aDir) == &FRONT_FACE) {
					vec3 _aDir = a->fz == aDir ? a->fy : a->fz;
					vec3 _bDir = b->fz == bDir ? b->fy : b->fz;
					if (abs(_aDir) == abs(_bDir)) return false;
					else if (abs(_aDir) == vec3(1, 0, 0)) return true;
				}
			}
			else if (faceFor(aDir) == &FRONT_FACE) {
				return true;
			}
			else if (faceFor(aDir) == &RIGHT_FACE) {
				if (faceFor(bDir) == &FRONT_FACE) {
					return false;
				}
				else {
					return true;
				}
			}
			else if (faceFor(aDir) == &LEFT_FACE) {
				if (faceFor(bDir) == &FRONT_FACE || faceFor(bDir) == &RIGHT_FACE) {
					return false;
				}
				else {
					return true;
				}
			}
			else if (faceFor(aDir) == &UP_FACE) {
				if (faceFor(bDir) == &FRONT_FACE || faceFor(bDir) == &RIGHT_FACE || faceFor(bDir) == &DOWN_FACE) {
					return false;
				}
				else {
					return true;
				}
			}
			else {
				return false;
			}
		};
	}

	class CompareWhiteEdges {
		public:
		bool operator()(const Cube* a, const Cube* b) const {
			if (a == b) return false;
			vec3 aDir = a->directionOf(WHITE);
			vec3 bDir = b->directionOf(WHITE);
			if (faceFor(aDir) == faceFor(bDir)) {
				if (faceFor(aDir) == &FRONT_FACE) {
					vec3 _aDir = a->fz == aDir ? a->fy : a->fz;
					vec3 _bDir = b->fz == bDir ? b->fy : b->fz;
					if (abs(_aDir) == abs(_bDir)) return false;
					else if (abs(_aDir) == vec3(1, 0, 0)) return true;
				}
			}
			else if (faceFor(aDir) == &FRONT_FACE) {
				return true;
			}
			else if (faceFor(aDir) == &RIGHT_FACE) {
				if (faceFor(bDir) == &FRONT_FACE) {
					return false;
				}
				else {
					return true;
				}
			}
			else if (faceFor(aDir) == &LEFT_FACE) {
				if (faceFor(bDir) == &FRONT_FACE || faceFor(bDir) == &RIGHT_FACE) {
					return false;
				}
				else {
					return true;
				}
			}
			else {
				return false;
			}
		}
	};

	struct EdgeColor {
		Cube* cube;
		vec3 color;
	};
}