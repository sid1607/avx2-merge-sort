#include <immintrin.h> 
#include <utility>
#include <string>
#include <vector>

#define SIMD_SIZE 8
#define SORT_SIZE 64

typedef struct masks {
  __m256i load_store_mask;
  __m256i rev_idx_mask;
  __m256i swap_128;
} masks;

extern masks global_masks;

__m256i load_reg256(int *a);

__m256i load_reg256(int64_t
                    *a);

void store_reg256(int *a, __m256i& b);

__m256i reverse(__m256i& v);

__m256i interleave_low(__m256i& a, __m256i& b);

__m256i interleave_high(__m256i& a, __m256i& b);

void minmax(__m256i& a, __m256i& b, __m256i& minab, __m256i& maxab);

void minmax(__m256i& a, __m256i& b);

void minmax64(__m256i& a, __m256i& b);

__m256i register_shuffle(__m256i& a, __m256i& mask);

void sort64(__m256i* row);

void sort64(__m256i& row0, __m256i& row1, __m256i& row2, __m256i& row3,
            __m256i& row4, __m256i& row5, __m256i& row6, __m256i& row7);

void sort16(__m256i& row0, __m256i& row1, __m256i& row2, __m256i& row3);

void sort_columns(__m256i& a0, __m256i& a1, __m256i& a2, __m256i& a3,
                  __m256i& a4, __m256i& a5, __m256i& a6, __m256i& a7);

std::pair<__m256i, __m256i> bitonic_merge(__m256i& a, __m256i& b);

__m256i intra_register_sort(__m256i& l8);

void initialize();

void merge_phase(int *a, int *out, int start, int mid, int end);

void merge_pass(int *in, int *out, int n, int merge_size);

std::pair<std::vector<int>, std::vector<int>> 
    merge(std::vector<int>& a, std::vector<int>& b);

std::pair<std::vector<int>, std::vector<int>>
    merge_sort(std::vector<int>& a, std::vector<int>& b);

void test_minmax64();
void test_minmax();
