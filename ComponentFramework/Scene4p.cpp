#include <glew.h>
#include <iostream>
#include <SDL.h>
#include <SDL3/SDL_events.h>
#include "Scene4p.h"
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include "Quaternion.h"
#include <QMath.h>
#include <DQMath.h>

using namespace MATHEX;

Scene4p::Scene4p() :
	shader{nullptr},
	sphereMesh{nullptr},
	drawInWireMode{true},
	target{ nullptr },
	skeletonPlane(),
	angle(),
	forearmLength(),
	joints(),
	upperarmLength()
{
	Debug::Info("Created Scene4p: ", __FILE__, __LINE__);
}

Scene4p::~Scene4p() {
	Debug::Info("Deleted Scene4p: ", __FILE__, __LINE__);
}

bool Scene4p::OnCreate() {
	Debug::Info("Loading assets Scene4p: ", __FILE__, __LINE__);

	// Umer's last quiz
	Vec4 v = Vec4(0, -1, 0, 0);
	DualQuat L = DualQuat(0, 0, 0, 1, 0, 0, 0, 0);
	Vec4 newDir = ((L * v) * DQMath::inverse(L)).point;




	target = new Body();
	target->SetRadius(0.1f);
	target->pos = Vec3(3, 3, 0);


	for (int i = 0; i < joints.size(); i++) {
		joints[i] = new Body();
		joints[i]->SetRadius(0.1f);
		joints[i]->pos = Vec3(0, 3, 0);
	}

	joints[0]->SetRadius(0.4f);

	joints[0]->pos = Vec3(0, 4, 0); // head
	joints[1]->pos = Vec3(0, 3, 0); //neck
	joints[2]->pos = Vec3(0, 1, 0); //pelvis

	// left arm
	joints[3]->pos = Vec3(1, 3, 0); // shoulder
	joints[4]->pos = Vec3(1.5, 2, 0); //elbow
	joints[5]->pos = Vec3(1, 1, 0); // hand

	forearmLength = VMath::distance(joints[5]->pos, joints[4]->pos);
	upperarmLength = VMath::distance(joints[4]->pos, joints[3]->pos);

	// Make the plane where the skeleton lives
	// Can join three points to make a plane
	DualQuat lineHeadToNeck = join(Vec4(joints[0]->pos, 1), Vec4(joints[1]->pos, 1));
	// Join this with the shoulder to make a plane
	skeletonPlane = join(lineHeadToNeck, Vec4(joints[3]->pos, 1));

	sphereMesh = new Mesh("meshes/Sphere.obj");
	sphereMesh->OnCreate();

	shader = new Shader("shaders/defaultVert.glsl", "shaders/colourFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	cameraPos = Vec3(0, 0, 12);
	
	return true;
}

void Scene4p::OnDestroy() {
	Debug::Info("Deleting assets Scene0g: ", __FILE__, __LINE__);

	target->OnDestroy();
	delete target;

	for (Body* joint : joints) {
		joint->OnDestroy();
		delete joint;
	}

	sphereMesh->OnDestroy();
	delete sphereMesh;

	shader->OnDestroy();
	delete shader;
}

void Scene4p::HandleEvents(const SDL_Event& sdlEvent) {
	// Send the event to the trackball
	trackball.HandleEvents(sdlEvent);

	float deltaPos = 0.5f;
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
	case SDL_EVENT_MOUSE_MOTION:
		// Get the mouse position
		Vec4 mousePosPixelSpace = Vec4(sdlEvent.button.x, sdlEvent.button.y, 0, 1);

		Matrix4 NDCtoPixelMatrix = MMath::NDCtoViewport(1280, 720);
		Matrix4 pixelToNDCMatrix = MMath::inverse(NDCtoPixelMatrix);

		Vec4 mousePosNDCSpace = pixelToNDCMatrix * mousePosPixelSpace;
		// I want the position on the front face of the box
		mousePosNDCSpace.z = -1;

		// To get into camera space 
		// use the inverse projection matrix
		// THEN
		// divide out the w (perspective divide)

		Vec4 mousPosCameraSpace = MMath::inverse(projectionMatrix) * mousePosNDCSpace;
		// Don't forget to divide out the w
		mousPosCameraSpace = VMath::perspectiveDivide(mousPosCameraSpace);

		Matrix4 cameraToWorldMatrix = MMath::inverse(viewMatrix);
		Vec4 mousePosWorldSpace = cameraToWorldMatrix * mousPosCameraSpace;

		// Don't do this
		//target->pos = Vec3(mousePosWorldSpace);
		
		// Make a line through the camera position and the mouse position at the front plane
		DualQuat line = join(Vec4(cameraPos, 1.0f), mousePosWorldSpace);
		Vec4 target4d = meet(line, skeletonPlane);
		//target->pos = VMath::perspectiveDivide(target4d);
		
		break;
	}
}

