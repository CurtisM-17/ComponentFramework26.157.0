#ifndef BODY_H
#define BODY_H
#include <Vector.h> /// This is in GameDev
#include "Quaternion.h"
#include <assert.h>

using namespace MATH; 

/// Just forward declair these classes so I can define a pointer to them
/// Used later in the course
class Mesh;
class Texture;

class Body {
public:
    Body();
    ~Body();
private: /// Physics stuff
	Vec3 vel;
	Vec3 accel;
	float mass;

	Matrix3 rotInertia;
	Quaternion orientation;
	Vec3 angularVel, angularAcc;
	float radius;
	Vec3 color;
private: /// Graphics stuff 
	Mesh *mesh;
	Texture *texture;
	
public:
	Vec3 pos;

	bool OnCreate();
	void OnDestroy();
	void Update(float deltaTime);
	void Render() const;

	///////////// Getters /////////////
	Vec3 GetAccel() const { return accel; }
	Quaternion GetOrientation() const { return orientation; }
	Vec3 GetAngularVelocity() const { return angularVel; }
	Vec3 GetAngularAcceleration() const { return angularVel; }
	float GetRadius() const { return radius; }
	Vec3 GetPos() const { return pos; }
	// Build a model matrix to send to the GPU
	Matrix4 GetModelMatrix() const; // done in Body.cpp, no setter needed (pos * orientation * scale)
	Vec3 GetVelocity() { return vel; }
	float GetMass() const { return mass; }
	Vec3 GetColor() const { return Vec4(color.x, color.y, color.z, 255.0f) / 255.0f; }

	///////////// Setters /////////////
	void SetAccel(const Vec3& accel_) { accel = accel_; }
	void SetOrientation(const Quaternion newValue) { orientation = newValue; }
	void SetAngularVelocity(const Vec3 newValue) { angularVel = newValue; }
	void SetAngularAcceleration(const Vec3 newValue) { angularAcc = newValue; }
	void SetRadius(float newValue) { radius = newValue; }
	void SetPos(const Vec3 newValue) { pos = newValue; }
	void SetVelocity(const Vec3 newVal) { vel = newVal; }
	void SetMass(float mass_) {
		// Blow up if mass is zero or less
		assert(mass_ > VERY_SMALL);
		mass = mass_;
	}
	void SetColor(float r, float g, float b) { color = Vec3(r, g, b); }
	void SetColor(Vec3 _color) { color = _color; }

	///////////// Apply force /////////////
	void ApplyForce(Vec3 force);
	// Updates angular acceleration using torque and rotational inertia
	void ApplyTorque(Vec3 torque);

	///////////// Updates /////////////
	// Updates orientation quaternion using the angular velocity
	void UpdateOrientation(float deltaTime);
	// Updates angular velocity using rotational equations of motion 
	void UpdateAngularVelocity(float deltaTime);
	void UpdateVelocity(float deltaTime);
	// Update position or velocity using the equations of motion. It's handy
	// later on with constraints to have these as separate methods
	void UpdatePosition(float deltaTime);
	
	///////////// Constraints /////////////
	void StraightLineConstraint(float slope, float yIntercept, float deltaTime);
	void QuadraticConstraint(float a, float b, float c, float deltaTime);
	void CircleConstraint(Vec3 circleCentre, float radius, float deltaTime);
	void RodConstraint(Vec3 anchorPoint, float rodLength, float deltaTime);
};

#endif
