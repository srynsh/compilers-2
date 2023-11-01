#include <bits/stdc++.h>

using namespace std;

#ifndef KERNEL_H
#define KERNEL_H

vector<vector<float>> operator+(vector<vector<float>> const& vec1, vector<vector<float>> const& vec2);
vector<vector<float>> operator-(vector<vector<float>> const& vec1, vector<vector<float>> const& vec2);
vector<vector<float>> operator*(vector<vector<float>> const& vec1, vector<vector<float>> const& vec2);
vector<vector<float>> operator/(vector<vector<float>> const& vec1, vector<vector<float>> const& vec2);
vector<vector<float>> vec_sqrt(vector<vector<float>> const& vec);
bool check_valid_matrix(string s, int dim, int x=0, int y=0, int z=0);
vector<float> string_to_vec1d(string s);
vector<vector<float>> string_to_vec2d(string s);
vector<vector<vector<float>>> string_to_vec3d(string s);

#endif