#version 330 core
out vec4 FragColor;// 输出的颜色向量

//in vec3 ourColor;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float scale;
uniform float show;

void main() {
    vec2 offset = vec2(0.5f, 0.5f);
    vec2 coord = (texCoord - offset) * scale + offset;
    FragColor = mix(texture(texture1, coord), texture(texture2, coord), show); // vec4(ourColor, 1.0)乘到texture就是给纹理施加颜色，vec2(1.0f - coord.x, coord.y)纹理沿垂线翻转
}