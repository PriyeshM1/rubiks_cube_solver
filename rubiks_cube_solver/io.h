#pragma once

#include "model.h"
#include <fstream>

namespace rubiks {

	ostream& operator<< (ostream& out, const vec3 v) {
		out << v.x << ' ' << v.y << ' ' << v.z << ' ';
		return out;
	}


	ostream& operator << (ostream& out, const Cube& cube) {
		out << "pos: " << cube.pos << endl;
		out << "x direciton: " << cube.fx << endl;
		out << "y direciton: " << cube.fy << endl;
		out << "z direction: " << cube.fz << endl;
		if (dot(cube.xc, cube.xc)) out << "x color: " << cube.xc << endl;
		out << "y color: " << cube.yc << endl;
		out << "z color: " << cube.zc << endl;
		out << endl << endl;
		return out;
	}

	//istream& operator>>(istream& in, vec3 v) {
	//	float x, y, z;
	//	unsigned dim;
	//	in >> x >> dim >> y >> dim >> z >> dim;
	//	v.x = x; v.y = y; v.z = z;
	//	return in;
	//}

	void save(RubiksCube& rCube) {
		for_each(begin(rCube.cubes), end(rCube.cubes), [](Cube& c) { c.parent = nullptr; });
		ofstream fout("c:\\temp\\cube.rubiks", ios::binary);
		if (!fout) {
			throw std::runtime_error("unable to open file cube.rubiks");
		}
		fout.write((char*)&rCube, sizeof RubiksCube);
		fout.close();
	}

	void load(RubiksCube& rCube) {
		ifstream fin("c:\\temp\\cube.rubiks", ios::binary);
		if (!fin) {
			throw std::runtime_error("unable to open file cube.rubiks");
		}
		fin.read((char*)&rCube, sizeof RubiksCube);
		for_each(begin(rCube.cubes), end(rCube.cubes), [&](Cube& c) { c.parent = &rCube; });
	}
}