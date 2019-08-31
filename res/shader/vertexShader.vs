#version 330 core
// layout是个标识，通过layout (location = 0)设定了输入变量的位置值，使用in关键字在顶点着色器中声明所有的输入顶点属性(Vertex Attribute)，我们就创建了一个vec3输入变量aPos
layout(location = 0) in vec3 aPos;// 位置变量的属性位置值为 0
layout(location = 1) in vec3 aColor;// 颜色变量的属性位置值为 1
layout(location = 2) in vec2 aTexCoord;// 纹理变量的属性位置值为 2

out vec3 ourColor;
out vec2 texCoord;

uniform float xOffset = 0.0f;

void main() {
    gl_Position = vec4(aPos.x + xOffset, aPos.y, aPos.z, 1.0f);// vec.w分量不是用作表达空间中的位置的（我们处理的是3D不是4D），而是用在所谓透视除法(Perspective Division)上。
    ourColor = aColor;// 将ourColor设置为我们从顶点数据那里得到的输入颜色
    texCoord = aTexCoord;
}