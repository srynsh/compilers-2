#include <bits/stdc++.h>
#include "kernel.h"

/* To run: g++ --std=c++11 -o image kernel.cpp image.cpp */

using namespace std;

// Forward declaration of classes
class gray_image;

// Forward declaration of non-member functions
gray_image conv(gray_image &img, vector<vector<float>> kernel, int stride, float padding);
gray_image to_gray_image(vector<vector<float>> vec);


class image {
    private:
        int h;
        int w;

        int **red;
        int **green;
        int **blue;

    public:
        image(int h, int w, int color) {
            assert(w > 0 && h > 0);
            assert(w < 1000 && h < 1000);
            assert(color >= 0x000000 && color <= 0xFFFFFF);

            int red_c = ((color >> 16) & 0xFF);  // Extract the RR byte
            int green_c = ((color >> 8) & 0xFF);   // Extract the GG byte
            int blue_c = ((color) & 0xFF);

            this->w = w;
            this->h = h;

            red = (int **)malloc(w * sizeof(int *));
            green = (int **)malloc(w * sizeof(int *));
            blue = (int **)malloc(w * sizeof(int *));

            for (int i=0; i<w; i++) {
                red[i] = (int *)malloc(h * sizeof(int));
                green[i] = (int *)malloc(h * sizeof(int));
                blue[i] = (int *)malloc(h * sizeof(int));
            }

            for(int i=0; i<w; i++) {
                for(int j=0; j<h; j++) {
                    red[i][j] = red_c;
                    green[i][j] = green_c;
                    blue[i][j] = blue_c;
                }
            }
        }

        image(string filename) {
            load(filename, true);
        }

        void load(string filename, bool init=false) {
            FILE *f;
            unsigned char info[54];
            f = fopen(filename.c_str(), "rb");
            fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

            if (!init) {    
                for (int i=0; i<w; i++) {
                    free(red[i]);
                    free(green[i]);
                    free(blue[i]);
                }

                free(red);
                free(green);
                free(blue); 
            }    
            cout << *(long*)&info[18]<<endl;
            w = *(int*)&info[18];
            h = *(int*)&info[22];

            cout << "w: " << w << endl;
            cout << "h: " << h << endl;

            try {
                assert(w > 0 && h > 0);
            } catch (const std::exception& e) {
                cout << "Error: Incorrect file format or corrupted file" << e.what() << endl;
                return;
            }

            try {
                assert(w < 1400 && h < 1400);
            } catch (const std::exception& e) {
                cout << "Error: Image size too large" << e.what() << endl;
                return;
            }

            int size = 3 * w * h;
            unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
            fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
            fclose(f);

            red = (int **)malloc(w * sizeof(int *));
            green = (int **)malloc(w * sizeof(int *));
            blue = (int **)malloc(w * sizeof(int *));

            for (int i=0; i<w; i++) {
                red[i] = (int *)malloc(h * sizeof(int));
                green[i] = (int *)malloc(h * sizeof(int));
                blue[i] = (int *)malloc(h * sizeof(int));
            }

            for(int i=0; i<w; i++) {
                for(int j=0; j<h; j++) {
                    red[i][j] = data[(i+j*w)*3+2];
                    green[i][j] = data[(i+j*w)*3+1];
                    blue[i][j] = data[(i+j*w)*3+0];
                }
            }

            delete[] data;
        }

