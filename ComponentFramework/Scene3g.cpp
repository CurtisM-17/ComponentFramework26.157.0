#include <iostream>
#include <SDL.h>
#include <SDL3/SDL_events.h>
#include "Scene3g.h"
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include "Texture.h"
#include "Camera.h"

Scene3g::Scene3g(): sphere{nullptr}, shader{nullptr}, sphereMesh{nullptr},
					drawInWireMode{false} {
	Debug::Info("Created Scene3: ", __FILE__, __LINE__);
}

Scene3g::~Scene3g() {
	Debug::Info("Deleted Scene3: ", __FILE__, __LINE__);
}

bool Scene3g::OnCreate() {
	Debug::Info("Loading assets Scene3: ", __FILE__, __LINE__);
	sphere = new Body();
	sphere->OnCreate();

	// Meshes
	sphereMesh = new Mesh("meshes/Sphere.obj");
	sphereMesh->OnCreate();

	skullMesh = new Mesh("meshes/Skull.obj");
	skullMesh->OnCreate();

	marioMesh = new Mesh("meshes/Mario.obj");
	marioMesh->OnCreate();

	// Textures
	earthTexture = new Texture();
	earthTexture->LoadImage("textures/earthclouds.jpg");

	moonTexture = new Texture();
	moonTexture->LoadImage("textures/skull_texture.jpg");

	marioTexture = new Texture();
	marioTexture->LoadImage("textures/mario_main.png");

	eyeTexture = new Texture();
	eyeTexture->LoadImage("textures/evilEye.jpg");

	// Shader
	shader = new Shader("shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	// Matrices
	//projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	//viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	camera.pos = Vec3(0.0f, 0.0f, 5.0f);

	earthModelMatrix.loadIdentity();
	moonModelMatrix.loadIdentity();
	marioModelMatrix.loadIdentity();
	leftEyeMatrix.loadIdentity();
	rightEyeMatrix.loadIdentity();

	// Lights
	lightPos = Vec3(5.0f, 0.0f, 0.0f);

	return true;
}

void Scene3g::OnDestroy() {
	Debug::Info("Deleting assets Scene3: ", __FILE__, __LINE__);
	sphere->OnDestroy();
	delete sphere;

	sphereMesh->OnDestroy();
	delete sphereMesh;

	skullMesh->OnDestroy();
	delete skullMesh;

	marioMesh->OnDestroy();
	delete marioMesh;

	shader->OnDestroy();
	delete shader;
}

void Scene3g::HandleEvents(const SDL_Event &sdlEvent) {
	trackball.HandleEvents(sdlEvent);

	switch (sdlEvent.type) {
	case SDL_EVENT_KEY_DOWN:
		switch (sdlEvent.key.scancode) {
			case SDL_SCANCODE_W:
				drawInWireMode = !drawInWireMode;
				break;
			default:
				break;
		}
		break;
	}
}

void Scene3g::Update(const float deltaTime) {
	camera.orientation = trackball.getQuat();

	const static float EARTH_SPIN_SPEED = 50.0f;
	static float totalElapsed = 0.0f;
	totalElapsed += deltaTime;

	const float MOON_SPEED = (totalElapsed * EARTH_SPIN_SPEED) / 27.3f;

	const static Vec3 moonPos = Vec3(3.0f, 0.0f, 0.0f);

	earthModelMatrix = MMath::rotate(
		totalElapsed * EARTH_SPIN_SPEED, 
		Vec3(0.0f, 1.0f, 0.0f)) *
		MMath::rotate(90.0f, Vec3(-1.0f, 0.0f, 0.0f)
	);

	moonModelMatrix = 
		MMath::rotate(MOON_SPEED, Vec3(0.0f, 1.0f, 0.0f)) * // spin
		MMath::rotate(MOON_SPEED, Vec3(0.0f, 1.0f, 0.0f)) * // orbit
		MMath::translate(moonPos) * // move
		MMath::rotate(90.0f, Vec3(0.0f, -1.0f, 0.0f)) * // rotate axis
		MMath::scale(Vec3(0.2f, 0.2f, 0.2f/*0.272*/) // scale
	);

	marioModelMatrix = 
		moonModelMatrix * 
		MMath::rotate(MOON_SPEED * 10.0f, Vec3(0.0f, 1.0f, 0.0f)) * // orbit
		MMath::translate(Vec3(3.0f, 0.0f, 0.0f)) *
		MMath::rotate(90.0f, Vec3(0.0f, 1.0f, 0.0f) // rotate axis
	);

	leftEyeMatrix = (
		moonModelMatrix *
		MMath::translate(Vec3(0.65f, 0.2f, 0.85f)) *
		MMath::rotate(-90.0f, Vec3(0.0f, 1.0f, 0.0f)) * // rotate axis
		MMath::scale(Vec3(0.35f, 0.35f, 0.35f)) // scale
	);

	rightEyeMatrix = (
		moonModelMatrix *
		MMath::translate(Vec3(-0.65f, 0.2f, 0.85f)) *
		MMath::rotate(-90.0f, Vec3(0.0f, 1.0f, 0.0f)) * // rotate axis
		MMath::scale(Vec3(0.35f, 0.35f, 0.35f)) // scale
	);
}

void Scene3g::Render() const {
	/// Set the background color then clear the screen
	glEnable(GL_DEPTH_TEST);
	//glClearColor(135.0f/255.0f, 206.0f/255.0f, 250.0f/255.0f, 0.0f); // bg
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(drawInWireMode){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}else{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glUseProgram(shader->GetProgram());
	glUniform3fv(shader->GetUniformID("lightPos"), 1, lightPos); // light
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, camera.projectionMatrix);
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera.GetViewMatrix());

	// Earth
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, earthModelMatrix);
	glBindTexture(GL_TEXTURE_2D, earthTexture->getTextureID());
	sphereMesh->Render(GL_TRIANGLES);

	// Moon
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, moonModelMatrix);
	glBindTexture(GL_TEXTURE_2D, moonTexture->getTextureID());
	skullMesh->Render(GL_TRIANGLES);

	// Mario
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, marioModelMatrix);
	glBindTexture(GL_TEXTURE_2D, marioTexture->getTextureID());
	marioMesh->Render(GL_TRIANGLES);

	// Eyeballs
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, leftEyeMatrix);
	glBindTexture(GL_TEXTURE_2D, eyeTexture->getTextureID());
	sphereMesh->Render(GL_TRIANGLES);

	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, rightEyeMatrix);
	sphereMesh->Render(GL_TRIANGLES);

	//
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}



	
