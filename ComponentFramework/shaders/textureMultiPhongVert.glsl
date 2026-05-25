#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 uvCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightPos[6]; // max lights
uniform int lightCount;

layout(location = 0) out vec3 vertNormal;
layout(location = 1) out vec3 lightDir[6];
layout(location = 7) out vec3 eyeDir; 
layout(location = 8) out vec2 textureCoords;


void main() {
	textureCoords = uvCoord;
	textureCoords.y *= -1.0;
	mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
	vertNormal = normalize(normalMatrix * vNormal); /// Rotate the normal to the correct orientation 
	vec3 vertPos = vec3(modelMatrix * vVertex);
	vec3 vertDir = normalize(vertPos);
	eyeDir = -vertDir;

	// A lot of behaviour broke strangely when I did this and 
	// my lights no longer looked the same, would really like to figure out why

	for (int i = 0; i < lightCount; i++) {
        lightDir[i] = normalize(lightPos[i] - vertPos);
    }

	/// vVertex is in Model space
	/// modelMatrix * vVertex is in World space
	/// viewMatrix * modelMatrix * vVertex is in Camera space
	/// projectionMatrix * viewMatrix * modelMatrix * vVertex is in NDC space
	/// I'll explain spaces in class.
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vVertex;
}