#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMat;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0)); // from local space to world space
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal; // from local to world
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosLightSpace = lightSpaceMat * vec4(vs_out.FragPos, 1.0); // from world to light
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0); // world to view to perspective
}

/*
��Ե�һ��Դ�����Ĳ�����lightSpaceMat
*/