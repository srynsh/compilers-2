#include <bits/stdc++.h>
#include <omp.h>
#include "kernel.hpp"

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

    #pragma omp parallel for collapse(2)
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

    #pragma omp parallel for collapse(2)
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

    #pragma omp parallel for collapse(2)
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

    #pragma omp parallel for collapse(2)
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

    #pragma omp parallel for collapse(2)
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

// for 1D vectors

// + operator
vector<float> operator+(vector<float> const& vec1, vector<float> const& vec2) {
    /*
        params:
            vec1: 1D vector of floats (of dimension (width))
            vec2: 1D vector of floats (of dimension (width))
    */

    assert(vec1.size() == vec2.size());


    int w = vec1.size();

    vector<float> new_vec(w);

    #pragma omp parallel for
    for (int i=0; i<w; i++) {
        new_vec [i] = vec1[i] + vec2[i];
    }

    return new_vec;
}

// - operator
vector<float> operator-(vector<float> const& vec1, vector<float> const& vec2) {
    /*
        params:
            vec1: 1D vector of floats (of dimension (width))
            vec2: 1D vector of floats (of dimension (width))
    */

    assert(vec1.size() == vec2.size());


    int w = vec1.size();

    vector<float> new_vec(w);

    #pragma omp parallel for
    for (int i=0; i<w; i++) {
        new_vec [i] = vec1[i] - vec2[i];
    }

    return new_vec;
}

// * operator
vector<float> operator*(vector<float> const& vec1, vector<float> const& vec2) {
    /*
        params:
            vec1: 1D vector of floats (of dimension (width))
            vec2: 1D vector of floats (of dimension (width))
    */

    assert(vec1.size() == vec2.size());


    int w = vec1.size();

    vector<float> new_vec(w);

    #pragma omp parallel for 
    for (int i=0; i<w; i++) {
        new_vec [i] = vec1[i] * vec2[i];
    }

    return new_vec;
}

// / operator
vector<float> operator/(vector<float> const& vec1, vector<float> const& vec2) {
    /*
        params:
            vec1: 1D vector of floats (of dimension (width))
            vec2: 1D vector of floats (of dimension (width))
    */

    assert(vec1.size() == vec2.size());


    int w = vec1.size();

    vector<float> new_vec(w);

    #pragma omp parallel for 
    for (int i=0; i<w; i++) {
        new_vec [i] = vec1[i] / vec2[i];
    }

    return new_vec;
}

// sqrt function
vector<float> vec_sqrt(vector<float> const& vec) {
    /*
        params:
            vec: 1D vector of floats (of dimension (width))
    */

    int w = vec.size();

    vector<float> new_vec(w);

    #pragma omp parallel for 
    for (int i=0; i<w; i++) {
        new_vec [i] = std::sqrt(vec[i]);
    }

    return new_vec;
}

// for 3D vectors

// + operator
vector<vector<vector<float>>> operator+(vector<vector<vector<float>>> const& vec1, vector<vector<vector<float>>> const& vec2) {
    /*
        params:
            vec1: 3D vector of floats (of dimension (width x height x frame_rate))
            vec2: 3D vector of floats (of dimension (width x height x frame_rate))
            vec3: 3D vector of floats (of dimension (width x height x frame_rate))
    */

    assert(vec1.size() == vec2.size());
    assert(vec1[0].size() == vec2[0].size());
    assert(vec1[0][0].size() == vec2[0][0].size());


    int w = vec1.size();
    int h = vec1[0].size();
    int f = vec1[0][0].size();

    vector<vector<vector<float>>> new_vec(w, vector<vector<float>>(h, vector<float>(f)));

    #pragma omp parallel for collapse(3)
    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            for (int k=0; k<f; k++) {
                new_vec [i][j][k] = vec1[i][j][k] + vec2[i][j][k];
            }
        }
    }

    return new_vec;
}

// - operator
vector<vector<vector<float>>> operator-(vector<vector<vector<float>>> const& vec1, vector<vector<vector<float>>> const& vec2) {
    /*
        params:
            vec1: 3D vector of floats (of dimension (width x height x frame_rate))
            vec2: 3D vector of floats (of dimension (width x height x frame_rate))
            vec3: 3D vector of floats (of dimension (width x height x frame_rate))
    */

    assert(vec1.size() == vec2.size());
    assert(vec1[0].size() == vec2[0].size());
    assert(vec1[0][0].size() == vec2[0][0].size());


    int w = vec1.size();
    int h = vec1[0].size();
    int f = vec1[0][0].size();

    vector<vector<vector<float>>> new_vec(w, vector<vector<float>>(h, vector<float>(f)));

    #pragma omp parallel for collapse(3)
    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            for (int k=0; k<f; k++) {
                new_vec [i][j][k] = vec1[i][j][k] - vec2[i][j][k];
            }
        }
    }

    return new_vec;
}

