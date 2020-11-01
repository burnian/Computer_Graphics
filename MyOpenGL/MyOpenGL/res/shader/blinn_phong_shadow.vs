#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 FragWPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragLightSpacePos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMat; // world to light to light's perspective transformation matrix

void main()
{
    vs_out.FragWPos = vec3(model * vec4(aPos, 1.0)); // local space to world space
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal; // local to world
    vs_out.TexCoords = aTexCoords;
    vs_out.FragLightSpacePos = lightSpaceMat * vec4(vs_out.FragWPos, 1.0); // world to light's perspective
    gl_Position = projection * view * vec4(vs_out.FragWPos, 1.0); // world to viewer to viewer's perspective
}

/*
for single light source, the key parameter is lightSpaceMat
*/