#pragma once

#include <stack>
#include <queue>
#include <functional>
#include <algorithm>
#include <glm/glm.hpp>
#include "model.h"
#include "moves.h"
#include "io.h"
#include "util.h"

namespace rubiks {

	class Solver {
	public:
		virtual queue<Move*> solve(RubiksCube& cube) = 0;
	};

	class SimpleSolver : public Solver {
	public:
		SimpleSolver() {}

		virtual queue<Move*> solve(RubiksCube& cube) {
			auto copy = cube;
			queue<Move*> moves;
			steps.push(daisy);
			while (!steps.empty()) {
				auto step = steps.top();
				steps.pop();
				while (!step(copy, moves));
			}
			return moves;
		}

	private:
		using Step = function<bool(RubiksCube&, queue<Move*>&)>;
		stack<Step> steps;
		const static int BOTTOM = 1;
		const static int SIDES = 0;

		bool whiteCrossFormed(RubiksCube& cube) {
			const Cube& center = cube.center(WHITE);
			auto edges = cube.edgesAround(center);
			return all_of(edges.begin(), edges.end(), [&](Cube& c) { 
				vec3 color = c.zc == WHITE ? c.yc : c.zc;
				return c.colorFor(*faceFor(center.fz)) == WHITE && faceFor(c.directionOf(color))->center(cube).zc == color;
			});
		}

		Step daisy = [&](RubiksCube& cube, queue<Move*>& moves) {

#ifdef DEBUG
			auto original = cube;
#endif
			
			auto daisyFormed = [&]() {
				auto& yellowCenter = cube.center(YELLOW);
				auto edges = cube.edgesAround(yellowCenter);
				return all_of(edges.begin(), edges.end(), [&](Cube& c) { return c.colorFor(*faceFor(yellowCenter.fz)) == WHITE; });
			};


			// also check if white cross formed and exit early
			if (cube.isSolved()) return true;

			// TODO rotate yellow center face to the top
			if (!UP_FACE.contains(cube.center(YELLOW))) {
				auto& center = cube.center(YELLOW);
				const Face& face = *faceFor(center.directionOf(YELLOW));

				auto rot = [&]() {
					float d = dot(face.direction, UP_FACE.direction);
					vec3 a = abs(d) == 1 ? vec3(1, 0, 0) : cross(UP_FACE.direction, face.direction);
					return Rotation{ a, -degrees(acos(d)) };
				};

				Move* move = new Spin(rot(), "Spin any");
				move->applyTo(cube);
				moves.push(move);
			}


			if (whiteCrossFormed(cube)) {
				steps.push(whiteCorners);
				return true;
			}
			if (daisyFormed()) {
				steps.push(whiteCross);
				return true;
			}

			cout << "executing daisy" << endl;

			auto edges = cube.find([](Cube& c) {
				return (c.yc == WHITE || c.zc == WHITE) && c.type == EDGE && faceFor(c.directionOf(WHITE)) != &UP_FACE;
			});
		//	priority_queue<Cube*, vector<Cube&>, CompareWhiteEdges> heap;

		//	for_each(edges.begin(), edges.end(), [&](Cube* c) { heap.push(c); });
			auto edgesCopy = edges;
		//	sort(edges.begin(), edges.end(), compareBy(WHITE));



		//	return true;

			for (int i = 0; i < edges.size(); i++){
 				Cube& edge = edges[i];
				
				auto dir = [&]() { return edge.directionOf(WHITE); };
				auto altDir = [&]() { return edge.fz == dir() ? edge.fy : edge.fz; };	// direction of other edge color

				if (dir() == UP_FACE.direction) continue;

				int dp = int(abs(dot(dir(), UP_FACE.direction)));
				vec3 loc = vec3(0);
				switch (dp) {
				case BOTTOM:
					loc = vec3(1, -1, 1) * edge.pos;

					do {
						Cube& currentOccupant = cube.find([&](Cube& c) { return c.type == EDGE && c.pos == loc; })[0];
						if (currentOccupant.colorFor(UP_FACE) == WHITE) {
							U.applyTo(cube);	// create space by rotating the top face
							moves.push(&U);
							continue;
						}

						// rotate edge into place
						Move& move = *moveFor(altDir());
						move.applyTo(cube);	 // with every move check if cube is solved
						move.applyTo(cube);
						moves.push(&move);
						moves.push(&move);
						break;
					} while (true);
					break;
				case SIDES:
					const Face& f = *faceFor(dir());
					if (f.direction != FRONT_FACE.direction) {	// rotate edge face to front
						if (dir() == RIGHT_FACE.direction) {
							SPIN_RIGHT.applyTo(cube);
							moves.push(&SPIN_RIGHT);
						}
						else if (dir() == LEFT_FACE.direction) {	// edge is on left face
							SPIN_LEFT.applyTo(cube);
							moves.push(&SPIN_LEFT);
						}
						else if (BACK_FACE.isIn(dir())) {
							SPIN_RIGHT.applyTo(cube);
							SPIN_RIGHT.applyTo(cube);
							moves.push(&SPIN_RIGHT);
							moves.push(&SPIN_RIGHT);
						}
					}
					if (abs(altDir()) == UP_FACE.direction) {
						auto topFrontEdgeIsWhite = [&]() { 
							auto c = cube.find([&](Cube& c) { return c.type == EDGE && c.pos == vec3{0, 1, 1}; })[0];
							return c.get().colorFor(UP_FACE) == WHITE;
						};
						while (topFrontEdgeIsWhite()) {
							U.applyTo(cube);
							moves.push(&U);
						}
						F.applyTo(cube);
						moves.push(&F);
					}

					loc = round(static_cast<mat4>(SPIN_UP) * vec4(edge.pos, 1)).xyz;
					do {
						Cube& currentOccupant = cube.find([&](Cube& c) { return c.type == EDGE && c.pos == loc; })[0];
						if (currentOccupant.colorFor(UP_FACE) == WHITE) {
							U.applyTo(cube);	// create space by rotating the top face
							moves.push(&U);
							continue;
						}
						// rotate edge into place
						if (altDir() == RIGHT_FACE.direction) {
							R.applyTo(cube);
							moves.push(&R);
						}
						else {
							_L.applyTo(cube);
							moves.push(&_L);
						}
						break;
					} while (true);
					break;
				}

			}

			steps.push(whiteCross);

#ifdef DEBUG
			if (!daisyFormed()) {
				save(original);
				throw "daisy did not formed";
			}
#endif
			return true;
		};;

