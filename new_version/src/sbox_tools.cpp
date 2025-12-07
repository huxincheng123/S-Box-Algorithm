#include "sbox_tools.h"

void print_vector_as_binary_matrix(const vector<int>& v, int n) {
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            int x = v[n - 1 - col];
            int bit = (x >> (n - 1 - row)) & 1;
            printf("%d", bit);
            if (col + 1 < n) printf(" ");
        }
        printf("\n");
    }
}