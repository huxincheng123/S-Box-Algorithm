#pragma once
#include "common.h"

// 将“按列存储”的 n×n 线性变换 cols 作用到向量 x 上
inline int apply_linear(const vector<int>& cols, int x, int n) {
    int y = 0;
    for (int j = 0; j < n; ++j) if (x >> j & 1) y ^= cols[j];
    return y;
}

// 规范化 S-box: out[x] = orig[x ^ c] ^ orig[c]
inline void normalize_sbox(const int *orig, int c, int n, vector<int>& out) {
    const int size = 1 << n;
    out.resize(size);
    for (int i = 0; i < size; ++i) out[i] = orig[i ^ c] ^ orig[c];
}

// 将长度为 n 的向量逆序后，按位展开成 n×n 的 0/1 矩阵并输出
void print_vector_as_binary_matrix(const vector<int>& v, int n);