		Step whiteCross = [&](RubiksCube& cube, queue<Move*>& moves) {

#ifdef DEBUG
			auto original = cube;
#endif

			if (cube.isSolved()) return true;
			if (whiteCrossFormed(cube)) {
				steps.push(whiteCorners);
				return true;
			}

			cout << "executing white cross" << endl;

			auto edges = cube.edgesOf(WHITE);
			// TODO prioritise based on eges that are already in position
			for (int i = 0; i < edges.size(); i++) {
				Cube& edge = edges[i];
				auto dir = [&]() { return edge.directionOf(WHITE); };
				auto altDir = [&]() { return edge.fz == dir() ? edge.fy : edge.fz; };
				auto altColor = [&]() { return edge.colorFor(*faceFor(altDir())); };
				auto inPosition = [&](const Cube& edge) {};

				while (faceFor(altDir()) != &FRONT_FACE) {
					SPIN_RIGHT.applyTo(cube);
					moves.push(&SPIN_RIGHT);
				}

				// rotate until center matches alt color
				while (FRONT_FACE.center(cube).zc != altColor()) {
					auto& center = FRONT_FACE.center(cube);
					auto& _altColor = altColor();
					d.applyTo(cube);
					moves.push(&d);
				}

				F.applyTo(cube);
				F.applyTo(cube);
				moves.push(&F);
				moves.push(&F);
			}

#ifdef DEBUG
			if (!whiteCrossFormed(cube)) {
				save(original);
				throw "white cross did not formed";
			}
#endif

			steps.push(whiteCorners);

			return true;
		};

