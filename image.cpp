#include <bits/stdc++.h>
#include "image.hpp"
#include "./frontend/headers/kernel.hpp"
#include "./frontend/headers/load_bmp.hpp"

/* To run: g++ --std=c++11 -o image frontend/headers/kernel.cpp frontend/headers/load_bmp.cpp image.cpp */

// using namespace std;

// // Forward declaration of classes
// class gray_image;

// // Forward declaration of non-member functions
// gray_image conv(gray_image &img, std::vector< std::vector<float> > kernel, int stride, float padding);
// gray_image to_gray_image(std::vector< std::vector<float> > vec);

#define MAX_SIZE 2000

void swap(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
}

///////////////////////////////////////////////////////////////////////////////////////////
////////////////////// Image Class ////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


/*------------------------------------------------------------------------
 * Constructors and related functions
 *------------------------------------------------------------------------*/

/// @brief Initializes an image of given dimensions with all pixels set to given value
image::image(int h, int w, int color) {
    assert(w > 0 && h > 0);
    assert(w < MAX_SIZE && h < MAX_SIZE);
    assert(color >= 0x000000 && color <= 0xFFFFFF);

    int red_c = ((color >> 16) & 0xFF);  // Extract the RR byte
    int green_c = ((color >> 8) & 0xFF);   // Extract the GG byte
    int blue_c = ((color) & 0xFF);

    this->h = h;
    this->w = w;

    red = new uint8_t*[w];
    green = new uint8_t*[w];
    blue = new uint8_t*[w];

    for (int i=0; i<w; i++) {
        this->red[i] = new uint8_t[h];
        this->green[i] = new uint8_t[h];
        this->blue[i] = new uint8_t[h];
    }

    for(int i=0; i<w; i++) {
        for(int j=0; j<h; j++) {
            this->red[i][j] = red_c;
            this->green[i][j] = green_c;
            this->blue[i][j] = blue_c;
        }
    }

    this->buffer_size = 54 + 3*this->w*this->h;
    this->made = 1;
}

/// @brief Initializes an image from a given file
image::image(std::string filename) {
    this->load(filename, true);
}

/// @brief Loads a .bmp
void image::load(std::string filename, bool init) {
    if (!init) {    
        for (int i=0; i<this->w; i++) {
            delete [] this->red[i];
            delete [] this->green[i];
            delete [] this->blue[i];
        }

        delete [] this->red;
        delete [] this->green;
        delete [] this->blue; 

        if (!this->made) {
            delete this->FileBuffer;
        }
    }    

    if (!FillAndAllocate(this->FileBuffer, filename.c_str(), this->h, this->w, this->buffer_size)) {
        cout << "File read error" << endl; 
        return;
    }

    RGB_Allocate(this->red, this->h, this->w);
    RGB_Allocate(this->green, this->h, this->w);
    RGB_Allocate(this->blue, this->h, this->w);
    GetPixlesFromBMP24( this->red,  this->green, this->blue, this->buffer_size, this->h, this->w, this->FileBuffer);

    this->made = 0;
    this->flip();
}

/*------------------------------------------------------------------------
 * Image display functions
 *------------------------------------------------------------------------*/

void image::frame(std::string filename) {
    
    if (made) {
        flip();
        frame_self(filename);
        flip();
    } else {
        flip();
        frame_pre(filename);
        flip();
    }
    // flip();
}

