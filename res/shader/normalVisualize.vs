#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vec3 normal = mat3(transpose(inverse(view * model))) * aNormal;
    vs_out.normal = normalize(vec3(projection * vec4(normal, 0.0)));
}