#include <bits/stdc++.h>
#include "./frontend/headers/image.hpp"
#include "./frontend/headers/turtle.hpp"
#include "./frontend/headers/kernel.hpp"
#include "./frontend/headers/load_bmp.hpp"

using namespace std;

turtle __tglobal__;

int bar(float p, int q, float r){
for (int i = 0; i < 20; i++ ) 
{
int s = 0;
s = p + q * i + r;
if (s >= 5) 
{
r = p * q;
}

else 
{
r = p + q;
}

}


return q;
}

image operations(image i1, image i2, image i3, image i4){
i1.noise(5);
i2.pixelate(6);
i3.vflip();
i4.sobel();
i4 = i2 * i1 ^ 14;
return i1;
}

int main(){
int a, b;
float f, g;
image i1(50, 50, 16777215);
image i2(50, 50, 16777215);
image i3(50, 50, 16777215);
image i4(50, 50, 16777215);
video v1(50, 50, 30);
video v2(50, 50, 60);
a = bar(f, b, g);
i1.draw("line", { 1, 1, 45, 45});
image ret_i = operations(i1, i2, i3, i4);
i1.paint();
i2.frame("./out_i2.bmp");
i3.frame("./out_i3.bmp");
i4.frame("./out_i4.bmp");
}