void image::frame_self(std::string filename) {
    FILE *f;
    int filesize = 54 + 3*w*h;  //w is your image width, h is image height, both int
    unsigned char *canvas = NULL;
    canvas = (unsigned char *)malloc(3*w*h);
    memset(canvas,0,3*w*h);

    for(int i=0; i<h; i++) {
        for(int j=0; j<w; j++) {
            int x=i; int y=j;
            canvas[(y+x*w)*3+0] = (unsigned char)(blue[j][i]);
            canvas[(y+x*w)*3+1] = (unsigned char)(green[j][i]);
            canvas[(y+x*w)*3+2] = (unsigned char)(red[j][i]);
        } 
    }

    unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
    unsigned char bmppad[3] = {0,0,0};

    bmpfileheader[ 2] = (unsigned char)(filesize    );
    bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
    bmpfileheader[ 4] = (unsigned char)(filesize>>16);
    bmpfileheader[ 5] = (unsigned char)(filesize>>24);

    bmpinfoheader[ 4] = (unsigned char)(       w    );
    bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
    bmpinfoheader[ 6] = (unsigned char)(       w>>16);
    bmpinfoheader[ 7] = (unsigned char)(       w>>24);
    bmpinfoheader[ 8] = (unsigned char)(       h    );
    bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
    bmpinfoheader[10] = (unsigned char)(       h>>16);
    bmpinfoheader[11] = (unsigned char)(       h>>24);

    f = fopen(filename.c_str(), "wb");
    fwrite(bmpfileheader, 1, 14, f);
    fwrite(bmpinfoheader, 1, 40, f);

    for(int i=0; i<h; i++) {
        fwrite(canvas+(w*(h-i-1)*3),3,w,f);
        fwrite(bmppad,1,(4-(w*3)%4)%4,f);
    }

    free(canvas);
    fclose(f);
}

void image::frame_pre(std::string filename) {
    WriteOutBmp24(FileBuffer, filename.c_str(), buffer_size, h, w, red, green, blue);
}

void image::paint() {
    // TODO: KG has to do this
}

/*------------------------------------------------------------------------
 * Image manipulation functions
 *------------------------------------------------------------------------*/

void image::draw(std::string shape, std::vector<int> params) {
    if (shape == "circle") {
        int r = params[2];
        int cx = params[0];
        int cy = params[1];
        bool fill = params[3];

        cout << cx << " " << cy << " " << r << endl;

        for (int y=0; y<h; y++) {
            for (int x=0; x<w; x++) {
                
                if ((x-cx)*(x-cx) + (y-cy)*(y-cy) <= r*r) {
                    red[x][y] = 120;
                    green[x][y] = 120;
                    blue[x][y] = 120;
                }
            }
        }

        // TODO: draw circle, we will think of doing some efficient way later
    } else if (shape == "line") {
        int start_x = params[0];
        int start_y = params[1];
        int end_x = params[2];
        int end_y = params[3];

        // TODO: draw line, we will think of doing some efficient way later
    } else if (shape == "rectangle") {
        int cx = params[0];
        int cy = params[1];
        int w = params[2];
        int h = params[3];
        bool fill = params[4];

        // TODO: draw rectangle, we will think of doing some efficient way later
    }
}

/*------------------------------------------------------------------------
 * Getters and Setters
 *------------------------------------------------------------------------*/

int image::get_height() {
    return h;
}

int image::get_width() {
    return w;
}

/*------------------------------------------------------------------------
 * Destructor
 *------------------------------------------------------------------------*/

image::~image() {
    for (int i=0; i<w; i++) {
        delete [] red[i];
        delete [] green[i];
        delete [] blue[i];
    }

    delete [] red;
    delete [] green;
    delete [] blue;

    if (!made) {
        delete FileBuffer;
    }
}