void Scene4p::Update(const float deltaTime) {
	// left arm
	// joints[3]->pos // shoulder
	// joints[4]->pos // elbow
	// joints[5]->pos // hand

	// Let's do the backwards part of FABRIK
	// Step one, put the hand on the target
	joints[5]->pos = target->pos;

	// Step two. Stick the elbow on the hand
	// And translate along a line from hand to elbow
	// by the amount forearmLength
	DualQuat lineHandToElbow = join(Vec4(joints[5]->pos, 1), Vec4(joints[4]->pos, 1));
	joints[4]->pos = joints[5]->pos;
	DualQuat translatorHandToElbow = DQMath::translateAlongLine(forearmLength, lineHandToElbow);
	// Next step is the sandwich
	Vec4 elbowPos4d = DQMath::rigidTransformation(translatorHandToElbow, Vec4(joints[4]->pos, 1));
	// Just in case the w is messed up, divide it out
	joints[4]->pos = VMath::perspectiveDivide(elbowPos4d);

	// Now for the forward part of FABRIK
	// Step three. Make a line from shoulder to elbow
	// Stick the elbow on the shoulder
	// translate along the line by upperarmLength
	DualQuat lineShoulderToElbow = join(Vec4(joints[3]->pos, 1), Vec4(joints[4]->pos, 1));
	joints[4]->pos = joints[3]->pos;
	DualQuat translatorShoulderToElbow = DQMath::translateAlongLine(upperarmLength, lineShoulderToElbow);
	elbowPos4d = DQMath::rigidTransformation(translatorShoulderToElbow, Vec4(joints[4]->pos, 1));
	joints[4]->pos = VMath::perspectiveDivide(elbowPos4d);

	// Step four. Do the same thing for the hand
	// Make a line from elbow to hand
	DualQuat lineElbowToHand = join(Vec4(joints[4]->pos, 1), Vec4(joints[5]->pos, 1));
	// Stick the hand on the elbow
	joints[5]->pos = joints[4]->pos;
	// Translate by the forearm length
	DualQuat translatorElbowToHand = DQMath::translateAlongLine(forearmLength, lineElbowToHand);
	// Sandwich time!
	Vec4 handPos4d = DQMath::rigidTransformation(translatorElbowToHand, Vec4(joints[5]->pos, 1));
	// Divide out the w
	joints[5]->pos = VMath::perspectiveDivide(handPos4d);

	// sine wave
	totalTime += deltaTime;
	static float angularVelocity = 1.0f;
	angle = angularVelocity * totalTime;

	target->pos.x = sin(angle + 90.0f * DEGREES_TO_RADIANS) + 3;
	target->pos.y = sin(angle) + 3;

	viewMatrix = 
		MMath::toMatrix4(QMath::inverse(cameraOrientation)) *
		MMath::translate(-cameraPos);
}

void Scene4p::Render() const {
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

	//                r   g     b  alpha (we dont use alpha just yet)
	Vec4 pink = Vec4(237, 130, 210, 0) / 255.0f;
	Vec4 darkerPink = Vec4(227, 43, 181, 0) / 255.0f;

	glUniform4fv(shader->GetUniformID("color"), 1, darkerPink);
	for (Body* joint : joints) {
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, joint->GetModelMatrix());
		sphereMesh->Render(GL_TRIANGLES);
	}

	// Rendering the mouse target position
	/*
	Vec4 white = Vec4(1, 1, 1, 0);
	glUniform4fv(shader->GetUniformID("color"), 1, white);
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, target->GetModelMatrix());
	sphereMesh->Render(GL_TRIANGLES);
	*/

	glUseProgram(0);
}



	
