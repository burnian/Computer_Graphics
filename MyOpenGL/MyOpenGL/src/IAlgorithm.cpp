#include <iostream>
#include <math.h>

#define xxx 12345

using namespace std;


void PutPixel(int x, int y) {
	cout << x << " " << y << endl;
}

void DDAline(int x0, int y0, int x1, int y1) {
	float dx = abs(x1 - x0);
	float dy = abs(y1 - y0);
	float eps = dx > dy ? dx : dy;
	float incX = dx / eps;
	float incY = dy / eps;
	float x = x0;
	float y = y0;
	for (int i = 0; i <= eps; i++) {
		PutPixel(floor(x + 0.5), floor(y + 0.5));
		x += incX;
		y += incY;
	}
}


int main_() {
	//DDAline(0, 0, 2, 1);
	//const char* c =
	//	"a\n"
	//	"b\n";

	//std::cout << c << strlen(c) << std::endl;

	//const char*��ʾ�����޸�ָ�����ݣ�char* const��ʾ�����޸�ָ��
	//const string��ʾ�Ȳ����޸�ָ���ֲ����޸�ָ�����ݣ��൱�ڶ�����һ��������
	const char* p = "hello";
	std::cout << p << std::endl;
	p = "world";
	std::cout << p << std::endl;

	cin.get();
	return 0;
}