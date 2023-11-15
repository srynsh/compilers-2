#include <bits/stdc++.h>

using namespace std;

#ifndef KERNEL_HPP
#define KERNEL_HPP

vector<vector<float>> operator+(vector<vector<float>> const& vec1, vector<vector<float>> const& vec2);
vector<vector<float>> operator-(vector<vector<float>> const& vec1, vector<vector<float>> const& vec2);
vector<vector<float>> operator*(vector<vector<float>> const& vec1, vector<vector<float>> const& vec2);
vector<vector<float>> operator/(vector<vector<float>> const& vec1, vector<vector<float>> const& vec2);
vector<vector<float>> vec_sqrt(vector<vector<float>> const& vec);
bool check_valid_matrix(string s, int dim, int x=0, int y=0, int z=0);

#endif // KERNEL_HPP