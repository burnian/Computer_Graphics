#version 330 core
struct Material {
    sampler2D texture_diffuse1; // 默认值为0，因为只有一张diffuse纹理，所以只有texture_diffuse1，若有第二张，则需添加texture_diffuse2
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