		Step whiteCorners = [&](RubiksCube& cube, queue<Move*>& moves) {
#ifdef DEBUG
			auto original = cube;
#endif
			
			bool nonStrict = false;
			

			auto topCornersFirst = [&](Cube& a, Cube& b) {
				if (&a == &b || a.pos.y == b.pos.y) return false;
				if (a.pos.y == 1) return true;
				else return false;
			};

			if (cube.isSolved()) return true;
			if (cube.layerIsSolved(LAYER_ONE)) {
				steps.push(layer2Edges);
				return true;
			}

			cout << "executing white corners" << endl;

			auto corners = cube.cornersOf(WHITE);
			sort(begin(corners), end(corners), topCornersFirst);
			

			// TODO order by corners point in the up direction

			for (int i = 0; i < corners.size(); i++) {
	//			if (i == 2) break;
				Cube& corner = corners[i];

				auto InRightCorner = [&]() { return corner.pos.x == 1 && corner.pos.z == 1; };

				
				if (cube.isInPlace(corner)) continue;

				while (!InRightCorner()) {
					SPIN_RIGHT.applyTo(cube);
					moves.push(&SPIN_RIGHT);
				}

				if (DOWN_FACE.contains(corner)) {	// flip corner to up face
					R_U_RI_UI(cube, moves, 1);
				}


				while (!cube.isInPlace(corner, nonStrict)) {
					d.applyTo(cube);
					moves.push(&d);
				} 

				
				while (!cube.isInPlace(corner)) {
					R_U_RI_UI(cube, moves, 1);

				} 

			}
#ifdef DEBUG
			if (!cube.layerIsSolved(LAYER_ONE)) {
				save(original);
				throw "layer one was not solved";
			}
#endif

			steps.push(layer2Edges);

			return true;
		};

		Step layer2Edges = [&](RubiksCube& cube, queue<Move*>& moves) {
#ifdef DEBUG
			auto original = cube;
#endif
			
			if (cube.isSolved()) return true;
			if (cube.layerIsSolved(LAYER_TWO)) {
				steps.push(yellowCross);
				return true;
			}

			cout << "executing layer 2 edges" << endl;

			auto topEdgessFirst = [&](Cube& a, Cube& b) {
				if (&a == &b || a.pos.y == b.pos.y) return false;
				if (a.pos.y == 1) return true;
				else return false;
			};

			vector<reference_wrapper<Cube>> edges = cube.find([&](Cube& c) { 
				return c.type == EDGE && (c.yc != WHITE && c.zc != WHITE) 
					&& (c.yc != YELLOW && c.zc != YELLOW)
					&& !cube.isInPlace(c) && (c.pos.y != -1);
			});
			sort(begin(edges), end(edges), topEdgessFirst);

			vec3 rightEdge{ 1, 0, 1 };
			vec3 leftEdge{ -1, 0, 1 };

			for (int i = 0; i < edges.size(); i++) {
				
				
				Cube& edge = edges[i];
				
				cout << "edge: " << i << ", pos" << edge.pos << endl;
				//if (i == 3) return true;
				while (!(FRONT_FACE.contains(edge))) {
					SPIN_RIGHT.applyTo(cube);
					moves.push(&SPIN_RIGHT);
				}

				if (edge.pos == leftEdge) {
					SPIN_LEFT.applyTo(cube);
					moves.push(&SPIN_LEFT);
				}

				bool nonStrict = false;
				if (edge.pos == rightEdge) {
					U.applyTo(cube);
					R.applyTo(cube);
					U.applyTo(cube);
					_R.applyTo(cube);
					_U.applyTo(cube);
					
					SPIN_RIGHT.applyTo(cube);

					_L.applyTo(cube);
					_U.applyTo(cube);
					L.applyTo(cube);
					U.applyTo(cube);

					moves.push(&U);
					moves.push(&R);
					moves.push(&U);
					moves.push(&_R);
					moves.push(&_U);
					moves.push(&SPIN_RIGHT);
					moves.push(&_L);
					moves.push(&_U);
					moves.push(&L);
					moves.push(&U);

				}

				while (edge.colorFor(FRONT_FACE) != FRONT_FACE.center(cube).zc) {
					d.applyTo(cube);
					moves.push(&d);
				}

				if(edge.colorFor(UP_FACE) == LEFT_FACE.center(cube).zc){
					_U.applyTo(cube);
					_L.applyTo(cube);
					_U.applyTo(cube);
					L.applyTo(cube);
					U.applyTo(cube);

					SPIN_LEFT.applyTo(cube);

					R.applyTo(cube);
					U.applyTo(cube);
					_R.applyTo(cube);
					_U.applyTo(cube);

					moves.push(&_U);
					moves.push(&_L);
					moves.push(&_U);
					moves.push(&L);
					moves.push(&U);
					moves.push(&SPIN_LEFT);
					moves.push(&R);
					moves.push(&U);
					moves.push(&_R);
					moves.push(&_U);
				}
				else if (edge.colorFor(UP_FACE) == RIGHT_FACE.center(cube).zc) {
					U.applyTo(cube);
					R.applyTo(cube);
					U.applyTo(cube);
					_R.applyTo(cube);
					_U.applyTo(cube);

					SPIN_RIGHT.applyTo(cube);

					_L.applyTo(cube);
					_U.applyTo(cube);
					L.applyTo(cube);
					U.applyTo(cube);

					moves.push(&U);
					moves.push(&R);
					moves.push(&U);
					moves.push(&_R);
					moves.push(&_U);
					moves.push(&SPIN_RIGHT);
					moves.push(&_L);
					moves.push(&_U);
					moves.push(&L);
					moves.push(&U);
				}
			}


#ifdef DEBUG
			if (!cube.layerIsSolved(LAYER_TWO)) {
				save(original);
				throw "second layer was not solved";
			}
#endif
			steps.push(yellowCross);

			return true;
		};

