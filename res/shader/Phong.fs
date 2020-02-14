#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal; 
in vec3 LightPos;

out vec4 FragColor;

uniform Material material;
uniform Light light; 

void main() {
	// ������
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	// ������ ��۲���λ���޹�
	vec3 norm = normalize(Normal);
	vec3 towardLight = normalize(LightPos - FragPos);
	float diffuseStrength = max(dot(towardLight, norm), 0.0); // ��Դ�ǲ���
	vec3 diffuse = light.diffuse * diffuseStrength * texture(material.diffuse, TexCoords).rgb;
	// ���淴�� ��۲���λ�����
	vec3 towardView = normalize(-FragPos);
	vec3 reflectDir = reflect(-towardLight, norm);
	float temp = pow(max(dot(towardView, reflectDir), 0.0), material.shininess); // �۲�ǲ���
	vec3 specular = light.specular * temp * texture(material.specular, TexCoords).rgb;
	// �Է���
	vec3 emission = texture(material.emission, TexCoords).rgb;

	FragColor = vec4(ambient + diffuse + specular + emission, 1.0);
}

/*
����ɫ�㷨�Թ��յļ��㶼����ƬԪ��ɫ���н��еġ�
�۲�����ģ�ͱ���ĸ߹�ֲ���Ȼ���ʹ�Դ����״��
���ƣ�ģ�����еĲ�ֵ�м�����ɫ���ǻ����������������ģ������Ƕ�����ɫ�Ĳ�ֵ��������ʵ��Ȼ��
���ƣ���Ҫ�Դ����м����й��ռ��㡣
*/