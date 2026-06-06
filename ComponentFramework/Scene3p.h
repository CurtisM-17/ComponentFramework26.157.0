#ifndef SCENE3P_H
#define SCENE3P_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include <Plane.h>
#include "Trackball.h"
#include <vector>
#include "Camera.h"

using namespace MATH;
using namespace MATHEX; // Plane class is hidden behind MATHEX

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;

class Scene3p : public Scene {
private:
	Mesh* sphereMesh; // vertices and normals of the sphere from the obj file
	// One body for the jelly fish head
	Body* jellyfishHead;
	// Smaller spheres just to show us where the tentacles are anchored 
	std::vector<Body*> anchors;
	// And the smallest spheres will make up the tentacles 
	std::vector<Body*> tentacleSpheres;
	// These variables will tells us how long each tentacle will be
	// and the spacing between individual spheres 
	const int numSpheresPerAnchor = 10;
	const float spacing = 1.0f;

	Trackball trackball;

	// Don't tell Scott, but I'm gonna make a camera
	Camera camera;
	//Vec3       cameraPos;
	Vec3       cameraOffset;
	Quaternion cameraOrientation; // this is the new one
	Quaternion cameraOrientationOld;

	Shader* shader;
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	bool drawInWireMode;

public:
	explicit Scene3p();
	virtual ~Scene3p();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;
};


#endif // Scene3p_H