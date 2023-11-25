#include <bits/stdc++.h>
#include "./frontend/headers/image.hpp"
#include "./frontend/headers/turtle.hpp"
#include "./frontend/headers/kernel.hpp"
#include "./frontend/headers/load_bmp.hpp"

using namespace std;

turtle __tglobal__;

image flower(image __i__, int h, int w, int r, int i, int j, turtle __t__)
{
    __t__.set_image(&__i__);
    __t__.go_to(w / 2, h / 2);
    __t__.pendown();
    __t__.set_pen_color(16711680);
    for (int k = 0; k < i; k++)
    {
        for (int l = 0; l < j; l++)
        {
            __t__.arc(r, 15);
        }
    
        __t__.rotate(315);
    }

    return __t__.get_image();
}

int main()
{
    image temp(300, 300, 0xffffff);
    video v(300, 300, 1);
    for (int i = 1; i <= 8; i++)
    {
        for (int j = 1; j <= 24; j++)
        {
            std::cout << "i = " << i << " j = " << j << std::endl;
            // flower(temp, 300, 300, 50, i, j, __tglobal__).frame("demo/frame" + std::to_string(i) + "_" + std::to_string(j) + ".bmp");
            v.concat_frame(flower(temp, 300, 300, 50, i, j, __tglobal__));
        }
    }

    v.play();
}