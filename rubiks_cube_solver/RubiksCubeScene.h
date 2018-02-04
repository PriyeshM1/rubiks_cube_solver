#pragma once

#include <ncl/gl/Scene.h>
#include <ncl/gl/Scene.h>
#include <algorithm>
#include <queue>
#include "moves.h"
#include "solver.h"
#include "CubePainter.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class RubiksCubeScene : public Scene {
public:
	RubiksCubeScene() : Scene("Rubik's Cube solver") {
		_useImplictShaderLoad = true;
		angle = 0;
		move = nullptr;
	}

	virtual void init() override {
		painter = new CubePainter(rubiksCube);
		painter->init();
		initLights();
		using namespace rubiks;

		solver = new SimpleSolver;

		cam.view = glm::lookAt(vec3(3.0f, 3.25f, 3.25f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		nextMove();
		glClearColor(0.5, 0.5, 0.5, 1);
	}

	void initLights() {
		light[0].position = vec4(0, 0, 3, 1);
		light[1].position = vec4(0, 3, 0, 1);
		light[2].position = vec4(3, 0, 0, 1);
		light[3].position = vec4(0, 0, -3, 1);
		light[4].position = vec4(0, -3, 0, 1);
		light[5].position = vec4(-3, 0, 0, 1);

		for (int i = 1; i < 6; i++) light[i].on = true;
	}

	virtual void display() override {
		shader("default")([&](Shader& s) {
			mat4 model;
			s.sendUniformLight("light[0]", light[0]);
			painter->paint(s, cam, move, angle);
		});
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
		if (move) {
			move->applyTo(rubiksCube);
			undo.push(move); 
			move = nullptr;
		}
		if (!moves.empty()) {
			move = moves.front();
			moves.pop();
			angle = 0;
		}
		else {
			scrambling = false;
		}
	}

	virtual void processInput(const Key& key) override {
		using namespace rubiks;
		queue<Move*> scram;
		if (key.status == Key::RELEASED && move == nullptr) {
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
				//scram = superFlip();
				scram = scramble(50);
				while (!scram.empty()) {
					moves.push(scram.front());
					scram.pop();
				}
				scrambling = true;
				speed = 700;
				break;
			case 'e':
				moves.push(&d);
				break;
			case 263:
				moves.push(&SPIN_LEFT);
				break;
			case 262:
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
			case 'i':
				load(rubiksCube);
				break;
			case 'o':
				save(rubiksCube);
				break;
			case 'c':
				rubiksCube.reset();
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
	CubePainter* painter;
};