#include <bits/stdc++.h>
#include <sys/types.h>
#include "image.hpp"
#include "./frontend/headers/kernel.hpp"
#include "./frontend/headers/load_bmp.hpp"

using namespace std;

// To run: g++ --std=c++11 test.cpp image.cpp frontend/headers/kernel.cpp frontend/headers/load_bmp.cpp
// For openmp with macos: g++-13 test.cpp image.cpp frontend/headers/kernel.cpp frontend/headers/load_bmp.cpp -ld_classic -fopenmp


// float return_parabola_point(float x){
//     return ((-x*x + 40*x - 400)/16)+65;
// }

int main() {
    image img("./images/inputs/lena_color.bmp");
    // gray_image new_img = img.grayscale();
    image new_img2 = img.blur(50);
    new_img2 = new_img2.sharpen(50);
    // image new_img4 = img.sobel();
    // image new_img5 = img.pixelate(6);
    // image new_img6 = img.invert();
    // image new_img7 = img.noise(15);
    // image new_img8 = img.bnw();
    // image new_img9 = img.hflip();
    // image new_img10 = img.vflip();
    // image new_img11 = img.T();

    img.frame("./images/outputs_color/output_orig.bmp");
    // new_img.frame("./images/outputs_color/output_grayscale.bmp");
    new_img2.frame("./images/outputs_color/output_blur.bmp");
    // new_img3.frame("./images/outputs_color/output_sharpen.bmp");
    // new_img4.frame("./images/outputs_color/output_sobel.bmp");
    // new_img5.frame("./images/outputs_color/output_pixelate.bmp");
    // new_img6.frame("./images/outputs_color/output_invert.bmp");
    // new_img7.frame("./images/outputs_color/output_noise.bmp");
    // new_img8.frame("./images/outputs_color/output_bnw.bmp");
    // new_img9.frame("./images/outputs_color/output_hflip.bmp");
    // new_img10.frame("./images/outputs_color/output_vflip.bmp");
    // new_img11.frame("./images/outputs_color/output_transpose.bmp");

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