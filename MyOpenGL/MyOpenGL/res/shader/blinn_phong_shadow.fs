#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess; // 64
}; 
uniform Material material;

struct DirLight {
	// light
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	// shadow
	sampler2D depthMap;
};
uniform DirLight dirLight;

in VS_OUT {
    vec3 FragWPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragLightSpacePos;
} fs_in;

uniform vec3 viewWPos; // WPos means world space position

// LPos means light space position
float CalcDirLightShadow(vec4 fragLPos, DirLight light, vec3 normal) {
    // perform perspective divide, but when fragLPos is based on orthogonal projection，
	// the w value is still the same 1, there is no need to do the dividing like below.
	// transforming the position from common space to NDC, from range [-w,w] to range [-1,1]
	vec3 projCoords = fragLPos.xyz / fragLPos.w;
	// transforming the NDC's clip space range from [-1,1] to [0,1]
	projCoords = projCoords * 0.5 + 0.5;
	float currentDepth = projCoords.z;
	// the depth of every fragment outside the frustum is larger than 1, but the maximun of closest
	// depth is always be 1, then the fragments outside the frustum will be rendered like they are
	// occluded by the edge of the frustum, but in fact, they aren't.
	if(currentDepth > 1.0) return 1.0;
	// sampling closest depth from the shadow map generated from the depth_map shader before.
	// float closestDepth = texture(depthMap, projCoords.xy).r;
	// the value in shadow map is discrete, it results in the continuous fragments may sample the same
	// closest depth, because they are near from each other and far from light source. When their 
	// distances to the light source is different, one maybe larger than the closest depth and one maybe
	// not, this results in the shadow stripes.
	// Solving this by adding bias which is related to the angle between the normal and the light
	// direction, the bias is in direct proportion to the angle.
	float bias = max(0.05 * (1.0 - dot(normal, normalize(-light.direction))), 0.005);
	currentDepth -= bias;

	float shadow = 0.0;
	// textureSize function returns the width(the same as height) of the shadow map
	vec2 texelSize = 1.0 / textureSize(light.depthMap, 0);
	// multi-sampling to avoid the mosaic shadow
	for(int x = -1; x <= 1; ++x)
		for(int y = -1; y <= 1; ++y) {
			// Percentage-closer filtering
			float PCFDepth = texture(light.depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth > PCFDepth ? 0.0 : 1.0;
		}

	return shadow / 9.0;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 frag2ViewWS, float shadow) {
    vec3 towardLight = normalize(-light.direction);
    // diffuse shading
    float diffWeight = max(dot(normal, towardLight), 0.0);
    // specular shading
	vec3 halfwayDir = normalize(towardLight + frag2ViewWS);
	float specWeight = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * texture(material.diffuse, fs_in.TexCoords).rgb;
    vec3 diffuse = light.diffuse * diffWeight * texture(material.diffuse, fs_in.TexCoords).rgb;
    vec3 specular = light.specular * specWeight * texture(material.specular, fs_in.TexCoords).rgb;
    return ambient + shadow * (diffuse + specular);
}

void main() {
	vec3 norm = normalize(fs_in.Normal);
	vec3 frag2ViewWS = normalize(viewWPos - fs_in.FragWPos);
	// phase 1: Directional lighting
	float shadow = CalcDirLightShadow(fs_in.FragLightSpacePos, dirLight, norm);
	vec3 result = CalcDirLight(dirLight, norm, frag2ViewWS, shadow);

	// FragColor = vec4(vec3(shadow), 1.0);
	FragColor = vec4(result, 1.0);
}
