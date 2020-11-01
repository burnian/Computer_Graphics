#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess; // 64
}; 
uniform Material material;

struct PointLight {
	// light
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	float constant;				// point light whole attenuation
    float linear;				// linear attenuation on the edge
    float quadratic;			// quadratic attenuation on the edge
	// shadow
	float farPlane;				// the far plane of the light-view space
	samplerCube depthCubemap;	// storing the closest depth in the light-view space
};
uniform PointLight pointLight;

in VS_OUT {
    vec3 FragWPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform vec3 viewWPos;

float CalcPointLightShadow(vec3 fragWPos, PointLight light, vec3 normal) {
    vec3 light2Frag = fragWPos - light.position;
    float closestDepth = texture(light.depthCubemap, light2Frag).r;
	closestDepth *= light.farPlane;
	float currentDepth = length(light2Frag);
	//float bias = max(0.05 * (1.0 - dot(normal, normalize(-light2Frag))), 0.005);
	float bias = 0.05;
	float shadow = currentDepth - bias > closestDepth ? 0.0 : 1.0; 

	return shadow;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragWPos, vec3 frag2ViewWS, float shadow) {
    vec3 towardLight = normalize(light.position - fragWPos);
    // diffuse shading
    float diffWeight = max(dot(normal, towardLight), 0.0);
	// specular shading
	vec3 halfwayDir = normalize(towardLight + frag2ViewWS);
	float specWeight = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // distance attenuation
    float dist = length(light.position - fragWPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);    
    // combine results
    vec3 ambient = light.ambient * texture(material.diffuse, fs_in.TexCoords).rgb * attenuation;
    vec3 diffuse = light.diffuse * diffWeight * texture(material.diffuse, fs_in.TexCoords).rgb * attenuation;
    vec3 specular = light.specular * specWeight * texture(material.specular, fs_in.TexCoords).rgb * attenuation;
    return ambient + shadow * (diffuse + specular);
}

void main() {
	vec3 norm = normalize(fs_in.Normal);
	vec3 frag2ViewWS = normalize(viewWPos - fs_in.FragWPos);
    float shadow = CalcPointLightShadow(fs_in.FragWPos, pointLight, norm);
	vec3 result = CalcPointLight(pointLight, norm, fs_in.FragWPos, frag2ViewWS, shadow);

	// FragColor = vec4(vec3(shadow), 1.0);
	FragColor = vec4(result, 1.0);
}


/*
used for generating point light shadow depth map
*/