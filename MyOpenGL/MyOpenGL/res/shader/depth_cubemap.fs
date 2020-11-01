#version 330 core
in vec4 FragWPos;

uniform vec3 lightWPos;
uniform float farPlane;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(FragWPos.xyz - lightWPos);
    
    // map to [0;1] range by dividing by farPlane
    // write this as modified depth
    gl_FragDepth = lightDistance / farPlane;
}