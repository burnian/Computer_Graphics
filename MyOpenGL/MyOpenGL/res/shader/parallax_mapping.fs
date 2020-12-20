#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D normal;
    sampler2D displace;
}; 
uniform Material material;

in VS_OUT {
    vec3 fragPosWS; // world space
	vec2 uvTS; // tangent space / UV space
    vec3 lightPosTS;
    vec3 viewPosTS;
    vec3 fragPosTS;
} fs_in;

uniform float heightScale;

vec2 ParallaxMapping(vec2 uvTS, vec3 viewDirTS){
	float height = texture(material.displace, uvTS).r;
	// viewDirTS是fragment指向viewer，越平行于tangent space的xy平面，
	// 替换的uv点就越远离fragment点，这么计算是为了模拟透视投影，因为这种光影
	// trick并未对uv点进行只有vertex才会接受的透视变换
	//vec2 p = viewDirTS.xy / viewDirTS.z * (height * heightScale);
	return uvTS - viewDirTS.xy * (height * heightScale);
}

void main() {
    vec3 viewDirTS = normalize(fs_in.viewPosTS - fs_in.fragPosTS);
	vec2 displaceUvTS = ParallaxMapping(fs_in.uvTS, viewDirTS);
	if(displaceUvTS.x > 1.0 || displaceUvTS.y > 1.0 || displaceUvTS.x < 0.0 || displaceUvTS.y < 0.0)
		discard;

    // obtain normal from normal map in range [0,1]
    vec3 normalTS = texture(material.normal, displaceUvTS).rgb;
    // transform normal vector to range [-1,1]
    normalTS = normalize(normalTS * 2.0 - 1.0);

	// get diffuse color
    vec3 color = texture(material.diffuse, displaceUvTS).rgb;
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    vec3 lightDirTS = normalize(fs_in.lightPosTS - fs_in.fragPosTS);
    float diffWeight = max(dot(lightDirTS, normalTS), 0.0);
    vec3 diffuse = diffWeight * color;
    // specular
    vec3 reflectDirTS = reflect(-lightDirTS, normalTS);
    vec3 halfwayDirTS = normalize(lightDirTS + viewDirTS);  
    float specWeight = pow(max(dot(normalTS, halfwayDirTS), 0.0), 32.0);
    vec3 specular = vec3(0.2) * specWeight;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
