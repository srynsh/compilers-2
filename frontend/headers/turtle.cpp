#include <bits/stdc++.h>
#include "./turtle.hpp"

float set_angle(float angle) {
    if (angle > 2 * M_PI) {
        angle -= 2 * M_PI;
    }

    if (angle < 0) {
        angle += 2 * M_PI;
    }

    return angle;
}

turtle::turtle() {
    this->x = 0;
    this->y = 0;
    this->angle = 0; // In radians
    this->pen_color = 0; // default color is black
    this->pen_state = UP;
}

turtle::turtle(int x, int y, float angle, int pen_color, POSITION pen_state) {
    this->x = x;
    this->y = y;
    this->angle = angle * M_PI / 180;
    this->pen_color = pen_color;
    this->pen_state = pen_state;
}

void turtle::set_image(image* img) {
    this->img = new image(*img);
    // Copy pixels
    this->height = img->get_height();
    this->width = img->get_width();
}

void turtle::forward(int distance) {
    int new_x = this->x + distance * cos(this->angle);
    int new_y = this->y + distance * sin(this->angle);
    if (this->pen_state == DOWN) {
        std::vector<float> line = {(float)this->x, (float)this->y, (float)new_x, (float)new_y, (float)this->pen_color};
        this->img->draw("line", line);
    }
    this->x = new_x;
    this->y = new_y;
} 

void turtle::rotate(float angle) {
    this->angle = set_angle(this->angle + angle * M_PI / 180);
}

void turtle::penup() {
    this->pen_state = UP;
}

void turtle::pendown() {
    this->pen_state = DOWN;
}

void turtle::set_pen_color(int color) {
    this->pen_color = color;
}

void turtle::circle(int radius) {
    if (this->pen_state == DOWN) {
        vector<float> circle = {this->x - radius * sin(this->angle), this->y + radius * cos(this->angle), (float)radius, (float)this->pen_color};
        this->img->draw("circle", circle);
    }
}

void turtle::arc(int radius, float angle) { // input is in degrees
    if (this->pen_state == DOWN) {
        vector<float> arc = {this->x - radius * sin(this->angle), this->y + radius * cos(this->angle), (float)radius, (float)(this->angle * 180 / M_PI + 270, angle + 270 + this->angle * 180 / M_PI), (float)this->pen_color};
        this->img->draw("arc", arc);
    }

    std::cout << "center is :" << this->x - radius * sin(this->angle) << " " << this->y + radius * cos(this->angle) << std::endl;
    cout << "angle is :" << this->angle * 180 / M_PI << " " << angle + this->angle * 180 / M_PI<< endl;
    this->x = this->x - radius * sin(this->angle) + radius * sin(this->angle + angle * M_PI / 180);
    this->y = this->y + radius * cos(this->angle) - radius * cos(this->angle + angle * M_PI / 180);
    this->angle = set_angle(this->angle + angle * M_PI / 180);
}

image turtle::get_image() {
    return *this->img;
}