void image::flip() {
    for (int i=0; i<this->w; i++) {
        for (int j=0; j<this->h/2; j++) {
            swap(this->red[i][j], this->red[i][h-1-j]);
            swap(this->green[i][j], this->green[i][h-1-j]);
            swap(this->blue[i][j], this->blue[i][h-1-j]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
////////////////////// Gray Scale Image Class /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


/*------------------------------------------------------------------------
 * Constructors and related functions
 *------------------------------------------------------------------------*/

/// @brief Initializes an image of given dimensions with all pixels set to given value
/// @param h height in pixels
/// @param w width in pixels
/// @param color color of all pixels (0-255)
gray_image::gray_image(int h, int w, int color) {

    assert(w > 0 && h > 0);
    assert(w < MAX_SIZE && h < MAX_SIZE);
    assert(color >= 0x000000 && color <= 0xFFFFFF);

    int gray_c = color;

    this->w = w;
    this->h = h;

    gray = (int **)malloc(w * sizeof(int *));

    for (int i=0; i<w; i++) {
        gray[i] = (int *)malloc(h * sizeof(int));
    }

    for(int i=0; i<w; i++) {
        for(int j=0; j<h; j++) {
            gray[i][j] = gray_c;
        }
    }
}

/// @brief Initializes an image from a given file (RGB -> Gray)
/// @param filename path to the .bmp file
gray_image::gray_image(std::string filename) {
    load(filename, true);
}

/// @brief Copy constructor
/// @param img 
gray_image::gray_image(const gray_image &img) {
    /*
        params:
            img: image to be copied
    */

    h = img.get_height();
    w = img.get_width();


    gray = (int **)malloc(w * sizeof(int *));

    for (int i=0; i<w; i++) {
        gray[i] = (int *)malloc(h * sizeof(int));
    }

    for(int i=0; i<w; i++) {
        for(int j=0; j<h; j++) {
            // gray[i][j] = img.get_pixel(i, j);
            int pixel = img.get_pixel(i, j);
            if (pixel < 0) {
                pixel = 0; // clip to 0 if value is negative
            } else if (pixel > 255) {
                pixel = 255; // clip to 255 if value exceeds 255
            }
            gray[i][j] = pixel;
        }
    }
}

/*------------------------------------------------------------------------
 * Getters and Setters
 *------------------------------------------------------------------------*/

int gray_image::get_height() const {
    return h;
}

int gray_image::get_width() const {
    return w;
}

/// @brief Returns the color of the pixel at (x, y)
/// @param x 
/// @param y 
/// @return int
int gray_image::get_pixel(int x, int y) const {
    /*
        params:
            x: x coordinate of pixel
            y: y coordinate of pixel
    */

    assert(x >= 0 && x < w);
    assert(y >= 0 && y < h);

    return gray[x][y];
}


/// @brief Sets the pixel at (x, y) to given color
void gray_image::set_pixel(int x, int y, int color) {
    /*
        params:
            x: x coordinate of pixel
            y: y coordinate of pixel
            color: color of pixel (0-255)
    */

    assert(x >= 0 && x < w);
    assert(y >= 0 && y < h);
    // assert(color >= 0 && color <= 255);

    gray[x][y] = color;
}

/// @brief Loads a .bmp in RGB format and converts it to gray scale
void gray_image::load(std::string filename, bool init) {
    /*
        params:
            filename: path to the .bmp file
            init: if true, initializes the image, else, overwrites the image (first deallocate memory)
    */

    FILE *f;
    unsigned char info[54];
    f = fopen(filename.c_str(), "rb");
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // If init is false, deallocate memory
    if (!init) {    
        for (int i=0; i<w; i++) {
            free(gray[i]);
        }

        free(gray);
    }    

    w = *(int*)&info[18];
    h = *(int*)&info[22];

    try {
        assert(w > 0 && h > 0);
    } catch (const std::exception& e) {
        cout << "Error: Incorrect file format or corrupted file" << e.what() << endl;
        return;
    }

    try {
        assert(w < MAX_SIZE && h < MAX_SIZE);
    } catch (const std::exception& e) {
        cout << "Error: Image size too large" << e.what() << endl;
        return;
    }

    int size = 3 * w * h;
    unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
    fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
    fclose(f);

    gray = (int **)malloc(w * sizeof(int *));

    for (int i=0; i<w; i++) {
        gray[i] = (int *)malloc(h * sizeof(int));
    }

    for(int i=0; i<w; i++) {
        for(int j=0; j<h; j++){
            // Uses the NTSC formula to convert RGB to gray
            // See https://support.ptc.com/help/mathcad/r9.0/en/index.html#page/PTC_Mathcad_Help/example_grayscale_and_color_in_images.html
            gray[i][j] = 0.299*data[(i+j*w)*3+2] + 0.587*data[(i+j*w)*3+1] + 0.114*data[(i+j*w)*3+0];
        }
    }

    delete[] data;
}

/*------------------------------------------------------------------------
 * Image display functions
 *------------------------------------------------------------------------*/

/// @brief Convert the image to a .bmp file, and save it
void gray_image::frame(std::string filename) {
    /*
        params:
            filename: path to the .bmp file
    */

    FILE *f;
    int filesize = 54 + 3*w*h;  //w is your image width, h is image height, both int
    unsigned char *canvas = NULL;
    canvas = (unsigned char *)malloc(3*w*h);
    memset(canvas,0,3*w*h);

    // Clip all values to 0-255
    for(int i=0; i<w; i++) {
        for(int j=0; j<h; j++) {
            if (gray[i][j] < 0) {
                gray[i][j] = 0; // clip to 0 if value is negative
            } else if (gray[i][j] > 255) {
                gray[i][j] = 255; // clip to 255 if value exceeds 255
            }
        }
    }

    for(int i=0; i<w; i++) {
        for(int j=0; j<h; j++) {
            int x=i; int y=(h-1)-j;
            canvas[(x+y*w)*3+2] = (unsigned char)(gray[i][j]);
            canvas[(x+y*w)*3+1] = (unsigned char)(gray[i][j]);
            canvas[(x+y*w)*3+0] = (unsigned char)(gray[i][j]);
        }
    }

    unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
    unsigned char bmppad[3] = {0,0,0};

    bmpfileheader[ 2] = (unsigned char)(filesize    );
    bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
    bmpfileheader[ 4] = (unsigned char)(filesize>>16);
    bmpfileheader[ 5] = (unsigned char)(filesize>>24);

    bmpinfoheader[ 4] = (unsigned char)(       w    );
    bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
    bmpinfoheader[ 6] = (unsigned char)(       w>>16);
    bmpinfoheader[ 7] = (unsigned char)(       w>>24);
    bmpinfoheader[ 8] = (unsigned char)(       h    );
    bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
    bmpinfoheader[10] = (unsigned char)(       h>>16);
    bmpinfoheader[11] = (unsigned char)(       h>>24);

    f = fopen(filename.c_str(), "wb");
    fwrite(bmpfileheader, 1, 14, f);
    fwrite(bmpinfoheader, 1, 40, f);

    for(int i=0; i<h; i++) {
        fwrite(canvas+(w*(h-i-1)*3),3,w,f);
        fwrite(bmppad,1,(4-(w*3)%4)%4,f);
    }

    free(canvas);
    fclose(f);
}

void gray_image::paint() {
    // TODO: KG has to do this
}

/*------------------------------------------------------------------------
 * Operators
 *------------------------------------------------------------------------*/

gray_image gray_image::operator-(gray_image const& img) {

    assert(w == img.get_width());
    assert(h == img.get_height());

    gray_image new_img(h, w, 0);

    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            int val = gray[i][j] - img.get_pixel(i, j); 
            new_img.set_pixel(i, j, val);
        }
    }

    return new_img;
}

gray_image gray_image::operator+(gray_image const& img) {

    assert(w == img.get_width());
    assert(h == img.get_height());

    gray_image new_img(h, w, 0);

    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            int val = gray[i][j] + img.get_pixel(i, j); 
            new_img.set_pixel(i, j, val);
        }
    }

    return new_img;
}

