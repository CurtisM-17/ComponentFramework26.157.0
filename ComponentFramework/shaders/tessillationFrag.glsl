in vec2 uvCoordFromEval;
in vec3 normalFromEval;
out vec4 fragColor;
uniform sampler2D textureData;
void main() {
    ///fragColor = texture(textureData,uvCoordFromEval);
    fragColor = vec4(1.0,1.0,1.0,1.0);
}
