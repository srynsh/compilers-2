#include <bits/stdc++.h>
#include "./frontend/headers/image.hpp"
#include "./frontend/headers/turtle.hpp"
#include "./frontend/headers/kernel.hpp"
#include "./frontend/headers/load_bmp.hpp"

using namespace std;

turtle __tglobal__;

int main(){
image i1(200, 200, 16711680);
image i2(200, 200, 255);
image i3 = (i1 + i2) / 2;
i3.paint();
}