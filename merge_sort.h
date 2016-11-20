#include <immintrin.h> 
#include <utility>

#define SIMD_SIZE 8

typedef struct masks {
  __m256i load_store_mask;
  __m256i rev_idx_mask;
  __m256i swap_128;
} masks;

extern masks global_masks;

__m256i load_reg256(int *a);

void store_reg256(int *a, __m256i& b);

__m256i reverse(__m256i& v);

__m256i interleave_low(__m256i& a, __m256i& b);

__m256i interleave_high(__m256i& a, __m256i& b);

void minmax(__m256i& a, __m256i& b, __m256i& minab, __m256i& maxab);

void minmax(__m256i& a, __m256i& b);

__m256i register_shuffle(__m256i& a, __m256i& mask);

std::pair<__m256i, __m256i> bitonic_merge(__m256i& a, __m256i& b);

void sort_columns(__m256i& a0, __m256i& a1, __m256i& a2, __m256i& a3,
                  __m256i& a4, __m256i& a5, __m256i& a6, __m256i& a7);

__m256i intra_register_sort(__m256i& l8);

void initialize();

void test_sort_column();

void merge_phase(int *a, int *b, int *out, int merge_size);

void merge_pass(int *in, int *out, int n, int merge_size);