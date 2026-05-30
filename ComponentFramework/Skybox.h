#pragma once
#include <iostream>

using std::string;

class Skybox {
	const string negx;
	const string negy;
	const string negz;
	const string posx;
	const string posy;
	const string posz;
public:
	Skybox(
		const char* negx_,
		const char* negy_,
		const char* negz_,
		const char* posx_,
		const char* posy_,
		const char* posz_
	);
	~Skybox();
};

