#include <iostream>
#include <SDL.h>
#include <SDL3/SDL_events.h>
#include "Scene2g.h"
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include "Texture.h"

/*

Arrow keys or A & D to rotate skull left and right

*/

Scene2g::Scene2g(): 
	sphere{nullptr}, shader{nullptr}, sphereMesh{nullptr},
	drawInWireMode{ false }, skullMesh{ nullptr }, skullTexture{ nullptr },
	eyeTexture{ nullptr }, skullModelMatrix(), leftEyeMatrix(), rightEyeMatrix(),
	projectionMatrix(), viewMatrix(), lightPos(), skullRotVelocity(), skullRotation() {
	Debug::Info("Created Scene2: ", __FILE__, __LINE__);
}

Scene2g::~Scene2g() {
	Debug::Info("Deleted Scene2: ", __FILE__, __LINE__);
}

bool Scene2g::OnCreate() {
	Debug::Info("Loading assets Scene2: ", __FILE__, __LINE__);
	sphere = new Body();
	sphere->OnCreate();

	// Meshes
	sphereMesh = new Mesh("meshes/Sphere.obj");
	sphereMesh->OnCreate();

	skullMesh = new Mesh("meshes/Skull.obj");
	skullMesh->OnCreate();

	// Textures
	skullTexture = new Texture();
	skullTexture->LoadImage("textures/skull_texture.jpg");

	eyeTexture = new Texture();
	eyeTexture->LoadImage("textures/evilEye_v2.jpg");

	// Shader
	shader = new Shader("shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	// Matrices
	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	
	skullModelMatrix.loadIdentity();
	leftEyeMatrix.loadIdentity();
	rightEyeMatrix.loadIdentity();

	// Lights
	lightPos = Vec3(5.0f, 0.0f, 0.0f);

	// Viewport (ripped this from trackball)
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	invNDC = MMath::inverse(MMath::NDCtoViewport(viewport[2], viewport[3]));

	return true;
}

void Scene2g::OnDestroy() {
	Debug::Info("Deleting assets Scene2: ", __FILE__, __LINE__);
	sphere->OnDestroy();
	delete sphere;

	sphereMesh->OnDestroy();
	delete sphereMesh;

	skullMesh->OnDestroy();
	delete skullMesh;

	shader->OnDestroy();
	delete shader;
}

void Scene2g::HandleEvents(const SDL_Event &sdlEvent) {
	switch (sdlEvent.type) {
	case SDL_EVENT_KEY_DOWN:
		switch (sdlEvent.key.scancode) {
			case SDL_SCANCODE_W:
				drawInWireMode = !drawInWireMode;
				break;
			///// Head rotation
			case SDL_SCANCODE_LEFT:
			case SDL_SCANCODE_A:
				// left
				skullRotVelocity = 1.0f;
				break;
			case SDL_SCANCODE_RIGHT:
			case SDL_SCANCODE_D:
				// right
				skullRotVelocity = -1.0f;
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
			skullRotVelocity = 0;
			break;
		}
	}
}

void Scene2g::Update(const float deltaTime) {
	const static float EYE_SCALE = 0.25f;
	const static float EYE_DIST = 0.575f;
	const static float EYE_DEPTH = 0.9f;
	const static float EYE_HEIGHT = 0.175f;
	const static float SKULL_ROT_SPEED = 20.0f;
	const static float MAX_SKULL_ANGLE = 45.0f;

	skullRotation = SDL_clamp(
		skullRotation + (skullRotVelocity * SKULL_ROT_SPEED * deltaTime),
		-MAX_SKULL_ANGLE,
		MAX_SKULL_ANGLE
	);

	// Mouse position
	Vec3 mousePos{};
	SDL_GetMouseState(&mousePos.x, &mousePos.y);
	mousePos = invNDC * mousePos;

	skullModelMatrix =
		MMath::translate(Vec3(0.0f, 0.0f, -3.0f)) *
		MMath::rotate(skullRotation, Vec3(0.0f, -1.0f, 0.0f)
	);

	/////////// EYES ///////////

	// Extract just the skull's rotation
	Matrix4 skullRotation = skullModelMatrix; // copy the matrix
	// Zero out the translation column so that there's only the rotation left
	skullRotation[12] = 0.0f;
	skullRotation[13] = 0.0f;
	skullRotation[14] = 0.0f;

	Matrix4 invSkullRotation = MMath::transpose(skullRotation); // invert the rotation
	
	const static float EYE_ROTATION_FACTOR = 50.0f; // maximum rotation angle
	Vec2 eyeRotationAngle = Vec2(
		(-EYE_ROTATION_FACTOR + (EYE_ROTATION_FACTOR - -EYE_ROTATION_FACTOR)) * mousePos.x,
		(-EYE_ROTATION_FACTOR + (EYE_ROTATION_FACTOR - -EYE_ROTATION_FACTOR)) * mousePos.y
	);

	leftEyeMatrix = (
		skullModelMatrix *
		MMath::translate(Vec3(EYE_DIST, EYE_HEIGHT, EYE_DEPTH)) *
		invSkullRotation * // cancel out the skull's rotation so that the eyes always face the origin/camera
		MMath::rotate(-90.0f, Vec3(0.0f, 1.0f, 0.0f)) * // pivot to face straight
		MMath::rotate(eyeRotationAngle.x, Vec3(0.0f, 1.0f, 0.0f)) * // angle towards cursor (X)
		MMath::rotate(eyeRotationAngle.y, Vec3(0.0f, 0.0f, 1.0f)) * // angle towards cursor (Y)
		MMath::scale(Vec3(EYE_SCALE, EYE_SCALE, EYE_SCALE))
	);

	rightEyeMatrix = (
		skullModelMatrix *
		MMath::translate(Vec3(-EYE_DIST, EYE_HEIGHT, EYE_DEPTH)) *
		invSkullRotation *
		MMath::rotate(-90.0f, Vec3(0.0f, 1.0f, 0.0f)) *
		MMath::rotate(eyeRotationAngle.x, Vec3(0.0f, 1.0f, 0.0f)) * // angle towards cursor (X)
		MMath::rotate(eyeRotationAngle.y, Vec3(0.0f, 0.0f, 1.0f)) * // angle towards cursor (Y)
		MMath::scale(Vec3(EYE_SCALE, EYE_SCALE, EYE_SCALE))
	);
}

void Scene2g::Render() const {
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

	GLuint program = shader->GetProgram();

	glUseProgram(program);
	glUniform3fv(glGetUniformLocation(program, "lightPos"), 1, lightPos); // light
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, viewMatrix);

	// Skull
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, skullModelMatrix);
	glBindTexture(GL_TEXTURE_2D, skullTexture->getTextureID());
	skullMesh->Render(GL_TRIANGLES);

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