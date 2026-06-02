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

	Quaternion orientation;
	Vec3 angularVel, angularAcc;
	float radius;
private: /// Graphics stuff 
	Mesh *mesh;
	Texture *texture;
	
public:
	Vec3 pos;

	bool OnCreate();
	void OnDestroy();
	void Update(float deltaTime);
	void Render() const;
	void ApplyForce(Vec3 force);
	void ApplyTorque(Vec3 torque);
	void SetAccel(const Vec3 &accel_) { accel = accel_;}
	Quaternion GetOrientation() const { return orientation; }
	void SetOrientation(const Quaternion newValue) { orientation = newValue; }
	void UpdateOrientation(float deltaTime);
	void UpdateAngularVelocity(float deltaTime);
	void UpdateVelocity(float deltaTime);
	void UpdatePosition(float deltaTime);
	Vec3 GetAngularVelocity() { return angularVel; }
	void SetAngularVelocity(const Vec3 newValue) { angularVel = newValue; }
	void SetAngularAcceleration(const Vec3 newValue) {angularAcc = newValue; }
	void SetRadius(float newValue) { radius = newValue; }
	float GetRadius() const { return radius; }
	Vec3 GetPos() const { return pos; }
	void SetPos(const Vec3 newValue) { pos = newValue; }
	Matrix4 GetModelMatrix() const;
	void SetVelocity(const Vec3 newVal) { vel = newVal; }
	Vec3 GetVelocity() { return vel; }
	void SetMass(float mass_) {
		// Blow up if mass is zero or less
		assert(mass_ > VERY_SMALL);
		mass = mass_;
	}
	float GetMass() const {
		return mass;
	}
	void StraightLineConstraint(float slope, float yIntercept, float deltaTime);
	void QuadraticConstraint(float a, float b, float c, float deltaTime);
};

#endif
