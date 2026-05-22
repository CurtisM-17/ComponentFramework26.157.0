#pragma once
#include "Body.h"

namespace COLLISION {

	bool detection(const Body& a, const Body& b) {
		// TODO for part 1
		// Remember game physics 1? Collision detection between circles
		// Work out the distance between sphere a and b
		// And compare it to radius of a + radius of b
		// if distance > radius of a + radius of b then return false
	}

	// I will change the velocities of ball a and b (so no const keyword)
	void response(Body& a, Body& b) {
		
	}

}