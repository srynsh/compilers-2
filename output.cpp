#include "image.hpp"
#include "kernel.hpp"
#include "load_bmp.hpp"
#include "turtle.hpp"
#include <bits/stdc++.h>

using namespace std;

image blur(image i) { return i.blur(30); }

int main() {
  image a("./images/inputs/blackbuck.bmp");
  a = blur(a);
  a.paint();
  a.frame("./lesgooooo.bmp");
}