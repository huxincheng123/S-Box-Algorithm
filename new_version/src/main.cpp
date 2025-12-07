#include "common.h"
#include "globals.h"
#include "xor_base.h"
#include "sbox_tools.h"

FILE* g_console_out = stdout;
FILE* g_log_file    = nullptr;

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
    if (g_log_file)   fflush(g_log_file);
    return ret_console ? ret_console : ret_file;
}

// 外部实现
void solve();

int main() {
//  input file
    freopen("data_703.txt", "r", stdin);
    g_console_out = fdopen(dup(STDOUT_FILENO), "w");
    if (!g_console_out) g_console_out = stdout;
    g_log_file = fopen("result.txt", "w");
    solve();
    return 0;
}