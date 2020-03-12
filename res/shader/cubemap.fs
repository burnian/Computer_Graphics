#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 viewPos;
uniform samplerCube skybox; // cubemap texture sampler

void main() {
	vec3 towardFrag = normalize(Position - viewPos);
	vec3 reflectDir = reflect(towardFrag, normalize(Normal));
    FragColor = texture(skybox, reflectDir);
}