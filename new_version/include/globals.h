#pragma once
#include "common.h"
#include "xor_base.h"

extern const int N;
extern int is_odd[];
extern int n, test_num;
extern int is_in_check_of_A[], is_in_check_of_B[];
extern int is_in_new_of_A[], is_in_new_of_B[];
extern int is_visit_A[], is_visit_B[];
extern vector<int> new_of_A, new_of_B;
extern vector<array<int,2>> check_of_A, check_of_B;
extern int value_of_A[], value_of_B[];
extern int s1[], s2[], s1_inverse[], s2_inverse[];
extern int original_s1[], original_s2[];
extern int tmp_original_s1[], tmp_original_s2[];
extern int original_s1_inverse[], original_s2_inverse[];
extern int rank_A, rank_B;
extern xor_base xor_base_A, xor_base_B;