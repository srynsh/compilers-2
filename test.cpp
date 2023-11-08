#include <bits/stdc++.h>

using namespace std;

float a(int a, float b) {
    return 0.0;
}

int main() {
    int* a = new int(69);
    a[3] = 7;
    cout << sizeof(a) << endl;
    cout << a[1+2] << endl;
    cout << *a << endl;
}
