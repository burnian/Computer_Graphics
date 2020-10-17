#version 330 core
struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

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

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal;
uniform DirLight dirLight;
uniform SpotLight spotLight;

// ƽ�й�
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 towardView){
    vec3 towardLight = normalize(-light.direction);
    // diffuse shading ��۲���λ���޹�
    float diffStrength = max(dot(normal, towardLight), 0.0); // ����ǿ�Ȳ���
    // specular shading ��۲���λ�����
    vec3 reflectDir = reflect(-towardLight, normal);
    float specStrength = pow(max(dot(towardView, reflectDir), 0.0), 32.0);
    // combine results
    vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diffStrength * texture(texture_diffuse1, TexCoords).rgb;
    vec3 specular = light.specular * specStrength * texture(texture_specular1, TexCoords).rgb;
    return ambient + diffuse + specular;
}

// �۹��
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 towardView){
	vec3 towardLight = normalize(light.position - fragPos);
    // diffuse shading
	float diffStrength = max(dot(normal, towardLight), 0.0);
    // specular shading
	vec3 reflectDir = reflect(-towardLight, normal);
	float specStrength = pow(max(dot(towardView, reflectDir), 0.0), 32.0);
	// ����˥��
	float dist = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);
	// ��Ե˥���������ԣ�cosine
	float cosTheta = dot(towardLight, normalize(-light.direction)); // ���ߺ;۹�Ʒ���нǵ�����ֵ
	float epsilon = light.innerCos - light.outerCos;
	float intensity = clamp((cosTheta - light.outerCos) / epsilon, 0.0, 1.0);    
	// combine results
    vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgb * attenuation * intensity;
    vec3 diffuse = light.diffuse * diffStrength * texture(texture_diffuse1, TexCoords).rgb * attenuation * intensity;
    vec3 specular = light.specular * specStrength * texture(texture_specular1, TexCoords).rgb * attenuation * intensity;
    return ambient + diffuse + specular;
}

void main() {
	vec3 norm = normalize(Normal);
	vec3 towardView = normalize(viewPos - FragPos);
	// phase 1: Directional lighting
	vec3 result = CalcDirLight(dirLight, norm, towardView);
	// phase 2: Point lights
	//for(int i = 0; i < NR_POINT_LIGHTS; i++)
	// result += CalcPointLight(pointLights[i], norm, FragPos, towardView);
	// phase 3: Spot light
	// result += CalcSpotLight(spotLight, norm, FragPos, towardView);
	// phase 4: emission �Է���
	//result += texture(material.emission, TexCoords).rgb;

	//float ratio = 1.00 / 1.52;
    //vec3 I = normalize(Position - cameraPos);
    //vec3 R = refract(I, normalize(Normal), ratio);
    //FragColor = vec4(texture(skybox, R).rgb, 1.0);

	FragColor = vec4(result, 1.0);
}