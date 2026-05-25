#ifndef SCENE1_H
#define SCENE1_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include "Texture.h"
#include <array>
using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;

class Scene1g : public Scene {
private:
	Body* sphere;
	Shader* shader;

	Mesh* sphereMesh;
	Mesh* skullMesh;
	Mesh* marioMesh;

	Texture* earthTexture;
	Texture* moonTexture;
	Texture* marioTexture;
	Texture* eyeTexture;

	Matrix4 earthModelMatrix, moonModelMatrix, marioModelMatrix;
	Matrix4 leftEyeMatrix, rightEyeMatrix;

	Matrix4 projectionMatrix, viewMatrix;
	bool drawInWireMode;


	Vec3 lightPos;
public:
	explicit Scene1g();
	virtual ~Scene1g();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
};


#endif // SCENE0_H