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

int main() {
    image img(200, 100, 0xffffff);
    // img.frame("test.bmp");
    // int h = img.get_height();
    // int w = img.get_width();

    // vector<int> v;
    // v.push_back(10);
    // v.push_back(w/5);
    // v.push_back(h/2);

    // cout << "h: " << h << endl;
    // cout << "w: " << w << endl;

    // img.draw("circle", v);

    // img.frame("test2.bmp");

    img.load("./greenland_grid_velo.bmp");
    img.frame("test3.bmp");

    return 0;
}