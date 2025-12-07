#pragma once
#include <bits/stdc++.h>
#include <unistd.h>

#define is insert
#define pb push_back
#define fi first
#define se second
using namespace std;
typedef long long ll;

// 全局 printf 重定向
extern FILE* g_console_out;
extern FILE* g_log_file;
int dual_printf(const char* fmt, ...);
#define printf dual_printf
