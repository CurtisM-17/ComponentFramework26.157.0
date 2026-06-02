#pragma once
#include <iostream>
#include <glew.h>
#include "Mesh.h"
#include "Shader.h"

class Skybox {
	const char* negx;
	const char* negy;
	const char* negz;
	const char* posx;
	const char* posy;
	const char* posz;

	GLuint textureID;
	Mesh* cube;
	Shader* shader;
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

	Shader* GetShader() const { return shader; }

	bool LoadImages();
	void Render() const;
};