gray_image gray_image::operator/(float const& val) {

    assert(val > 0);

    gray_image new_img(h, w, 0);

    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            int val = (int)gray[i][j]/val; 
            new_img.set_pixel(i, j, val);
        }
    }

    return new_img;
}

gray_image& gray_image::operator=(gray_image const& img) {
    /*
        params:
            img: image to be copied
    */

    if (this == &img) {
        return *this; // handle self assignment
    }

    for (int i=0; i<w; i++) {
        free(gray[i]);
    }

    free(gray);

    h = img.get_height();
    w = img.get_width();

    gray = (int **)malloc(w * sizeof(int *));

    for (int i=0; i<w; i++) {
        gray[i] = (int *)malloc(h * sizeof(int));
    }

    for(int i=0; i<w; i++) {
        for(int j=0; j<h; j++) {
            int pixel = img.get_pixel(i, j);
            if (pixel < 0) {
                pixel = 0; // clip to 0 if value is negative
            } else if (pixel > 255) {
                pixel = 255; // clip to 255 if value exceeds 255
            }
            gray[i][j] = pixel;
        }
    }

    return *this;
}

gray_image gray_image::operator*(gray_image const& img) {
    /*
        params:
            img: image to be multiplied
    */

    assert(w == img.get_width());
    assert(h == img.get_height());

    gray_image new_img(h, w, 0);

    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            int val = gray[i][j] * img.get_pixel(i, j); 
            new_img.set_pixel(i, j, val);
        }
    }

    return new_img;
}

