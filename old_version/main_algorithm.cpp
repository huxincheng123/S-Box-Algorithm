#pragma GCC optimize(3)
#include <iostream>
#include <vector>
#include <set>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <random>
#include <map>
#include <algorithm>
#include <cstdarg>
#include <unistd.h>
#define is insert
#define pb push_back
#define fi first
#define se second

using namespace std;

typedef long long ll;

const int N = 1e7 + 10;
int is_odd[N];
static FILE* g_console_out = stdout;
static FILE* g_log_file = nullptr;

int dual_printf(const char* fmt, ...) {
    va_list args;
    int ret_console = 0, ret_file = 0;
    if (g_console_out) {
        va_start(args, fmt);
        ret_console = vfprintf(g_console_out, fmt, args);
        va_end(args);
    }
    if (g_log_file) {
        va_start(args, fmt);
        ret_file = vfprintf(g_log_file, fmt, args);
        va_end(args);
    }
    if (g_console_out) fflush(g_console_out);
    if (g_log_file) fflush(g_log_file);
    return ret_console ? ret_console : ret_file;
}

#define printf dual_printf

struct xor_base {
    static const int max_size = 1010;
    int base[max_size], value[max_size];
    int n;
    void initial(int rank) {
        for (int i = 0; i < rank; ++i) {
            base[i] = value[i] = 0;
        }
        n = rank;
    }
    int insert(int x, int val) {
        for (int i = n - 1; i >= 0; --i) {
            if (!(x >> i & 1)) {
                continue;
            }
            if (!base[i]) {
                base[i] = x;
                value[i] = val;
                return 1;
            }
            x ^= base[i];
            val ^= value[i];
        }
        if (!val)
            return 0;
        return -1;
    }
    bool check(int x, int val) {
        for (int i = 0; i < n; ++i) {
            if (base[i]) {
                int h1 = value[i] & val;
                int h2 = base[i] & x;
                int u = h1 ^ h2;
                if (is_odd[u]) {
                    return false;
                }
            }
        }
        return true;
    }
} xor_base_A, xor_base_B;

// -------------------- 新增工具函数 --------------------
// 将“按列存储”的 n×n 线性变换 cols 作用到向量 x 上：cols[j] 是第 j 列（即基向量 e_j 的像）
static inline int apply_linear(const vector<int>& cols, int x, int n) {
    int y = 0;
    for (int j = 0; j < n; ++j) if (x >> j & 1) y ^= cols[j];
    return y;
}

// 规范化 S-box: out[x] = orig[x ^ c] ^ orig[c]
static inline void normalize_sbox(const int *orig, int c, int n, vector<int>& out) {
    const int size = 1 << n;
    out.resize(size);
    for (int i = 0; i < size; ++i) out[i] = orig[i ^ c] ^ orig[c];
}
// ------------------------------------------------------

void print_vector_as_binary_matrix(const vector<int>& v, int n) {
    // 将长度为 n 的向量逆序后，按位展开成 n×n 的 0/1 矩阵并输出
    // 每一列对应一个逆序后的数字，从高位到低位对应矩阵的不同行
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            int x = v[n - 1 - col];                  // 逆序后的第 col 列对应的数
            int bit = (x >> (n - 1 - row)) & 1;      // 从高位到低位依次作为矩阵的不同行
            printf("%d", bit);
            if (col + 1 < n) printf(" ");
        }
        printf("\n");
    }
}

int n;
int test_num;
int is_in_check_of_A[N], is_in_check_of_B[N];
int is_in_new_of_A[N], is_in_new_of_B[N];
int is_visit_A[N], is_visit_B[N];
vector<int> new_of_A, new_of_B;
vector<array<int,2>> check_of_A, check_of_B;
int value_of_A[N], value_of_B[N];
int s1[N], s2[N];
int s1_inverse[N], s2_inverse[N];
int original_s1[N], original_s2[N];
int tmp_original_s1[N], tmp_original_s2[N];
int original_s1_inverse[N], original_s2_inverse[N];
int rank_A, rank_B;

