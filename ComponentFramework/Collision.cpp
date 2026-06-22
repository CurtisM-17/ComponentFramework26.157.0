#include "Collision.h"
#include "Scene2p.h"

bool COLLISION::detection(const Body& a, const Body& b) {
	// TODO for part 1
	// Remember game physics 1? Collision detection between circles
	// Work out the distance between sphere a and b
	// And compare it to radius of a + radius of b
	// if distance > radius of a + radius of b then return false

	float distance = VMath::mag(a.GetPos() - b.GetPos());

	return distance <= (a.GetRadius() + b.GetRadius());
}


bool COLLISION::response(Body& a, Body& b) {
	// TODO for part 2
	// Follow Umer's scribbles in the assignment doc

	// STEP 1
	// Find the collision normal
	Vec3 normal = VMath::normalize(b.GetPos() - a.GetPos());

	// STEP 2
	// Find the relative velocity along the normal
	float vrel = VMath::dot(a.GetVelocity() - b.GetVelocity(), normal); // (ball a vel - ball b vel) DOT normal

	if (vrel < 0) return false; // Ignore

	// STEP 3
	// Set the bounciness parameter
	float epsilon = 1.0f;

	// STEP 4
	// Calculate the impulse (big J eqn)
	float J = -(1 + epsilon) * vrel / (1.0f / a.GetMass() + 1 / b.GetMass());

	// LAST STEP
	// Update the velocities
	//ball a vel = ball a vel + J * normal / mass_a
	// ball b vel = ball b vel - J * normal / mass_b
	a.SetVelocity(a.GetVelocity() + J * normal / a.GetMass());
	b.SetVelocity(b.GetVelocity() - J * normal / b.GetMass());
	
	return true;
}