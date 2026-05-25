#pragma once
#include <Vector.h> /// This is in GameDev
using namespace MATH;

struct SceneLight {
private:
	Vec3 position;
	Vec4 color;
	Vec4 specular;
	bool enabled; // was just kind of playing around with this but it doesn't work very well
public:
	SceneLight();
	~SceneLight();

	Vec3 GetPosition() const {return position;}
	void SetPosition(Vec3 newValue) {position = newValue;}
	Vec4 GetColor() const { return color; }
	void SetColor(Vec4 newValue) { color = newValue; }
	Vec4 GetSpecular() const { return specular; }
	void SetSpecular(Vec4 newValue) { specular = newValue; }
	bool IsEnabled() const { return enabled; }
	void SetEnabled(bool newValue) { enabled = newValue; }
};