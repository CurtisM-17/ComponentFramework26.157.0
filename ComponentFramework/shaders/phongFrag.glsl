#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec3 vertNormal;
layout(location = 1) in vec3 lightDir;
layout(location = 2) in vec3 eyeDir; 

void main() {
	vec4 ks = vec4(0.3, 0.3, 0.3, 0.0);
	vec4 kd = vec4(0.75, 0.0, 0.0, 0.0);
	vec4 ka = 0.1 * kd;
	float diff = max(dot(vertNormal, lightDir), 0.0);

	/// Reflection is based incedent direction which means a vector from the light source
	/// not the direction to the light source
	vec3 reflection = normalize(reflect(-lightDir, vertNormal));
	float spec = max(dot(eyeDir, reflection), 0.0);
	spec = pow(spec,14.0); // shininess
	//fragColor =  ka + (diff * kd) + (spec * ks);
	fragColor =  ka + (diff * kd) + (spec * ks);
}