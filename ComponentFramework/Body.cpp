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
	radius{1.0f},
	rotInertia(),
	orientation(),
	angularVel(),
	angularAcc()
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

void Body::ApplyForce(Vec3 force) {
	accel = force / mass;
}

void Body::ApplyTorque(Vec3 torque) {
	// Update the angular acceleration using the inverse rotational inertia matrix multiplied by the torque vector
	// Apply torque to the ball and get it to spin
	// update angular acceleration
	// angularAcc = inverse of rationInertia * torque
	float i_edge = ((2 / 5) * mass * radius * radius) + (mass * radius * radius);
	rotInertia = Matrix3(
		i_edge,	 0,    0,
		  0,   i_edge, 0,
		  0,     0,  i_edge
	);

	SetAngularAcceleration(MMath::inverse(rotInertia) * torque);
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

void Body::CircleConstraint(Vec3 circleCentre, float radius, float deltaTime) {
	// Code up Umer's scribbles
	float positionConstraint =
		(pos.x - circleCentre.x) * (pos.x - circleCentre.x)
		+ (pos.y - circleCentre.y) * (pos.y - circleCentre.y)
		- radius * radius;
	// JV is the velocity constraint
	float JV =
		2 * pos.x * vel.x + 2 * pos.y * vel.y
		- 2 * circleCentre.x * vel.x - 2 * circleCentre.y * vel.y;
	// JT is the Jacobian transposed. Remember Scott's vectors are columns
	// JT is the direction to punch
	Vec3 JT = Vec3(
		-2 * circleCentre.x + 2 * pos.x,
		-2 * circleCentre.y + 2 * pos.y,
		0); // TODO for YOU. Work this out for a sphere

	// beta is a constant for the bias term
	float beta = 0.1;
	// Abort if deltaTime = 0
	if (deltaTime < VERY_SMALL) return;
	float bias = beta * positionConstraint / deltaTime;

	// Jacobian multiplied with its transpose
	// Umer is using pow here instead to do a square
	float JJT =
		pow(-2 * circleCentre.x + 2 * pos.x, 2)
		+ pow(-2 * circleCentre.y + 2 * pos.y, 2);

	// Lagrangian is how much to punch by
	float lagrangian = mass * (-JV - bias) / JJT;

	// Finally update velocity
	vel += JT * lagrangian / mass;
}

bool Body::OnCreate() {
	return true;
}

void Body::OnDestroy() {
}

void Body::Render() const {
}

