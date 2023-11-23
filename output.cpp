#include <bits/stdc++.h>
#include "./frontend/headers/image.hpp"
#include "./frontend/headers/turtle.hpp"
#include "./frontend/headers/kernel.hpp"
#include "./frontend/headers/load_bmp.hpp"

using namespace std;

turtle _t_global;

image blur(image i){
return i.blur(10);
}

image dabadoo( image i, int h, int w, float a, turtle t){
t.set_image(&i);
t.penup();
t.forward(w / 2);
t.rotate(90);
t.forward(a / 2);
return t.get_image();
}


int main(){
image a("./images/inputs/blackbuck.bmp");
a = dabadoo(blur(a), 100, 100, 100.000000, _t_global);
}