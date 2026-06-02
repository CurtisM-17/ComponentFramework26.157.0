#version 450
#extension GL_ARB_separate_shader_objects : enable

uniform samplerCube skybox;
in vec3 uvwCoords;

layout(location = 0) out vec4 fragColor;

void main() {
	fragColor = texture(skybox, uvwCoords);
}