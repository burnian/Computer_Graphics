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

    float constant;
    float linear;
    float quadratic;
};

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main() {
	// ������
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	// ������ ��۲���λ���޹�
	vec3 norm = normalize(Normal);
	vec3 towardLight = normalize(light.position - FragPos);
	float diffuseStrength = max(dot(towardLight, norm), 0.0); // ��Դ�ǲ���
	vec3 diffuse = light.diffuse * diffuseStrength * texture(material.diffuse, TexCoords).rgb;
	// ���淴�� ��۲���λ����أ�����ʹ��view space���м���
	vec3 towardView = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-towardLight, norm);
	float temp = pow(max(dot(towardView, reflectDir), 0.0), material.shininess); // �۲�ǲ���
	vec3 specular = light.specular * temp * texture(material.specular, TexCoords).rgb;
	// �Է���
	//vec3 emission = texture(material.emission, TexCoords).rgb;
	// ˥��
	float d = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * d + light.quadratic * d * d);

	FragColor = vec4((ambient + diffuse + specular) * attenuation, 1.0);
}

/*
����ɫ�㷨�Թ��յļ��㶼����ƬԪ��ɫ���н��еġ�
�۲�����ģ�ͱ���ĸ߹�ֲ���Ȼ���ʹ�Դ����״��
���ƣ�ģ�����еĲ�ֵ�м�����ɫ���ǻ����������������ģ������Ƕ�����ɫ�Ĳ�ֵ��������ʵ��Ȼ��
���ƣ���Ҫ�Դ����м����й��ռ��㡣
*/