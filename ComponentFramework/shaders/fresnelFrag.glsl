#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vertexDir;
layout(location = 1) in vec3 refractionDir;
layout(location = 2) in vec3 reflectionDir;
layout(location = 3) in vec3 normalCameraSpace;

layout(location = 0) out vec4 fragColor;

uniform samplerCube skybox;

void main() {
	vec4 refractColor = texture(skybox, refractionDir);
	vec4 reflectColor = texture(skybox, reflectionDir);

	vec3 vertDir = normalize(vertexDir);

	fragColor = mix(refractColor, reflectColor, max(dot(normalCameraSpace, -vertDir), 0.0));
}