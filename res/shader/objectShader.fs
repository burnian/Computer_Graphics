#version 330 core
in vec3 FragPos;
in vec3 Normal; 

out vec4 FragColor;
  
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightRay = normalize(lightPos - FragPos);
	float diffuseStrength = max(dot(lightRay, norm), 0.0);
	vec3 diffuse = diffuseStrength * lightColor;

    FragColor = vec4((ambient + diffuse) * objectColor, 1.0);
}