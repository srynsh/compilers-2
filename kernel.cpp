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