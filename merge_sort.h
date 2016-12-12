#include <immintrin.h> 
#include <utility>
#include <string>
#include <vector>
#include <tuple>

#define SIMD_SIZE 4
#define SORT_SIZE 32

struct entry {
  int32_t key;
  int32_t oid;

  entry() {};

  entry(int32_t key, int32_t oid) : key(key), oid(oid) {};
};

typedef long long sort_ele_type;

inline __m256i load_reg256(sort_ele_type *a) {
  return *(__m256i*)a;
}

inline void store_reg256(sort_ele_type *a, __m256i& b) {
  *((__m256i*)a) = b;
}

std::pair<sort_ele_type *, sort_ele_type *> 
  merge_sort(sort_ele_type *a, sort_ele_type *b, size_t len);

void print_register(const __m256i& a, const std::string& msg);

void print_array(entry *a, const std::string& msg, const int size=4);

