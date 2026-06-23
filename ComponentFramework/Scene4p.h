#ifndef SCENE4P_H
#define SCENE4P_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include <Plane.h>
#include "Trackball.h"
#include <vector>
#include <array>

using namespace MATH;
using namespace MATHEX; // Plane class is hidden behind MATHEX

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;

class Scene4p : public Scene {
private:
	Mesh*   sphereMesh; // vertices and normals of the sphere from the obj file
	// Umer is using a standard array
	// Daniel wants to use this one too
	std::array<Body*, 17> joints; // I have 17 joints or bones
	// You could also do this
	// Body* joints[17];
	Body* target;
	Plane skeletonPlane;
	float forearmLength, upperarmLength;
	float shinLength, thighLength;
	Vec3 leftToeOffset, rightToeOffset;
	float angle, totalTime;
	bool renderCursor;

	int currentLimb; // 0-3; 0 = right arm, 1 = left arm, 2 = left leg, 3 = right leg
	
	Trackball trackball;

	// Don't tell Scott, but I'm gonna make a camera
	Vec3       cameraPos;
	Vec3       cameraOffset;
	Quaternion cameraOrientation; // this is the new one
	Quaternion cameraOrientationOld;

	Shader* shader;
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	bool drawInWireMode;

public:
	explicit Scene4p();
	virtual ~Scene4p();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;

	void IK(int from, int to, float length);
};


#endif // Scene4p_H