gray_image gray_image::sqrt() {
    gray_image new_img(h, w, 0);

    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            int val = (int)std::sqrt(gray[i][j]); 
            new_img.set_pixel(i, j, val);
        }
    }

    return new_img;
}

/// @brief convolution operator: performs a stride 1 convolution with given kernel and no padding
/// @param kernel (height x width)
gray_image gray_image::operator*(std::vector<std::vector<float>> kernel) {

    gray_image new_img = conv(*this, kernel, 1, 0);

    return new_img;
}

/*------------------------------------------------------------------------
 * Image manipulation functions
 *------------------------------------------------------------------------*/

/// @brief Converts a gray-scale image to 2D std::vector
std::vector<std::vector<float>> gray_image::to_vector() {
    std::vector<std::vector<float>> vec(w, std::vector<float>(h));

    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            vec[i][j] = gray[i][j];
        }
    }

    return vec;
}

/**
 * @brief Blurs the image by simple averaging of neighbouring pixel values
 * The larger the kernel size, the more the blur
 * 
 * @param k 
 * @return gray_image 
 */
gray_image gray_image::blur(int k) {

    std::vector<std::vector<float>> kernel(k, std::vector<float>(k));

    for (int i=0; i<k; i++) {
        for (int j=0; j<k; j++) {
            kernel[i][j] = 1.0/(k*k); // Set all values to 1/(k*k) to get average on convolving
        }
    }

    gray_image new_img = conv(*this, kernel, 1, (k-1)/2.0); // same padding => (k-1)/2

    return new_img;
}

/**
 * @brief Sharpen the image by subtracting the blurred image from the original image, and adding the result to the original image
 *  The larger the kernel size, the more the sharpening
 * 
 * @param k kernel size
 * @return gray_image 
 */
gray_image gray_image::sharpen(int k) {

    gray_image blurred = blur(k);

    gray_image new_img = *this - blurred + *this;

    return new_img;
}

/**
 * @brief Sobel filter (https://en.wikipedia.org/wiki/Sobel_operator)
 * Detects edges in the image
 * @return gray_image 
 */
gray_image gray_image::sobel() {

    std::vector<std::vector<float>> kernel_x(3, std::vector<float>(3));
    std::vector<std::vector<float>> kernel_y(3, std::vector<float>(3));

    kernel_x[0][0] = 1; kernel_x[0][1] = 0; kernel_x[0][2] = -1;
    kernel_x[1][0] = 2; kernel_x[1][1] = 0; kernel_x[1][2] = -2;
    kernel_x[2][0] = 1; kernel_x[2][1] = 0; kernel_x[2][2] = -1;

    kernel_y[0][0] = 1; kernel_y[0][1] = 2; kernel_y[0][2] = 1;
    kernel_y[1][0] = 0; kernel_y[1][1] = 0; kernel_y[1][2] = 0;
    kernel_y[2][0] = -1; kernel_y[2][1] = -2; kernel_y[2][2] = -1;

    gray_image new_img_x = conv(*this, kernel_x, 1, 1);
    gray_image new_img_y = conv(*this, kernel_y, 1, 1);

    gray_image new_img = new_img_x + new_img_y;
    // vec_x = new_img_x.to_vector(); vec_y = new_img_y.to_vector();
    // gray_image new_img = to_gray_image(vec_sqrt(vec_x*vec_x + vec_y*vec_y)); 

    return new_img;
}

/**
 * @brief Pixelate the image by replacing each pixel with the maximum value of the neighbouring pixels
 * The larger the kernel size, the more the pixelation
 * 
 * @param k kernel size
 * @return gray_image 
 */
gray_image gray_image::pixelate(int k) {

    gray_image new_img(h, w, 0);

    // We basically split the image into k*k blocks, and replace each pixel in the block with the maximum value of the block
    for (int i=0; i<w; i+=k) { 
        for (int j=0; j<h; j+=k) {
            int max_val = 0;

            // Finding max value in a block
            for (int m=0; m<k; m++) {
                for (int n=0; n<k; n++) {
                    int x = i + m; 
                    int y = j + n; 
                    // x and y when m and n are 0 -> where bottom left corner of kernel is placed
                    if (x >= 0 && x < w && y >= 0 && y < h) {
                        max_val = max(max_val, gray[x][y]);
                    }
                }
            }

            // Setting all pixels in the block to max value
            for (int m=0; m<k; m++) {
                for (int n=0; n<k; n++) {
                    int x = i + m; 
                    int y = j + n; 
                    // x and y when m and n are 0 -> where bottom left corner of kernel is placed
                    if (x >= 0 && x < w && y >= 0 && y < h) {
                        new_img.set_pixel(x, y, max_val);
                    }
                }
            }
        }
    }

    return new_img;
}

