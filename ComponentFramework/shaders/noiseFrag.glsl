#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 uvwCoords;
layout(location = 1) in vec3 refractionDir;
layout(location = 2) in vec3 reflectionDir;
layout(location = 3) in vec3 vertexDir;
layout(location = 4) in vec3 normalCameraSpace;

layout(location = 0) out vec4 fragColor;

uniform samplerCube skybox;

void main() {
	vec4 refractColor = texture(skybox, refractionDir);
	vec4 reflectColor = texture(skybox, reflectionDir);

	float F0 = 0.12;
	float hDotV = clamp(dot(normalCameraSpace, vertexDir), 0.0, 1.0); // use a clamp instead of max for safety

	// learnopengl.com says the fresnel equation is F0 + (1-F0)(1-(h DOT v))^5
	float fresnel = F0 + (1.0 - F0) * pow(1.0 - hDotV, 5.0);

	fragColor = mix(refractColor, reflectColor, fresnel);
}