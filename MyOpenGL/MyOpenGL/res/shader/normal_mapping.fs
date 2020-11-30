#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D normal;
}; 
uniform Material material;

in VS_OUT {
    vec3 fragPosWS; // world space
	vec2 uvTS; // tangent space / UV space
    vec3 lightPosTS;
    vec3 viewPosTS;
    vec3 fragPosTS;
} fs_in;

void main() {
    // obtain normal from normal map in range [0,1]
    vec3 normalTS = texture(material.normal, fs_in.uvTS).rgb;
    // transform normal vector to range [-1,1]
    normalTS = normalize(normalTS * 2.0 - 1.0);

	// get diffuse color
    vec3 color = texture(material.diffuse, fs_in.uvTS).rgb;
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    vec3 lightDirTS = normalize(fs_in.lightPosTS - fs_in.fragPosTS);
    float diffWeight = max(dot(lightDirTS, normalTS), 0.0);
    vec3 diffuse = diffWeight * color;
    // specular
    vec3 viewDirTS = normalize(fs_in.viewPosTS - fs_in.fragPosTS);
    vec3 reflectDirTS = reflect(-lightDirTS, normalTS);
    vec3 halfwayDirTS = normalize(lightDirTS + viewDirTS);  
    float specWeight = pow(max(dot(normalTS, halfwayDirTS), 0.0), 32.0);

    vec3 specular = vec3(0.2) * specWeight;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
