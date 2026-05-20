#include "Body.h"
#include <QMath.h>
#include <MMath.h>
Body::Body() : 
	pos{},
	vel{},
	accel{},
	mass{ 1.0f },
	mesh{ nullptr },
	texture{ nullptr },
	radius{1.0f}
{}

Body::~Body() {}

void Body::Update(float deltaTime) {
	/// From 1st semester physics class
	pos += vel * deltaTime + 0.5f * accel * deltaTime * deltaTime;
	vel += accel * deltaTime;
}

void Body::UpdateOrientation(float deltaTime) {
	float angleInRadiansPerSec = VMath::mag(angularVel);
	float angleRadians = angleInRadiansPerSec * deltaTime;
	float angleDegrees = angleRadians * RADIANS_TO_DEGREES;

	if (angleDegrees < VERY_SMALL) return;
	Vec3 axisOfRotation = VMath::normalize(angularVel);

	Quaternion rotation = QMath::angleAxisRotation(angleDegrees, axisOfRotation);
	orientation *= rotation;
}

void Body::UpdateAngularVelocity(float deltaTime) {
	angularVel += angularAcc * deltaTime;
}

Matrix4 Body::GetModelMatrix() const
{
	Matrix4 T = MMath::translate(pos);
	Matrix4 R = MMath::toMatrix4(orientation);
	Matrix4 S = MMath::scale(radius, radius, radius);
	return T * R * S;
}

void Body::ApplyForce(Vec3 force) {
	accel = force / mass;
}

void Body::ApplyTorque(Vec3 torque) {
	// Part 2: Apply torque to the ball and get it to spin
	Matrix3 rotationIntertia; // reference assignment document
	// update angular acceleration
	// angularAcc = inverse of rationInertia * torque
	// 
}

bool Body::OnCreate() {
	return true;
}

void Body::OnDestroy() {
}

void Body::Render() const {
}

