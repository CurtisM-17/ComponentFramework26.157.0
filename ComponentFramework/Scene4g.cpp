#include <iostream>
#include <SDL.h>
#include <SDL3/SDL_events.h>
#include "Scene4g.h"
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include "Texture.h"

/*

WASD or arrow keys to rotate the cube around

*/

Scene4g::Scene4g() :
	sphere{ nullptr }, shader{ nullptr }, sphereMesh{ nullptr },
	drawInWireMode{ false }, sphereModelMatrix(), viewMatrix(), 
	lightPos(), cubeRotVelocity(), cubeRotation(Vec2(35, 0)),
	cubeMesh(), totalTime(), perlinNoiseID()
{
	Debug::Info("Created Scene4: ", __FILE__, __LINE__);
}

Scene4g::~Scene4g() {
	Debug::Info("Deleted Scene4: ", __FILE__, __LINE__);
}

bool Scene4g::OnCreate() {
	Debug::Info("Loading assets Scene4: ", __FILE__, __LINE__);
	sphere = new Body();
	sphere->OnCreate();

	// Meshes
	sphereMesh = new Mesh("meshes/Skull.obj");
	sphereMesh->OnCreate();

	cubeMesh = new Mesh("meshes/Cube.obj");
	cubeMesh->OnCreate();

	// Shader
	shader = new Shader("shaders/noiseVert.glsl", "shaders/noiseFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	perlinNoiseID = CreateNoise3D();

	// Matrices
	camera.pos = Vec3(0, 0, 3.5f);
	viewMatrix = camera.GetViewMatrix();

	sphereModelMatrix.loadIdentity();
	cubeModelMatrix.loadIdentity();

	// Lights
	lightPos = Vec3(5.0f, 0.0f, 0.0f);

	// Skybox
	camera.skybox = new Skybox(
		"textures/underwater/negx.png",
		"textures/underwater/negy.png",
		"textures/underwater/negz.png",
		"textures/underwater/posx.png",
		"textures/underwater/posy.png",
		"textures/underwater/posz.png"
	);

	camera.skybox->LoadImages();

	return true;
}

void Scene4g::OnDestroy() {
	Debug::Info("Deleting assets Scene4: ", __FILE__, __LINE__);
	sphere->OnDestroy();
	delete sphere;

	sphereMesh->OnDestroy();
	delete sphereMesh;

	shader->OnDestroy();
	delete shader;

	DeleteNoise3D();
}

void Scene4g::HandleEvents(const SDL_Event& sdlEvent) {
	trackball.HandleEvents(sdlEvent);

	switch (sdlEvent.type) {
	case SDL_EVENT_KEY_DOWN:
		switch (sdlEvent.key.scancode) {
		case SDL_SCANCODE_Z:
			drawInWireMode = !drawInWireMode;
			break;
		///// Cube rotation
		case SDL_SCANCODE_LEFT:
		case SDL_SCANCODE_A:
			// left
			cubeRotVelocity.x = 1.0f;
			break;
		case SDL_SCANCODE_RIGHT:
		case SDL_SCANCODE_D:
			// right
			cubeRotVelocity.x = -1.0f;
			break;
		case SDL_SCANCODE_UP:
		case SDL_SCANCODE_W:
			// left
			cubeRotVelocity.y = 1.0f;
			break;
		case SDL_SCANCODE_DOWN:
		case SDL_SCANCODE_S:
			// right
			cubeRotVelocity.y = -1.0f;
			break;
		default:
			break;
		}
		break;

	case SDL_EVENT_KEY_UP:
		switch (sdlEvent.key.scancode) {
		case SDL_SCANCODE_LEFT:
		case SDL_SCANCODE_A:
		case SDL_SCANCODE_RIGHT:
		case SDL_SCANCODE_D:
			// stop rotating
			cubeRotVelocity.x = 0;
			break;
		case SDL_SCANCODE_UP:
		case SDL_SCANCODE_W:
		case SDL_SCANCODE_DOWN:
		case SDL_SCANCODE_S:
			// stop rotating
			cubeRotVelocity.y = 0;
			break;
		}
	}
}

void Scene4g::Update(const float deltaTime) {
	// /*
	const static float ROT_SPEED = 20.0f;
	const static float MAX_ANGLE = 45.0f;

	cubeRotation.x = SDL_clamp(
		cubeRotation.x + (cubeRotVelocity.x * ROT_SPEED * deltaTime),
		-MAX_ANGLE,
		MAX_ANGLE
	);

	cubeRotation.y = SDL_clamp(
		cubeRotation.y + (cubeRotVelocity.y * ROT_SPEED * deltaTime),
		-MAX_ANGLE,
		MAX_ANGLE
	);

	sphereModelMatrix = (
		MMath::translate(Vec3(2.0f, 1.0f, -2.0f)) *
		MMath::rotate(cubeRotation.x, Vec3(0.0f, -1.0f, 0.0f)) *
		MMath::rotate(cubeRotation.y, Vec3(-1.0f, 0.0f, 0.0f)) *
		MMath::scale(Vec3(0.75f, 0.75f, 0.75f))
	);

	camera.orientation = QMath::inverse(trackball.getQuat()); // camera

	totalTime += deltaTime;

}

void Scene4g::Render() const {
	/// Set the background color then clear the screen
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	camera.Render();
	glEnable(GL_DEPTH_TEST);

	if (drawInWireMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	GLuint program = shader->GetProgram();
	glUseProgram(program);

	glUniform3fv(glGetUniformLocation(program, "lightPos"), 1, lightPos); // light
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera.projectionMatrix);
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera.GetViewMatrix());
	glUniform1f(shader->GetUniformID("totalTime"), totalTime);

	glBindTexture(GL_TEXTURE_CUBE_MAP, camera.skybox->GetTextureID());
	glBindTexture(GL_TEXTURE_3D, perlinNoiseID);

	// Sphere
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, sphereModelMatrix);
	sphereMesh->Render(GL_TRIANGLES);

	//
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}