#include "Collision.h"

bool COLLISION::detection(const Body& a, const Body& b) {
	// TODO for part 1
	// Remember game physics 1? Collision detection between circles
	// Work out the distance between sphere a and b
	// And compare it to radius of a + radius of b
	// if distance > radius of a + radius of b then return false

	return true;
}


void COLLISION::response(Body& a, Body& b) {
	// TODO for part 2
	// Follow Umer's scribbles in the assignment doc

	// STEP 1
	// Find the collision normal
	Vec3 normal; // normalize(ball b pos - ball a pos) 

	// STEP 2
	// Find the relative velocity along the normal
	float vrel = 0; // (ball a vel - ball b vel) DOT normal

	// STEP 3
	// Set the bounciness parameter
	float epsilon = 1.0f;

	// STEP 4
	// Calculate the impulse (big J eqn)
	float J = 0; // -(1 + epsilon) * vrel / (1/mass_a + 1/mass_b)

	// LAST STEP
	// Update the velocities
	// ball a vel = ball a vel + J * normal / mass_a
	// ball b vel = ball b vel - J * normal / mass_b
}