// rubiks_cube_solver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define GLM_SWIZZLE 
//#define DEBUG

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

	return 0;
}

