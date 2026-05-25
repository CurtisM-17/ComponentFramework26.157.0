#ifndef SCENE2P_H
#define SCENE2P_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include <Plane.h>
#include "Trackball.h"

using namespace MATH;
using namespace MATHEX; // Plane class is hidden behind MATHEX

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;

class Scene2p : public Scene {
private:
	Body* plane;
	Mesh* planeMesh;
	Plane planeShape; // this will hold the plane normal and distance to origin

	Mesh* sphereMesh; // vertices and normals of the sphere from the obj file

	Body* cueBall;
	Body* targetBall;

	Shader* shader;
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	bool    drawInWireMode;

	Trackball trackball;
	Vec3       cameraPos;
	Quaternion cameraOrientation, oldCameraOrientation;

public:
	explicit Scene2p();
	virtual ~Scene2p();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;
};


#endif // SCENE2P_H