#include <glew.h>
#include <iostream>
#include <SDL.h>
#include <SDL3/SDL_events.h>
#include "Scene0p.h"
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include "Quaternion.h"
#include <QMath.h>
#include <PMath.h>

Scene0p::Scene0p() :
	plane{ nullptr }
	, cueBall{ nullptr }
	, targetBall{ nullptr }
	, shader{ nullptr }
	, planeMesh{ nullptr }
	, sphereMesh{ nullptr }
	, drawInWireMode{ true } {
	Debug::Info("Created Scene0p: ", __FILE__, __LINE__);
}

Scene0p::~Scene0p() {
	Debug::Info("Deleted Scene0p: ", __FILE__, __LINE__);
}

bool Scene0p::OnCreate() {
	Debug::Info("Loading assets Scene0p: ", __FILE__, __LINE__);
	plane = new Body();
	plane->OnCreate();
	plane->SetRadius(1.0f);

	planeMesh = new Mesh("meshes/Plane.obj");
	planeMesh->OnCreate();

	cueBall = new Body();
	cueBall->OnCreate();
	cueBall->SetAngularVelocity(Vec3(0, 0, 0));
	cueBall->SetAngularAcceleration(Vec3(0, 0, 0));
	cueBall->SetPos(Vec3(0, 1, 0));
	cueBall->SetRadius(1.0f);

	targetBall = new Body();
	targetBall->OnCreate();
	targetBall->SetAngularVelocity(Vec3(0, 0, 0));
	targetBall->SetAngularAcceleration(Vec3(0, 0, 0));
	targetBall->SetPos(Vec3(-1, 1, -5));
	targetBall->SetRadius(1.0f);

	sphereMesh = new Mesh("meshes/Sphere.obj");
	sphereMesh->OnCreate();

	shader = new Shader("shaders/defaultVert.glsl", "shaders/colourFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	viewMatrix = MMath::lookAt(Vec3(0.0f, 3.0f, 10.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	// Testing out quaternions
	Quaternion quatOne = Quaternion(1, Vec3(0, 0, 0));
	Quaternion quat_i = Quaternion(0, Vec3(1, 0, 0));
	Quaternion quat_j = Quaternion(0, Vec3(0, 1, 0));
	Quaternion quat_k = Quaternion(0, Vec3(0, 0, 1));
	// Add quaternions to get the average between them
	Quaternion quat_one_plus_i = quatOne + quat_i;
	// Don't forget to normalize quaternions!
	quat_one_plus_i = QMath::normalize(quat_one_plus_i);

	plane->SetOrientation(quat_one_plus_i); // rotation of 90 degrees about x
	// First three numbers is the normal
	// Last number is distance to the origin
	planeShape.set(0, 1, 0, 0);
	return true;
}

void Scene0p::OnDestroy() {
	Debug::Info("Deleting assets Scene0g: ", __FILE__, __LINE__);
	plane->OnDestroy();
	delete plane;
	planeMesh->OnDestroy();
	delete planeMesh;

	cueBall->OnDestroy();
	delete cueBall;
	targetBall->OnDestroy();
	delete targetBall;
	sphereMesh->OnDestroy();
	delete sphereMesh;

	shader->OnDestroy();
	delete shader;
}

void Scene0p::HandleEvents(const SDL_Event& sdlEvent) {
	float angleDeg = 1.0f;
	switch (sdlEvent.type) {
	case SDL_EVENT_KEY_DOWN:
		switch (sdlEvent.key.scancode) {
		case SDL_SCANCODE_W:
			drawInWireMode = !drawInWireMode;
			break;
		case SDL_SCANCODE_LEFT:
		{
			Quaternion rotation = QMath::angleAxisRotation(angleDeg, Vec3(0, 0, 1));
			// Apply this rotation to the orientation
			// Be careful as order matters
			plane->SetOrientation(rotation * plane->GetOrientation());
			planeShape.n = QMath::rotate(planeShape.n, rotation);
		}
		break;
		case SDL_SCANCODE_RIGHT:
		{
			Quaternion rotation = QMath::angleAxisRotation(angleDeg, Vec3(0, 0, -1));
			plane->SetOrientation(rotation * plane->GetOrientation());
			planeShape.n = QMath::rotate(planeShape.n, rotation);
		}
		break;
		// TODO for YOU
		// Code the UP and DOWN key to rotate the plane towards or away from you
		case SDL_SCANCODE_UP:
		{
			Quaternion rotation = QMath::angleAxisRotation(angleDeg, Vec3(-1, 0, 0));
			plane->SetOrientation(rotation * plane->GetOrientation());
			planeShape.n = QMath::rotate(planeShape.n, rotation);
		}
		break;
		case SDL_SCANCODE_DOWN:
		{
			Quaternion rotation = QMath::angleAxisRotation(angleDeg, Vec3(1, 0, 0));
			plane->SetOrientation(rotation * plane->GetOrientation());
			planeShape.n = QMath::rotate(planeShape.n, rotation);
		}
		break;
		default:
			break;
		}
		break;
	}
}

void Scene0p::Update(const float deltaTime) {
	// We actually want to calculate the torque using the angle of the plane
	float angleRadians = 0; // acos(plane normal DOT up vector)
	float distanceToPivot = 0; // radius * sin(angleRadians)
	float weight = 0; // mass * 9.8
	float torqueMag = weight * distanceToPivot;
	// Umer has revealed how to determine the direction of the torque
	// TODO for YOU
	Vec3 torqueDir; // up CROSS normal (don't forget to normalize after)

	// TODO for YOU for part 2 of assignment
	// Vec3 torque = Vec3(0, 0, 1);

	// TODO for part 3
	// Don't hardcode the torque vector like above
	Vec3 torque = torqueMag * torqueDir;
	cueBall->ApplyTorque(torque);
	cueBall->UpdateAngularVelocity(deltaTime);
	cueBall->UpdateOrientation(deltaTime);
	// TODO for YOU for part 1 of assignment
	// Convert angular velocity into a linear one
	float velMag = 0; // magnitude of angular velocity * radius
	Vec3  velDir; // normalized angular velocity CROSS plane normal vector
	cueBall->SetVelocity(velMag * velDir);
	cueBall->Update(deltaTime);

	// Keep the ball on the plane
	// We will use Scott's PMath to find the distance (instead of Umer's math in the assignment doc)
	float distanceFromPlane = PMath::distance(cueBall->GetPos(), planeShape);
	// TODO for YOU
	// Based on Umer's scribbles
	// Move the ball along the plane normal by the amount (radius - distanceFromPlane)
	// sphere->pos += .....

}

void Scene0p::Render() const {
	/// Set the background color then clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (drawInWireMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glUseProgram(shader->GetProgram());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, viewMatrix);

	//                r  g  b  alpha (we dont use alpha just yet)
	Vec4 green = Vec4(0, 1, 0, 0);
	Vec4 white = Vec4(1, 1, 1, 0);
	Vec4 red = Vec4(1, 0, 0, 0);

	glUniform4fv(shader->GetUniformID("color"), 1, green);
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, plane->GetModelMatrix());
	planeMesh->Render(GL_TRIANGLES);

	glUniform4fv(shader->GetUniformID("color"), 1, red);
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, targetBall->GetModelMatrix());
	sphereMesh->Render(GL_TRIANGLES);

	glUniform4fv(shader->GetUniformID("color"), 1, white);
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, cueBall->GetModelMatrix());
	sphereMesh->Render(GL_TRIANGLES);

	glUseProgram(0);
}




