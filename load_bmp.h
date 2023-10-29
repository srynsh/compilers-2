#pragma once
#include <bits/stdc++.h>
using namespace std;

#pragma pack(1)

void RGB_Allocate(unsigned char**& dude, int rows, int cols);

bool FillAndAllocate(char*& buffer, const char* Picture, int& rows, int& cols, int& BufferSize);

void GetPixlesFromBMP24(unsigned char** reds, unsigned char** greens, unsigned char** blues, int end, int rows, int cols, char* FileReadBuffer);

void WriteOutBmp24(char* FileBuffer, const char* NameOfFileToCreate, int BufferSize, int rows, int cols, unsigned char** reds, unsigned char** greens, unsigned char** blues);
