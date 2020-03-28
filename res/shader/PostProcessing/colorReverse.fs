#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main() { 
	// 颜色翻转特效
    FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
}  