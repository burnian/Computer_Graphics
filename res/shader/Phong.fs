#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 direction;
	float innerEdge; // cosine
	float outerEdge; // cosine

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
	// 环境光
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	// 自发光
	//vec3 emission = texture(material.emission, TexCoords).rgb;
	// 聚光灯
	vec3 spotColor = vec3(0.0);
	vec3 towardLight = normalize(light.position - FragPos);
	float theta = dot(towardLight, normalize(-light.direction)); // 光线和聚光灯方向夹角
	if(theta > light.outerEdge) { // 亮部
		// 漫反射 与观察者位置无关
		vec3 norm = normalize(Normal);
		float diffuseStrength = max(dot(towardLight, norm), 0.0); // 光源角参数
		vec3 diffuse = light.diffuse * diffuseStrength * texture(material.diffuse, TexCoords).rgb;
		// 镜面反射 与观察者位置相关，这里使用view space进行计算
		vec3 towardView = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-towardLight, norm);
		float temp = pow(max(dot(towardView, reflectDir), 0.0), material.shininess); // 观察角参数
		vec3 specular = light.specular * temp * texture(material.specular, TexCoords).rgb;
		// 距离衰减
		float d = length(light.position - FragPos);
		float attenuation = 1.0 / (light.constant + light.linear * d + light.quadratic * d * d);

		if(theta < light.innerEdge){
			// 边缘衰减 非线性，cosine
			float epsilon = light.innerEdge - light.outerEdge;
			float intensity = clamp((theta - light.outerEdge) / epsilon, 0.0, 1.0);    
			diffuse *= intensity;
			specular *= intensity;
		}
		spotColor = (diffuse + specular) * attenuation;
	}

	FragColor = vec4(ambient + spotColor, 1.0);
}

/*
冯着色算法对光照的计算都是在片元着色器中进行的。
观察结果：模型表面的高光分布自然，呈光源的形状。
优势：模型所有的插值中间点的颜色都是基于物理光照来计算的，而不是顶点颜色的插值，更加真实自然。
劣势：需要对大量中间点进行光照计算。
*/