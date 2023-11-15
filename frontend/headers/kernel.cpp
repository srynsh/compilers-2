#include <bits/stdc++.h>

using namespace std;
/* Operator overloading for vectors */

// + operator
vector<vector<float>> operator+(vector<vector<float>> const& vec1, vector<vector<float>> const& vec2) {
    /*
        params:
            vec1: 2D vector of floats (of dimension (width x height))
            vec2: 2D vector of floats (of dimension (width x height))
    */

    assert(vec1.size() == vec2.size());
    assert(vec1[0].size() == vec2[0].size());

    int w = vec1.size();
    int h = vec1[0].size();

    vector<vector<float>> new_vec(w, vector<float>(h));

    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            new_vec [i][j] = vec1[i][j] + vec2[i][j];
        }
    }

    return new_vec;
}

// - operator
vector<vector<float>> operator-(vector<vector<float>> const& vec1, vector<vector<float>> const& vec2) {
    /*
        params:
            vec1: 2D vector of floats (of dimension (width x height))
            vec2: 2D vector of floats (of dimension (width x height))
    */

    assert(vec1.size() == vec2.size());
    assert(vec1[0].size() == vec2[0].size());

    int w = vec1.size();
    int h = vec1[0].size();

    vector<vector<float>> new_vec(w, vector<float>(h));

    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            new_vec [i][j] = vec1[i][j] - vec2[i][j];
        }
    }

    return new_vec;
}


// * operator
vector<vector<float>> operator*(vector<vector<float>> const& vec1, vector<vector<float>> const& vec2) {
    /*
        params:
            vec1: 2D vector of floats (of dimension (width x height))
            vec2: 2D vector of floats (of dimension (width x height))
    */

    assert(vec1.size() == vec2.size());
    assert(vec1[0].size() == vec2[0].size());

    int w = vec1.size();
    int h = vec1[0].size();

    vector<vector<float>> new_vec(w, vector<float>(h));

    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            new_vec [i][j] = vec1[i][j] * vec2[i][j];
        }
    }

    return new_vec;
}

// / operator
vector<vector<float>> operator/(vector<vector<float>> const& vec1, vector<vector<float>> const& vec2) {
    /*
        params:
            vec1: 2D vector of floats (of dimension (width x height))
            vec2: 2D vector of floats (of dimension (width x height))
    */

    assert(vec1.size() == vec2.size());
    assert(vec1[0].size() == vec2[0].size());

    int w = vec1.size();
    int h = vec1[0].size();

    vector<vector<float>> new_vec(w, vector<float>(h));

    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            new_vec [i][j] = vec1[i][j] / vec2[i][j];
        }
    }

    return new_vec;
}

// sqrt function
vector<vector<float>> vec_sqrt(vector<vector<float>> const& vec) {
    /*
        params:
            vec: 2D vector of floats (of dimension (width x height))
    */

    int w = vec.size();
    int h = vec[0].size();

    vector<vector<float>> new_vec(w, vector<float>(h));

    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            new_vec [i][j] = std::sqrt(vec[i][j]);
        }
    }

    return new_vec;
}

// Checks if a string of braces and numbers can form a valid x/x*y/x*y*z matrix
bool check_valid_matrix(string s, int dim, int x, int y, int z) {
    assert (dim == 1 || dim == 2 || dim == 3);
    assert ( (dim == 1 && x > 0) || (dim == 2 && x > 0 && y > 0) || (dim == 3 && x > 0 && y > 0 && z > 0) );

    int n = s.length();

    int cur_dim = 0, max_dim = 0;
    int dim_list[3] = {0, 0, 0};
    
    for(auto c : s) {

        if (c == '{') {
            cur_dim++;
            max_dim = max(max_dim, cur_dim);
            if (cur_dim > dim) 
                return false;
        }

        else if (c == '}') {
            if (cur_dim == 1) {
                if (dim_list[0] != x-1) 
                    return false;
            }
            else if (cur_dim == 2) {
                if (dim_list[1] != y-1) 
                    return false;
            }
            else if (cur_dim == 3) {
                if (dim_list[2] != z-1) 
                    return false;
            }
            dim_list[cur_dim-1] = 0;
            cur_dim--;
        }

        else if (c == ',') 
            dim_list[cur_dim-1]++;
        
        else 
            continue;
    }

    if (max_dim != dim) 
        return false;
    else
        return true;
}