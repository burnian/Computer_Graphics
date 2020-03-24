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
	//FragColor = vec4(vec3(gl_FragCoord.z), 1.0); // ��ƽ��ͷ���е����ֵ
    //if(texColor.a < 0.1) discard;

	//if(gl_FragCoord.x < 640)
    //    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //else
    //    FragColor = texture(texture1, TexCoords);

	// ��ǰƬԪ�������滹�Ǳ���
	// if(gl_FrontFacing)
    //     FragColor = texture(texture1, TexCoords);
    // else
    //     FragColor = texture(backTexture, TexCoords);
    
	FragColor = texture(material.texture_diffuse1, TexCoords);
}