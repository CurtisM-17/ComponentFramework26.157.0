#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vVertex;

layout(location = 0) uniform mat4 projectionMatrix;
layout(location = 1) uniform mat4 orientationMatrix;

out vec3 uvwCoords;

void main() {
	uvwCoords = vVertex.xyz;

	gl_Position = projectionMatrix * orientationMatrix * vVertex;
}