		Step yellowCross = [&](RubiksCube& cube, queue<Move*>& moves) {

#ifdef DEBUG
			auto original = cube;
			save(original);

#endif

			auto yellowCrossFormed = [&]() {
				const Cube& center = cube.center(YELLOW);
				auto edges = cube.edgesAround(center);
				return all_of(edges.begin(), edges.end(), [&](Cube& c) { return c.colorFor(*faceFor(center.fz)) == YELLOW; });
			};

			if (cube.isSolved()) return true;
			if (yellowCrossFormed()) {
				steps.push(yellowCorner);
				return true;
			}

			cout << "executing yellow cross step" << endl;

			auto linePattern = [&]() {
				return  (cube.cubeAt(vec3(-1, 1, 0)).colorFor(UP_FACE) == YELLOW && cube.cubeAt(vec3(1, 1, 0)).colorFor(UP_FACE) == YELLOW)
					|| (cube.cubeAt(vec3(0, 1, 1)).colorFor(UP_FACE) == YELLOW && cube.cubeAt(vec3(0, 1, -1)).colorFor(UP_FACE) == YELLOW);
			};

			auto anglePattern = [&]() {
				// rotate until right corner is found
				vec3 loc0{ 1, 1, 0 };
				vec3 loc1{ 0, 1, 1 };
				float amount = 0;

				for (int i = 0; i < 4; i++) {
					if (cube.cubeAt(loc0).colorFor(UP_FACE) == YELLOW && cube.cubeAt(loc1).colorFor(UP_FACE) == YELLOW) return true;
					amount += 90.f;
					mat4 m = rotate(mat4(1), radians(amount), { 0, 1, 0 });
					loc0 = round(m * vec4(loc0, 1.0)).xyz;
					loc1 = round(m * vec4(loc1, 1.0)).xyz;
		
				}
				return false;
			};

			auto linePatternMoves = [&]() {
				F.applyTo(cube);
				R.applyTo(cube);
				U.applyTo(cube);
				_R.applyTo(cube);
				_U.applyTo(cube);
				_F.applyTo(cube);

				moves.push(&F);
				moves.push(&R);
				moves.push(&U);
				moves.push(&_R);
				moves.push(&_U);
				moves.push(&_F);
			};

			auto anglePatternMoves = [&]() {
				while (!(cube.cubeAt(vec3(1, 1, 0)).colorFor(UP_FACE) == YELLOW && cube.cubeAt(vec3(0, 1, 1)).colorFor(UP_FACE) == YELLOW)) {
					SPIN_RIGHT.applyTo(cube);
					moves.push(&SPIN_RIGHT);
				}
				f.applyTo(cube);
				R.applyTo(cube);
				U.applyTo(cube);
				_R.applyTo(cube);
				_U.applyTo(cube);
				_f.applyTo(cube);

				moves.push(&f);
				moves.push(&R);
				moves.push(&U);
				moves.push(&_R);
				moves.push(&_U);
				moves.push(&_f);
			};

			Rotation r{ {0, 1, 0}, 0 };

			if (linePattern()) {
				if (cube.cubeAt(vec3(1, 1, 0)).colorFor(UP_FACE) != YELLOW || cube.cubeAt(vec3(-1, 1, 0)).colorFor(UP_FACE) != YELLOW) {
					SPIN_RIGHT.applyTo(cube);
					moves.push(&SPIN_RIGHT);
				}

				linePatternMoves();

			}
			else if (anglePattern()) {
				anglePatternMoves();
			}
			else {	// DOT pattern
				linePatternMoves();
				anglePatternMoves();
			}

#ifdef DEBUG
			if (!yellowCrossFormed()) {
				save(original);
				throw "yellow cross was not formed";
			}
#endif

			steps.push(yellowCorner);

			return true;
		};

