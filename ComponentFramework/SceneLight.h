#pragma once
#include <Vector.h> /// This is in GameDev
using namespace MATH;

class SceneLight {
private:
	Vec3 position;
	Vec3 color;
	bool enabled;
public:
	SceneLight();
	~SceneLight();
};