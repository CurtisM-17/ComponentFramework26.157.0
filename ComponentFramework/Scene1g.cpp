#include <iostream>
#include <SDL.h>
#include <SDL3/SDL_events.h>
#include "Scene1g.h"
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include "Texture.h"
#include "SceneLight.h"
#include <array>

Scene1g::Scene1g(): sphere{nullptr}, shader{nullptr}, sphereMesh{nullptr},
					drawInWireMode{false} {
	Debug::Info("Created Scene1: ", __FILE__, __LINE__);
}

Scene1g::~Scene1g() {
	Debug::Info("Deleted Scene1: ", __FILE__, __LINE__);
}

bool Scene1g::OnCreate() {
	Debug::Info("Loading assets Scene1: ", __FILE__, __LINE__);
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
	shader = new Shader("shaders/textureMultiPhongVert.glsl", "shaders/textureMultiPhongFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	// Matrices
	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	
	earthModelMatrix.loadIdentity();
	moonModelMatrix.loadIdentity();
	marioModelMatrix.loadIdentity();
	leftEyeMatrix.loadIdentity();
	rightEyeMatrix.loadIdentity();

	// Lights
	SceneLight* light1 = CreateLight(Vec3(15.0f, 0.0f, 0.0f));
	//SceneLight* light2 = CreateLight(Vec3(0.0f, 5.0f, 0.0f));
	//light1->SetColor(Vec3(1, 0, 0));
	CreateLight(Vec3(-5, -5, 0));

	return true;
}

void Scene1g::OnDestroy() {
	Debug::Info("Deleting assets Scene1: ", __FILE__, __LINE__);
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

	for (int i = 0; i < lights.size(); i++) {
		delete lights[i];
		lights[i] = nullptr;
	}
}

void Scene1g::HandleEvents(const SDL_Event &sdlEvent) {
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

void Scene1g::Update(const float deltaTime) {
	const static float EARTH_SPIN_SPEED = 50.0f;
	static float totalElapsed = 0.0f;
	totalElapsed += deltaTime;

	const float MOON_SPEED = (totalElapsed * EARTH_SPIN_SPEED) / 27.3f;

	const static Vec3 moonPos = Vec3(3.0f, 0.0f, 0.0f);

	earthModelMatrix = 
		//MMath::translate(Vec3(-2, 0, 0)) *
		MMath::rotate(totalElapsed * EARTH_SPIN_SPEED, Vec3(0.0f, 1.0f, 0.0f)) *
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

void Scene1g::Render() const {
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

	//glGetProgramiv(shader->GetProgram(), GL_LINK_STATUS, &success);
	//std::cout << "Link status: " << success << std::endl;

	// Lights
	int count = static_cast<int>(lights.size()); // did have to look up to use static cast because I was getting warnings, would like to understand it more though
	count = std::min(count, 6); // maximum light count
	std::vector<float> lightPositions(count * 3); // convert into a vector of floats
	std::vector<float> lightColors(count * 4); // colors/diffuse
	std::vector<float> lightSpeculars(count * 4); // colors

	for (int i = 0; i < count; i++) {
		if (!lights[i]->IsEnabled()) continue;

		Vec3 pos = lights[i]->GetPosition();
		Vec4 col = lights[i]->GetColor();
		Vec4 spec = lights[i]->GetSpecular();

		lightPositions[i * 3] = pos.x;
		lightPositions[i * 3 + 1] = pos.y;
		lightPositions[i * 3 + 2] = pos.z;

		lightColors[i * 4] = col.x;
		lightColors[i * 4 + 1] = col.y;
		lightColors[i * 4 + 2] = col.z;
		lightColors[i * 4 + 3] = col.w;

		lightSpeculars[i * 4] = spec.x;
		lightSpeculars[i * 4 + 1] = spec.y;
		lightSpeculars[i * 4 + 2] = spec.z;
		lightSpeculars[i * 4 + 3] = spec.w;
	}

	GLuint program = shader->GetProgram();

	glUniform3fv(glGetUniformLocation(program, "lightPos"), count, lightPositions.data());
	glUniform3fv(glGetUniformLocation(program, "lightColors"), count, lightColors.data());
	glUniform1i(glGetUniformLocation(program, "lightCount"), count);

	// Matrices
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, viewMatrix);

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