		Step yellowCorner = [&](RubiksCube& cube, queue<Move*>& moves) {
#ifdef DEBUG
			auto original = cube;
			save(original);

#endif

			auto yellowCornersFormed = [&]() {
				const Cube& center = cube.center(YELLOW);
				auto corners = cube.cornersAround(center);
				return all_of(corners.begin(), corners.end(), [&](Cube& c) { 
					vector<vec3> colors = c.colors();
					vector<vec3> faces = c.faces();
					for (int i = 0; i < colors.size(); i++) {
						if (faceFor(faces[i])->center(cube).zc != colors[i]) return false;
					}

					return c.colorFor(*faceFor(center.fz)) == YELLOW;
				});
			};

			if (cube.isSolved()) return true;
			if (yellowCornersFormed()) {
				steps.push(solveLayer3);
				return true;
			}

			cout << "executiing yellow corner" << endl;

			auto findOutOfPlace = [&](vector<reference_wrapper<Cube>>& corners) {
				return filter(corners, [&](Cube& c) { return !cube.isInPlace(c, false); });
			};


			auto inPlace = [&](vector<reference_wrapper<Cube>>& corners, bool strict = false) {
				int count = count_if(corners.begin(), corners.end(), [&](Cube& c) { return cube.isInPlace(c, strict); });
				return count;
			};

			auto isAdjacentSwap = [&](vector<reference_wrapper<Cube>>& corners) {
				auto res = findOutOfPlace(corners);
				vector<vec3> faces0 = filter(res[0].get().faces(), [](vec3 v) { return faceFor(v) != &UP_FACE; });
				vector<vec3> faces1 = filter(res[1].get().faces(), [](vec3 v) { return faceFor(v) != &UP_FACE; });
				return any_of(faces0.begin(), faces0.end(), [&](vec3 dir0) {
					return any_of(faces1.begin(), faces1.end(), [&](vec3 dir1) {
						return dir0 == dir1;
					});
				});
			};

			auto swapAdjacentCorners = [&]() {
				for (int i = 0; i < 3; i++) {
					R.applyTo(cube);
					U.applyTo(cube);
					_R.applyTo(cube);
					_U.applyTo(cube);

					moves.push(&R);
					moves.push(&U);
					moves.push(&_R);
					moves.push(&_U);
				}

				SPIN_RIGHT.applyTo(cube);
				moves.push(&SPIN_RIGHT);

				for (int i = 0; i < 3; i++) {
					_L.applyTo(cube);
					_U.applyTo(cube);
					L.applyTo(cube);
					U.applyTo(cube);

					moves.push(&_L);
					moves.push(&_U);
					moves.push(&L);
					moves.push(&U);
				}
			};


			auto corners = cube.cornersOf(YELLOW);


			while (inPlace(corners) < 2) {
				U.applyTo(cube);
				moves.push(&U);
			}

			if (inPlace(corners) == 2) {
				if (isAdjacentSwap(corners)) {
					auto res = findOutOfPlace(corners);
					Cube& corner0 = res[0];
					Cube& corner1 = res[1];
					while (!(RIGHT_FACE.contains(corner0) && RIGHT_FACE.contains(corner1))) {
						SPIN_LEFT.applyTo(cube);
						moves.push(&SPIN_LEFT);
					} 

					swapAdjacentCorners();

					while (inPlace(corners) < 2) {
						U.applyTo(cube);
						moves.push(&U);
					}
				}
				else {	// diagonal corners
					swapAdjacentCorners();
					

					while (inPlace(corners) < 2) {
						U.applyTo(cube);
						moves.push(&U);
					}
					
					
					auto res = findOutOfPlace(corners);
					Cube& corner0 = res[0];
					Cube& corner1 = res[1];

					while (!(RIGHT_FACE.contains(corner0) && RIGHT_FACE.contains(corner1))) {
						SPIN_LEFT.applyTo(cube);
						moves.push(&SPIN_LEFT);
					}

					swapAdjacentCorners();

					while (inPlace(corners) < 4) {
						U.applyTo(cube);
						moves.push(&U);
					}
					
				}
			}

			
			SPIN_DOWN.applyTo(cube);
			SPIN_DOWN.applyTo(cube);
			moves.push(&SPIN_DOWN);
			moves.push(&SPIN_DOWN);

			vec3 bottomRight = vec3(1, -1, 1);

			auto outOfPlaceCorners = find_if(corners.begin(), corners.end(), [&](Cube& c) { return !cube.isInPlace(c); });

			for (; outOfPlaceCorners != corners.end(); outOfPlaceCorners++) {
				Cube& corner = *outOfPlaceCorners;
				cout << "corner: " << corner.pos << endl;
				while (corner.pos != bottomRight) {
					D.applyTo(cube);
					moves.push(&D);
				}

				while (faceFor(corner.directionOf(YELLOW)) != &DOWN_FACE) {
					R_U_RI_UI(cube, moves, 1);
				}
			}

			SPIN_UP.applyTo(cube);
			SPIN_UP.applyTo(cube);
			moves.push(&SPIN_UP);
			moves.push(&SPIN_UP);

			// Any top corner will do
			Cube& corner = cube.cubeAt({ 1, 1, 1 });	// front top right corner
			vec3 color = corner.colorFor(FRONT_FACE);
			auto face = [&]() { return *faceFor(corner.directionOf(color)); };
			while (face().center(cube).zc != color) {
				U.applyTo(cube);
				moves.push(&U);
			}
			

#ifdef DEBUG
			if (!yellowCornersFormed()) {
				save(original);
				throw "yellow cross was not formed";
			}
#endif
			steps.push(solveLayer3);
			// TODO next step
			return true;
		};
		
