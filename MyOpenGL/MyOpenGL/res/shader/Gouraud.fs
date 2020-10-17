#version 330 core
in vec3 vertexColor;

out vec4 FragColor;

uniform vec3 objectColor;

void main() {
	FragColor = vec4(vertexColor * objectColor, 1.0);
}

// vertexColor 的值是根据 vs 计算出的三角形对应的三个顶点的颜色值插值出来的