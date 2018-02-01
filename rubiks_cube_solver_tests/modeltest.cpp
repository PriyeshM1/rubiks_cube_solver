#include "stdafx.h"

#define GLM_SWIZZLE 
#define DEBUG

#include <algorithm>
#include <iterator>
#include <future>
#include <chrono>
#include "CppUnitTest.h"
#include "../rubiks_cube_solver/model.h"
#include "../rubiks_cube_solver/moves.h"
#include "../rubiks_cube_solver/solver.h"
#include "../rubiks_cube_solver/util.h"
#include "../rubiks_cube_solver/io.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace rubiks;

namespace rubiks_cube_solver_tests
{		
	TEST_CLASS(ModelUnitTest)
	{
	public:
		
		TEST_METHOD(CubeInitialStateIsSolved)
		{

			RubiksCube cube;
			Assert::IsTrue(cube.isSolved(), L"initial state of cube should be solved", LINE_INFO());
			Assert::IsTrue(cube.layerIsSolved(LAYER_ONE), L"Layer one should be in solved state", LINE_INFO());
			Assert::IsTrue(cube.layerIsSolved(LAYER_TWO), L"Layer two should be in solved state", LINE_INFO());
			Assert::IsTrue(cube.layerIsSolved(LAYER_THREE), L"Layer three should be in solved state", LINE_INFO());
		}
		
		TEST_METHOD(CubeReturnedToSolvedStateAfterReset) {
			RubiksCube cube;
			R.applyTo(cube);
			Assert::IsFalse(cube.isSolved(), L"Cube is not be in solved state after a move");
			cube.reset();
			Assert::IsTrue(cube.isSolved(), L"Cube should be in solved state after reset");
		}

		TEST_METHOD(AllCubesShouldBeInPlaceOnASolvedCube) {
			RubiksCube cube;
			for_each(begin(cube.cubes), end(cube.cubes), [&](Cube& c) {
				Assert::IsTrue(cube.isInPlace(c), L"Cubes on a solved RibiksCube should be in place");
			});
		}

		TEST_METHOD(CornersWithColorSwappedShouldBeInPlaceInNonStrictQuery) {
			RubiksCube cube;
			FaceMove moves[8] = { R, U, _R, _U, R, U, _R, _U };
			for_each(begin(moves), end(moves), [&](Move& m) { return m.applyTo(cube); });
			Assert::IsFalse(cube.isSolved(), L"Cube is not be in solved state after a move");

			Cube& c = cube.cubeAt(vec3(1, -1, 1));
			Assert::IsFalse(cube.isInPlace(c), L"Swapped corner should not be in place in strict check");
			Assert::IsTrue(cube.isInPlace(c, false), L"swapped corner should be in place in non strict check");

		}

		TEST_METHOD(EdgesWithColorSwappedShouldBeInPlaceInNonStrictQuery) {
			RubiksCube cube;
			Move* m[22] = {
				&R, &U, &_R, &_U, &SPIN_RIGHT, &_L, &_U, &L, &U,
				&U, &SPIN_LEFT, &U, &R, &U, &_R, &_U, &SPIN_RIGHT,
				&_L, &_U, &L, &U, &SPIN_LEFT
			};

			for (int i = 0; i < 22; i++) m[i]->applyTo(cube);
			Assert::IsFalse(cube.isSolved(), L"Cube is not be in solved state after a move");

			Cube& c = cube.cubeAt(vec3(1, 0, 1));
			Assert::IsFalse(cube.isInPlace(c), L"Swapped edge should not be in place in strict check");
			Assert::IsTrue(cube.isInPlace(c, false), L"swapped edge should be in place in non strict check");
		}

		TEST_METHOD(FindCubeByFaceColors) {
			RubiksCube cube;
			Cube* c = &cube.findBy({ RED }).front().get();

			Assert::AreEqual(int(c->type), int(CENTER), L"cube type should have been center");
			Assert::IsTrue(c->zc == RED, L"cube color should have been red");

			c = &cube.findBy({ RED, GREEN, WHITE }).front().get();
			Assert::AreEqual(int(c->type), int(CORNER), L"cube type should have been corner");
			Assert::IsTrue(c->zc == RED, L"cube color should have been red");
			Assert::IsTrue(c->xc == GREEN, L"cube color should have been green");
			Assert::IsTrue(c->yc ==  WHITE, L"cube color should have been white");
		}

		TEST_METHOD(FindCubeByFaceColorsWhenCubeIsScrambled) {
			RubiksCube cube;
			scramble(cube);
			Assert::IsFalse(cube.isSolved(), L"Cube should not be solved after scrambling");

			Cube* c = &cube.findBy({ RED }).front().get();

			Assert::AreEqual(int(c->type), int(CENTER), L"cube type should have been center");
			Assert::IsTrue(c->zc == RED, L"cube color should have been red");

			c = &cube.findBy({ RED, GREEN, WHITE }).front().get();
			Assert::AreEqual(int(c->type), int(CORNER), L"cube type should have been corner");
			Assert::IsTrue(c->zc == RED, L"cube color should have been red");
			Assert::IsTrue(c->xc == GREEN, L"cube color should have been green");
			Assert::IsTrue(c->yc == WHITE, L"cube color should have been white");
		}

		TEST_METHOD(FindCornersAroundCenter) {
			RubiksCube cube;
			const Cube& center = cube.center(BLUE);
			auto corners = cube.cornersAround(center);

			Assert::IsFalse(corners.empty(), L"corners should exist around center");
			Assert::IsTrue(all_of(corners.begin(), corners.end(), [](Cube& c) { return c.xc == BLUE; }), L"corners around blue center should be blue");
		}

		TEST_METHOD(ShouldBeAbleToSolveAnyScrambledCube) {
			RubiksCube cube;
			RubiksCube copy;
			SimpleSolver solver;
			auto run = [&]() {
				try {
					return solver.solve(cube);
				}
				catch (const char* msg) {
					Logger::WriteMessage(msg);
					return queue<Move*>{};
				}
			};
			bool failed = false;
			chrono::seconds timeout{ 10 };
			for (int i = 0; i < 50000; i++) {
				scramble(cube);
				copy = cube;
				future<queue<Move*>> f = async(launch::async, run);
				future_status status; 
				
				status = f.wait_for(timeout);

				if (status == future_status::timeout || f.get().empty()) {
					save(copy);
					auto msg = "solver unable to solve the cube on iteration:" + to_string(i);
					Logger::WriteMessage(msg.c_str());
					failed = true;
					break;
				}
			}
			
			Assert::IsFalse(failed, L"solver was unable to solve the cube");
		}

		TEST_METHOD(AdjacentSwapDictectedInsteadOfDiagonalSwap) {
			RubiksCube cube;
			load(cube);

			auto findOutOfPlace = [&](vector<reference_wrapper<Cube>>& corners) {
				return filter(corners, [&](Cube& c) { return !cube.isInPlace(c, false); });
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
			
			auto inPlace = [&](vector<reference_wrapper<Cube>>& corners, bool strict = false) {
				int count = count_if(corners.begin(), corners.end(), [&](Cube& c) { return cube.isInPlace(c, strict); });
				return count;
			};

			auto corners = cube.cornersAround(cube.center(YELLOW));

			Assert::IsFalse(cube.isSolved(), L"Cube should not be solved");
			Cube& c0 = cube.cubeAt({ 1, 1, 1 });
			Assert::IsFalse(cube.isInPlace(c0, false), L"c0 should be out of place");

			Cube& c1 = cube.cubeAt({ 1, 1, -1 });
			Assert::IsTrue(cube.isInPlace(c1, false), L"c1 should be in place");

			Cube& c2 = cube.cubeAt({ -1, 1, -1 });
			Assert::IsFalse(cube.isInPlace(c2, false), L"c2 should be out of place");

			Cube& c3 = cube.cubeAt({ -1, 1, 1 });
			Assert::IsTrue(cube.isInPlace(c1, false), L"c3 should be in place");

			auto res = findOutOfPlace(corners);
			Assert::IsTrue(res.size() == 2, L"There should be 2 cubes out of place");

			Assert::IsFalse(cube.isInPlace(res[0]), L"out of place cube expected");
			Assert::IsFalse(cube.isInPlace(res[1]), L"out of place cube expected");

			Assert::IsTrue(res[0].get().pos == c0.pos || res[0].get().pos == c2.pos, L"out of place should match expected");
			Assert::IsTrue(res[1].get().pos == c0.pos || res[1].get().pos == c2.pos, L"out of place should match expected");

			Assert::IsFalse(isAdjacentSwap(corners), L"pattern should not be adjacent swap");

			auto swapAdjacentCorners = [&]() {
				for (int i = 0; i < 3; i++) {
					R.applyTo(cube);
					U.applyTo(cube);
					_R.applyTo(cube);
					_U.applyTo(cube);

				}

				SPIN_RIGHT.applyTo(cube);

				for (int i = 0; i < 3; i++) {
					_L.applyTo(cube);
					_U.applyTo(cube);
					L.applyTo(cube);
					U.applyTo(cube);
				}
			};
			swapAdjacentCorners();
			while (inPlace(corners) < 2) {
				U.applyTo(cube);
			}
			Assert::IsTrue(isAdjacentSwap(corners), L"pattern should not be adjacent swap");
		}
	};
}