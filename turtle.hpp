#include <bits/stdc++.h>
#include <vector>
#include "./frontend/headers/kernel.hpp"
#include "./frontend/headers/load_bmp.hpp"
#include "./image.hpp"

#ifndef TURTLE_HPP
#define TURTLE_HPP

enum POSITION {
    UP,
    DOWN
};

class turtle {
private:
    int x;
    int y;
    float angle;
    int pen_color;
    int height;
    int width;
    image* img;

    POSITION pen_position;
public:
    turtle();

    turtle(int x, int y, float angle, int pen_color, POSITION pen_position);

    void set_image(image* img);

    void forward(int distance);

    void rotate(float angle);

    void penup();

    void pendown();

    void set_pen_color(int color);

    void circle(int radius);

    void arc(int radius, float angle);

};

#endif