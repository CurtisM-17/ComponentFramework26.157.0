#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fragColor;
uniform sampler2D diffuseTexture;
layout(location = 0) in vec3 vertNormal;
layout(location = 1) in vec3 lightDir[6];
layout(location = 7) in vec3 eyeDir; 
layout(location = 8) in vec2 textureCoords;

uniform int lightCount;
uniform vec4 lightColors[];

void main() {
	vec4 ks = vec4(0.3, 0.3, 0.3, 0.0);
	vec4 kd = vec4(1.0, 1.0, 1.0, 0.0);
	vec4 ka = 0.1 * kd;

	vec4 lighting = ka; // because phong says ambient is before the sum

	for (int i = 0; i < lightCount; i++) {
        float diff = max(dot(vertNormal, lightDir[i]), 0.0);

		/// Reflection is based incedent direction which means a vector from the light source
		/// not the direction to the light source
        vec3 reflection = normalize(reflect(-lightDir[i], vertNormal));
        float spec = max(dot(eyeDir, reflection), 0.0);
		spec = pow(spec,14.0); // shininess

		float intensity = 1.5;
		lighting += intensity * (diff * kd + spec * ks);
    }
	
	vec4 textureColor = texture(diffuseTexture, textureCoords);
	fragColor = lighting * textureColor;
}