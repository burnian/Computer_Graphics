#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
	//FragColor = vec4(vec3(gl_FragCoord.z), 1.0); // 在平截头体中的深度值
    FragColor = texture(texture1, TexCoords);
}