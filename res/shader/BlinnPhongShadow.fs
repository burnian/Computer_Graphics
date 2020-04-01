#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess; // 64
}; 
uniform Material material;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

uniform sampler2D depthMap;
uniform vec3 viewPos;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

out vec4 FragColor;

// ��Ӱ���㣬�����е����궼��light space�µ�
float ShadowCalculation(vec4 fragPosLightSpace, DirLight light, vec3 normal) {
    // perform perspective divide������ͶӰ��Ϊwֵû���޸ģ����Բ���Ҫ��w������͸��ͶӰ��Ҫ��
	// ��ƬԪͶӰ����ת��ΪNDC����[-w,w]->[-1,1]
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// [-1,1]->[0,1]
	projCoords = projCoords * 0.5 + 0.5;
	// ��֮ǰ��light space�¼����������Ӱ������ͼ������������ֵ�����в���
	// ������ͼ�е�ֵ����ɢ�ģ����Ծ��Դ��Զ��ƬԪ���ܲ�������ͬ����С���ֵ�������꣬���Խ��ƿ������������ע��
	float closestDepth = texture(depthMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
    vec3 towardLight = normalize(-light.direction);
	float bias = max(0.05 * (1.0 - dot(normal, towardLight)), 0.005);  

	return currentDepth - bias > closestDepth ? 0.0 : 1.0;
}

// ƽ�й�
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 towardView, float shadow){
    vec3 towardLight = normalize(-light.direction);
    // diffuse shading ��۲���λ���޹�
    float diffStrength = max(dot(normal, towardLight), 0.0); // ����ǿ�Ȳ���
    // specular shading ��۲���λ����أ������Phong ��ͬ
	vec3 halfwayDir = normalize(towardLight + towardView);
	float specStrength = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * texture(material.diffuse, fs_in.TexCoords).rgb;
    vec3 diffuse = light.diffuse * diffStrength * texture(material.diffuse, fs_in.TexCoords).rgb;
    vec3 specular = light.specular * specStrength * texture(material.specular, fs_in.TexCoords).rgb;
    return ambient + shadow * (diffuse + specular);
}


void main() {
	vec3 norm = normalize(fs_in.Normal);
	vec3 towardView = normalize(viewPos - fs_in.FragPos);
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, dirLight, norm);
	// phase 1: Directional lighting
	vec3 result = CalcDirLight(dirLight, norm, towardView, shadow);
    
    FragColor = vec4(result, 1.0);
}