        void frame(string filename) {
            FILE *f;
            int filesize = 54 + 3*w*h;  //w is your image width, h is image height, both int
            unsigned char *canvas = NULL;
            canvas = (unsigned char *)malloc(3*w*h);
            memset(canvas,0,3*w*h);

            for(int i=0; i<w; i++) {
                for(int j=0; j<h; j++) {
                    int x=i; int y=(h-1)-j;
                    canvas[(x+y*w)*3+2] = (unsigned char)(red[i][j]);
                    canvas[(x+y*w)*3+1] = (unsigned char)(green[i][j]);
                    canvas[(x+y*w)*3+0] = (unsigned char)(blue[i][j]);
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

        void paint() {
            // TODO: KG has to do this
        }

        void draw(string shape, vector<int> params) {
            if (shape == "circle") {
                int r = params[0];
                int cx = params[1];
                int cy = params[2];
                bool fill = params[3];

                for (int y=0; y<h; y++) {
                    for (int x=0; x<w; x++) {
                        
                        if ((x-cx)*(x-cx) + (y-cy)*(y-cy) <= r*r) {
                            red[x][y] = 0;
                            green[x][y] = 0;
                            blue[x][y] = 0;
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

        int get_height() {
            return h;
        }

        int get_width() {
            return w;
        }

        ~image() {
            for (int i=0; i<w; i++) {
                free(red[i]);
                free(green[i]);
                free(blue[i]);
            }

            free(red);
            free(green);
            free(blue);
        }
};

// Class for gray scale image
class gray_image {
    private:
    // Dimensions
    int h; // height in pixels
    int w; // width in pixels

    // Pixel values
    int **gray; // 2D array of gray pixel values. Range: 0-255

    public:

        /* Constructors */

        // Initializes an image of given dimensions with all pixels set to given value
        gray_image(int h, int w, int color) {
            /*
                params:
                    h: height in pixels
                    w: width in pixels
                    color: color of all pixels (0-255)
            */

            assert(w > 0 && h > 0);
            assert(w < 1000 && h < 1000);
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

        // Initializes an image from a given file (RGB -> Gray)
        gray_image(string filename) {
            /*
                params:
                    filename: path to the .bmp file
            */

            load(filename, true);
        }

        // Copy constructor
        gray_image(const gray_image &img) {
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
                    gray[i][j] = img.get_pixel(i, j);
                }
            }
        }

        /* Get stuff */

        int get_height() const {
            return h;
        }

        int get_width() const {
            return w;
        }

        // Returns the color of the pixel at (x, y)
        int get_pixel(int x, int y) const {
            /*
                params:
                    x: x coordinate of pixel
                    y: y coordinate of pixel
            */

            assert(x >= 0 && x < w);
            assert(y >= 0 && y < h);

            return gray[x][y];
        }

        /* Set stuff */

        // Sets the pixel at (x, y) to given color
        void set_pixel(int x, int y, int color) {
            /*
                params:
                    x: x coordinate of pixel
                    y: y coordinate of pixel
                    color: color of pixel (0-255)
            */

            assert(x >= 0 && x < w);
            assert(y >= 0 && y < h);
            assert(color >= 0 && color <= 255);

            gray[x][y] = color;
        }

        // Loads a .bmp in RGB format and converts it to gray scale
        void load(string filename, bool init=false) {
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
                assert(w < 1000 && h < 1000);
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

        /* Output */

        // Convert the image to a .bmp file, and save it
        void frame(string filename) {
            /*
                params:
                    filename: path to the .bmp file
            */

            FILE *f;
            int filesize = 54 + 3*w*h;  //w is your image width, h is image height, both int
            unsigned char *canvas = NULL;
            canvas = (unsigned char *)malloc(3*w*h);
            memset(canvas,0,3*w*h);

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

        void paint() {
            // TODO: KG has to do this
        }


        /* Operators */

        // - operator
        gray_image operator-(gray_image const& img) {
            /*
                params:
                    img: image to be subtracted
            */

            assert(w == img.get_width());
            assert(h == img.get_height());

            gray_image new_img(h, w, 0);

            for (int i=0; i<w; i++) {
                for (int j=0; j<h; j++) {
                    int val = gray[i][j] - img.get_pixel(i, j); 
                    if (val < 0) {
                        val = 0; // clip to 0 if value is negative
                    }
                    new_img.set_pixel(i, j, val);
                }
            }

            return new_img;
        }

        // + operator
        gray_image operator+(gray_image const& img) {
            /*
                params:
                    img: image to be added
            */

            assert(w == img.get_width());
            assert(h == img.get_height());

            gray_image new_img(h, w, 0);

            for (int i=0; i<w; i++) {
                for (int j=0; j<h; j++) {
                    int val = gray[i][j] + img.get_pixel(i, j); 
                    if (val > 255) {
                        val = 255; // clip to 255 if value exceeds 255
                    }
                    new_img.set_pixel(i, j, val);
                }
            }

            return new_img;
        }

        // / operator
        gray_image operator/(float const& val) {
            /*
                params:
                    val: value to be divided by
            */

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

        // = operator
        gray_image& operator=(gray_image const& img) {
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
                    gray[i][j] = img.get_pixel(i, j);
                }
            }

            return *this;
        }

        // * operator
        gray_image operator*(gray_image const& img) {
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
                    if (val > 255) {
                        val = 255; // clip to 255 if value exceeds 255
                    }
                    new_img.set_pixel(i, j, val);
                }
            }

            return new_img;
        }

        // sqrt operator
        gray_image sqrt() {
            gray_image new_img(h, w, 0);

            for (int i=0; i<w; i++) {
                for (int j=0; j<h; j++) {
                    int val = (int)std::sqrt(gray[i][j]); 
                    new_img.set_pixel(i, j, val);
                }
            }

            return new_img;
        }

        // convolution operator
        // performs a stride 1 convolution with given kernel and no padding
        gray_image operator*(vector<vector<float>> kernel) {
            /*
                params:
                    kernel: convolution kernel (height x width)
            */

            gray_image new_img = conv(*this, kernel, 1, 0);

            return new_img;
        }


        /* Different Filters and Functions */

        // Converts a gray-scale image to 2D vector
        vector<vector<float>> to_vector() {
            vector<vector<float>> vec(w, vector<float>(h));

            for (int i=0; i<w; i++) {
                for (int j=0; j<h; j++) {
                    vec[i][j] = gray[i][j];
                }
            }

            return vec;
        }

        // Blurs the image by simple averaging of neighbouring pixel values
        // The larger the kernel size, the more the blur
        gray_image blur(int k) {
            /*
                params:
                    k: kernel size
            */

            vector<vector<float>> kernel(k, vector<float>(k));

            for (int i=0; i<k; i++) {
                for (int j=0; j<k; j++) {
                    kernel[i][j] = 1.0/(k*k); // Set all values to 1/(k*k) to get average on convolving
                }
            }

            gray_image new_img = conv(*this, kernel, 1, (k-1)/2.0); // same padding => (k-1)/2

            return new_img;
        }

        // Sharpen the image by subtracting the blurred image from the original image, and adding the result to the original image
        // The larger the kernel size, the more the sharpening
        gray_image sharpen(int k) {
            /*
                params:
                    k: kernel size
            */

            gray_image blurred = blur(k);

            gray_image new_img = *this - blurred + *this;

            return new_img;
        }

        // Sobel filter (https://en.wikipedia.org/wiki/Sobel_operator)
        // Detects edges in the image
        gray_image sobel() {
            vector<vector<float>> kernel_x(3, vector<float>(3));
            vector<vector<float>> kernel_y(3, vector<float>(3));

            kernel_x[0][0] = 1; kernel_x[0][1] = 0; kernel_x[0][2] = -1;
            kernel_x[1][0] = 2; kernel_x[1][1] = 0; kernel_x[1][2] = -2;
            kernel_x[2][0] = 1; kernel_x[2][1] = 0; kernel_x[2][2] = -1;

            kernel_y[0][0] = 1; kernel_y[0][1] = 2; kernel_y[0][2] = 1;
            kernel_y[1][0] = 0; kernel_y[1][1] = 0; kernel_y[1][2] = 0;
            kernel_y[2][0] = -1; kernel_y[2][1] = -2; kernel_y[2][2] = -1;

            gray_image new_img_x = conv(*this, kernel_x, 1, 1);
            gray_image new_img_y = conv(*this, kernel_y, 1, 1);

            gray_image new_img = new_img_x + new_img_y;
            // vector<vector<float>> vec_x, vec_y; // Need to do this to prevent clipping
            // vec_x = new_img_x.to_vector(); vec_y = new_img_y.to_vector();
            // gray_image new_img = to_gray_image(vec_sqrt(vec_x*vec_x + vec_y*vec_y)); 


            return new_img;
        }

        // Pixelate the image by replacing each pixel with the maximum value of the neighbouring pixels
        // The larger the kernel size, the more the pixelation
        gray_image pixelate(int k) {
            /*
                params:
                    k: kernel size
            */

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

        // Invert the colors of the image
        gray_image invert() {
            gray_image new_img(h, w, 255);

            return new_img - *this;
        }

        // Add gaussian noise to the image
        // The larger the variance, the more the noise
        gray_image noise(float var) {
            /*
                params:
                    var: variance of the gaussian distribution
            */

            gray_image new_img(h, w, 0);

            std::default_random_engine generator;
            std::normal_distribution<double> distribution(0.0, var);

            for (int i=0; i<w; i++) {
                for (int j=0; j<h; j++) {
                    int val = gray[i][j] + distribution(generator);
                    if (val < 0) {
                        val = 0; // clip to 0 if value is negative
                    } else if (val > 255) {
                        val = 255; // clip to 255 if value exceeds 255
                    }
                    new_img.set_pixel(i, j, val);
                }
            }

            return new_img;
        }

        // Changes an image to just black and white
        gray_image bnw(int thr=100) {
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

        // Horizontal flip
        gray_image hflip() {
            gray_image new_img(h, w, 0);

            for (int i=0; i<w; i++) {
                for (int j=0; j<h; j++) {
                    new_img.set_pixel(i, j, gray[w-i-1][j]);
                }
            }

            return new_img;
        }

        // Vertical flip
        gray_image vflip() {
            gray_image new_img(h, w, 0);

            for (int i=0; i<w; i++) {
                for (int j=0; j<h; j++) {
                    new_img.set_pixel(i, j, gray[i][h-j-1]);
                }
            }

            return new_img;
        }

        // Transpose (Equivalent to rotating the image by 90 degrees clockwise)
        gray_image T() {
            gray_image new_img(w, h, 0);

            for (int i=0; i<w; i++) {
                for (int j=0; j<h; j++) {
                    new_img.set_pixel(j, i, gray[i][j]);
                }
            }

            return new_img;
        }

        /* Destructor */ 

        ~gray_image() {
            for (int i=0; i<w; i++) {
                free(gray[i]);
            }

            free(gray);
        }

};

// Convolution function
// Convolves a gray scale image with a given kernel
gray_image conv(gray_image &img, vector<vector<float>> kernel, int stride, float padding) {
    /*
        params:
            img: gray scale image
            kernel: convolution kernel (height x width)
            stride: stride of convolution
            padding: zero padding of convolution
    */

    int h = img.get_height();
    int w = img.get_width();

    vector<int> v;
    v.push_back(10);
    v.push_back(w/5);
    v.push_back(h/2);

    cout << "h: " << h << endl;
    cout << "w: " << w << endl;

    img.draw("circle", v);

    img.frame("test2.bmp");

    img.load("./test2.bmp");
    img.frame("test3.bmp");

    return 0;
}


