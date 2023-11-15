#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ncurses.h>
#include <unistd.h>
#include "image.hpp"


using namespace std;

int return_parabola_point(int x){
    return -1*(x*x + 400 - 40*x - 16*65)/16;
}

int return_color(int r, int g, int b){
    return (r*256*256) + (g*256) + b;
}

int main(){
    image new_img = image(200, 200, 4);
    // new_img.frame("./images/outputs/vid/base.bmp");
    // new_img.draw("circle", {10, 30, 20, 245});
    int multiplier = 0;
    int y = 0;
    int color = 0;
    int divisions = 200/6;
    int high = 244;
    int low = 38;
    int step = (high-low)/divisions;
    for (int i=0;i<200;i++){
        system("clear");
        // new_img.load("./images/outputs/vid/base.bmp");
        image new_img = image(200, 200, 4);
        while ((y = return_parabola_point(i-multiplier*64))<0){
            multiplier++;
        }
        if (i<1*divisions){
            color = return_color(high, low+step*i, low);
        }
        else if (i<2*divisions){
            color = return_color(high-step*(i-1*divisions), high, low);
        }
        else if (i<3*divisions){
            color = return_color(low, high, low+step*(i-2*divisions));
        }
        else if (i<4*divisions){
            color = return_color(low, high-step*(i-3*divisions), high);
        }
        else if (i<5*divisions){
            color = return_color(low+step*(i-4*divisions), low, high);
        }
        else if (i<6*divisions){
            color = return_color(high, low, high-step*(i-5*divisions));
        }
        else{
            color = return_color(high, low, low);
        }
        new_img.draw("circle", {i, y, 20, color});
        new_img.frame("./images/outputs/vid/vid.bmp");
        system("tiv -h 1500 -w 1000 ./images/outputs/vid/vid.bmp");
        usleep(70000);
    }
}