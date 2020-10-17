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

// 阴影计算，函数中的坐标都基于light space
float ShadowCalculation(vec4 fragPosLightSpace, DirLight light, vec3 normal) {
    // perform perspective divide，正交投影因为w值没有修改，所以不需要除w，但是透视投影需要。
	// 把片元投影坐标转化为NDC，值域从[-w,w]->[-1,1]
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// 从NDC的clip space的值域[-1,1]到纹理空间的值域[0,1]
	projCoords = projCoords * 0.5 + 0.5;
	float currentDepth = projCoords.z;
	// 超出光源平截头体远裁面的片元深度值大于1，则该函数恒返回0，总有阴影，而实际上这部分片元可能没有遮挡，不该有阴影
	if(currentDepth > 1.0) return 1.0;
	// 对之前在light space下计算出来的阴影纹理贴图（保存的是深度值）进行采样
	// float closestDepth = texture(depthMap, projCoords.xy).r;
	// 阴影纹理也是存在分辨率的，所以其值也是离散的，但片元坐标却是近似连续的，这就使得距光源较远且距离较近的连续
	// 片元可能采样到同一个纹素对应的深度值d，必然存在其中一个片元深度小于d，另一个深度大于d，条纹阴影就出现了
	// 通过给片元深度添加垂直偏移量来解决条纹阴影问题
	float bias = max(0.05 * (1.0 - dot(normal, normalize(-light.direction))), 0.005);
	currentDepth -= bias;

	float shadow = 0.0;
	// 计算纹素在纹理空间[0,1]中的尺寸
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for(int x = -1; x <= 1; ++x)
		for(int y = -1; y <= 1; ++y) {
			float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth > pcfDepth ? 0.0 : 1.0;
		}

	return shadow / 9.0;
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