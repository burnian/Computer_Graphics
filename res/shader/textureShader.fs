#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;
uniform sampler2D backTexture;

void main()
{
	//FragColor = vec4(vec3(gl_FragCoord.z), 1.0); // ��ƽ��ͷ���е����ֵ
    //if(texColor.a < 0.1) discard;

	//if(gl_FragCoord.x < 640)
    //    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //else
    //    FragColor = texture(texture1, TexCoords);

	// ��ǰƬԪ�������滹�Ǳ���
	if(gl_FrontFacing)
        FragColor = texture(texture1, TexCoords);
    else
        FragColor = texture(backTexture, TexCoords);
}