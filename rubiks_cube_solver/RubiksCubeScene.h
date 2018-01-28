#pragma once

#include <ncl/gl/Scene.h>
#include <ncl/gl/Scene.h>
#include <algorithm>
#include <queue>
#include "moves.h"
#include "solver.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class RubiksCubeScene : public Scene {
public:
	RubiksCubeScene() : Scene("Rubik's Cube solver") {
		_useImplictShaderLoad = true;
		angle = 0;
	}

	virtual void init() override {
		initCube();
		using namespace rubiks;
		//moves.push(&R);
		//moves.push(&U);
		//moves.push(&_R);
		//moves.push(&_U);
		//moves.push(&SPIN_RIGHT);
		//moves.push(&_L);
		//moves.push(&_U);
		//moves.push(&L);
		//moves.push(&U);


		moves.push(&R);
		moves.push(&_U);
		moves.push(&_L);
		moves.push(&_D);
		moves.push(&F);
		moves.push(&_D);
		moves.push(&_F);

	//	R.applyTo(rubiksCube);
	//	_L.applyTo(rubiksCube);
	//	F.applyTo(rubiksCube);

		solver = new SimpleSolver;
	//	moves = solver->solve(rubiksCube);
		//moves = scramble(20);

		cam.view = glm::lookAt(vec3(3.0f, 3.25f, 3.25f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		nextMove();
		glClearColor(0.5, 0.5, 0.5, 1);
	}

	void initCube() {
		using namespace rubiks;
		for (int i = 0; i < NUM_CUBES; i++) {
			cubes[i][0] = new ncl::gl::Cube(1.0f, 10, ncl::gl::BLACK);
			cubes[i][1] = new ncl::gl::Cube(0.9, 10, vec4(rubiksCube.cubes[i].zc, 1.0));
			if (rubiksCube.cubes[i].type == EDGE || rubiksCube.cubes[i].type == CORNER) {
				cubes[i][2] = new ncl::gl::Cube(0.9, 10, vec4(rubiksCube.cubes[i].yc, 1.0));
			}
			if (rubiksCube.cubes[i].type == CORNER) {
				cubes[i][3] = new ncl::gl::Cube(0.9, 10, vec4(rubiksCube.cubes[i].xc, 1.0));
			}
		}		
	}

	virtual void display() override {
		shader("default")([&](Shader& s) {
			using namespace rubiks;
			mat4 model;
			s.sendUniformLight("light[0]", light[0]);
			for (int i = 0; i < NUM_CUBES; i++) {
				auto cube = rubiksCube.cubes[i]; 
				if (move != nullptr && move->affects(cube)) {
					model = rotate(mat4(1), radians(angle), move->rotation.axis);
					mat4& base = translate(model, rubiksCube.cubes[i].pos);
					cam.model = base;
					s.sendComputed(cam);
					cubes[i][0]->draw(s);

					vec3 pos = vec3(0.1);
					cam.model = translate(base, cube.fz * pos);
					s.sendComputed(cam);
					cubes[i][1]->draw(s);

					if (rubiksCube.cubes[i].type == EDGE || rubiksCube.cubes[i].type == CORNER) {
						cam.model = translate(base, cube.fy * pos);
						s.sendComputed(cam);
						cubes[i][2]->draw(s);
					}

					if (rubiksCube.cubes[i].type == CORNER) {
						cam.model = translate(base, cube.fx * pos);
						s.sendComputed(cam);
						cubes[i][3]->draw(s);
					}
				}
				else {
					mat4& base = translate(mat4(1), rubiksCube.cubes[i].pos);
					cam.model = base;
					s.sendComputed(cam);
					cubes[i][0]->draw(s);

					vec3 pos = vec3(0.1);
					cam.model = translate(base, cube.fz * pos);
					s.sendComputed(cam);
					cubes[i][1]->draw(s);

					if (rubiksCube.cubes[i].type == EDGE || rubiksCube.cubes[i].type == CORNER) {
						cam.model = translate(base, cube.fy * pos);
						s.sendComputed(cam);
						cubes[i][2]->draw(s);
					}

					if (rubiksCube.cubes[i].type == CORNER) {
						cam.model = translate(base, cube.fx * pos);
						s.sendComputed(cam);
						cubes[i][3]->draw(s);
					}
				}
			}
		});
	}

	void draw(rubiks::Cube& cube, int i, mat4& model, Shader& s) {
		using namespace rubiks;
		cam.model = model;
		s.sendComputed(cam);
		cubes[i][0]->draw(s);

		vec3 pos = vec3(0.1);
		cam.model = translate(model, cube.fz * pos);
		s.sendComputed(cam);
		cubes[i][1]->draw(s);

		if (rubiksCube.cubes[i].type == EDGE || rubiksCube.cubes[i].type == CORNER) {
			cam.model = translate(model, cube.fy * pos);
			s.sendComputed(cam);
			cubes[i][2]->draw(s);
		}

		if (rubiksCube.cubes[i].type == CORNER) {
			cam.model = translate(model, cube.fx * pos);
			s.sendComputed(cam);
			cubes[i][3]->draw(s);
		}
	}
	
	virtual void resized() override {
		cam.projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.3f, 100.0f);
	}

	virtual void update(float dt) override {
		if (move) {
			float limit = move->rotation.amout;
			if (limit > 0) {
				angle += dt * speed;
				if (angle >= limit) {
					nextMove();
				}
			}
			else if (limit < 0) {
				angle -= dt * speed;
				if (angle <= limit) {
					nextMove();
				}
			}
		}
	}

	void nextMove() {
		if (move) move->applyTo(rubiksCube);
		if (!moves.empty()) {
			move = moves.front();
			moves.pop();
			angle = 0;
		}
		else {
			scrambling = false;
			move = nullptr;
		}
	}

	virtual void processInput(const Key& key) override {
		using namespace rubiks;
		queue<Move*> scram;
		if (key.status == Key::RELEASED & move == nullptr) {
			switch (key.value()) {
			case 'r':
				moves.push(&R);
				break;
			case 'R':
				moves.push(&_R);
				break;
			case 'l':
				moves.push(&L);
				break;
			case 'L':
				moves.push(&_L);
				break;
			case 'u':
				moves.push(&U);
				break;
			case 'U':
				moves.push(&_U);
				break;
			case 'd':
				moves.push(&D);
				break;
			case 'D':
				moves.push(&_D);
				break;
			case 'f':
				moves.push(&F);
				break;
			case 'F':
				moves.push(&_F);
				break;
			case 'b':
				moves.push(&B);
				break;
			case 'B':
				moves.push(&_B);
				break;
			case 's':
				scram = scramble(50);
				while (!scram.empty()) {
					moves.push(scram.front());
					scram.pop();
				}
				scrambling = true;
				speed = 700;
				break;
			case 262:
				moves.push(&SPIN_LEFT);
				break;
			case 263:
				moves.push(&SPIN_RIGHT);
				break;
			case 264:
				moves.push(&SPIN_DOWN);
				break;
			case 265:
				moves.push(&SPIN_UP);
				break;
			case ' ':
				moves = solver->solve(rubiksCube);
				break;
			case 'o':
				load(rubiksCube);
				break;
			default:
				break;
			}
			if (!scrambling) speed = 300;
			nextMove();
		}
	}

private:
	float angle;
	ncl::gl::Cube* cubes[NUM_CUBES][4];
	ncl::gl::Sphere* sphere;
	rubiks::RubiksCube rubiksCube;
	queue<rubiks::Move*> moves;
	stack<rubiks::Move*> undo;
	rubiks::Move* move;
	float speed = 300;
	bool scrambling = false;
	rubiks::Solver* solver;
};