void initial_s_box1(int c) {
    for (int i = 0; i < (1 << n); ++i) {
        s1[i] = original_s1[i ^ c] ^ original_s1[c];
        s1_inverse[s1[i]] = i;
    }
}

void initial_s_box2(int c) {
    for (int i = 0; i < (1 << n); ++i) {
        s2[i] = original_s2[i ^ c] ^ original_s2[c];
        s2_inverse[s2[i]] = i;
    }
}

int BaseA[1010],BaseB[1010];
int ValA[1010],ValB[1010];

//initial the set
void initial_all_set() {
    for (int i = 0; i < (1 << n); ++i) {
        is_in_check_of_A[i] = is_in_check_of_B[i] = 0;
        is_in_new_of_A[i] = is_in_new_of_B[i] = 0;
        is_visit_A[i] = is_visit_B[i] = 0;
    }
    check_of_A.clear();
    check_of_B.clear();
    new_of_A.clear();
    new_of_B.clear();
    is_in_check_of_A[0] = 1;
    is_in_check_of_B[0] = 1;
    is_in_new_of_A[0] = 1;
    is_in_new_of_B[0] = 1;
    check_of_A.pb({0,0});
    check_of_B.pb({0,0});
    is_visit_A[0] = 1;
    is_visit_B[0] = 1;
}

