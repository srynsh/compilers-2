#include <bits/stdc++.h>

using namespace std;

int main() {
    int* a = new int(69);
    a[3] = 7;
    cout << sizeof(a) << endl;
    cout << a[3] << endl;
    cout << *a << endl;
}
