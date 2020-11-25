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

// there are 20 points which belong to a third order cube excludes the 6 face
// points and the central point.
vec3 sampleOffsetDirections[20] = vec3[] (
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float CalcPointLightShadow(vec3 fragWPos, PointLight light, vec3 normal) {
    vec3 light2Frag = fragWPos - light.position;
//  float closestDepth = texture(light.depthCubemap, light2Frag).r;
//	closestDepth *= light.farPlane;
	float currentDepth = length(light2Frag);
	float bias = max(0.05 * (1.0 - dot(normal, normalize(-light2Frag))), 0.005);
//  without PCF
//	float shadow = currentDepth - bias > closestDepth ? 0.0 : 1.0; 
//  with PCF
//	float shadow = 0.0;
//	float samples = 4.0;
//	float offset = 0.1; // half edge length
//	float istep = 2 * offset / samples;
//	for(float x = -offset; x < offset; x += istep) {
//		for(float y = -offset; y < offset; y += istep) {
//			for(float z = -offset; z < offset; z += istep) {
//				float closestDepth = texture(light.depthCubemap, light2Frag + vec3(x, y, z)).r; 
//				closestDepth *= light.farPlane;   // undo mapping [0,1]
//				if(currentDepth - bias <= closestDepth)
//					shadow += 1.0;
//			}
//		}
//	}
//	return shadow / (samples * samples * samples);
//  with optimized PCF
	float shadow = 0.0;
	int samples = 20;
	// the closer the viewer to the fragment, the more accurate the sampling.
	float diskRadius = (1.0 + (length(viewWPos - fragWPos) / light.farPlane)) / 25.0;  
	for(int i = 0; i < samples; ++i) {
		float closestDepth = texture(light.depthCubemap, light2Frag + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= light.farPlane;   // undo mapping [0,1]
		if(currentDepth - bias <= closestDepth)
			shadow += 1.0;
	}
	return shadow / float(samples);
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