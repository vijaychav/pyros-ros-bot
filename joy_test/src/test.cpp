#include<iostream>
using namespace std;
int main()
{
	int a = -32767;
	int l = (a&0x00ff);
	int h = ((a>>8)&0x00ff);
	cout << h << " " << l << endl;
	return 0;
}
