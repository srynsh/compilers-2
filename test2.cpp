#include <bits/stdc++.h>

using namespace std;

void a(string* s){
    string str = "hi";
    *s = str;
}

int main(){
    string *s;
    a(s);
    std::cout << *s << std::endl;
}