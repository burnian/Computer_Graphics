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
	// 环境光
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	// 漫反射 与观察者位置无关
	vec3 norm = normalize(Normal);
	vec3 towardLight = normalize(LightPos - FragPos);
	float diffuseStrength = max(dot(towardLight, norm), 0.0); // 光源角参数
	vec3 diffuse = light.diffuse * diffuseStrength * texture(material.diffuse, TexCoords).rgb;
	// 镜面反射 与观察者位置相关
	vec3 towardView = normalize(-FragPos);
	vec3 reflectDir = reflect(-towardLight, norm);
	float temp = pow(max(dot(towardView, reflectDir), 0.0), material.shininess); // 观察角参数
	vec3 specular = light.specular * temp * texture(material.specular, TexCoords).rgb;
	// 自发光
	vec3 emission = texture(material.emission, TexCoords).rgb;

	FragColor = vec4(ambient + diffuse + specular + emission, 1.0);
}

/*
冯着色算法对光照的计算都是在片元着色器中进行的。
观察结果：模型表面的高光分布自然，呈光源的形状。
优势：模型所有的插值中间点的颜色都是基于物理光照来计算的，而不是顶点颜色的插值，更加真实自然。
劣势：需要对大量中间点进行光照计算。
*/