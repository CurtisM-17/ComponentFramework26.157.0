#pragma once
#include <Vector.h> /// This is in GameDev
using namespace MATH;

class SceneLight {
private:
	Vec3 position;
	Vec3 color;
	bool enabled; // was just kind of playing around with this but it doesn't work very well
public:
	SceneLight();
	~SceneLight();

	Vec3 GetPosition() const {return position;}
	void SetPosition(Vec3 newValue) {position = newValue;}
	Vec3 GetColor() const { return color; }
	void SetColor(Vec3 newValue) { color = newValue; }
	bool IsEnabled() const { return enabled; }
	void SetEnabled(bool newValue) { enabled = newValue; }
};