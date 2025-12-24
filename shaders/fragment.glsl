#version 330 core

in vec3 vertexColor;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D tex0;

void main() {
    FragColor = vec4(vertexColor, 1.0f);
    // FragColor = texture(tex0, texCoord);
}