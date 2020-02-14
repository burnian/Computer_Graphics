#version 330 core
layout(location = 0) in vec3 aPos;// 位置变量的属性位置值为 0
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoords = aTexCoords;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}

// 计算尽量往 vs 里放，不要放 fs 中，坐标或向量名中没有特别提示一律按world space 处理