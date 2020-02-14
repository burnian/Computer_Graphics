#version 330 core
layout(location = 0) in vec3 aPos;// 位置变量的属性位置值为 0
layout(location = 1) in vec3 aNormal;

out vec3 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 lightColor;

void main() {
	vec4 viewModelPos = view * model * vec4(aPos, 1.0);
	gl_Position = projection * viewModelPos;

	vec3 FragPos = vec3(viewModelPos);
	vec3 Normal = normalize(mat3(view * transpose(inverse(model))) * aNormal);

	// 环境光
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

	// 漫反射
	vec3 LightPos = vec3(view * vec4(lightPos, 1.0));
	vec3 towardLight = normalize(LightPos - FragPos);
	float diffuseStrength = max(dot(towardLight, Normal), 0.0);
	vec3 diffuse = diffuseStrength * lightColor;

	// 镜面反射
	float specularStrength = 1.0;
	int shininess = 32;
	vec3 towardView = normalize(-FragPos);
	vec3 reflectDir = reflect(-towardLight, Normal);
	float temp = pow(max(dot(towardView, reflectDir), 0.0), shininess);
	vec3 specular = specularStrength * temp * lightColor;

	vertexColor = ambient + diffuse + specular;
}

/*
高洛德着色算法对光照的计算都是在顶点着色器中进行的。
观察结果：模型表面的高光总是呈三角形分布，而不是光源的形状，有明显的斑纹。
优势：避免了对模型的大量插值中间点进行光照计算。
劣势：所有中间点的颜色都不再是基于物理光照了，而是顶点颜色的插值，显得不自然。
*/