/// @brief Invert the colors of the image
gray_image gray_image::invert() {
    gray_image new_img(h, w, 255);

    return new_img - *this;
}

/**
 * @brief Add gaussian noise to the image
 * The larger the variance, the more the noise
 * 
 * @param var 
 * @return gray_image 
 */
gray_image gray_image::noise(float var) {

    gray_image new_img(h, w, 0);

    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0.0, var);

    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            int val = gray[i][j] + distribution(generator);
            new_img.set_pixel(i, j, val);
        }
    }

    return new_img;
}

/**
 * @brief Changes an image to just black and white
 * 
 * @param thr 
 * @return gray_image 
 */
gray_image gray_image::bnw(int thr) {
    gray_image new_img(h, w, 0);

    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            if (gray[i][j] > thr) {
                new_img.set_pixel(i, j, 255);
            } else {
                new_img.set_pixel(i, j, 0);
            }
        }
    }
    return new_img;
}

/// @brief Horizontal flip
gray_image gray_image::hflip() {
    gray_image new_img(h, w, 0);

    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            new_img.set_pixel(i, j, gray[w-i-1][j]);
        }
    }

    return new_img;
}

/// @brief Vertical flip
gray_image gray_image::vflip() {
    gray_image new_img(h, w, 0);

    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            new_img.set_pixel(i, j, gray[i][h-j-1]);
        }
    }

    return new_img;
}

/// @brief Transpose (Equivalent to rotating the image by 90 degrees clockwise)
gray_image gray_image::T() {
    gray_image new_img(w, h, 0);

    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            new_img.set_pixel(j, i, gray[i][j]);
        }
    }

    return new_img;
}

/*------------------------------------------------------------------------
 * Destructor
 *------------------------------------------------------------------------*/

gray_image::~gray_image() {
    for (int i=0; i<w; i++) {
        free(gray[i]);
    }

    free(gray);
}


///////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Convolves a gray scale image with a given kernel
 * 
 * @param img 
 * @param kernel 
 * @param stride 
 * @param padding 
 * @return gray_image 
 */
gray_image conv(gray_image &img, std::vector< std::vector<float> > kernel, int stride, float padding) {

    int h = img.get_height();
    int w = img.get_width();

    int k_h = kernel.size();
    int k_w = kernel[0].size();

    int new_h = (h - k_h + 2*padding)/stride + 1;
    int new_w = (w - k_w + 2*padding)/stride + 1;

    gray_image new_img(new_h, new_w, 0);

    for (int i=0; i<new_w; i++) {
        for (int j=0; j<new_h; j++) {
            float sum = 0;
            for (int m=0; m<k_w; m++) {
                for (int n=0; n<k_h; n++) {
                    int x = i*stride + m - padding;
                    int y = j*stride + n - padding;

                    // x and y when m and n are 0 -> where bottom left corner of kernel is placed
                    if (x >= 0 && x < w && y >= 0 && y < h) {
                        sum += img.get_pixel(x, y) * kernel[k_h - n - 1][m];
                        // sum += kernel[k][l]*img.get_pixel(i*stride + l, j*stride + (k_h - k - 1));
                    }
                }
            }
            new_img.set_pixel(i, j, (int)sum);
        }
    }

    return new_img;
}

// Converts 2D std::vector of floats to gray-scale image
gray_image to_gray_image(std::vector< std::vector<float> > vec) {
    /*
        params:
            vec: 2D std::vector of floats (of dimension (width x height))
    */

    int w = vec.size();
    int h = vec[0].size();

    gray_image img(h, w, 0);

    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            img.set_pixel(i, j, (int)vec[i][j]);
        }
    }

    return img;
}