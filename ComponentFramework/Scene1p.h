#ifndef SCENE1P_H
#define SCENE1P_H
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

class Scene1p : public Scene {
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
	explicit Scene1p();
	virtual ~Scene1p();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;

	static Vec3 NormOrNothing(const Vec3 v);
};


#endif // SCENE1P_H