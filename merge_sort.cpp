#include "merge_sort.h"
#include <iostream>
#include <stdlib.h>

masks global_masks;

__m256i load_reg256(int *a) {
  return _mm256_maskload_epi32(a, global_masks.load_store_mask);
}

void store_reg256(int *a, __m256i& b) {
  _mm256_maskstore_epi32(a, global_masks.load_store_mask, b);
}

inline __m256i reverse(__m256i& v) {
  return _mm256_permutevar8x32_epi32(v, global_masks.rev_idx_mask);
}

inline __m256i interleave_low(__m256i& a, __m256i& b) {
  return _mm256_unpacklo_epi32(a,b);
}

inline __m256i interleave_high(__m256i& a, __m256i& b) {
  return _mm256_unpackhi_epi32(a,b);
}

inline void minmax(__m256i& a, __m256i& b, __m256i& minab, __m256i& maxab){
    minab = _mm256_min_epu32(a, b);
    maxab = _mm256_max_epu32(a, b);
    return;
}

inline void minmax(__m256i& a, __m256i& b){
    auto t = a;
    a = _mm256_min_epu32(a, b);
    b = _mm256_max_epu32(t, b);
    return;
}

inline __m256i shuffle(__m256i& a, int* idx_array) {
    __m256i idx = _mm256_load_si256((__m256i *)idx_array);
    return _mm256_permutevar8x32_epi32(a, idx);
}

inline __m256i register_shuffle(__m256i& a, __m256i& mask) {
    return _mm256_permutevar8x32_epi32(a, mask);
}


std::pair<__m256i, __m256i> bitonic_merge(__m256i& a, __m256i& b) {
  __m256i minabr, maxabr;
  __m256i br = reverse(b);
  minmax(a,br, minabr, maxabr);
  return std::make_pair(intra_register_sort(minabr), 
    intra_register_sort(maxabr));
}

void sort_columns(__m256i& a0, __m256i& a1, __m256i& a2, __m256i& a3,
                  __m256i& a4, __m256i& a5, __m256i& a6, __m256i& a7) {

    minmax(a0,a1);
    minmax(a2,a3);
    minmax(a4,a5);
    minmax(a6,a7);

    minmax(a0,a2);
    minmax(a1,a3);
    minmax(a4,a6);
    minmax(a5,a7);

    minmax(a1,a2);
    minmax(a0,a4);
    minmax(a5,a6);
    minmax(a3,a7);

    minmax(a1,a5);
    minmax(a2,a6);

    minmax(a1,a4);
    minmax(a3,a6);

    minmax(a2,a4);
    minmax(a3,a5);

    minmax(a3,a4);
}

__m256i intra_register_sort(__m256i& l8) {
  __m256i min, max;
  // phase 1
  auto l8_1 = register_shuffle(l8, global_masks.swap_128);
  minmax(l8, l8_1, min, max);
  auto l4 = _mm256_permute2x128_si256(min, max, 0x20);
  // phase 2
  auto l4_1 = _mm256_shuffle_epi32(l4, 0x4e);
  minmax(l4, l4_1, min, max);
  auto l2 = _mm256_unpacklo_epi64(min, max);
  // phase 3
  auto l2_1 = _mm256_shuffle_epi32(l2, 0xb1);
  minmax(l2, l2_1, min, max);
  min = _mm256_shuffle_epi32(min, 0xd8);
  max = _mm256_shuffle_epi32(max, 0xd8);
  return _mm256_unpacklo_epi32(min, max);
}

void print_test_array(__m256i res, const std::string& msg) {
  std::cout << msg << std::endl;
  for (int i=0; i<8; i++)
    std::cout << ((int *)&res)[i] << "\t";
  std::cout << std::endl;
}

void initialize() {
  // directly set load store mask in 32B aligned memory
  alignas(32) int load_store_mask[8] = 
    {1<<31,1<<31,1<<31,1<<31,1<<31,1<<31,1<<31,1<<31};
  global_masks.load_store_mask = 
      _mm256_load_si256((__m256i *) &load_store_mask[0]);

  // load the remaining masks
  int rev_idx_mask[8] = {7,6,5,4,3,2,1,0};
  int swap_128[8] = {4,5,6,7,0,1,2,3};
  global_masks.rev_idx_mask = load_reg256(&rev_idx_mask[0]);
  global_masks.swap_128 = load_reg256(&swap_128[0]);
}

void test_basic() {
  __m256i min, max;
  int test_arr1[8] = {1,2,3,4,15,16,17,18};
  int test_arr2[8] = {11,12,13,14,5,6,7,8};
  int idx[8] = {4,5,6,7,0,1,2,3};
  __m256i test1 = load_reg256(&test_arr1[0]);
  __m256i test2 = load_reg256(&test_arr2[0]);
  __m256i mask = load_reg256(&idx[0]);
  print_test_array(test1, "test1");
  print_test_array(test2, "test2");
  print_test_array(reverse(test1), "Reverse Output");
  print_test_array(register_shuffle(test1, mask), "Register shuffle");
  print_test_array(interleave_low(test1, test2), "interleave_low");
  print_test_array(interleave_high(test1, test2), "interleave_high");
  minmax(test1, test2, min, max);
  print_test_array(min, "Minimum");
  print_test_array(max, "Maximum");
}
