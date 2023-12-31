#include <bits/stdc++.h>

using namespace std;

#pragma pack(1)

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct tagBITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

uint8_t** reds;
uint8_t** greens;
uint8_t** blues;
int rows;
int cols;

void RGB_Allocate(unsigned char**& dude, int rows, int cols) {
    dude = new uint8_t*[cols];
     
    for (int i = 0; i < cols; i++) {
        dude[i] = new uint8_t[rows];
    }
}

bool FillAndAllocate(char*& buffer, const char* Picture, int& rows, int& cols, int& BufferSize) { //Returns 1 if executed sucessfully, 0 if not sucessfull
    ifstream file(Picture);

    if (file) {
        file.seekg(0, ios::end);
        streampos length = file.tellg();
        file.seekg(0, ios::beg);

        buffer = new char[length];
        file.read(&buffer[0], length);

        PBITMAPFILEHEADER file_header;
        PBITMAPINFOHEADER info_header;

        file_header = (PBITMAPFILEHEADER) (&buffer[0]);
        info_header = (PBITMAPINFOHEADER) (&buffer[0] + sizeof(BITMAPFILEHEADER));
        rows = info_header->biHeight;
        cols = info_header->biWidth;
        BufferSize = file_header->bfSize;
        
        // print the header
        for (int i = 0; i < 54; i++) {
            cout << (int)buffer[i];
        }

        cout << endl;

        return 1;
    } else {
        cout << "File" << Picture << " don't Exist!" << endl;
        return 0;
    }
}

void GetPixlesFromBMP24(unsigned char** reds, unsigned char** greens, unsigned char** blues, int end, int rows, int cols, char* FileReadBuffer) { // end is BufferSize (total size of file)
    int count = 1;
    int extra = cols % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.
    
    for (int i = 0; i < rows; i++){
        count += extra;
        
        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                    case 0:
                        reds[j][i] = FileReadBuffer[end - count++];
                        break;
                    case 1:
                        greens[j][i] = FileReadBuffer[end - count++];
                        break;
                    case 2:
                        blues[j][i] = FileReadBuffer[end - count++];
                        break;
                }
            }
        }
    }
}

void WriteOutBmp24(char* FileBuffer, const char* NameOfFileToCreate, int BufferSize, int rows, int cols, unsigned char** reds, unsigned char** greens, unsigned char** blues) {
    ofstream write(NameOfFileToCreate);

    if (!write) {
        cout << "Failed to write " << NameOfFileToCreate << endl;
        return;
    }

    int count = 1;
    int extra = cols % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.

    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                    case 0: //reds
                        FileBuffer[BufferSize - count] = reds[j][i];
                        break;
                    case 1: //green
                        FileBuffer[BufferSize - count] = greens[j][i];
                        break;
                    case 2: //blue
                        FileBuffer[BufferSize - count] = blues[j][i];
                        break;
                }

                count++;
            }
        }
    }

    write.write(FileBuffer, BufferSize);
}

void GetPixlesFromBMP8(unsigned char** gray, int end, int rows, int cols, char* FileReadBuffer) { // end is BufferSize (total size of file)
    int count = 1;
    int extra = cols % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.

    // print the header
    for (int i = 0; i < 54; i++) {
        cout << (int)FileReadBuffer[i];
    }
    cout << endl;
    
    for (int i = 0; i < rows; i++){
        count += extra;
        
        for (int j = cols - 1; j >= 0; j--) {
            gray[j][i] = FileReadBuffer[end - count++];
        }
    }
}

void WriteOutBmp8(char* FileBuffer, const char* NameOfFileToCreate, int BufferSize, int rows, int cols, unsigned char** gray) {
    ofstream write(NameOfFileToCreate);

    if (!write) {
        cout << "Failed to write " << NameOfFileToCreate << endl;
        return;
    }

    int count = 1;
    int extra = cols % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.


    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = cols - 1; j >= 0; j--) {
            FileBuffer[BufferSize - count] = gray[j][i];
            count++;
        }
    }

    write.write(FileBuffer, BufferSize);
}