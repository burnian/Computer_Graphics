#version 330 core
#define NR_POINT_LIGHTS 4

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
}; 
uniform Material material;

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight {
    vec3 position;
    vec3 direction;
	float innerCos; // ��Ȧ�ͷ��������ļн�����ֵ
	float outerCos; // ��Ȧ�ͷ��������ļн�����ֵ
    
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform SpotLight spotLight;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 viewPos;
uniform bool spotLightOn;

// ƽ�й�
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 towardView){
    vec3 towardLight = normalize(-light.direction);
    // diffuse shading ��۲���λ���޹�
    float diffStrength = max(dot(normal, towardLight), 0.0); // ����ǿ�Ȳ���
    // specular shading ��۲���λ����أ������Phong ��ͬ
	vec3 halfwayDir = normalize(towardLight + towardView);
	float specStrength = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diffStrength * texture(material.diffuse, TexCoords).rgb;
    vec3 specular = light.specular * specStrength * texture(material.specular, TexCoords).rgb;
    return ambient + diffuse + specular;
}

// ���Դ
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 towardView) {
    vec3 towardLight = normalize(light.position - fragPos);
    // diffuse shading
    float diffStrength = max(dot(normal, towardLight), 0.0);
	// specular shading ��۲���λ����أ������Phong ��ͬ
	vec3 halfwayDir = normalize(towardLight + towardView);
	float specStrength = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // ����˥��
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);    
    // combine results
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb * attenuation;
    vec3 diffuse = light.diffuse * diffStrength * texture(material.diffuse, TexCoords).rgb * attenuation;
    vec3 specular = light.specular * specStrength * texture(material.specular, TexCoords).rgb * attenuation;
    return ambient + diffuse + specular;
}

// �۹��
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 towardView){
	vec3 towardLight = normalize(light.position - fragPos);
    // diffuse shading
	float diffStrength = max(dot(normal, towardLight), 0.0);
	// specular shading ��۲���λ����أ������Phong ��ͬ
	vec3 halfwayDir = normalize(towardLight + towardView);
	float specStrength = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	// ����˥��
	float dist = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);
	// ��Ե˥���������ԣ�cosine
	float cosTheta = dot(towardLight, normalize(-light.direction)); // ���ߺ;۹�Ʒ���нǵ�����ֵ
	float epsilon = light.innerCos - light.outerCos;
	float intensity = clamp((cosTheta - light.outerCos) / epsilon, 0.0, 1.0);    
	// combine results
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb * attenuation * intensity;
    vec3 diffuse = light.diffuse * diffStrength * texture(material.diffuse, TexCoords).rgb * attenuation * intensity;
    vec3 specular = light.specular * specStrength * texture(material.specular, TexCoords).rgb * attenuation * intensity;
    return ambient + diffuse + specular;
}

void main() {
	vec3 norm = normalize(Normal);
	vec3 towardView = normalize(viewPos - FragPos);
	// phase 1: Directional lighting
	vec3 result = CalcDirLight(dirLight, norm, towardView);
	// phase 2: Point lights
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLights[i], norm, FragPos, towardView);
	// phase 3: Spot light
	if(spotLightOn)
		result += CalcSpotLight(spotLight, norm, FragPos, towardView);
	// phase 4: emission �Է���
	//result += texture(material.emission, TexCoords).rgb;

	FragColor = vec4(result, 1.0);
}

/*
����-����ɫ�㷨�ͷ���ɫ�㷨����Ҫ��������ڸ߹ⲿ�֣�����-�����ý�ƽ����������normalȡ��������ǳ��������͹۲�����ȡ���
�۲�����ģ�ͱ���ĸ߹�ֲ��ȷ����Ȼ������Զ���߹�Ϊ0��������-���򴦴�����0��
���ƣ�ģ�����еĲ�ֵ�м�����ɫ���ǻ����������������ģ������Ƕ�����ɫ�Ĳ�ֵ��������ʵ��Ȼ��
���ƣ���Ҫ�Դ����м����й��ռ��㡣
*/