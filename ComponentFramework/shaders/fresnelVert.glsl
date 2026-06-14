#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vVertex;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 uvCoord;

layout(location = 0) uniform mat4 projectionMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 modelMatrix;

// Send 3d texture coordinates over to the fragment
layout(location = 0) out vec3 refractionDir;
layout(location = 1) out vec3 reflectionDir;
layout(location = 2) out vec3 vertexDir;
layout(location = 3) out vec3 normalCameraSpace;


void main() {
	//uvwCoords = vVertex;
    //outUVWCoord.y *= -1;
    
    // inVertex is model space (or Paul Neale)
    // modelMatrix * vec4(inVertex, 1.0) is in world space
    // viewMatrix * modelMatrix * vec4(inVertex, 1.0) is camera space
    vertexDir = normalize(vec3(viewMatrix * modelMatrix * vec4(vVertex, 1.0)));
    // Work out vector from camera to vertex

    // This next move will mess up if the object has been rotated
    // BEWARE, you need to do this weird move to kill translation and fix any stretching
    normalCameraSpace = normalize(mat3(inverse(transpose(viewMatrix * modelMatrix))) * vNormal);

    //// REFRACTION
    // Need the indices of refraction (from material A to B)
    float indexA = 1.0f;
    float indexB = 1.5f;
    // Need the ratio of the two (usually called eta)
    // Thanks Miguel, it looks like it's indexA over indexB
    float eta = indexA / indexB;
    // Bend the light!
    refractionDir = refract(vertexDir, normalCameraSpace, eta);
    reflectionDir = reflect(vertexDir, normalCameraSpace);

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vVertex, 1.0);
}