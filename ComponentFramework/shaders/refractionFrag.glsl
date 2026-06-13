#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 uvwCoords;
layout(location = 1) in vec3 refractionDir;

layout(location = 0) out vec4 fragColor;

uniform samplerCube skybox;

void main() {
	fragColor = texture(skybox, refractionDir);
}