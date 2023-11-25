#include <bits/stdc++.h>
#include "./frontend/headers/image.hpp"
#include "./frontend/headers/turtle.hpp"
#include "./frontend/headers/kernel.hpp"
#include "./frontend/headers/load_bmp.hpp"

using namespace std;

turtle __tglobal__;

int main(){
image i("./images/inputs/greenland_grid_velo.bmp");
image i1 = i.crystallize(500);
image i2 = i.blur(5);
image i3 = i.sharpen(5);
image i4 = i.sobel();
image i5 = i.T();
image i6 = i.vflip();
image i7 = i.hflip();
image i8 = i.invert();
image i9 = i.noise(10);
image i10 = i.bnw(200);
i1.frame("./demo2/1.bmp");
i2.frame("./demo2/2.bmp");
i3.frame("./demo2/3.bmp");
i4.frame("./demo2/4.bmp");
i5.frame("./demo2/5.bmp");
i6.frame("./demo2/6.bmp");
i7.frame("./demo2/7.bmp");
i8.frame("./demo2/8.bmp");
i9.frame("./demo2/9.bmp");
i10.frame("./demo2/10.bmp");
}