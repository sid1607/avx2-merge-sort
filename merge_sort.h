#include <immintrin.h> 
#include <utility>
#include <string>
#include <vector>
#include <tuple>

#define SIMD_SIZE 8
#define SORT_SIZE 64

typedef struct masks {
  __m128i load_store_mask;
} masks;

extern masks global_masks;

__m256i load_reg256(int64_t *a);

void store_reg256(int64_t *a, __m256i& b);

__m256i reverse(__m256i& v);

__m256i interleave_low(__m256i& a, __m256i& b);

__m256i intleave_high(__m256i& a, __m256i& b);

void minmax(__m256i& a, __m256i& b, __m256i& minab, __m256i& maxab);

void minmax(__m256i& a, __m256i& b);

void minmax64(__m256i& a, __m256i& b);

__m256i register_shuffle(__m256i& a, __m256i& mask);

void sort64(__m256i* row);

void sort64(__m256i& row0, __m256i& row1, __m256i& row2, __m256i& row3,
            __m256i& row4, __m256i& row5, __m256i& row6, __m256i& row7);

//return  8-element sorted array
void sort32_64i(__m256i& row0, __m256i& row1, __m256i& row2, __m256i& row3,
            __m256i& row4, __m256i& row5, __m256i& row6, __m256i& row7);

void sort16_64i(__m256i& row0, __m256i& row1, __m256i& row2, __m256i& row3);



void sort_columns(__m256i& a0, __m256i& a1, __m256i& a2, __m256i& a3,
                  __m256i& a4, __m256i& a5, __m256i& a6, __m256i& a7);

void bitonic_merge(__m256i& a, __m256i& b, __m256i& c, __m256i& d);

void intra_register_sort(__m256i& reg);

void test_basic();

void initialize();

void merge_phase(int64_t *a, int64_t *out, int64_t start, int64_t mid, int64_t end);

void merge_pass(int64_t *in, int64_t *out, int64_t n, int64_t merge_size);

std::pair<std::vector<int64_t>, std::vector<int64_t>> 
    merge(std::vector<int64_t>& a, std::vector<int64_t>& b);

void test_minmax64();

void test_minmax();