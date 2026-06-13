#include <iostream>
#include <SDL.h>
#include <SDL3/SDL_events.h>
#include "Scene3p.h"
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include "Quaternion.h"
#include <QMath.h>

Scene3p::Scene3p() :
	jellyfishHead{ nullptr },
	shader{ nullptr },
	sphereMesh{ nullptr },
	drawInWireMode{ true } {
	Debug::Info("Created Scene3p: ", __FILE__, __LINE__);
}

Scene3p::~Scene3p() {
	Debug::Info("Deleted Scene3p: ", __FILE__, __LINE__);
}

bool Scene3p::OnCreate() {
	Debug::Info("Loading assets Scene3p: ", __FILE__, __LINE__);
	// From the assignment doc
	// I set the position of my jellyfishHead to be at [-1.5, 4, -25]
	// with a radius of 6.
	jellyfishHead = new Body();
	jellyfishHead->SetPos(Vec3(-1.5, 4, -25));
	jellyfishHead->SetRadius(6.0f);

	// I copied this stuff from the assignment doc
	// but you don't need the modelmatrix stuff
	const int numAnchors = 10;
	Vec3 anchorPos(-6.0f, 0.0f, -25);
	for (int i = 0; i < numAnchors; i++) {
		anchors.push_back(new Body());
		anchors[i]->pos = anchorPos;
		anchors[i]->SetRadius(0.5f);
		// Move the anchor position for the next swing through this loop
		anchorPos += Vec3(spacing, 0, 0);
	}

	// Umer will attempt one tentacle
	// TODO for YOU (part 1 of assignment)
	// Set up all 10 tentacles
	for (int i = 0; i < numSpheresPerAnchor; i++) {
		tentacleSpheres.push_back(new Body());
		// I learned this trick from Daniel to get the latest one pushed onto the list
		tentacleSpheres.back()->pos = anchors[0]->pos + Vec3(0, -spacing * (i+1), 0);
		tentacleSpheres.back()->SetRadius(0.2f);

	}

	sphereMesh = new Mesh("meshes/Sphere.obj");
	sphereMesh->OnCreate();

	shader = new Shader("shaders/defaultVert.glsl", "shaders/colourFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	// Set the camera's position
	cameraOffset = Vec3(0, -4, 30);
	camera.pos = jellyfishHead->GetPos() + cameraOffset;

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

void Scene3p::OnDestroy() {
	Debug::Info("Deleting assets Scene0g: ", __FILE__, __LINE__);
	jellyfishHead->OnDestroy();
	delete jellyfishHead;

	sphereMesh->OnDestroy();
	delete sphereMesh;

	shader->OnDestroy();
	delete shader;

	// Delete all the memory in the vector list of anchors
	for (Body* anchor : anchors) {
		delete anchor;
	}
	anchors.clear();

	for (Body* tentacleSphere : tentacleSpheres) {
		delete tentacleSphere;
	}
	tentacleSpheres.clear();
}

void Scene3p::HandleEvents(const SDL_Event& sdlEvent) {
	// Send the event to the trackball
	trackball.HandleEvents(sdlEvent);

	float deltaPos = 1.0f;
	switch (sdlEvent.type) {
	case SDL_EVENT_KEY_DOWN:
		switch (sdlEvent.key.scancode) {
		case SDL_SCANCODE_W:
			drawInWireMode = !drawInWireMode;
			break;
		case SDL_SCANCODE_LEFT:
		{
			jellyfishHead->pos.x -= deltaPos;
			anchors[0]->pos.x -= deltaPos;
		}
		break;
		case SDL_SCANCODE_RIGHT:
		{
			jellyfishHead->pos.x += deltaPos;
			anchors[0]->pos.x += deltaPos;
		}
		break;
		case SDL_SCANCODE_DOWN:
		{
			jellyfishHead->pos.y -= deltaPos;
			anchors[0]->pos.y -= deltaPos;
		}
		break;
		case SDL_SCANCODE_UP:
		{
			jellyfishHead->pos.y += deltaPos;
			anchors[0]->pos.y += deltaPos;
		}
		break;
		case SDL_SCANCODE_I:
		{
			jellyfishHead->pos.z += deltaPos;
			anchors[0]->pos.z += deltaPos;
		}
		break;
		case SDL_SCANCODE_P:
		{
			jellyfishHead->pos.z -= deltaPos;
			anchors[0]->pos.z -= deltaPos;
		}
		break;
		// TODO for YOU
		// Code the UP and DOWN key and something for IN and OUT 

		case SDL_SCANCODE_SPACE:
		{
		}
		break;

		default:
			break;
		}
		break;
	}
}

void Scene3p::Update(const float deltaTime) {
	Vec3 gravAcc = Vec3(0, -1.0f, 0);
	// Umer needs an index to refer to the tentacle above
	for (int i = 0; i < tentacleSpheres.size(); i++) {
		// Fg = m * g
		Vec3 gravForce = tentacleSpheres[i]->GetMass() * gravAcc;
		// F_drag = -dragCoeff * vel
		float dragCoeff = 0.5f;
		Vec3 dragForce = -dragCoeff * tentacleSpheres[i]->GetVelocity();

		tentacleSpheres[i]->ApplyForce(gravForce + dragForce);
		tentacleSpheres[i]->UpdateVelocity(deltaTime);
		// Do the constraint to correct the velocity
		//float slope = 2;
		//float yIntercept = 3;
		//tentacleSpheres[i]->StraightLineConstraint(slope, yIntercept, deltaTime);

		//float a = 2;
		//float b = -4;
		//float c = 1;
		//tentacleSpheres[i]->QuadraticConstraint(a, b, c, deltaTime);

		//tentacleSpheres[i]->UpdatePosition(deltaTime);
	}

	// Do the circle constraint for one tentacle sphere
	{
		Vec3 circleCentre = anchors[0]->pos;
		float radius = spacing;
		tentacleSpheres[0]->CircleConstraint(circleCentre, radius, deltaTime);
	}

	for (int i = 0; i < tentacleSpheres.size(); i++) {
		tentacleSpheres[i]->UpdatePosition(deltaTime);

		
	}

	

	// Umer is making a orbit camera based on his scribbles on the board
	// Fixed now thanks to Daniel
	// Rotate the offset
	// First figure out the change in orientation
	/*
	cameraOrientationOld = cameraOrientation;
	cameraOrientation = trackball.getQuat();
	Quaternion changeInOrientation
		= cameraOrientation * QMath::inverse(cameraOrientationOld);
	// Then rotate the cameraOffset
	cameraOffset = QMath::rotate(cameraOffset, changeInOrientation);
	// Then use the offset
	camera.pos = jellyfishHead->GetPos() + cameraOffset;
	*/

	// Rebuild the camera's view matrix
	// Based on position and orientation
	// Scott demands we go back to the origin
	// AND THEN look back down -z
	//viewMatrix =
		//MMath::toMatrix4(QMath::inverse(cameraOrientation)) *
		//MMath::translate(-cameraPos);
}

void Scene3p::Render() const {
	/// Set the background color then clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glDisable(GL_DEPTH_TEST);
	//camera.Render();
	//glEnable(GL_DEPTH_TEST);

	if (drawInWireMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glUseProgram(shader->GetProgram());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, camera.GetViewMatrix());

	//                r   g     b  alpha (we dont use alpha just yet)
	Vec4 pink = Vec4(242, 124, 177, 0) / 255.0f;
	Vec4 darkerPink = Vec4(219, 8, 98, 0) / 255.0f;

	glUniform4fv(shader->GetUniformID("color"), 1, pink);
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, jellyfishHead->GetModelMatrix());
	sphereMesh->Render(GL_TRIANGLES);

	glUniform4fv(shader->GetUniformID("color"), 1, darkerPink);
	for (Body* anchor : anchors) {
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, anchor->GetModelMatrix());
		sphereMesh->Render(GL_TRIANGLES);
	}

	glUniform4fv(shader->GetUniformID("color"), 1, pink);
	for (Body* tentacleSphere : tentacleSpheres) {
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, tentacleSphere->GetModelMatrix());
		sphereMesh->Render(GL_TRIANGLES);
	}

	glUseProgram(0);
}