// * operator
vector<vector<vector<float>>> operator*(vector<vector<vector<float>>> const& vec1, vector<vector<vector<float>>> const& vec2) {
    /*
        params:
            vec1: 3D vector of floats (of dimension (width x height x frame_rate))
            vec2: 3D vector of floats (of dimension (width x height x frame_rate))
            vec3: 3D vector of floats (of dimension (width x height x frame_rate))
    */

    assert(vec1.size() == vec2.size());
    assert(vec1[0].size() == vec2[0].size());
    assert(vec1[0][0].size() == vec2[0][0].size());


    int w = vec1.size();
    int h = vec1[0].size();
    int f = vec1[0][0].size();

    vector<vector<vector<float>>> new_vec(w, vector<vector<float>>(h, vector<float>(f)));

    #pragma omp parallel for collapse(3)
    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            for (int k=0; k<f; k++) {
                new_vec [i][j][k] = vec1[i][j][k] * vec2[i][j][k];
            }
        }
    }

    return new_vec;
}

// / operator
vector<vector<vector<float>>> operator/(vector<vector<vector<float>>> const& vec1, vector<vector<vector<float>>> const& vec2) {
    /*
        params:
            vec1: 3D vector of floats (of dimension (width x height x frame_rate))
            vec2: 3D vector of floats (of dimension (width x height x frame_rate))
            vec3: 3D vector of floats (of dimension (width x height x frame_rate))
    */

    assert(vec1.size() == vec2.size());
    assert(vec1[0].size() == vec2[0].size());
    assert(vec1[0][0].size() == vec2[0][0].size());


    int w = vec1.size();
    int h = vec1[0].size();
    int f = vec1[0][0].size();

    vector<vector<vector<float>>> new_vec(w, vector<vector<float>>(h, vector<float>(f)));

    #pragma omp parallel for collapse(3)
    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            for (int k=0; k<f; k++) {
                new_vec [i][j][k] = vec1[i][j][k] / vec2[i][j][k];
            }
        }
    }

    return new_vec;
}

// sqrt function
vector<vector<vector<float>>> vec_sqrt(vector<vector<vector<float>>> const& vec) {
    /*
        params:
            vec: 3D vector of floats (of dimension (width x height x frame_rate))
    */

    int w = vec.size();
    int h = vec[0].size();
    int f = vec[0][0].size();

    vector<vector<vector<float>>> new_vec(w, vector<vector<float>>(h, vector<float>(f)));

    #pragma omp parallel for collapse(3)
    for (int i=0; i<w; i++) {
        for (int j=0; j<h; j++) {
            for (int k=0; k<f; k++) {
                new_vec [i][j][k] = std::sqrt(vec[i][j][k]);
            }
        }
    }

    return new_vec;
}

// Assignment operator

vector<float>& vectors::operator=(vector<float> const& vec)
{
    if (&this->vec1 == &vec) {
        return this->vec1; // handle self assignment
    }

    #pragma omp parallel for 
    for (int i=0; i<vec.size(); i++) {
        this->vec1[i] = vec[i];
    }

    return this->vec1;
}

vector<vector<float>>& vectors::operator=(vector<vector<float>> const& vec)
{
    if (&this->vec2 == &vec) {
        return this->vec2; // handle self assignment
    }

    #pragma omp parallel for collapse(2)
    for (int i=0; i<vec.size(); i++) {
        for (int j=0; j<vec[0].size(); j++) {
            this->vec2[i][j] = vec[i][j];
        }
    }

    return this->vec2;
}

vector<vector<vector<float>>>& vectors::operator=(vector<vector<vector<float>>> const& vec)
{
    if (&this->vec3 == &vec) {
        return this->vec3; // handle self assignment
    }

    #pragma omp parallel for collapse(3)
    for (int i=0; i<vec.size(); i++) {
        for (int j=0; j<vec[0].size(); j++) {
            for (int k=0; k<vec[0][0].size(); k++) {
                this->vec3[i][j][k] = vec[i][j][k];
            }
        }
    }

    return this->vec3;
}