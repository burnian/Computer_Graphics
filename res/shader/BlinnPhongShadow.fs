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

// 阴影计算，函数中的坐标都是light space下的
float ShadowCalculation(vec4 fragPosLightSpace, DirLight light, vec3 normal) {
    // perform perspective divide，正交投影因为w值没有修改，所以不需要除w，但是透视投影需要。
	// 把片元投影坐标转化为NDC，从[-w,w]->[-1,1]
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// [-1,1]->[0,1]
	projCoords = projCoords * 0.5 + 0.5;
	// 对之前在light space下计算出来的阴影纹理贴图（保存的是深度值）进行采样
	// 纹理贴图中的值是离散的，所以距光源较远的片元可能采样到相同的最小深度值，其坐标，可以近似看做连续，这就注定
	float closestDepth = texture(depthMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
    vec3 towardLight = normalize(-light.direction);
	float bias = max(0.05 * (1.0 - dot(normal, towardLight)), 0.005);  

	return currentDepth - bias > closestDepth ? 0.0 : 1.0;
}

// 平行光
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 towardView, float shadow){
    vec3 towardLight = normalize(-light.direction);
    // diffuse shading 与观察者位置无关
    float diffStrength = max(dot(normal, towardLight), 0.0); // 光照强度参数
    // specular shading 与观察者位置相关，这里和Phong 不同
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