#include <bits/stdc++.h>
#include "kernel.hpp"
#include "load_bmp.hpp"

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
class video;
class gray_video;

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

        int **red;
        int **green;
        int **blue;

        // Private display functions
        void frame_self(std::string filename);
        void frame_pre(std::string filename);

    public:
        int buffer_size;
        char* FileBuffer;
        bool made;
        // Constructors
        image(int h, int w, int color);
        image(std::string filename);
        image(const image &img); // Copy constructor

        void load(std::string filename, bool init=false);

        // Display functions
        void frame(std::string filename);
        void paint();
        void color_pixel(int x, int y, int color);

        // Image manipulation functions
        image draw(std::string shape, std::vector<float> params);
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
        image crystallize(int k=500);

        // Getters
        int get_height() const;
        int get_width() const;
        int get_pixel(int i, int j, int channel) const;
        bool get_made() const;

        void set_pixel(int i, int j, int channel, int value);

        // Operators
        image& operator=(image const img);
        image operator+(image const img);
        image operator-(image const img);
        image operator*(image const img);
        image operator/(float const val);

        image& operator=(gray_image const img);

        image& operator=(int const val);
        image operator+(int const val);
        friend image operator+(int const val, image const img);
        image operator-(int const val);
        friend image operator-(int const val, image const img);
        image operator*(int const val);
        friend image operator*(int const val, image const img);
        image operator/(int const val);

        void operator=(float const val);
        image operator+(float const val);
        friend image operator+(float const val, image const img);
        image operator-(float const val);
        friend image operator-(float const val, image const img);
        image operator*(float const val);
        friend image operator*(float const val, image const img);

        void operator=(bool const val);
        image operator+(bool const val);
        friend image operator+(bool const val, image const img);
        image operator-(bool const val);
        friend image operator-(bool const val, image const img);
        image operator*(bool const val);
        friend image operator*(bool const val, image const img);
        image operator/(bool const val);

        image operator+(gray_image const img);
        friend image operator+(gray_image const img1, image const img2);
        image operator-(gray_image const img);
        friend image operator-(gray_image const img1, image const img2);
        image operator*(gray_image const img);
        friend image operator*(gray_image const img1, image const img2);
        image operator/(gray_image const img);
        friend image operator/(gray_image const img1, image const img2);

        image operator^(int const value);
        image operator^(float const value);
        image operator^(bool const value);
        friend image operator^(int const value, image const img);
        friend image operator^(float const value, image const img);
        friend image operator^(bool const value, image const img);


        friend image operator ~(image const img);

        video operator+(video const vid);
        video operator+(gray_video const vid);

        char* get_buffer () const;
        int get_buffer_size () const;
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
        void flip();
    
    public:
        int buffer_size;
        char* FileBuffer;
        bool made;
        // Constructors
        gray_image(int h, int w, int color);
        gray_image(std::string filename);
        gray_image(const gray_image &img); // Copy constructor

        void load(std::string filename, bool init=false);

        // Display functions
        void frame(std::string filename);
        void paint();

        // Operators
        gray_image operator+(gray_image const img);
        gray_image operator-(gray_image const img);
        gray_image operator*(gray_image const img);
        gray_image operator/(float const val);
        gray_image& operator=(gray_image const img);
        gray_image sqrt();
        gray_image operator*(std::vector<std::vector<float>> kernel);  // performs a stride 1 convolution with given kernel and no padding

        void operator=(int const val);
        gray_image operator+(int const val);
        friend gray_image operator+(int const val, gray_image const img);
        gray_image operator-(int const val);
        friend gray_image operator-(int const val, gray_image const img);
        gray_image operator*(int const val);
        friend gray_image operator*(int const val, gray_image const img);
        gray_image operator/(int const val);

        void operator=(float const val);
        gray_image operator+(float const val);
        friend gray_image operator+(float const val, gray_image const img);
        gray_image operator-(float const val);
        friend gray_image operator-(float const val, gray_image const img);
        gray_image operator*(float const val);
        friend gray_image operator*(float const val, gray_image const img);

        void operator=(bool const val);
        gray_image operator+(bool const val);
        friend gray_image operator+(bool const val, gray_image const img);
        gray_image operator-(bool const val);
        friend gray_image operator-(bool const val, gray_image const img);
        gray_image operator*(bool const val);
        friend gray_image operator*(bool const val, gray_image const img);
        gray_image operator/(bool const val);

        gray_image& operator=(image val); 

        gray_image operator^(int const value);
        gray_image operator^(float const value);
        gray_image operator^(bool const value);
        friend gray_image operator^(int const value, gray_image const img);
        friend gray_image operator^(float const value, gray_image const img);
        friend gray_image operator^(bool const value, gray_image const img);

        friend gray_image operator ~(gray_image const img);

        video operator+(video const vid); 
        gray_video operator+(gray_video const vid);


        // Getters and Setters
        int get_height() const;
        int get_width() const;
        int get_pixel(int i, int j) const;
        bool get_made() const;
        int get_buffer_size() const;
        char* get_buffer() const;

        void set_pixel(int i, int j, int color);

        // Image manipulation functions
        gray_image draw(std::string shape, std::vector<float> params);
        std::vector<std::vector<float>> to_vector();
        gray_image blur(int k=2);
        gray_image sharpen(int k=1);
        gray_image sobel();
        gray_image pixelate(int k);
        gray_image invert();
        gray_image noise(float var);
        gray_image bnw(int thr=100);
        gray_image hflip();
        gray_image vflip();
        gray_image T();
        gray_image crystallize(int k=500);
        image to_image();

        // Destructor
        ~gray_image();
};

class video {
    private:
        int h;
        int w;
        int fps;
        std::vector<image> frames_vec;
    
    public:
        // Constructors
        video(int h, int w, int fps);

        // Display functions
        void play();

        // Operators
        image operator[](int i) const; // For access
        image& operator[](int i); // For assignment
        video operator+(video const vid); // Concatenation
        video& operator=(video const val);
        video& operator=(gray_video const val);
        video operator+(gray_video const vid); // Concatenation
        video operator+(image const img); // Concatenation
        video operator+(gray_image img); // Concatenation

        // Getters and Setters
        int get_height() const;
        int get_width() const;
        int get_fps() const;
        int get_num_frames() const;
        image get_frame(int i) const;
        std::vector<image> get_frames() const;

        void set_fps(int fps);
        void set_frame(int i, image const& frame);
        void concat_frame(image const& frame);

        friend video operator~(video const vid);

};

class gray_video {
    private:
        int h;
        int w;
        int fps;
        std::vector<gray_image> frames_vec;
    
    public:
        // Constructors
        gray_video(int h, int w, int fps);

        // Display functions
        void play();

        // Operators
        gray_image operator[](int i) const; // For access
        gray_image &operator[](int i); // For assignment
        gray_video operator+(gray_video const vid); // Concatenation
        gray_video& operator=(video const val);
        gray_video& operator=(gray_video const val);
        gray_video operator+(gray_image const img); // Concatenation
        video operator+(image const img); // Concatenation
        video operator+(video const vid); // Concatenation

        // Getters and Setters
        int get_height() const;
        int get_width() const;
        int get_fps() const;
        int get_num_frames() const;
        gray_image get_frame(int i) const;
        std::vector<gray_image> get_frames() const;

        void set_fps(int fps);
        void set_frame(int i, gray_image const& frame);
        void concat_frame(gray_image const& frame);

        friend gray_video operator~(gray_video const vid);
};

#endif