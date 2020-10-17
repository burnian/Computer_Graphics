#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMat;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMat * model * vec4(aPos, 1.0);
}

/*
单纯用来计算对应视角坐标系下的深度贴图
*/