#include <glew.h>
#include <iostream>
#include <SDL.h>
#include <SDL3/SDL_events.h>
#include "Scene1p.h"
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include "Quaternion.h"
#include <PMath.h>

Scene1p::Scene1p() : 
	plane{nullptr}, 
	shader{nullptr}, 
	planeMesh{nullptr},
	sphere{nullptr},
	sphereMesh{nullptr},
	drawInWireMode{true} {
	Debug::Info("Created Scene1: ", __FILE__, __LINE__);
}

Scene1p::~Scene1p() {
	Debug::Info("Deleted Scene1: ", __FILE__, __LINE__);
}

bool Scene1p::OnCreate() {
	Debug::Info("Loading assets Scene1: ", __FILE__, __LINE__);
	
	sphere = new Body();
	sphere->OnCreate();
	sphere->SetRadius(1.0f);
	sphere->SetPos(Vec3(0, sphere->GetRadius(), 0));
	//sphere->SetAngularVelocity(Vec3(0, 0, -1));
	sphereMesh = new Mesh("meshes/Sphere.obj");

	sphereMesh->OnCreate();
	
	plane = new Body();
	plane->OnCreate();
	planeMesh = new Mesh("meshes/Plane.obj");
	planeMesh->OnCreate();

	shader = new Shader("shaders/defaultVert.glsl", "shaders/defaultFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, 10.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	
	// quaternions
	Quaternion quat_1 = Quaternion(1, Vec3(0, 0, 0));
	Quaternion quat_i = Quaternion(0, Vec3(1, 0, 0));
	Quaternion quat_j = Quaternion(0, Vec3(0, 1, 0));
	Quaternion quat_k = Quaternion(0, Vec3(0, 0, 1));

	Quaternion addedQuat = QMath::normalize(quat_1 + quat_i);
	plane->SetOrientation(addedQuat);
	planeShape.set(0, 1, 0, 0);

	return true;
}

void Scene1p::OnDestroy() {
	Debug::Info("Deleting assets Scene1p: ", __FILE__, __LINE__);
	plane->OnDestroy();
	delete plane;

	sphereMesh->OnDestroy();
	delete sphereMesh;

	planeMesh->OnDestroy();
	delete planeMesh;

	shader->OnDestroy();
	delete shader;
}

void Scene1p::HandleEvents(const SDL_Event& sdlEvent) {
	float angleDeg = 1.0f;
	switch (sdlEvent.type) {
	case SDL_EVENT_KEY_DOWN:
		switch (sdlEvent.key.scancode) {
		case SDL_SCANCODE_Q:
			drawInWireMode = !drawInWireMode;
			break;
		case SDL_SCANCODE_A:
		case SDL_SCANCODE_LEFT:
		{
			Quaternion rotation = QMath::angleAxisRotation(angleDeg, Vec3(0, 0, 1));
			// Apply this rotation to the orientation
			// Be careful as order matters
			plane->SetOrientation(rotation * plane->GetOrientation());
			planeShape.n = QMath::rotate(planeShape.n, rotation);
		}
		break;
		case SDL_SCANCODE_D:
		case SDL_SCANCODE_RIGHT:
		{
			Quaternion rotation = QMath::angleAxisRotation(angleDeg, Vec3(0, 0, -1));
			plane->SetOrientation(rotation * plane->GetOrientation());
			planeShape.n = QMath::rotate(planeShape.n, rotation);
		}
		break;
		// TODO for YOU
		// Code the UP and DOWN key to rotate the plane towards or away from you
		case SDL_SCANCODE_W:
		case SDL_SCANCODE_UP:
		{
			Quaternion rotation = QMath::angleAxisRotation(angleDeg, Vec3(-1, 0, 0));
			plane->SetOrientation(rotation * plane->GetOrientation());
			planeShape.n = QMath::rotate(planeShape.n, rotation);
		}
		break;
		case SDL_SCANCODE_S:
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

Vec3 Scene1p::NormOrNothing(const Vec3 v) {
	// doing it this way because Scott's will throw division by zero errors
	float mag = VMath::mag(v);
	Vec3 newVec = v;

	if (mag > VERY_SMALL) {
		// normalize
		// I already calculate the magnitude so may as well just do this here too instead of using the method
		newVec = Vec3(
			newVec.x / mag,
			newVec.y / mag,
			newVec.z / mag
		);
	}

	return newVec;
}

void Scene1p::Update(const float deltaTime) {
	const static Vec3 upVector = Vec3(0, 1, 0);
	float radius = sphere->GetRadius();

	////// Torque //////

	// torque magnitude
	float angleRad = acos(VMath::dot(planeShape.n, upVector)); // acos(plane normal DOT up vector)
	float distToPivot = radius * sin(angleRad); // radius * sin(angleRad)
	float weight = sphere->GetMass() * 9.8f; // mass * 9.8
	float torqueMag = weight * distToPivot;

	// torque direction: surface upvector CROSS surface normal
	Vec3 torqueDir = NormOrNothing(VMath::cross(upVector, planeShape.n));

	// apply the torque
	Vec3 torque = torqueDir * torqueMag;
	sphere->ApplyTorque(torque);

	////// Velocity //////
	// Convert angular velocity to linear velocity
	Vec3 linearVel = VMath::cross(sphere->GetAngularVelocity(), planeShape.n * radius); // angular velocity CROSS rVector
	sphere->SetVelocity(linearVel);

	sphere->UpdateAngularVelocity(deltaTime);
	sphere->UpdateOrientation(deltaTime);
	sphere->Update(deltaTime);

	// Keep the ball on the plane
	float distFromPlane = PMath::distance(sphere->GetPos(), planeShape); // normal DOT point position

	float correction = radius - distFromPlane;

	// Move along plane normal
	sphere->SetPos(sphere->GetPos() + planeShape.n * correction);

	////////////// Rebuild the model matrix //////////////
	// Translate * Rotate * Scale
	Matrix4 translation = MMath::translate(sphere->GetPos());
	Matrix4 rotation = MMath::toMatrix4(sphere->GetOrientation());
	Matrix4 scale = MMath::scale(radius, radius, radius);

	sphereModelMatrix = translation * rotation * scale;

	// Build plane model matrix too. Only a rotation
	planeModelMatrix = MMath::toMatrix4(plane->GetOrientation());
}

void Scene1p::Render() const {
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
	
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, sphereModelMatrix);
	sphereMesh->Render(GL_TRIANGLES);

	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, planeModelMatrix);
	planeMesh->Render(GL_TRIANGLES);

	glUseProgram(0);
}



	
