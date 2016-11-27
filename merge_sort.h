#include <immintrin.h> 
#include <utility>
#include <string>
#include <vector>
#include <tuple>

#define SIMD_SIZE 4
#define SORT_SIZE 32

struct masks {
  __m256i load_store_mask;
};

struct entry {
  int32_t key;
  int32_t oid;
};

typedef long long int int64;

extern masks global_masks;

__m256i load_reg256(int64 *a);

void store_reg256(int64 *a, __m256i& b);

__m256i reverse(__m256i& v);

__m256i interleave_low(__m256i& a, __m256i& b);

__m256i intleave_high(__m256i& a, __m256i& b);

void minmax(const __m256i& a, const __m256i& b, __m256i& c, __m256i& d);

void minmax(__m256i& a, __m256i& b);

//return 8-element sorted array
void sort32_64i(__m256i *rows);

void bitonic_merge(__m256i& a, __m256i& b, __m256i& c, __m256i& d);

void intra_register_sort(__m256i& reg);

void test_basic();

void initialize();

void merge_phase(int64 *a, int64 *out, int start, int mid, int end);

void merge_pass(int64 *in, int64 *out, int n, int merge_size);

std::pair<std::vector<int64>, std::vector<int64>> 
    merge_sort(std::vector<int64>& a, std::vector<int64>& b);

void print_register(const __m256i& a, const std::string& msg);

void print_array(entry *a, const std::string& msg, const int size=4);

