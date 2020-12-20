#version 330 core
layout (location = 0) in vec3 aPos; // local space coords
layout (location = 1) in vec3 aNormal; // vector base on the coords space of aPos, along with its axis z.
layout (location = 2) in vec2 aTexCoords; // uv space coords
layout (location = 3) in vec3 aTangent; // vector base on the coords space of aPos, along with its axis x.
layout (location = 4) in vec3 aBitangent; // vector base on the coords space of aPos, along with its axis y.

out VS_OUT {
    vec3 fragPosWS; // world space
	vec2 uvTS; // tangent space / UV space
    vec3 lightPosTS;
    vec3 viewPosTS;
    vec3 fragPosTS;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPosWS;
uniform vec3 viewPosWS;

// calculate world to tangent space transformation matrix.
mat3 CalcTBN() {
	// local to world, keeping perpendicular relationship between the normal and its plane.
	mat3 normalMat = transpose(inverse(mat3(model)));
	// 通过施密特正交化和单位化来得到三个两两正交的向量
	vec3 T = normalize(normalMat * aTangent);
	vec3 N = normalize(normalMat * aNormal);
	// T and N may not be perpendicular with each other anymore. Making them perpendicular
	// with each other again using Gram-Schmidt process.
	T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
	return transpose(mat3(T, B, N)); // Orthogonal Matrix
}

void main()
{
	vs_out.fragPosWS = vec3(model * vec4(aPos, 1.0));
    vs_out.uvTS = aTexCoords;
	
	mat3 TBN = CalcTBN();
	vs_out.lightPosTS = TBN * lightPosWS;
    vs_out.viewPosTS = TBN * viewPosWS;
    vs_out.fragPosTS = TBN * vs_out.fragPosWS;

    gl_Position = projection * view * vec4(vs_out.fragPosWS, 1.0);
}

/*
正交矩阵定义：由行之间两两正交，同时列之间两两正交的单位向量组成的方阵。
正交矩阵性质：A * A^T = A^T * A = E
*/