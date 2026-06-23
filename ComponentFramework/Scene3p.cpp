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
	Vec3 anchorPos(-6.0f, 0.0f, -25.0f);
	for (int i = 0; i < numAnchors; i++) {
		anchors.push_back(new Body());
		anchors[i]->pos = anchorPos;
		anchors[i]->SetRadius(0.5f);
		// Move the anchor position for the next swing through this loop
		anchorPos += Vec3(spacing, 0, 0);

		// Set up all 10 tentacles
		for (int j = 0; j < numSpheresPerAnchor; j++) {
			Body* sphere = new Body();

			// I learned this trick from Daniel to get the latest one pushed onto the list
			sphere->pos = anchors[i]->pos + Vec3(0, -spacing * (j + 1), 0);
			sphere->SetRadius(0.2f);

			tentacleSpheres.push_back(sphere);
		}
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
		tentacleSphere = nullptr;
	}
	tentacleSpheres.clear();
}

// Move anchors with sphere
void Scene3p::MoveAnchors(Vec3 movement) { 
	for (int i = 0; i < anchors.size(); i++) {
		anchors[i]->pos += movement;
	}
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
			MoveAnchors(Vec3(-deltaPos, 0, 0));
		}
		break;
		case SDL_SCANCODE_RIGHT:
		{
			jellyfishHead->pos.x += deltaPos;
			MoveAnchors(Vec3(deltaPos, 0, 0));
		}
		break;
		case SDL_SCANCODE_DOWN:
		{
			jellyfishHead->pos.y -= deltaPos;
			MoveAnchors(Vec3(0, -deltaPos, 0));
		}
		break;
		case SDL_SCANCODE_UP:
		{
			jellyfishHead->pos.y += deltaPos;
			MoveAnchors(Vec3(0, deltaPos, 0));
		}
		break;
		case SDL_SCANCODE_I:
		{
			jellyfishHead->pos.z += deltaPos;
			MoveAnchors(Vec3(0, 0, deltaPos));
		}
		break;
		case SDL_SCANCODE_P:
		{
			jellyfishHead->pos.z -= deltaPos;
			MoveAnchors(Vec3(0, 0, -deltaPos));
		}
		break;

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
	const static Vec3 gravAcc = Vec3(0, -30.0f, 0);
	const static float dragCoeff = 20.5f;
	
	for (int anchor = 0; anchor < anchors.size(); anchor++) {
		for (int i = 0; i < numSpheresPerAnchor; i++) { // each sphere in this anchor
			int sphereIndex = anchor * numSpheresPerAnchor + i; // anchor 0 is 0-9 but anchor 1 is 10-19 and so on
			Body* sphere = tentacleSpheres[sphereIndex];

			// Fg = m * g
			Vec3 gravForce = sphere->GetMass() * gravAcc;
			// F_drag = -dragCoeff * vel
			Vec3 dragForce = -dragCoeff * sphere->GetVelocity();

			//if (VMath::mag(sphere->GetVelocity()) > 1.0f) {
				// Switch to turbulent flow if the spheres are moving fast
				// -cv^2
				// Keeps the constraint stable when the spheres whip around
				//dragForce = -dragCoeff * sphere->GetVelocity() * VMath::mag(sphere->GetVelocity());
			//}

			sphere->ApplyForce(gravForce + dragForce);
			sphere->UpdateVelocity(deltaTime);

			Vec3 anchorPoint;
			if (i == 0) {
				// First sphere in the tentacle (hanging from the anchor)
				anchorPoint = anchors[anchor]->pos;
			}
			else {
				// Every other sphere hangs from the sphere above it
				int parentIndex = anchor * numSpheresPerAnchor + (i - 1);
				anchorPoint = tentacleSpheres[parentIndex]->pos;
			}

			// Rod Constraint
			sphere->RodConstraint(anchorPoint, spacing, deltaTime);

			// Do the constraint to correct the velocity
			//float slope = 2;
			//float yIntercept = 3;
			//tentacleSpheres[i]->StraightLineConstraint(slope, yIntercept, deltaTime);

			//float a = 2;
			//float b = -4;
			//float c = 1;
			//tentacleSpheres[i]->QuadraticConstraint(a, b, c, deltaTime);

			sphere->UpdatePosition(deltaTime);
		}
	}

	//if (true) return;

	/*
	// Do the circle constraint for one tentacle sphere
	{
		Vec3 circleCentre = anchors[0]->pos;
		float radius = spacing;
		tentacleSpheres[0]->CircleConstraint(circleCentre, radius, deltaTime);
	}

	for (int i = 0; i < tentacleSpheres.size(); i++) {
		tentacleSpheres[i]->UpdatePosition(deltaTime);

		
	}
	*/
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