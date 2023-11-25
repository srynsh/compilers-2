#include <bits/stdc++.h>
#include "./frontend/headers/image.hpp"
#include "./frontend/headers/turtle.hpp"
#include "./frontend/headers/kernel.hpp"
#include "./frontend/headers/load_bmp.hpp"

using namespace std;

turtle __tglobal__;

image flower( image __i__, int h, int w, int r, int i, turtle __t__){
__t__.set_image(&__i__);
__t__.go_to(w / 2, h / 2);
__t__.pendown();
for (int k = 0; k < i; k++ ) 
{
__t__.arc(r, 15);
}


return __t__.get_image();
}


int main(){
image temp(300, 300, 16777215);
video v(300, 300, 2);
for (int i = 1; i <= 24; i++ ) 
{
v.concat_frame(flower(temp, 300, 300, 50, i, __tglobal__));
}


v.play();
}