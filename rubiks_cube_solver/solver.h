#pragma once

#include <stack>
#include <queue>
#include <functional>
#include <algorithm>
#include <glm/glm.hpp>
#include "model.h"
#include "moves.h"
#include "io.h"

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

		Step daisy = [&](RubiksCube& cube, queue<Move*>& moves) {

#ifdef DEBUG
			auto original = cube;
#endif
			
			auto daisyFormed = [&]() {
				auto& yellowCenter = *cube.find([&](Cube& c) { return c.type == CENTER && c.zc == YELLOW; })[0];
				auto edges = cube.edgesAround(yellowCenter);
				return all_of(edges.begin(), edges.end(), [&](Cube* c) { return c->colorFor(*faceFor(yellowCenter.fz)) == WHITE; });
			};

			if (daisyFormed()) return true;

			vector<Cube*> edges = cube.find([](Cube& c) {
				return (c.yc == WHITE || c.zc == WHITE) && c.type == EDGE && faceFor(c.directionOf(WHITE)) != &UP_FACE;
			});
		//	priority_queue<Cube*, vector<Cube*>, CompareWhiteEdges> heap;

		//	for_each(edges.begin(), edges.end(), [&](Cube* c) { heap.push(c); });
			auto edgesCopy = edges;
		//	sort(edges.begin(), edges.end(), compareBy(WHITE));

			// TODO check if daisy formed already and exit early

			for (int i = 0; i < edges.size(); i++){
 				Cube& edge = *edges[i];
				
				auto dir = [&]() { return edge.directionOf(WHITE); };
				auto altDir = [&]() { return edge.fz == dir() ? edge.fy : edge.fz; };	// direction of other edge color

				if (dir() == UP_FACE.direction) continue;

				int dp = int(abs(dot(dir(), UP_FACE.direction)));
				vec3 loc = vec3(0);
				switch (dp) {
				case BOTTOM:
					loc = vec3(1, -1, 1) * edge.pos;

					do {
						Cube& currentOccupant = *cube.find([&](Cube& c) { return c.type == EDGE && c.pos == loc; })[0];
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
							auto c = *cube.find([&](Cube& c) { return c.type == EDGE && c.pos == vec3{0, 1, 1}; })[0];
							return c.colorFor(UP_FACE) == WHITE;
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
						Cube& currentOccupant = *cube.find([&](Cube& c) { return c.type == EDGE && c.pos == loc; })[0];
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

		Step whiteCross = [](RubiksCube& cube, queue<Move*>& moves) {

			auto isWhite = [](Cube* c) { return c->yc == WHITE; };

			auto crossFormed = [&]() { return false; };

			auto axis = [&](Cube& edge) {
				if (edge.fy == DOWN_FACE.direction) return abs(edge.fy);
				else return abs(edge.fz);
			};

			return true;
		};
	};
}