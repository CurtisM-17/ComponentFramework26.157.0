#ifndef SCENE0P_H
#define SCENE0P_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include <Plane.h>

using namespace MATH;
using namespace MATHEX; // Plane class is hidden behind MATHEX

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;

class Scene0p : public Scene {
private:
	Body* plane;
	// Umer is getting annoyed with having a separate model matrix. 
	// He will shove it into the Body class
	Mesh* planeMesh;
	Plane   planeShape; // this will hold the plane normal and distance to origin

	Mesh* sphereMesh; // vertices and normals of the sphere from the obj file

	Body* cueBall;
	Body* targetBall;

	Shader* shader;
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	bool drawInWireMode;

public:
	explicit Scene0p();
	virtual ~Scene0p();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;
};


#endif // SCENE2P_H