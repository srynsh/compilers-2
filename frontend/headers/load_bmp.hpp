#pragma once
#include <bits/stdc++.h>
using namespace std;

#ifndef LOAD_BMP_HPP
#define LOAD_BMP_HPP

// #pragma pack(1)
#pragma pack(push, 1) 

void RGB_Allocate(unsigned char**& dude, int rows, int cols);

bool FillAndAllocate(char*& buffer, const char* Picture, int& rows, int& cols, int& BufferSize);

void GetPixlesFromBMP24(unsigned char** reds, unsigned char** greens, unsigned char** blues, int end, int rows, int cols, char* FileReadBuffer);

void WriteOutBmp24(char* FileBuffer, const char* NameOfFileToCreate, int BufferSize, int rows, int cols, unsigned char** reds, unsigned char** greens, unsigned char** blues);

void WriteOutBmp8(char* FileBuffer, const char* NameOfFileToCreate, int BufferSize, int rows, int cols, unsigned char** gray);

void GetPixlesFromBMP8(unsigned char** gray, int end, int rows, int cols, char* FileReadBuffer);

#pragma pack(pop) 

#endif // LOAD_BMP_HPP