void solve() {
clock_t start, end;
    start = clock();
    int number_of_success = 0;
    // input the number of test
    scanf("%d", &test_num);
    ll cnt = 0;
    vector<ll> result;
    for (int test_id = 1; test_id <= test_num; ++test_id) {
        ll tmp_cnt = cnt;
        bool printed_sep = false;
        // input n and S-boxes
        scanf("%d", &n);
        printf("Test pair #%d:\n", test_id);
        for (int i = 0; i < (1 << n); ++i) {
            scanf("%d", &original_s1[i]);
        }
        for (int i = 0; i < (1 << n); ++i) {
            scanf("%d", &original_s2[i]);
        }
        for (int i = 0; i < (1 << n); ++i) {
            is_odd[i] = __builtin_popcount(i) & 1;
        }
        for (int i = 0; i < (1 << n); ++i) {
            original_s1_inverse[original_s1[i]] = i;
            original_s2_inverse[original_s2[i]] = i;
        }
        int success = 0;

        rank_A = 0;
        vector<vector<int>> H(n * 2, vector<int>(n * 2));
        vector<vector<int>> A1(n, vector<int>(n)), B1(n, vector<int>(n));
        vector<vector<int>> A2(n, vector<int>(n)), B2(n, vector<int>(n));
        vector<vector<int>> A2Inv(n, vector<int>(n)), B1Inv(n, vector<int>(n));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                int u = 0;
                for (int k = 0; k < (1 << n); ++k) {
                    if (k % (1 << (j + 1)) < (1 << j)) {
                        u ^= (original_s1[k] >> i & 1);
                    }
                }
                H[i][j] = u;
                H[i][j + n] = 0;
            }
        }
        for (int i = 0; i < n; ++i) {
            H[i][i + n] = 1;
        }
        for (int i = 0; i < n; ++i) {
            int r = -1;
            for (int j = rank_A; j < n; ++j) {
                if (H[j][i]) {
                    r = j;
                    break;
                }
            }
            if (r == -1)
                continue;
            swap(H[rank_A], H[r]);
            for (int j = 0; j < n; ++j) {
                if (j != rank_A && H[j][i]) {
                    for (int k = i; k < n * 2; ++k) {
                        H[j][k] ^= H[rank_A][k];
                    }
                }
            }
            ++rank_A;
        }
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                B1[i][j] = H[i][j + n];
            }
        }
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                H[i + n][j] = (i == j);
            }
        }
        rank_A = 0;
        for (int j = 0; j < n; ++j) {
            int r = -1;
            for (int i = rank_A; i < n; ++i) {
                if (H[j][i]) {
                    r = i;
                    break;
                }
            }
            if (r == -1)
                break;
            for (int k = 0; k < n * 2; ++k) {
                swap(H[k][r], H[k][rank_A]);
            }
            for (int i = 0; i < n; ++i) {
                if (i != rank_A && H[j][i]) {
                    for (int k = j; k < n * 2; ++k) {
                        H[k][i] ^= H[k][rank_A];
                    }
                }
            }
            ++rank_A;
        }
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                H[i][j] = H[i + n][j], H[i + n][j] = (i == j);
            }
        }
        int tmpr = 0;
        for (int j = 0; j < n; ++j) {
            int r = -1;
            for (int i = tmpr; i < n; ++i) {
                if (H[j][i]) {
                    r = i;
                    break;
                }
            }
            if (r == -1)
                break;
            for (int k = 0; k < n * 2; ++k) {
                swap(H[k][r], H[k][tmpr]);
            }
            for (int i = 0; i < n; ++i) {
                if (i != tmpr && H[j][i]) {
                    for (int k = j; k < n * 2; ++k) {
                        H[k][i] ^= H[k][tmpr];
                    }
                }
            }
            ++tmpr;
        }
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                A1[j][i] = H[i + n][j], H[i][j] = B1[i][j], H[i + n][j] = (i == j);
            }
        }
        tmpr = 0;
        for (int j = 0; j < n; ++j) {
            int r = -1;
            for (int i = tmpr; i < n; ++i) {
                if (H[j][i]) {
                    r = i;
                    break;
                }
            }
            if (r == -1)
                break;
            for (int k = 0; k < n * 2; ++k) {
                swap(H[k][r], H[k][tmpr]);
            }
            for (int i = 0; i < n; ++i) {
                if (i != tmpr && H[j][i]) {
                    for (int k = j; k < n * 2; ++k) {
                        H[k][i] ^= H[k][tmpr];
                    }
                }
            }
            ++tmpr;
        }
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                B1Inv[i][j] = H[i + n][j];
            }
        }

        rank_B = 0;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                int u = 0;
                for (int k = 0; k < (1 << n); ++k) {
                    if (k % (1 << (j + 1)) < (1 << j)) {
                        u ^= original_s2[k] >> i & 1;
                    }
                }
                H[i][j] = u;
                H[i][j + n] = 0;
            }
        }

        for (int i = 0; i < n; ++i) {
            H[i][i + n] = 1;
        }
        for (int i = 0; i < n; ++i) {
            int r = -1;
            for (int j = rank_B; j < n; ++j) {
                if (H[j][i]) {
                    r = j;
                    break;
                }
            }
            if (r == -1)
                continue;
            swap(H[rank_B], H[r]);
            for (int j = 0; j < n; ++j) {
                if (j != rank_B && H[j][i]) {
                    for (int k = i; k < n * 2; ++k) {
                        H[j][k] ^= H[rank_B][k];
                    }
                }
            }
            ++rank_B;
        }
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                B2[i][j] = H[i][j + n];
            }
        }
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                H[i + n][j] = (i == j);
            }
        }
        rank_B = 0;
        for (int j = 0; j < n; ++j) {
            int r = -1;
            for (int i = rank_B; i < n; ++i) {
                if (H[j][i]) {
                    r = i;
                    break;
                }
            }
            if (r == -1)
                break;
            for (int k = 0; k < n * 2; ++k) {
                swap(H[k][r], H[k][rank_B]);
            }
            for (int i = 0; i < n; ++i) {
                if (i != rank_B && H[j][i]) {
                    for (int k = j; k < n * 2; ++k) {
                        H[k][i] ^= H[k][rank_B];
                    }
                }
            }
            ++rank_B;
        }

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                H[i][j] = A2Inv[j][i] = H[i + n][j], H[i + n][j] = (i == j);
            }
        }
        tmpr = 0;
        for (int j = 0; j < n; ++j) {
            int r = -1;
            for (int i = tmpr; i < n; ++i) {
                if (H[j][i]) {
                    r = i;
                    break;
                }
            }
            if (r == -1)
                break;
            for (int k = 0; k < n * 2; ++k) {
                swap(H[k][r], H[k][tmpr]);
            }
            for (int i = 0; i < n; ++i) {
                if (i != tmpr && H[j][i]) {
                    for (int k = j; k < n * 2; ++k) {
                        H[k][i] ^= H[k][tmpr];
                    }
                }
            }
            ++tmpr;
        }

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                A2[j][i] = H[i + n][j];
            }
        }

        for (int i = 0; i < (1 << n); ++i) {
            tmp_original_s1[i] = original_s1[i];
            tmp_original_s2[i] = original_s2[i];
        }
        for (int i = 0; i < (1 << n); ++i) {
            int u = 0, v = 0;
            for (int j = 0; j < n; ++j) {
                if (i >> j & 1) {
                    for (int k = 0; k < n; ++k) {
                        u ^= (A1[k][j] << k);
                    }
                }
            }
            u = tmp_original_s1[u];
            for (int j = 0; j < n; ++j) {
                if (u >> j & 1) {
                    for (int k = 0; k < n; ++k) {
                        v ^= B1[k][j] << k;
                    }
                }
            }
            original_s1[i] = v;
        }
        for (int i = 0; i < (1 << n); ++i) {
            int u = 0, v = 0;
            for (int j = 0; j < n; ++j) {
                if (i >> j & 1) {
                    for (int k = 0; k < n; ++k) {
                        u ^= A2[k][j] << k;
                    }
                }
            }
            u = tmp_original_s2[u];
            for (int j = 0; j < n; ++j) {
                if (u >> j & 1) {
                    for (int k = 0; k < n; ++k) {
                        v ^= B2[k][j] << k;
                    }
                }
            }
            original_s2[i] = v;
        }
        initial_s_box1(0);

        printf("First S-box rank: %d, Second S-box rank: %d\n", rank_A, rank_B);

        if (rank_A != rank_B) {
            printf("Affine equivalent: NO\n");
            printf("---- End of test pair #%d ----\n\n", test_id);
            printed_sep = true;
            continue;
        }
        int rank = rank_A;
        for (int c2 = 0; c2 < (1 << n); ++c2) {
            initial_s_box2(c2);

            initial_all_set();
            xor_base_A.initial(rank);
            xor_base_B.initial(rank);
            int rejected = 0;
            int tempp = 0;
            int mn1 = 1, mn2 = 1;
            vector<pair<array<int, 3>, pair<vector<int>, vector<int>>>> st;
            int cur = 0;
            int lev = 0;
            while (!success) {
                if (rejected) {
                    if (!st.size())
                        break;
                    auto tmp = st.back();
                    st.pop_back();
                    mn1 = tmp.fi[0];
                    mn2 = tmp.fi[1];
                    cur = tmp.fi[2];
                    for (int i = 0; i < rank; ++i) {
                        xor_base_A.base[i] = tmp.se.fi[i];
                        xor_base_B.base[i] = tmp.se.se[i];
                    }
                    while (check_of_A.size() && check_of_A.back()[1] == lev) {
                        auto tmp_che = check_of_A.back();
                        check_of_A.pop_back();
                        int u = tmp_che[0];
                        is_visit_A[value_of_A[u]] = 0;
                        is_in_check_of_A[u] = is_in_new_of_A[u] = 0;
                    }
                    while (check_of_B.size() && check_of_B.back()[1] == lev) {
                        auto tmp_che = check_of_B.back();
                        check_of_B.pop_back();
                        int u = tmp_che[0];
                        is_visit_B[value_of_B[u]] = 0;
                        is_in_check_of_B[u] = is_in_new_of_B[u] = 0;
                    }
                    while (new_of_A.size()) {
                        int u = new_of_A.back();
                        new_of_A.pop_back();
                        if (!is_in_check_of_A[u]) {
                            is_in_check_of_A[u] = is_in_new_of_A[u] = 0;
                            is_visit_A[value_of_A[u]] = 0;
                        }
                    }
                    while (new_of_B.size()) {
                        int u = new_of_B.back();
                        new_of_B.pop_back();
                        if (!is_in_check_of_B[u]) {
                            is_in_check_of_B[u] = is_in_new_of_B[u] = 0;
                            is_visit_B[value_of_B[u]] = 0;
                        }
                    }
                    --lev;
                    rejected = 0;
                }
                else
                    cur = 0;
                ++cur;
                vector<int> tmp_base_A(rank), tmp_base_B(rank);
                for (int i = 0; i < rank; ++i) {
                    tmp_base_A[i] = xor_base_A.base[i];
                    tmp_base_B[i] = xor_base_B.base[i];
                }
                while (cur < 1 << n) {
                    if (mn1 < 1 << rank) {
                        if (cur >= 1 << rank || is_visit_A[cur]) {
                            ++cur;
                            continue;
                        }
                        if (!xor_base_B.check(mn1, cur)) {
                            ++cur;
                            continue;
                        }
                        new_of_A.pb(mn1);
                        is_in_new_of_A[mn1] = 1;
                        value_of_A[mn1] = cur;
                        is_visit_A[cur] = 1;
                        ++lev;
                    }
                    else if (mn2 < 1 << rank) {
                        if (cur >= 1 << rank || is_visit_B[cur]) {
                            ++cur;
                            continue;
                        }
                        if (!xor_base_A.check(mn2, cur)) {
                            ++cur;
                            continue;
                        }
                        new_of_B.pb(mn2);
                        is_in_new_of_B[mn2] = 1;
                        value_of_B[mn2] = cur;
                        is_visit_B[cur] = 1;
                        ++lev;
                    }
                    else {
                        if (cur < 1 << rank || is_visit_A[cur]) {
                            ++cur;
                            continue;
                        }
                        new_of_A.pb(mn1);
                        is_in_new_of_A[mn1] = 1;
                        value_of_A[mn1] = cur;
                        is_visit_A[cur] = 1;
                        ++lev;
                    }
                    break;
                }
                if (cur == 1 << n) {
                    rejected = 1;
                    continue;
                }
                st.pb({ {mn1,mn2,cur},{tmp_base_A,tmp_base_B} });
                int c1 = 0, c2i = 0;
                while (c1 < new_of_A.size() || c2i < new_of_B.size()) {
                    while (c1 < new_of_A.size()) {
                        int tmp = new_of_A[c1++];
                        if (is_in_check_of_A[tmp]) {
                            continue;
                        }
                        if (tmp < 1 << rank) {
                            if (value_of_A[tmp] >= 1 << rank) {
                                rejected = 1;
                                break;
                            }
                            if (!xor_base_B.check(tmp, value_of_A[tmp])) {
                                break;
                            }
                            xor_base_A.insert(tmp, value_of_A[tmp]);
                        }
                        else if (value_of_A[tmp] < 1 << rank) {
                            rejected = 1;
                            break;
                        }
                        for (int jj = 0; jj < (int)check_of_A.size(); ++jj) {
                            int j = check_of_A[jj][0];
                            if (is_in_check_of_A[j ^ tmp]) {
                                break;
                            }
                            if (!is_in_check_of_B[s2[j ^ tmp]] && !is_in_new_of_B[s2[j ^ tmp]]) {
                                if (is_visit_B[s1[value_of_A[j] ^ value_of_A[tmp]]]) {
                                    rejected = 1;
                                    break;
                                }
                                value_of_B[s2[j ^ tmp]] = s1[value_of_A[j] ^ value_of_A[tmp]];
                                is_visit_B[s1[value_of_A[j] ^ value_of_A[tmp]]] = 1;
                                new_of_B.pb(s2[j ^ tmp]);
                                is_in_new_of_B[s2[j ^ tmp]] = 1;
                            }
                            else if (value_of_B[s2[j ^ tmp]] != s1[value_of_A[j] ^ value_of_A[tmp]]) {
                                rejected = 1;
                                break;
                            }
                            if (is_in_new_of_A[j ^ tmp] && value_of_A[j ^ tmp] != (value_of_A[j] ^ value_of_A[tmp])) {
                                rejected = 1;
                                break;
                            }
                            if (!is_in_new_of_A[j ^ tmp] && is_visit_A[value_of_A[j] ^ value_of_A[tmp]]) {
                                rejected = 1;
                                break;
                            }
                            value_of_A[j ^ tmp] = value_of_A[j] ^ value_of_A[tmp];
                            is_visit_A[value_of_A[j] ^ value_of_A[tmp]] = 1;
                            is_in_check_of_A[j ^ tmp] = 1;
                            check_of_A.pb({ j ^ tmp,lev });
                            ++cnt;
                        }
                        if (rejected) {
                            break;
                        }
                    }
                    if (rejected) {
                        break;
                    }
                    while (c2i < new_of_B.size()) {
                        int tmp = new_of_B[c2i++];
                        if (is_in_check_of_B[tmp]) {
                            continue;
                        }
                        if (tmp < 1 << rank) {
                            if (value_of_B[tmp] >= 1 << rank) {
                                rejected = 1;
                                break;
                            }
                            if (!xor_base_A.check(tmp, value_of_B[tmp])) {
                                rejected = 1;
                                break;
                            }
                            xor_base_B.insert(tmp, value_of_B[tmp]);
                        }
                        else if (value_of_B[tmp] < 1 << rank) {
                            rejected = 1;
                            break;
                        }
                        for (int jj = 0; jj < (int)check_of_B.size(); ++jj) {
                            int j = check_of_B[jj][0];
                            if (is_in_check_of_B[j ^ tmp])
                                break;
                            if (!is_in_check_of_A[s2_inverse[j ^ tmp]] && !is_in_new_of_A[s2_inverse[j ^ tmp]]) {
                                if (is_visit_A[s1_inverse[value_of_B[j] ^ value_of_B[tmp]]]) {
                                    rejected = 1;
                                    break;
                                }
                                value_of_A[s2_inverse[j ^ tmp]] = s1_inverse[value_of_B[j] ^ value_of_B[tmp]];
                                is_visit_A[s1_inverse[value_of_B[j] ^ value_of_B[tmp]]] = 1;
                                new_of_A.pb(s2_inverse[j ^ tmp]);
                                is_in_new_of_A[s2_inverse[j ^ tmp]] = 1;
                            }
                            else if (value_of_A[s2_inverse[j ^ tmp]] != s1_inverse[value_of_B[j] ^ value_of_B[tmp]]) {
                                rejected = 1;
                                break;
                            }
                            if (is_in_new_of_B[j ^ tmp] && value_of_B[j ^ tmp] != (value_of_B[j] ^ value_of_B[tmp])) {
                                rejected = 1;
                                break;
                            }
                            if (!is_in_new_of_B[j ^ tmp] && is_visit_B[value_of_B[j] ^ value_of_B[tmp]]) {
                                rejected = 1;
                                break;
                            }
                            value_of_B[j ^ tmp] = value_of_B[j] ^ value_of_B[tmp];
                            is_visit_B[value_of_B[j] ^ value_of_B[tmp]] = 1;
                            is_in_check_of_B[j ^ tmp] = 1;
                            check_of_B.pb({ j ^ tmp,lev });
                            ++cnt;
                        }
                        if (rejected)
                            break;
                    }
                    if (rejected)
                        break;
                }
                if (rejected) {
                    continue;
                }
                if (check_of_B.size() == 1 << n) {
                    success = 1;
                    ++number_of_success;
                    printf("Affine equivalent: YES\n");

                    printf("First S-box:\n");
                    for (int i = 0; i < (1 << n); ++i) {
                        printf("%d ", tmp_original_s1[i]);
                    }
                    printf("\n");
                    printf("Second S-box:\n");
                    for (int i = 0; i < (1 << n); ++i) {
                        printf("%d ", tmp_original_s2[i]);
                    }
                    printf("\n");
                    vector<int> tmp_mat(n);
                    // 计算 A 的列向量（按列表示）
                    vector<int> Acols(n);
                    for (int i = 0; i < n; ++i) {
                        int u = 0, v = 0;
                        for (int j = 0; j < n; ++j) {
                            u ^= A2Inv[j][i] << j;
                        }
                        u = value_of_A[u];
                        for (int j = 0; j < n; ++j) {
                            if (u >> j & 1) {
                                for (int k = 0; k < n; ++k) {
                                    v ^= A1[k][j] << k;
                                }
                            }
                        }
                        tmp_mat[i] = v;
                        Acols[i] = v;
                    }
                    printf("L1 matrix:\n");
                    print_vector_as_binary_matrix(tmp_mat, n);

                    // 计算 B 的列向量（按列表示）
                    for (int i = 0; i < n; ++i) {
                        int u = 0, v = 0;
                        for (int j = 0; j < n; ++j) u ^= B2[j][i] << j;
                        u = value_of_B[u];
                        for (int j = 0; j < n; ++j) if (u >> j & 1) {
                            for (int k = 0; k < n; ++k) v ^= B1Inv[k][j] << k;
                        }
                        tmp_mat[i] = v;
                        // store Bcols as well
                    }
                    // we printed the first tmp_mat (Acols) above; print Bcols now
                    vector<int> Bcols(n);
                    for (int i = 0; i < n; ++i) {
                        int u = 0, v = 0;
                        for (int j = 0; j < n; ++j) u ^= B2[j][i] << j;
                        u = value_of_B[u];
                        for (int j = 0; j < n; ++j) if (u >> j & 1) {
                            for (int k = 0; k < n; ++k) v ^= B1Inv[k][j] << k;
                        }
                        Bcols[i] = v;
                    }
                    printf("L2 matrix:\n");
                    print_vector_as_binary_matrix(Bcols, n);

                    // a_sf 是在 Sf2 空间枚举的平移（你 for 循环里的变量 c2）
int a_sf = c2;

// 映回原 S2 输入空间：a_orig = A2 * a_sf
int a_orig = 0;
for (int j = 0; j < n; ++j) if (a_sf >> j & 1) {
    for (int k = 0; k < n; ++k) a_orig ^= (A2[k][j] << k);
}

// 恢复原式的 c1、c2：
// c1 = L1(a_orig)
int c1_vec = apply_linear(Acols, a_orig, n);

// c2 = S1(0) ^ L2( S2(a_orig) )
int c2_vec = tmp_original_s1[0] ^ apply_linear(Bcols, tmp_original_s2[a_orig], n);

char c1_bin[9], c2_bin[9];
for (int i = 7; i >= 0; --i) {
    c1_bin[7 - i] = (c1_vec >> i & 1) ? '1' : '0';
    c2_bin[7 - i] = (c2_vec >> i & 1) ? '1' : '0';
}
c1_bin[8] = c2_bin[8] = '\0';
printf("c1 (8-bit binary): %s\n", c1_bin);
printf("c2 (8-bit binary): %s\n", c2_bin);

                    printf("---- End of test pair #%d ----\n\n", test_id);
                    printed_sep = true;
                    break;
                }
                while (is_in_new_of_A[mn1] || is_in_check_of_A[mn1])
                    ++mn1;
                while (is_in_new_of_B[mn2] || is_in_check_of_B[mn2])
                    ++mn2;
            }
            if (success) {
                break;
            }
        }
        result.pb(cnt - tmp_cnt);
        if (success) {
            continue;
        }

        if (!success) {
            printf("Affine equivalent: NO\n");
        }
        if (!printed_sep) {
            printf("---- End of test pair #%d ----\n\n", test_id);
        }
    }
    double arv = (double)cnt / test_num;
    double s = 0;
    for (ll i : result) {
        s += (i - arv) * (i - arv);
    }
    s /= (test_num - 1);
    s = sqrt(s);
    end = clock();
    printf("Use time: %fs\n", (double)(end - start) / CLOCKS_PER_SEC);
    printf("The number of success: %d\n", number_of_success);
}

int main() {
    
    freopen("three_sbox.txt", "r", stdin);
    // freopen("data_21.txt", "r", stdin);
    g_console_out = fdopen(dup(STDOUT_FILENO), "w");
    if (!g_console_out) {
        g_console_out = stdout;
    }
    g_log_file = fopen("result.txt", "w");
    
    solve();
    return 0;
}

