---
title: "README"
author: "Team 4" 
---

### INTRODUCTION
Our language allows users to play and manipulate images. This is useful for making some flowcharts and diagrams. The language is designed to be easy to use and takes the best features from other languages like c++, python and rust. If we notice, we see that that there are not many image manipulation languages. Also they are often very complex and hard to use. Our language is designed to be easy to use and learn. It is also very powerful and can be used to make some very complex images. 

### DIFFICULTIES FACED
-- We faced a lot of while loading the images because we were making a general bmp loading function for all types of bmp headers.
-- We also faced a lot of difficulties while implementing the functions for the image manipulation. Beacause they were often very complex and had high time complexity. We thereofore parallelized the code to make it faster.
-- We faced some trouble while doing semantic analysis on multidiemnsional arrays using only SAG grammar.
-- Making the videos play on the terminal was also a challenge. We tried to implement it from scratch but this time was simply not possible.  

### HOW TO RUN

To run the videos and print on the terminal, you need to install the following packages:
```bash
$ cd TerminalImageViewer/src
$ make
$ sudo make install
```


```bash
$ cd codes/compilers-2/frontend
$ make clean
$ make all
$ ./bin/output < {path to input file}
$ cd ../
$ g++ -g -std=c++17 -o output output.cpp frontend/headers/image.cpp frontend/headers/kernel.cpp frontend/headers/load_bmp.cpp frontend/headers/turtle.cpp && ./output
```

This will run the code. It will also save all the files in the current working directory. 