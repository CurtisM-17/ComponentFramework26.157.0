#pragma once
#include "Body.h"
// Collision stuff is just a bunch of functions
// So we don't need a class. I'll just have a namespace
// I got this idea from the book "Beautiful C++: 30 Core Guidelines for Writing 
// Clean, Safe, and Fast Code" 

namespace COLLISION {

	bool detection(const Body& a, const Body& b);

	// I will change the velocities of ball a and b (so no const keyword)
	bool response(Body& a, Body& b);
}