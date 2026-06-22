#include <glew.h>
#include <iostream>
#include <SDL.h>
#include <SDL3/SDL_events.h>
#include "Scene5g.h"
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include "Texture.h"

Scene5g::Scene5g() : 
	shader{nullptr}, mesh{nullptr}, drawInWireMode{true} 
{
	Debug::Info("Created Scene5: ", __FILE__, __LINE__);
}

Scene5g::~Scene5g() {
	Debug::Info("Deleted Scene5: ", __FILE__, __LINE__);
}

bool Scene5g::OnCreate() {
	Debug::Info("Loading assets Scene5: ", __FILE__, __LINE__);
	
	mesh = new Mesh("meshes/Plane.obj");
	mesh->OnCreate();

	heightMap = new Texture();
	heightMap->LoadImage("textures/terrainHeight.png");

	shader = new Shader(
		"shaders/tessellationVert.glsl", 
		"shaders/tessellationFrag.glsl",
		"shaders/tessellationControl.glsl",
		"shaders/tessellationEval.glsl",
		nullptr
	);
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, 15.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	//modelMatrix.loadIdentity();
	modelMatrix = (
		MMath::translate(Vec3(0, 0, -20)) *
		MMath::rotate(60.0f, Vec3(-1.0f, 0.0f, 0.0f)) *
		MMath::scale(Vec3(3.0f, 3.0f, 3.0f))
	);
	return true;
}

void Scene5g::OnDestroy() {
	Debug::Info("Deleting assets Scene5: ", __FILE__, __LINE__);

	mesh->OnDestroy();
	delete mesh;

	shader->OnDestroy();
	delete shader;

	
}

void Scene5g::HandleEvents(const SDL_Event &sdlEvent) {
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

void Scene5g::Update(const float deltaTime) {
	
}

void Scene5g::Render() const {
	/// Set the background color then clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(drawInWireMode){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}else{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glUseProgram(shader->GetProgram());

	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, modelMatrix);
	glBindTexture(GL_TEXTURE_2D, heightMap->getTextureID());
	
	mesh->Render(GL_PATCHES);
	glUseProgram(0);
}



	
