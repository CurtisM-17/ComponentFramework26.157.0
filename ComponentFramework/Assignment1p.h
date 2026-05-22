#ifndef ASSIGNMENT1P_H
#define ASSIGNMENT1P_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include <Plane.h>

using namespace MATH;
using namespace MATHEX;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;

class Assignment1p : public Scene {
private:
	Body*   plane;
	Matrix4 planeModelMatrix;
	Mesh*   planeMesh;
	Plane	planeShape;

	Body*   sphere; // physics
	Matrix4 sphereModelMatrix; // translation, rotation, scale
	Mesh*   sphereMesh;

	Shader* shader;
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	bool    drawInWireMode;

public:
	explicit Assignment1p();
	virtual ~Assignment1p();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
};


#endif // SCENE1P_H