#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ncurses.h>
#include <unistd.h>
#include "image.hpp"


using namespace std;

int return_parabola_point(float x){
    return -1*(x*x + 400 - 40*x - 16*65)/16;
}

int return_color(int r, int g, int b){
    return (r*256*256) + (g*256) + b;
}

int main(){
    image new_img = image(100, 100, 4);
    // new_img.frame("./images/outputs/vid/base.bmp");
    // new_img.draw("circle", {10, 30, 20, 245});
    int multiplier = 0;
    float y = 0;
    float color = 0;
    int divisions = 100/6;
    int high = 244;
    int low = 38;
    int step = (high-low)/divisions;
    video new_vid = video(100, 100, 1);
    for (float i=0;i<20;i++){
        // new_img.load("./images/outputs/vid/base.bmp");
        image new_img = image(100, 100, 4);
        cout << i << endl;
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
        new_img.draw("circle", {i, y, 20, color, 1});
        new_vid = new_vid + new_img;
    }
    new_vid.play();

}
