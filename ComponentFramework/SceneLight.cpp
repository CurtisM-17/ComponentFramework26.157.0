#include "SceneLight.h"
#include "Scene1g.h"

SceneLight::SceneLight() :
	position(Vec3(0.0f, 0.0f, 0.0f)),
	color(Vec4(1.0f, 1.0f, 1.0f, 0.0f)),
	specular(Vec4(0.3f, 0.3f, 0.3f, 0.0f)),
	enabled(true) {
}

SceneLight::~SceneLight() {}