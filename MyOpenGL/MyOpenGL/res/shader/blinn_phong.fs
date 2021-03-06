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
	float innerCos; // 内圈和方向向量的夹角余弦值
	float outerCos; // 外圈和方向向量的夹角余弦值
    
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

// directional light
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 towardView){
    vec3 towardLight = normalize(-light.direction);
    // diffuse shading is independent of viewer's position
    float diffStrength = max(dot(normal, towardLight), 0.0);
    // specular shading is related to the viewer's position, which is different with Phong
	vec3 halfwayDir = normalize(towardLight + towardView);
	float specStrength = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diffStrength * texture(material.diffuse, TexCoords).rgb;
    vec3 specular = light.specular * specStrength * texture(material.specular, TexCoords).rgb;
    return ambient + diffuse + specular;
}

// point light
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 towardView) {
    vec3 towardLight = normalize(light.position - fragPos);
    // diffuse shading is independent of viewer's position
    float diffStrength = max(dot(normal, towardLight), 0.0);
	// specular shading is related to the viewer position, which is different with Phong
	vec3 halfwayDir = normalize(towardLight + towardView);
	float specStrength = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // distance attenuation
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);    
    // combine results
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb * attenuation;
    vec3 diffuse = light.diffuse * diffStrength * texture(material.diffuse, TexCoords).rgb * attenuation;
    vec3 specular = light.specular * specStrength * texture(material.specular, TexCoords).rgb * attenuation;
    return ambient + diffuse + specular;
}

// spot light
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 towardView){
	vec3 towardLight = normalize(light.position - fragPos);
    // diffuse shading is independent of viewer's position
	float diffStrength = max(dot(normal, towardLight), 0.0);
	// specular shading is related to the viewer position, which is different with Phong
	vec3 halfwayDir = normalize(towardLight + towardView);
	float specStrength = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // distance attenuation
	float dist = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);
	// non-linear edge attenuation, in cosine type
	float cosTheta = dot(towardLight, normalize(-light.direction));
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
	// phase 1: directional light
	vec3 result = CalcDirLight(dirLight, norm, towardView);
	// phase 2: point lights
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLights[i], norm, FragPos, towardView);
	// phase 3: spot light
	if(spotLightOn)
		result += CalcSpotLight(spotLight, norm, FragPos, towardView);
	// phase 4: emission, self-luminous
	//result += texture(material.emission, TexCoords).rgb;

	FragColor = vec4(result, 1.0);
}

/*
布林-冯着色算法和冯着色算法的主要差异就在于高光部分，布林-冯是用角平分线向量和normal取点积，冯是出射向量和观察向量取点积
观察结果：模型表面的高光分布比冯更自然，冯在远处高光为0，而布林-冯则处处大于0。
优势：模型所有的插值中间点的颜色都是基于物理光照来计算的，而不是顶点颜色的插值，更加真实自然。
劣势：需要对大量中间点进行光照计算。
*/