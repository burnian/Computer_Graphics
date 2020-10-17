#version 330 core
struct Material {
    sampler2D texture_diffuse1; // Ĭ��ֵΪ0����Ϊֻ��һ��diffuse��������ֻ��texture_diffuse1�����еڶ��ţ��������texture_diffuse2
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_height1;
};
uniform Material material;

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform samplerCube skybox;

// ƽ�й�
vec3 CalcDirLight(DirLight light, vec3 norm, vec3 frag2view){
    vec3 frag2light = normalize(-light.direction);
    // diffuse shading ��۲���λ���޹�
    float diffStrength = max(dot(norm, frag2light), 0.0); // ����ǿ�Ȳ��������dot��������cross
    // specular shading ��۲���λ�����
    vec3 reflectDir = reflect(-frag2light, norm);
    float specStrength = pow(max(dot(frag2view, reflectDir), 0.0), 32.0);
    // combine results
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diffStrength * texture(material.texture_diffuse1, TexCoords).rgb;
    vec3 specular = light.specular * specStrength * texture(material.texture_specular1, TexCoords).rgb;
    return ambient + diffuse + specular;
}

void main() {
	vec3 norm = normalize(Normal);
	vec3 frag2view = normalize(viewPos - FragPos);
	vec3 result = CalcDirLight(dirLight, norm, frag2view);
	// �ֲ��߹�
	result += texture(material.texture_height1, TexCoords).rgb * texture(skybox, reflect(-frag2view, norm)).rgb;

	FragColor = vec4(result, 1.0);
}