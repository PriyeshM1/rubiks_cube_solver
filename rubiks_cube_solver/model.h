#pragma once

#include <glm/glm.hpp>
#include <functional>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <initializer_list>

using namespace std;
using namespace glm;

const int NUM_CUBES = 26;
const int NUM_FACES = 6;

namespace rubiks {

	struct Face;
	class RubiksCube;

	enum Type { CORNER, EDGE, CENTER };

	const int LAYER_ONE = -1;
	const int LAYER_TWO = 0;
	const int LAYER_THREE = 1;

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
		RubiksCube* parent;

		void apply(const mat3 r) {
			pos = r * pos;
		}

		vec3 colorFor(const Face& face) const;

		vec3 directionOf(const vec3 color) const {
			if (color == xc) return fx;
			else if (color == yc) return fy;
			else return fz;	 // TODO fix this
		}

		vector<vec3> colors() {	
			switch (type)
			{
			case CENTER:
				return vector<vec3>{zc};
			case EDGE:
				return vector<vec3>{yc, zc};
			case CORNER:
				return vector<vec3>{xc, yc, zc};
			default:
				return vector<vec3>{};
			}
		}

		vector<vec3> faces() {
			switch (type)
			{
			case CENTER:
				return vector<vec3>{fz};
			case EDGE:
				return vector<vec3>{fy, fz};
			case CORNER:
				return vector<vec3>{fx, fy, fz};
			default:
				vector<vec3>{};
			}
		}
	};


	class RubiksCube {
	public:
		Cube cubes[NUM_CUBES];

		RubiksCube() {
			reset();
		}

		RubiksCube(const RubiksCube& original) {
			for (int i = 0; i < NUM_CUBES; i++) {
				cubes[i] = original.cubes[i];
				cubes[i].parent = this;
			}
		}

		void reset() {
			cubes[0] = { { -1, 1, 1 },{ -1, 0, 0 },{ 0, 1, 0 },{ 0, 0, 1 }, BLUE, YELLOW, RED, CORNER };
			cubes[1] = { { 0, 1, 1 }, vec3(0),{ 0, 1, 0 },{ 0, 0, 1 },{ 0, 0, 0 }, YELLOW, RED, EDGE };
			cubes[2] = { { 1, 1, 1 },{ 1, 0, 0 },{ 0, 1, 0 },{ 0, 0, 1 },GREEN,YELLOW, RED, CORNER };

			cubes[3] = { { -1, 0, 1 }, vec3(0),{ -1, 0, 0 },{ 0, 0, 1 },vec3(0), BLUE, RED, EDGE };
			cubes[4] = { vec3(0, 0, 1), vec3(0), vec3(0),{ 0, 0, 1 }, vec3(0), vec3(0), RED, CENTER };
			cubes[5] = { { 1, 0, 1 }, vec3(0),{ 1, 0, 0 },{ 0, 0, 1 },vec3(0),GREEN, RED, EDGE };

			cubes[6] = { { -1, -1, 1 },{ -1, 0, 0 },{ 0, -1, 0 },{ 0, 0, 1 },BLUE, WHITE, RED, CORNER };
			cubes[7] = { { 0, -1, 1 }, vec3(0),{ 0, -1, 0 },{ 0, 0, 1 },{ 0, 0, 0 }, WHITE, RED, EDGE };
			cubes[8] = { { 1, -1, 1 },{ 1, 0, 0 },{ 0, -1, 0 },{ 0, 0, 1 },GREEN, WHITE, RED, CORNER };

			cubes[9] = { { -1, 1, 0 }, vec3(0),{ -1, 0, 0 },{ 0, 1, 0 }, vec3(0),BLUE, YELLOW, EDGE };
			cubes[10] = { { 0, 1, 0 }, vec3(0), vec3(0) ,{ 0, 1, 0 }, vec3(0), vec3(0) , YELLOW, CENTER };
			cubes[11] = { { 1, 1, 0 }, vec3(0),{ 1, 0, 0 },{ 0, 1, 0 }, vec3(0), GREEN, YELLOW, EDGE };

			cubes[12] = { { -1, 0, 0 }, vec3(0), vec3(0),{ -1, 0, 0 }, vec3(0), vec3(0), BLUE, CENTER };
			cubes[13] = { { 1, 0, 0 }, vec3(0), vec3(0),{ 1, 0, 0 }, vec3(0), vec3(0), GREEN, CENTER };

			cubes[14] = { { -1, -1, 0 }, vec3(0),{ -1, 0, 0 },{ 0, -1, 0 }, vec3(0),BLUE, WHITE, EDGE };
			cubes[15] = { { 0, -1, 0 }, vec3(0), vec3(0) ,{ 0, -1, 0 }, vec3(0), vec3(0) , WHITE, CENTER };
			cubes[16] = { { 1, -1, 0 }, vec3(0),{ 1, 0, 0 },{ 0, -1, 0 }, vec3(0),GREEN, WHITE, EDGE };

			cubes[17] = { { -1, 1, -1 },{ -1, 0, 0 },{ 0, 1, 0 },{ 0, 0, -1 },BLUE, YELLOW, ORANGE, CORNER };
			cubes[18] = { { 0, 1, -1 }, vec3(0),{ 0, 1, 0 },{ 0, 0, -1 },{ 0, 0, 0 }, YELLOW, ORANGE, EDGE };
			cubes[19] = { { 1, 1, -1 },{ 1, 0, 0 },{ 0, 1, 0 },{ 0, 0, -1 },GREEN, YELLOW, ORANGE, CORNER };

			cubes[20] = { { -1, 0, -1 }, vec3(0),{ -1, 0, 0 },{ 0, 0, -1 },vec3(0),BLUE, ORANGE, EDGE };
			cubes[21] = { vec3(0, 0, -1), vec3(0), vec3(0),{ 0, 0, -1 }, vec3(0), vec3(0), ORANGE, CENTER };
			cubes[22] = { { 1, 0, -1 }, vec3(0),{ 1, 0, 0 },{ 0, 0, -1 },vec3(0), GREEN, ORANGE, EDGE };

			cubes[23] = { { -1, -1, -1 },{ -1, 0, 0 },{ 0, -1, 0 },{ 0, 0, -1 }, BLUE,  WHITE, ORANGE, CORNER };
			cubes[24] = { { 0, -1, -1 }, vec3(0),{ 0, -1, 0 },{ 0, 0, -1 },{ 0, 0, 0 }, WHITE, ORANGE, EDGE };
			cubes[25] = { { 1, -1, -1 },{ 1, 0, 0 },{ 0, -1, 0 },{ 0, 0, -1 }, GREEN , WHITE, ORANGE, CORNER };
			for (Cube& c : cubes) c.parent = this;
		}

		vector<reference_wrapper<Cube>> find(function<bool(Cube&)> predicate) {
			vector<reference_wrapper<Cube>> res;
			for (Cube& c : cubes) {
				if (predicate(c)) {
					res.push_back(c);
				}
			}
			return res;
		}

		vector<reference_wrapper<Cube>> edgesAround(const Cube& center);

		vector<reference_wrapper<Cube>> cornersAround(const Cube& center);

		vector<reference_wrapper<Cube>> edgesOf(const vec3 color) {
			return find([&](Cube& c) { return c.type == EDGE && (c.yc == color || c.zc == color); });
		}

		vector<reference_wrapper<Cube>> cornersOf(const vec3 color) {
			return find([&](Cube& c) { return c.type == CORNER && (c.xc == color || c.yc == color || c.zc == color); });
		}

		const Cube& center(const vec3 color) {
			return find([&](Cube& c) { return c.type == CENTER && c.zc == color; }).front();
		}

		 Cube& cubeAt(const vec3 pos) {
			vector<reference_wrapper<Cube>> res = find([&](Cube& c) { return c.pos == pos; });
			if (res.empty()) throw "No Cube found at pos: [" + to_string(pos.x) + ", " + to_string(pos.y) + ", " + to_string(pos.z) + "]";
			return res.front();
		}

		 vector<reference_wrapper<Cube>> getLayer(int id) {
			 return find([&](Cube c) { return c.pos.y == id; });
		 }

		 vector<reference_wrapper<Cube>> findBy(const initializer_list<vec3> colors) {
			 assert(colors.size() <= 3);
			 return find([&](Cube& c) {
				 vector<vec3> cColors = c.colors();
				 int size = cColors.size();
				 if (colors.size() != cColors.size()) return false;
				 return all_of(colors.begin(), colors.end(), [&](vec3 color) {
					 return any_of(cColors.begin(), cColors.end(), [&](vec3 oColor) {return color == oColor; });
				 });
			 });
		 }

		 bool isSolved();

		 bool layerIsSolved(int id);

		 bool isInPlace(Cube& c, bool strict = true);
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

		vector<reference_wrapper<Cube>> get(RubiksCube& rCube) const {
			vector<reference_wrapper<Cube>> res;
			for (int i = 0; i < NUM_CUBES; i++) {
				Cube& cube = rCube.cubes[i];
				if (contains(cube)) {
					res.push_back(cube);
				}
			}
			return res;
		}

		vector<reference_wrapper<Cube>> get(vector<reference_wrapper<Cube>> cubes) const {
			vector<reference_wrapper<Cube>> res;
			for (int i = 0; i < cubes.size(); i++) {
				Cube& cube = cubes[i];
				if (contains(cube)) {
					res.push_back(cube);
				}
			}
			return res;
		}

		bool isIn(const vec3& dir) const {
			return direction == dir;
		}

		bool layerIs(const vec3& color, RubiksCube& cube, int id) const {
			vector<reference_wrapper<Cube>> layer = cube.getLayer(id);
			for (int i = 0; i < layer.size(); i++) {
				if (layer[i].get().colorFor(*this) != color) return false;
			}
			return true;
		}

		bool is(const vec3& color, RubiksCube& rCube) const {
			for (auto& cube : rCube.cubes) {
				if (contains(cube) && cube.colorFor(*this) != color) return false;
			}
			return true;
		}

		Cube& center(RubiksCube& cube) const {
			return cube.find([&](Cube& c) { return c.type == CENTER && isIn(c.fz); }).front().get();
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

	const Face RIGHT_FACE{ { 1, 0, 0 } };
	const Face LEFT_FACE{ { -1, 0, 0 } };
	const Face UP_FACE{ { 0, 1, 0 } };
	const Face DOWN_FACE{ { 0, -1, 0 } };
	const Face FRONT_FACE{ { 0, 0, 1 } };
	const Face BACK_FACE{ { 0, 0, -1 } };


	const Face faces[6] = { UP_FACE, RIGHT_FACE, LEFT_FACE, FRONT_FACE, BACK_FACE, DOWN_FACE };
	const Face sides[4] = { RIGHT_FACE, LEFT_FACE, FRONT_FACE, BACK_FACE };

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

	bool noFilter(const Face* f) { 
		return true;  
	};

	vector<const Face*> facesFor(Cube& cube, function<bool(const Face*)> filter = noFilter) {
		vector<vec3> fDirs = cube.faces();
		vector<const Face*> faces;
		for (auto dir : fDirs) {
			auto f = faceFor(dir);
			if (filter(f)) {
				faces.push_back(f);
			}
		}
		return faces;
	}

	bool RubiksCube::isSolved() {
		return all_of(begin(faces)+1, end(faces)-1, [&](const Face& face) {
			vector<reference_wrapper<Cube>> cs = face.get(*this);
			vec3 color = cs.front().get().colorFor(face);
			return all_of(cs.begin() + 1, cs.end(), [&](Cube& c) { return c.colorFor(face) == color; });
		});
	}

	bool RubiksCube::layerIsSolved(int id) {
		assert(id >= LAYER_ONE && id <= LAYER_THREE);

		vector<reference_wrapper<Cube>> layer = getLayer(id);
		bool sidesSolved = all_of(begin(sides), end(sides), [&](const Face& face) {
			vector<reference_wrapper<Cube>> cs = face.get(layer);
			vec3 color = cs.front().get().colorFor(face);
			return all_of(cs.begin() + 1, cs.end(), [&](Cube& c) { return c.colorFor(face) == color; });
		});

		if (id == LAYER_TWO) return sidesSolved;

		const Face face = id == LAYER_ONE ? DOWN_FACE : UP_FACE;
		vec3 color = face.get(layer).front().get().colorFor(face);
		return face.is(color, *this);
	}

	bool RubiksCube::isInPlace(Cube& cube, bool strict) {
		if (cube.type == CENTER) return true;
		if (strict) {
			auto face = faceFor(cube.fx);
			if (face && face->center(*this).zc != cube.xc) return false;
			face = faceFor(cube.fy);
			if (face && face->center(*this).zc != cube.yc) return false;
			face = faceFor(cube.fz);
			if (face && face->center(*this).zc != cube.zc) return false;
		}
		else {
			auto filter = [&](const Face* f) { return f != nullptr && f != &DOWN_FACE && f != &UP_FACE; };
			vector<const Face*> faces = facesFor(cube, filter);

			vector<vec3> colors = cube.colors();
			auto no_white = remove_if(begin(colors), end(colors), [](vec3 c) { return c == WHITE || c == YELLOW; });
			vector<vec3>  centers(faces.size());
			std::transform(faces.begin(), faces.end(), centers.begin(), [&](const Face* face) {
				return face->center(*this).zc;
			});
			return all_of(colors.begin(), no_white, [&](vec3 color) {
				return any_of(centers.begin(), centers.end(), [&](vec3 center) {
					return center == color;
				});
			});
		}
		return true;
	}

	function<bool(Cube&, Cube&)> compareBy(const vec3& color) {
		return [&](Cube& a, Cube& b) {
			if (&a == &b) return false;
			vec3 aDir = a.directionOf(WHITE);
			vec3 bDir = b.directionOf(WHITE);
			if (faceFor(aDir) == faceFor(bDir)) {
				if (faceFor(aDir) == &FRONT_FACE) {
					vec3 _aDir = a.fz == aDir ? a.fy : a.fz;
					vec3 _bDir = b.fz == bDir ? b.fy : b.fz;
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
		bool operator()(const Cube& a, const Cube& b) const {
			if (&a == &b) return false;
			vec3 aDir = a.directionOf(WHITE);
			vec3 bDir = b.directionOf(WHITE);
			if (faceFor(aDir) == faceFor(bDir)) {
				if (faceFor(aDir) == &FRONT_FACE) {
					vec3 _aDir = a.fz == aDir ? a.fy : a.fz;
					vec3 _bDir = b.fz == bDir ? b.fy : b.fz;
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
}