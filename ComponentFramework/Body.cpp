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

void Body::UpdateVelocity(float deltaTime) {
	vel += accel * deltaTime;
}

void Body::UpdatePosition(float deltaTime) {
	// Ignore accel for our constraint
	pos += vel * deltaTime;
}

Matrix4 Body::GetModelMatrix() const {
	Matrix4 T = MMath::translate(pos);
	Matrix4 R = MMath::toMatrix4(orientation);
	Matrix4 S = MMath::scale(radius, radius, radius);
	return T * R * S;
}

void Body::StraightLineConstraint(float slope, float yIntercept, float deltaTime) {
	// Code up Umer's scribbles
	float positionConstraint = pos.y - slope * pos.x - yIntercept;
	// JV is the velocity constraint
	float JV = vel.y - slope * vel.x;
	// JT is the Jacobian transposed. Remember Scott's vectors are columns
	// JT is the direction to punch
	Vec3 JT = Vec3(-slope, 1, 0);
	// beta is a constant for the bias term
	float beta = 0.1;
	// Abort if deltaTime = 0
	if (deltaTime < VERY_SMALL) return;
	float bias = beta * positionConstraint / deltaTime;

	// Jacobian multiplied with its transpose
	float JJT = (slope * slope) + 1;

	// Lagrangian is how much to punch by
	float lagrangian = mass * (-JV - bias) / JJT;

	// Finally update velocity
	vel += JT * lagrangian / mass;
}

void Body::QuadraticConstraint(float a, float b, float c, float deltaTime) {
	// Code up Umer's scribbles
	float positionConstraint = pos.y - a * pos.x * pos.x - b * pos.x - c;
	// JV is the velocity constraint
	float JV = vel.y - a * 2 * pos.x * vel.x - b * vel.x;
	// JT is the Jacobian transposed. Remember Scott's vectors are columns
	// JT is the direction to punch
	Vec3 JT = Vec3(-a * 2 * pos.x - b, 1, 0);
	// beta is a constant for the bias term
	float beta = 0.01;
	// Abort if deltaTime = 0
	if (deltaTime < VERY_SMALL) return;
	float bias = beta * positionConstraint / deltaTime;

	// Jacobian multiplied with its transpose
	float JJT = (-a * 2 * pos.x - b) * (-a * 2 * pos.x - b) + 1;

	// Lagrangian is how much to punch by
	float lagrangian = mass * (-JV - bias) / JJT;

	// Finally update velocity
	vel += JT * lagrangian / mass;
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

