// rubiks_cube_solver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define GLM_SWIZZLE 

#include <ncl/gl/GlfwApp.h>
#include "RubiksCubeScene.h"
#include "model.h"
using namespace std;
using namespace ncl;
using namespace gl;

static ConsoleAppender CONSOLE_APPENDER;
vector<Appender*> Logger::appenders = { &CONSOLE_APPENDER };
const unsigned int Logger::level = LEVEL_DEBUG;

int main()
{
	Scene* scene = new RubiksCubeScene;
	start(scene);

/*	using namespace rubiks;
	RubiksCube rCube;
	R.applyTo(rCube);
	_U.applyTo(rCube);
	_L.applyTo(rCube);
	_D.applyTo(rCube);
	F.applyTo(rCube);
	_D.applyTo(rCube);
	_F.applyTo(rCube);

	
	rubiks::Cube* c = rCube.find([&](rubiks::Cube& c) {
		if (!(c.yc == rubiks::WHITE || c.zc == rubiks::WHITE)) return false;
		auto dir = c.yc == rubiks::WHITE ? c.fy : c.fz;
		return c.type == EDGE && dir == LEFT_FACE.direction;
	})[0];

	rubiks::Cube* b = rCube.find([&](rubiks::Cube& c) {
		if (!(c.yc == rubiks::WHITE || c.zc == rubiks::WHITE)) return false;
		auto dir = c.yc == rubiks::WHITE ? c.fy : c.fz;
		return c.type == EDGE && dir == RIGHT_FACE.direction;
	})[0];

	rubiks::Cube* a = rCube.find([&](rubiks::Cube& c) {
		if (!(c.yc == rubiks::WHITE || c.zc == rubiks::WHITE)) return false;
		auto dir = c.yc == rubiks::WHITE ? c.fy : c.fz;
		return c.type == EDGE && dir == FRONT_FACE.direction;
	})[0];

	auto comp = compareBy(rubiks::WHITE);

	assert(comp(a, a) == false);
	assert(comp(a, b) == true);
	if (comp(a, b)) {
		assert(comp(b, a) == false);
	}
	if (comp(a, b) == true && comp(b, c) == true) {
		assert(comp(a, c) == true);
	}*/

	return 0;
}

