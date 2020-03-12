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
	float innerCos; // 内圈和方向向量的夹角余弦值
	float outerCos; // 外圈和方向向量的夹角余弦值
    
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

// 平行光
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 towardView){
    vec3 towardLight = normalize(-light.direction);
    // diffuse shading 与观察者位置无关
    float diffStrength = max(dot(normal, towardLight), 0.0); // 光照强度参数
    // specular shading 与观察者位置相关
    vec3 reflectDir = reflect(-towardLight, normal);
    float specStrength = pow(max(dot(towardView, reflectDir), 0.0), 32.0);
    // combine results
    vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diffStrength * texture(texture_diffuse1, TexCoords).rgb;
    vec3 specular = light.specular * specStrength * texture(texture_specular1, TexCoords).rgb;
    return ambient + diffuse + specular;
}

// 聚光灯
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 towardView){
	vec3 towardLight = normalize(light.position - fragPos);
    // diffuse shading
	float diffStrength = max(dot(normal, towardLight), 0.0);
    // specular shading
	vec3 reflectDir = reflect(-towardLight, normal);
	float specStrength = pow(max(dot(towardView, reflectDir), 0.0), 32.0);
	// 距离衰减
	float dist = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);
	// 边缘衰减，非线性，cosine
	float cosTheta = dot(towardLight, normalize(-light.direction)); // 光线和聚光灯方向夹角的余弦值
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
	// phase 4: emission 自发光
	//result += texture(material.emission, TexCoords).rgb;

	//float ratio = 1.00 / 1.52;
    //vec3 I = normalize(Position - cameraPos);
    //vec3 R = refract(I, normalize(Normal), ratio);
    //FragColor = vec4(texture(skybox, R).rgb, 1.0);

	FragColor = vec4(result, 1.0);
}