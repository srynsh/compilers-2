#include <bits/stdc++.h>

using namespace std;

#ifndef KERNEL_H
#define KERNEL_H

vector<vector<float>> operator+(vector<vector<float>> const& vec1, vector<vector<float>> const& vec2);
vector<vector<float>> operator-(vector<vector<float>> const& vec1, vector<vector<float>> const& vec2);
vector<vector<float>> operator*(vector<vector<float>> const& vec1, vector<vector<float>> const& vec2);
vector<vector<float>> operator/(vector<vector<float>> const& vec1, vector<vector<float>> const& vec2);
vector<vector<float>> vec_sqrt(vector<vector<float>> const& vec);

#endif