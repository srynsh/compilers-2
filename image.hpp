#include <bits/stdc++.h>
#include "./frontend/headers/kernel.hpp"
#include "./frontend/headers/load_bmp.hpp"

#ifndef IMAGE_HPP
#define IMAGE_HPP

/**
 * @brief Class for image manipulation
 * 
 * @details The image class is used to manipulate images. It can be initialized or loaded from a file.
 * Various operations can be performed on the image, such as blurring, flipping, drawing shapes, etc.
 * The image can be saved to a file or displayed on the terminal.
 */

// Forward declaration of classes
class image;
class gray_image;

// Forward declaration of non-member functions
gray_image conv(image &img, std::vector< std::vector<std::vector <float> > > kernel, int stride, float padding);
image conv(
        image &img, std::vector< std::vector<std::vector <float> > > kernel1,
        std::vector< std::vector<std::vector <float> > > kernel2,
        std::vector< std::vector<std::vector <float> > > kernel3,
        int stride, 
        float padding);
image conv(
        image &img, 
        std::vector< std::vector<float> >  kernel1,
        std::vector< std::vector<float> >  kernel2,
        std::vector< std::vector<float> >  kernel3,
        int stride, 
        float padding);
gray_image conv(gray_image &img, std::vector< std::vector<float> > kernel, int stride, float padding);
gray_image to_gray_image(std::vector< std::vector<float> > vec);


class image {
    private:
        int h;
        int w;

        uint8_t **red;
        uint8_t **green;
        uint8_t **blue;
        int buffer_size;
        char* FileBuffer;
        bool made;

        // Private display functions
        void frame_self(std::string filename);
        void frame_pre(std::string filename);

    public:
        // Constructors
        image(int h, int w, int color);
        image(std::string filename);
        image(const image &img); // Copy constructor

        void load(std::string filename, bool init=false);

        // Display functions
        void frame(std::string filename);
        void paint();

        // Image manipulation functions
        void draw(std::string shape, std::vector<int> params);
        gray_image grayscale();
        image blur(int k=2);
        image clip();
        image sharpen(int num=1);
        image sobel();
        image pixelate(int k);
        image invert();
        image noise(float var);
        image bnw(int thr=100);
        image hflip();
        image vflip();
        image T();

        // Getters
        int get_height() const;
        int get_width() const;
        int get_pixel(int i, int j, int channel) const;

        void set_pixel(int i, int j, int channel, int value);

        // Operators
        image& operator=(image const& img);
        image operator+(image const& img);
        image operator-(image const& img);
        image operator*(image const& img);
        image operator/(float const& val);

        // Destructor
        ~image();

    private:
        void flip();
    
};

class gray_image {
    private:
        int h;
        int w;

        int **gray;
    
    public:
        // Constructors
        gray_image(int h, int w, int color);
        gray_image(std::string filename);
        gray_image(const gray_image &img); // Copy constructor

        void load(std::string filename, bool init=false);

        // Display functions
        void frame(std::string filename);
        void paint();

        // Operators
        gray_image operator+(gray_image const& img);
        gray_image operator-(gray_image const& img);
        gray_image operator*(gray_image const& img);
        gray_image operator/(float const& val);
        gray_image& operator=(gray_image const& img);
        gray_image sqrt();
        gray_image operator*(std::vector<std::vector<float>> kernel);  // performs a stride 1 convolution with given kernel and no padding

        // Getters and Setters
        int get_height() const;
        int get_width() const;
        int get_pixel(int i, int j) const;

        void set_pixel(int i, int j, int color);

        // Image manipulation functions
        std::vector<std::vector<float>> to_vector();
        gray_image blur(int k=2);
        gray_image sharpen(int k);
        gray_image sobel();
        gray_image pixelate(int k);
        gray_image invert();
        gray_image noise(float var);
        gray_image bnw(int thr=100);
        gray_image hflip();
        gray_image vflip();
        gray_image T();

        // Destructor
        ~gray_image();
};

#endif