#include <bits/stdc++.h>
#include <sys/types.h>
#include <vector>
#include "image.hpp"
#include "turtle.hpp"
#include "./frontend/headers/kernel.hpp"
#include "./frontend/headers/load_bmp.hpp"

using namespace std;

// To run: g++ --std=c++11 -w test.cpp image.cpp frontend/headers/kernel.cpp frontend/headers/load_bmp.cpp turtle.cpp 
// For openmp with macos: g++-13 -w test.cpp image.cpp turtle.cpp frontend/headers/kernel.cpp frontend/headers/load_bmp.cpp -ld_classic -fopenmp


// float return_parabola_point(float x){
//     return ((-x*x + 40*x - 400)/16)+65;
// }

int main() { 
    image i1(50, 50, 0xffffff);

    vector<float> l = {0, 0, 50, 50};
    vector<float> l1 = {0, 0, 0, 20};
    vector<float> l2 = {0, 0, 20, 0};
    i1.draw("line", l);
    i1.draw("line", l1);
    i1.draw("line", l2);

    vector<float> c = {25, 25, 10, 0xfd12f1, 0};
    i1.draw("circle", c);
    i1.frame("test1.bmp");

    image i2(1000, 1000, 0xffffff);
    vector<float> bl = {0, 0, 1000, 1000};
    vector<float> bl1 = {40, 40, 0, 700};
    vector<float> bl2 = {40, 40, 700, 0};
    // i2.draw("line", bl);
    // i2.draw("line", bl1);
    // i2.draw("line", bl2);

    int angles = 30;
    int angles1 = 270;

    vector<float> barc = {500, 500, 150, angles1, angles, 0};
    i2.draw("arc", barc);

    vector<float> bc = {500, 500, 100, 0xfd12f1, 0};
    vector<float> bc1 = {500, 500, 70, 0x69dead, 1};
    // i2.draw("circle", bc);
    // i2.draw("circle", bc1);

    turtle t1;
    t1.set_image(&i2);
    t1.forward(250);
    t1.rotate(90);
    t1.forward(400);
    t1.pendown();
    t1.rotate(300);
    // t1.set_pen_color(0x000000);
    // t1.forward(50); 
    // t1.circle(30);
    // t1.rotate(90);
    // t1.rotate(90);
    // t1.rotate(90);
    // t1.forward(50);

    // t1.rotate(270);
    // t1.rotate(-90);
    // t1.forward(50);
    t1.arc(100, 90);

    i2.frame("test2.bmp");

    image img("./images/inputs/lena_color.bmp");
    // gray_image new_img = img.grayscale();
    // image new_img2 = img.blur(50);
    // image new_img3 = img.sharpen(50);
    // image new_img4 = img.sobel();
    // image new_img5 = img.pixelate(6);
    // image new_img6 = img.invert();
    // image new_img7 = img.noise(15);
    // image new_img8 = img.bnw();
    // image new_img9 = img.hflip();
    // image new_img10 = img.vflip();
    // image new_img11 = img.T();
    image new_img12 = img.crystallize(5000);


    img.frame("./images/outputs_color/output_orig.bmp");
    // new_img.frame("./images/outputs_color/output_grayscale.bmp");
    // new_img2.frame("./images/outputs_color/output_blur.bmp");
    // new_img3.frame("./images/outputs_color/output_sharpen.bmp");
    // new_img4.frame("./images/outputs_color/output_sobel.bmp");
    // new_img5.frame("./images/outputs_color/output_pixelate.bmp");
    // new_img6.frame("./images/outputs_color/output_invert.bmp");
    // new_img7.frame("./images/outputs_color/output_noise.bmp");
    // new_img8.frame("./images/outputs_color/output_bnw.bmp");
    // new_img9.frame("./images/outputs_color/output_hflip.bmp");
    // new_img10.frame("./images/outputs_color/output_vflip.bmp");
    // new_img11.frame("./images/outputs_color/output_transpose.bmp");
    new_img12.frame("./images/outputs_color/output_crystallize.bmp");

    uint8_t** gray;
    int buffer_size;
    char* FileBuffer;
    int cols, rows;
    // Load the image
    FillAndAllocate(FileBuffer, "./images/inputs/lena.bmp", rows, cols, buffer_size);
    RGB_Allocate(gray, rows, cols);
    GetPixlesFromBMP8(gray, buffer_size, rows, cols, FileBuffer);
    WriteOutBmp8(FileBuffer, "./test.bmp", buffer_size, rows, cols, gray);

    return 0;
}


// int main(){
//     gray_image img("./images/inputs/blackbuck.bmp");
//     gray_image new_img = img.blur(5);
//     gray_image new_img2 = img.sharpen(20);
//     gray_image new_img3 = img.sobel();
//     gray_image new_img4 = img.hflip();
//     gray_image new_img5 = img.vflip();
//     gray_image new_img6 = img.T();
//     gray_image new_img7 = img.pixelate(5);
//     gray_image new_img8 = img.invert();
//     gray_image new_img9 = img.noise(15);
//     gray_image new_img10 = img.bnw();

//     img.frame("./images/outputs/output_orig.bmp");
//     new_img.frame("./images/outputs/output_blur.bmp");
//     new_img2.frame("./images/outputs/output_sharpen.bmp");
//     new_img3.frame("./images/outputs/output_sobel.bmp");
//     new_img4.frame("./images/outputs/output_hflip.bmp");
//     new_img5.frame("./images/outputs/output_vflip.bmp");
//     new_img6.frame("./images/outputs/output_transpose.bmp");
//     new_img7.frame("./images/outputs/output_pixelate.bmp");
//     new_img8.frame("./images/outputs/output_invert.bmp");
//     new_img9.frame("./images/outputs/output_noise.bmp");
//     new_img10.frame("./images/outputs/output_bnw.bmp");
// }


// // Check filters and functions
// int main() {
//     // std::vector<int> v;
//     // v.push_back(15);
//     // v.push_back(0);
//     // v.push_back(50);
//     // image i1(200, 100, 0);
//     // i1.test();
//     // i1.draw("circle", v);
//     // i1.frame("./images/outputs/out.bmp");
//     // i1.load("./images/inputs/snail.bmp");
//     // cout << i1.get_height() << endl;
//     // cout << i1.get_width() << endl;
//     // i1.draw("circle", v);
//     // i1.test();
//     // i1.frame("./images/outputs/out1.bmp");

//     std::vector<int> pos;
//     pos.push_back(0);
//     pos.push_back(40);
//     pos.push_back(10);
//     // make the circle bounce around
//     for (int i=0; i<116; i++) {
//         pos[0] = i;
//         if (return_parabola_point(i)<0){
//             pos[1] = return_parabola_point(i-64);
//         } else {
//             pos[1] = return_parabola_point(i);
//         }
//         image i1(200, 200, 0);
//         i1.draw("circle", pos);
//         i1.frame("images/outputs/vid/" + to_string(i) + ".bmp");
//     }