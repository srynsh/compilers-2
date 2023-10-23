#include <bits/stdc++.h>

using namespace std;



class image {
    private:
        int h;
        int w;

        int **red;
        int **green;
        int **blue;

    public:
        image(int w, int h, int color) {
            assert(h > 0 && w > 0);
            assert(h < 1000 && w < 1000);
            assert(color >= 0x000000 && color <= 0xFFFFFF);

            int red_c = ((color >> 16) & 0xFF);  // Extract the RR byte
            int green_c = ((color >> 8) & 0xFF);   // Extract the GG byte
            int blue_c = ((color) & 0xFF);

            this->h = h;
            this->w = w;

            red = (int **)malloc(h * sizeof(int *));
            green = (int **)malloc(h * sizeof(int *));
            blue = (int **)malloc(h * sizeof(int *));

            for (int i=0; i<h; i++) {
                red[i] = (int *)malloc(w * sizeof(int));
                green[i] = (int *)malloc(w * sizeof(int));
                blue[i] = (int *)malloc(w * sizeof(int));
            }

            for(int i=0; i<h; i++) {
                for(int j=0; j<w; j++) {
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
                for (int i=0; i<h; i++) {
                    free(red[i]);
                    free(green[i]);
                    free(blue[i]);
                }

                free(red);
                free(green);
                free(blue); 
            }    

            h = *(int*)&info[18];
            w = *(int*)&info[22];

            try {
                assert(h > 0 && w > 0);
            } catch (const std::exception& e) {
                cout << "Error: Incorrect file format or corrupted file" << e.what() << endl;
                return;
            }

            try {
                assert(h < 1000 && w < 1000);
            } catch (const std::exception& e) {
                cout << "Error: Image size too large" << e.what() << endl;
                return;
            }

            int size = 3 * h * w;
            unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
            fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
            fclose(f);

            red = (int **)malloc(h * sizeof(int *));
            green = (int **)malloc(h * sizeof(int *));
            blue = (int **)malloc(h * sizeof(int *));

            for (int i=0; i<h; i++) {
                red[i] = (int *)malloc(w * sizeof(int));
                green[i] = (int *)malloc(w * sizeof(int));
                blue[i] = (int *)malloc(w * sizeof(int));
            }

            for(int i=0; i<h; i++) {
                for(int j=0; j<w; j++) {
                    red[i][j] = data[(i+j*h)*3+2];
                    green[i][j] = data[(i+j*h)*3+1];
                    blue[i][j] = data[(i+j*h)*3+0];
                }
            }

            delete[] data;
        }

        void frame(string filename) {
            FILE *f;
            int filesize = 54 + 3*h*w;  //w is your image width, h is image height, both int
            unsigned char *canvas = NULL;
            canvas = (unsigned char *)malloc(3*h*w);
            memset(canvas,0,3*h*w);

            for(int i=0; i<h; i++) {
                for(int j=0; j<w; j++) {
                    int x=i; int y=(w-1)-j;
                    canvas[(x+y*h)*3+2] = (unsigned char)(red[i][j]);
                    canvas[(x+y*h)*3+1] = (unsigned char)(green[i][j]);
                    canvas[(x+y*h)*3+0] = (unsigned char)(blue[i][j]);
                }
            }

            unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
            unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
            unsigned char bmppad[3] = {0,0,0};

            bmpfileheader[ 2] = (unsigned char)(filesize    );
            bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
            bmpfileheader[ 4] = (unsigned char)(filesize>>16);
            bmpfileheader[ 5] = (unsigned char)(filesize>>24);

            bmpinfoheader[ 4] = (unsigned char)(       h    );
            bmpinfoheader[ 5] = (unsigned char)(       h>> 8);
            bmpinfoheader[ 6] = (unsigned char)(       h>>16);
            bmpinfoheader[ 7] = (unsigned char)(       h>>24);
            bmpinfoheader[ 8] = (unsigned char)(       w    );
            bmpinfoheader[ 9] = (unsigned char)(       w>> 8);
            bmpinfoheader[10] = (unsigned char)(       w>>16);
            bmpinfoheader[11] = (unsigned char)(       w>>24);

            f = fopen(filename.c_str(), "wb");
            fwrite(bmpfileheader, 1, 14, f);
            fwrite(bmpinfoheader, 1, 40, f);

            for(int i=0; i<w; i++) {
                fwrite(canvas+(h*(w-i-1)*3),3,h,f);
                fwrite(bmppad,1,(4-(h*3)%4)%4,f);
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

        ~image() {
            for (int i=0; i<h; i++) {
                free(red[i]);
                free(green[i]);
                free(blue[i]);
            }

            free(red);
            free(green);
            free(blue);
        }
};

int main() {
    image img(100, 100, 0x000000);
    img.frame("test.bmp");
    img.load("./img.bmp");
    img.frame("test2.bmp");
    return 0;
}