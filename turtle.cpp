#include <bits/stdc++.h>
#include "./turtle.hpp"

turtle::turtle() {
    this->x = 0;
    this->y = 0;
    this->angle = 0; // In radians
    this->pen_color = 0; // default color is black
    this->pen_position = UP;
}

turtle::turtle(int x, int y, float angle, int pen_color, POSITION pen_position) {
    this->x = x;
    this->y = y;
    this->angle = angle;
    this->pen_color = pen_color;
    this->pen_position = pen_position;
}

void turtle::set_image(image* img) {
    this->img = img;
    this->height = img->get_height();
    this->width = img->get_width();
}

void turtle::forward(int distance) {
    int new_x = this->x + distance * cos(this->angle);
    int new_y = this->y + distance * sin(this->angle);
    if (this->pen_position == DOWN) {
        vector<int> line = {this->x, this->y, new_x, new_y, this->pen_color};
        this->img->draw("line", line);
    }
    this->x = new_x;
    this->y = new_y;
} 

void turtle::rotate(float angle) {
    this->angle += angle;
}

void turtle::penup() {
    this->pen_position = UP;
}

void turtle::pendown() {
    this->pen_position = DOWN;
}

void turtle::set_pen_color(int color) {
    this->pen_color = color;
}

void turtle::circle(int radius) {
    if (this->pen_position == DOWN) {
        vector<int> circle = {this->x - radius * sin(this->angle), this->y + radius * cos(this->angle), radius, this->pen_color};
        this->img->draw("circle", circle);
    }
}

void turtle::arc(int radius, float angle) {
    if (this->pen_position == DOWN) {
        vector<int> arc = {this->x - radius * sin(this->angle), this->y + radius * cos(this->angle), radius, this->angle, angle + this->angle, this->pen_color};
        this->img->draw("arc", arc);
    }

    this->x = this->x - radius * sin(this->angle) + radius * sin(this->angle + angle);
    this->y = this->y + radius * cos(this->angle) - radius * cos(this->angle + angle);
    this->angle += angle;
}