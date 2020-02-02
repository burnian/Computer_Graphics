#version 330 core
in vec3 FragPos;
in vec3 Normal; 
in vec3 LightPos;

out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
	// 环境光
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
	// 漫反射
	vec3 norm = normalize(Normal);
	vec3 towardLight = normalize(LightPos - FragPos);
	float diffuseStrength = max(dot(towardLight, norm), 0.0);
	vec3 diffuse = diffuseStrength * lightColor;
	// 镜面反射
	float specularStrength = 0.5;
	int shininess = 32;
	vec3 towardView = normalize(-FragPos);
	vec3 reflectDir = reflect(-towardLight, norm);
	float temp = pow(max(dot(towardView, reflectDir), 0.0), shininess);
	vec3 specular = specularStrength * temp * lightColor;

	FragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
}