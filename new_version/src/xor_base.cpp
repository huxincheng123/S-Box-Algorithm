#include "xor_base.h"

extern int is_odd[];

void xor_base::initial(int rank) {
    for (int i = 0; i < rank; ++i) base[i] = value[i] = 0;
    n = rank;
}

int xor_base::insert(int x, int val) {
    for (int i = n - 1; i >= 0; --i) {
        if (!(x >> i & 1)) continue;
        if (!base[i]) {
            base[i] = x;
            value[i] = val;
            return 1;
        }
        x ^= base[i];
        val ^= value[i];
    }
    return val ? -1 : 0;
}

bool xor_base::check(int x, int val) {
    for (int i = 0; i < n; ++i) {
        if (base[i]) {
            int h1 = value[i] & val;
            int h2 = base[i] & x;
            int u = h1 ^ h2;
            if (is_odd[u]) return false;
        }
    }
    return true;
}