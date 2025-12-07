#pragma once
#include "common.h"

const int max_size = 1010;

class xor_base {
public:
    int base[max_size], value[max_size];
    int n;
    void initial(int rank);
    int insert(int x, int val);
    bool check(int x, int val);
};