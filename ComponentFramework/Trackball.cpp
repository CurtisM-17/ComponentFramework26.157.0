#include <glew.h>
#include <SDL.h>
#include "Trackball.h"
#include <VMath.h>
#include <QMath.h>

///https://www.khronos.org/opengl/wiki/Object_Mouse_Trackball
using namespace MATH;

Trackball::Trackball() {
	setWindowDimensions();
	mouseDown = false;
}

Trackball::~Trackball() {}

void Trackball::setWindowDimensions() {
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	invNDC = MMath::inverse(MMath::NDCtoViewport(viewport[2], viewport[3]));
}

void Trackball::HandleEvents(const SDL_Event& sdlEvent) {
	if (sdlEvent.type == SDL_EventType::SDL_EVENT_MOUSE_BUTTON_DOWN) {
		onLeftMouseDown(sdlEvent.button.x, sdlEvent.button.y);
	}
	else if (sdlEvent.type == SDL_EventType::SDL_EVENT_MOUSE_BUTTON_UP) {
		onLeftMouseUp(sdlEvent.button.x, sdlEvent.button.y);
	}
	else if (sdlEvent.type == SDL_EventType::SDL_EVENT_MOUSE_MOTION &&
		SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) {
		onMouseMove(sdlEvent.button.x, sdlEvent.button.y);
	}
	else if (sdlEvent.window.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
		setWindowDimensions();
	}
}

void Trackball::onLeftMouseDown(int x, int y) {
	beginV = getMouseVector(x, y);
	// Keep track of the quat before we start rotating
	prevQuat = mouseRotationQuat;
	mouseDown = true;
}

void Trackball::onLeftMouseUp(int x, int y) {
	mouseDown = false;
}

void Trackball::onMouseMove(int x, int y) {
	if (mouseDown == false) return;
	endV = getMouseVector(x, y);
	float cosAngle = VMath::dot(beginV, endV);
	if (cosAngle <= VERY_SMALL) {
		//printf("%f\n", cosAngle);
	}
	float angle = acos(cosAngle) * 180.0f / FLOAT_PI; /// acos() returns radians must convert to degrees
	Vec3 rotAxis = VMath::cross(beginV, endV);
	// Umer's quick hack to remove the roll (doesn't work 100%)
	rotAxis.z = 0;
	// Umer getting out of here if rotAxis is (0, 0, 0)
	if (VMath::mag(rotAxis) < VERY_SMALL) {
		return;
	}
	Quaternion delta = QMath::angleAxisRotation(angle, rotAxis); // UN - Songho used the name "delta" quaternion. I like that as it's the change in rotation

	// Umer trying another idea
	float factor = 100;
	Quaternion pitch = QMath::angleAxisRotation(factor * (beginV.y - endV.y), Vec3(-1, 0, 0));
	Quaternion yaw = QMath::angleAxisRotation(factor * (beginV.x - endV.x), Vec3(0, 1, 0));
	delta = pitch * yaw;
	// delta = yaw;

	//delta.print();

	mouseRotationQuat = prevQuat * delta; // UN - Spent a day realizing that order of multiplication is important here
}

///https://www.khronos.org/opengl/wiki/Object_Mouse_Trackball
const Vec3 Trackball::getMouseVector(int x, int y) {
	Vec3 mousePosition(static_cast<float>(x), static_cast<float>(y), 0.0f);
	Vec3 v = invNDC * mousePosition;
	float xSquared = v.x * v.x;
	float ySquared = v.y * v.y;
	if (xSquared + ySquared <= 0.5f) { /// see reference (1.0f*1.0f / 2.0f) 1.0 is the radius of the sphere
		/// if it's the sphere
		v.z = sqrt(1.0f - (xSquared + ySquared));
	}
	else {
		/// else it's the hyperbolic sheet
		v.z = 0.5f / sqrt(xSquared + ySquared);
		v = VMath::normalize(v);
	}
	return v;
}