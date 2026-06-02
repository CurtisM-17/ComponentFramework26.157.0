#pragma once
#include <MMath.h>
#include "Skybox.h"

class Camera {
public:
	Vec3 pos;
	Quaternion orientation;
	Matrix4 projectionMatrix;
	Skybox* skybox;

	Camera();
	~Camera();
	void Render() const;

	Matrix4 GetViewMatrix() const;
};

