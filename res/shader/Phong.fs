#version 330 core
in vec3 WorldPos;
in vec3 Normal; 

out vec4 FragColor;
  
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

void main() {
	// 环境光
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
	// 漫反射
	vec3 norm = normalize(Normal);
	vec3 towardLight = normalize(lightPos - WorldPos);
	float diffuseStrength = max(dot(towardLight, norm), 0.0);
	vec3 diffuse = diffuseStrength * lightColor;
	// 镜面反射
	float specularStrength = 0.5;
	int shininess = 32;
	vec3 towardView = normalize(viewPos - WorldPos);
	vec3 reflectDir = reflect(-towardLight, norm);
	float temp = pow(max(dot(towardView, reflectDir), 0.0), shininess);
	vec3 specular = specularStrength * temp * lightColor;

	FragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
}