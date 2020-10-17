#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;  

void main() { 
	// 锐化特效
	// 距离中心色点的偏移量
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );
	// 弱化周围颜色，强化中心颜色的权重值
    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
    // 根据偏移量来采样中心色点周围的颜色
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
	// 将权重值作用于周边颜色
    vec3 result = vec3(0.0);
    for(int i = 0; i < 9; i++)
        result += sampleTex[i] * kernel[i];
    
    FragColor = vec4(result, 1.0);
}  