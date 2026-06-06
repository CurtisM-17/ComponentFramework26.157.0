#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vVertex;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUVCoord;

layout(location = 0) uniform mat4 projectionMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 modelMatrix;

// Send 3d texture coordinates over to the fragment
layout(location = 0) out vec3 uvwCoords;
layout(location = 1) out vec3 reflectionDir;

void main() {
    uvwCoords = vVertex;
    //outUVWCoord.y *= -1;

    // inVertex is model space (or Paul Neale)
    // modelMatrix * vec4(inVertex, 1.0) is in world space
    // viewMatrix * modelMatrix * vec4(inVertex, 1.0) is camera space
    vec3 vertexPosCameraSpace = vec3(viewMatrix * modelMatrix * vec4(vVertex, 1.0));
    vec3 cameraPosCameraSpace = vec3(0, 0, 0);
    // Work out vector from camera to vertex
    vec3 vertexDir = vertexPosCameraSpace - cameraPosCameraSpace; // Umer really doesnt need to subtract by (0, 0 ,0)

    // This next move will mess up if the object has been rotated
    // BEWARE, you need to do this weird move to kill translation and fix any stretching
    vec3 normalCameraSpace = mat3(inverse(transpose(viewMatrix * modelMatrix))) * inNormal;
    reflectionDir = reflect(vertexDir, normalCameraSpace);


    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vVertex, 1.0);
}