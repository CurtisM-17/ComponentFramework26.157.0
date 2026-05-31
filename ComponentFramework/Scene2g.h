#ifndef SCENE2_H
#define SCENE2_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include "Texture.h"
#include "Camera.h"
#include "Trackball.h"
using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;

class Scene2g : public Scene {
private:
	Body* sphere;
	Shader* shader;

	Mesh* sphereMesh;
	Mesh* skullMesh;

	Texture* skullTexture;
	Texture* eyeTexture;

	Matrix4 skullModelMatrix, leftEyeMatrix, rightEyeMatrix;
	float skullRotVelocity, skullRotation;

	Matrix4 projectionMatrix, viewMatrix;
	bool drawInWireMode;
	Vec3 lightPos;

	Matrix4 invNDC;
public:
	explicit Scene2g();
	virtual ~Scene2g();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
};


#endif // SCENE0_H