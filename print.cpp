#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

int main(){

    ifstream ifs;
    ofstream ofs;
    char input[80] = "input.bmp";
    char output[80] = "output.txt";

    // cout<<"Input file name"<<endl;
    // cin>>input;
    ifs.open(input, ios::binary);

    if(!ifs)
    {
        cout<<"Error in opening file"<<endl;
        system("pause");
        return 0;
    }

    // cout<<"Output file name"<<endl;
    // cin>>output;
    ofs.open(output, ios::binary);

    ifs.seekg(2);

    int file_size;
    ifs.read((char*)&file_size, sizeof(int));

    ofs<<"Bitmap size: "<<file_size<<"\r\n";

    ifs.seekg(10);
    int beg;
    ifs.read((char*)&beg, sizeof(int));

    ofs<<"Beggining of image: "<<beg<<"\r\n";

    ifs.seekg(18);
    int columns;
    ifs.read((char*)&columns, sizeof(int));

    ofs<<"Column number: "<<columns<<"\r\n";

    ifs.seekg(22);
    int rows;
    ifs.read((char*)&rows, sizeof(int));

    ofs<<"Row number: "<<rows<<"\r\n";

    int image_size=0;
    columns+=(3*columns)%4;
    image_size=3*columns*rows;

    ofs<<"Size of image"<<image_size<<"\r\n";

    ifs.seekg(beg);

    unsigned char R,G,B;
    int R1,G1,B1;
    // make an array of int to store the values of R,G,B on heap
    int *R_arr = new int[image_size];
    int *G_arr = new int[image_size];
    int *B_arr = new int[image_size];

    for(int i=0; i<image_size; i+=3)
    {
        ifs.read((char*)&B, sizeof(unsigned char));
        ifs.read((char*)&G, sizeof(unsigned char));
        ifs.read((char*)&R, sizeof(unsigned char));

        R_arr[i] = (int)R;
        G_arr[i] = (int)G;
        B_arr[i] = (int)B;

        R1 = (int)R;
        G1 = (int)G;
        B1 = (int)B;

        ofs<<"R: "<<R1<<" G: "<<G1<<" B: "<<B1<<"  position in file: "<<ifs.tellg()<<"\r\n";
    }

    int avgR=0, avgG=0, avgB=0;

    for (int i=image_size;i>0;i-=9){
        // printf("\e[48;2;%d;%d;%dm \e[0m",R_arr[i*columns+j],G_arr[i*columns+j],B_arr[i*columns+j]);
        // printf("\e[48;2;%d;%d;%dm \e[0m",R_arr[i],G_arr[i],B_arr[i]);
        avgR = (R_arr[i] + R_arr[i-3] + R_arr[i-6])/3;
        avgG = (G_arr[i] + G_arr[i-3] + G_arr[i-6])/3;
        avgB = (B_arr[i] + B_arr[i-3] + B_arr[i-6])/3;
        printf("\e[48;2;%d;%d;%dm \e[0m",avgR,avgG,avgB);
    }
    // printf("\n");
}