		Step solveLayer3 = [&](RubiksCube& cube, queue<Move*>& moves) {
#ifdef DEBUG
			auto original = cube;
			save(original);

#endif
			
			auto mayBeSolveCube = [&]() {
				R_U_RI_UI(cube, moves, 1);
				LI_UI_L_U(cube, moves, 1);

				R_U_RI_UI(cube, moves, 5);
				LI_UI_L_U(cube, moves, 5);
			};

			auto applyMovesForWhen1FaceSolved = [&]() {
				auto& face = *findin(sides, [&](const Face* f) { return f->isSolved(cube); });
				Cube& center = face.center(cube);
				while (center.fz != FRONT_FACE.direction) {	// todo implemented == for face
					SPIN_RIGHT.applyTo(cube);
					moves.push(&SPIN_RIGHT);
				}

				mayBeSolveCube();
				if (!cube.isSolved()) {
					mayBeSolveCube();
				}
			};

			auto anyFaceSolved = [&]() {
				return exists(sides, [&](const Face* f) { return f->isSolved(cube); });
			};

			cout << "executing layer 3 solution" << endl;
			
			while (!cube.isSolved()) {
				if (anyFaceSolved()) {
					applyMovesForWhen1FaceSolved();
				}
				else {	// No face solved
					mayBeSolveCube();
					applyMovesForWhen1FaceSolved();
				}
			}

#ifdef DEBUG
			if (!cube.isSolved()) {
				save(original);
				throw "unable to solve cube";
			}
#endif

			return true;
		};
	};
}