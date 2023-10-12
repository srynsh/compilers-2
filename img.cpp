#include <bits/stdc++.h>

using namespace std;

unsigned char *canvas = NULL;
int **red;
int **green;
int **blue;
int w, h;

void draw_circle(int cx, int cy, int radius, int colour) {
    int red_c = ((colour >> 16) & 0xFF);  // Extract the RR byte
    int green_c = ((colour >> 8) & 0xFF);   // Extract the GG byte
    int blue_c = ((colour) & 0xFF);
    
    for (int i = 0; i<w; i++) {
        for (int j = 0; j<h; j++) {
            if ((i - cx)*(i - cx) + (j - cy)*(j - cy) < radius*radius) {
                red[i][j] = red_c;
                green[i][j] = green_c;
                blue[i][j] = blue_c;
            }
        }
    }
}

int main() {
    w = 600;
    h = 600;

    int cx = 300, cy=300; int r = 50;

    int start_x = 10, start_y = 20;
    int end_x = 40, end_y = 30;

    FILE *f;
    int filesize = 54 + 3*w*h;  //w is your image width, h is image height, both int

    canvas = (unsigned char *)malloc(3*w*h);
    memset(canvas,0,3*w*h);

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
            red[i][j] = 255;
            green[i][j] = 255;
            blue[i][j] = 255;
        }
    }

    float epsilon = 0.01;

    draw_circle(300, 300, 50, 0xff0000);

    for(int i=0; i<w; i++) {
        for(int j=0; j<h; j++) {
            int x=i; int y=(h-1)-j;
            int r = red[i][j];
            int g = green[i][j];
            int b = blue[i][j];
            canvas[(x+y*w)*3+2] = (unsigned char)(r);
            canvas[(x+y*w)*3+1] = (unsigned char)(g);
            canvas[(x+y*w)*3+0] = (unsigned char)(b);
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

    f = fopen("img.bmp","wb");
    fwrite(bmpfileheader,1,14,f);
    fwrite(bmpinfoheader,1,40,f);
    for(int i=0; i<h; i++) {
        fwrite(canvas+(w*(h-i-1)*3),3,w,f);
        fwrite(bmppad,1,(4-(w*3)%4)%4,f);
    }

    free(canvas);
    fclose(f);

}