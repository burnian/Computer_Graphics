#version 330 core
struct Material {
    sampler2D texture_diffuse1; // Ĭ��ֵΪ0����Ϊֻ��һ��diffuse��������ֻ��texture_diffuse1�����еڶ��ţ��������texture_diffuse2
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_height1;
};
uniform Material material;

out vec4 FragColor;

in vec2 TexCoords;

void main()
{
    FragColor = texture(material.texture_diffuse1, TexCoords);
}