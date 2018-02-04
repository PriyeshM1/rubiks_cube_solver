#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ncl/gl/Shader.h>
#include <ncl/gl/Cube.h>
#include <ncl/gl/Scene.h>
#include "model.h"
#include "moves.h"

using namespace glm;

class CubePainter {
public:
	CubePainter(rubiks::RubiksCube& cube) :rubiksCube(cube) {}

	~CubePainter() {;
	}

	void init() {
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

	void paint(ncl::gl::Shader& s, ncl::gl::GlmCam& cam, rubiks::Move* move, float angle) {
		using namespace rubiks;
		mat4 model;
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
	}

private:
	rubiks::RubiksCube& rubiksCube;
	ncl::gl::Cube* cubes[NUM_CUBES][4];

};