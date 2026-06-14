#ifndef SCENE3_H
#define SCENE3_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include "Texture.h"
#include "Camera.h"
#include "Trackball.h"
#include "Skybox.h"
using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;

class Scene3g : public Scene {
private:
	Body* sphere;
	Shader* shader;

	Mesh* sphereMesh;
	Mesh* cubeMesh;

	Matrix4 cubeModelMatrix, sphereModelMatrix;
	Vec2 cubeRotVelocity, cubeRotation;

	Matrix4 viewMatrix;
	bool drawInWireMode;
	Vec3 lightPos;

	Camera camera;
	Trackball trackball;

	Vec3 cameraOffset;
	Quaternion cameraOrientation, oldCameraOrientation;
public:
	explicit Scene3g();
	virtual ~Scene3g();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;
};


#endif // SCENE0_H