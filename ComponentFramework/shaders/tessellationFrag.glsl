#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 uvCoordFromEval;
layout(location = 1) in vec3 normalFromEval;
layout(location = 2) in vec3 eyeDir;
layout(location = 3) in vec3 lightDir;

out vec4 fragColor;

layout(binding=0) uniform sampler2D heightData;
layout(binding=1) uniform sampler2D normalData;
layout(binding=2) uniform sampler2D diffuseData;

void main() {
	vec4 ks = vec4(0.3, 0.3, 0.3, 0.3);
	vec4 kd = vec4(0.4, 0.4, 0.4, 0.4);
	vec4 ka = 0.5 * kd;

	vec3 normalFromMap = 2.0 * vec3(texture(normalData, uvCoordFromEval)) - vec3(1.0, 1.0, 1.0);
	normalFromMap = normalize(normalFromMap);
	float diff = max(dot(normalFromMap, lightDir), 0.0);

	/// Reflection is based incedent direction which means a vector from the light source
	/// not the direction to the light source
	vec3 reflection = normalize(reflect(-lightDir, normalFromMap));
	float spec = max(dot(eyeDir, reflection), 0.0);
	spec = pow(spec, 14.0); // shininess

	vec4 textureColor = texture(diffuseData, uvCoordFromEval);
	//fragColor =  ka + (diff * kd) + (spec * ks) * textureColor;

	vec3 color = 1.5 * textureColor.rgb * (ka.rgb + diff * kd.rgb) + (spec * ks.rgb);

	fragColor = vec4(color, textureColor.a);
	//fragColor = vec4(0.5, 0.5, 0.5, 0.5);
	//fragColor = textureColor;
}
