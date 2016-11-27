#include "merge_sort.h"
#include <iostream>
#include <stdlib.h>

masks global_masks;

__m256i load_reg256(int64 *a) {
  return _mm256_maskload_epi64(a, global_masks.load_store_mask);
}

void store_reg256(int64 *a, __m256i& b) {
  _mm256_maskstore_epi64(a, global_masks.load_store_mask, b);
}

inline __m256i reverse(__m256i& v) {
  return _mm256_permute4x64_epi64(v, 0x1b);
}

inline void minmax(const __m256i& a, const __m256i& b, __m256i& c, __m256i& d) {
  auto mask = _mm256_cmpgt_epi32 (a, b);
  mask = _mm256_shuffle_epi32(mask, 0xA0);
  c = _mm256_blendv_epi8(a, b, mask);
  d = _mm256_blendv_epi8(b, a, mask);
}

inline void minmax(__m256i& a, __m256i& b) {
  auto mask = _mm256_cmpgt_epi32 (a, b);
  mask = _mm256_shuffle_epi32(mask, 0xA0);
  auto t = a;
  a = _mm256_blendv_epi8(a, b, mask);
  b = _mm256_blendv_epi8(b, t, mask);
}

inline void transpose8_64i(__m256i *rows) {
  __m256i __t0 = _mm256_unpacklo_epi64(rows[0], rows[1]);
  __m256i __t1 = _mm256_unpackhi_epi64(rows[0], rows[1]);
  __m256i __t2 = _mm256_unpacklo_epi64(rows[2], rows[3]);
  __m256i __t3 = _mm256_unpackhi_epi64(rows[2], rows[3]);

  __m256i __t4 = _mm256_unpacklo_epi64(rows[4], rows[5]);
  __m256i __t5 = _mm256_unpackhi_epi64(rows[4], rows[5]);
  __m256i __t6 = _mm256_unpacklo_epi64(rows[6], rows[7]);
  __m256i __t7 = _mm256_unpackhi_epi64(rows[6], rows[7]);

  rows[0] = _mm256_permute2x128_si256(__t0, __t2, 0x20);
  rows[2] = _mm256_permute2x128_si256(__t1, __t3, 0x20);
  rows[4] = _mm256_permute2x128_si256(__t0, __t2, 0x31);
  rows[6] = _mm256_permute2x128_si256(__t1, __t3, 0x31);

  rows[1] = _mm256_permute2x128_si256(__t4, __t6, 0x20);
  rows[3] = _mm256_permute2x128_si256(__t5, __t7, 0x20);
  rows[5] = _mm256_permute2x128_si256(__t4, __t6, 0x31);
  rows[7] = _mm256_permute2x128_si256(__t5, __t7, 0x31);


}

inline void transpose4_64i(__m256i *rows) {
  __m256i __t0 = _mm256_unpacklo_epi64(rows[0], rows[1]);
  __m256i __t1 = _mm256_unpackhi_epi64(rows[0], rows[1]);
  __m256i __t2 = _mm256_unpacklo_epi64(rows[2], rows[3]);
  __m256i __t3 = _mm256_unpackhi_epi64(rows[2], rows[3]);

  rows[0] = _mm256_permute2x128_si256(__t0, __t2, 0x20);
  rows[1] = _mm256_permute2x128_si256(__t1, __t3, 0x20);
  rows[2] = _mm256_permute2x128_si256(__t0, __t2, 0x31);
  rows[3] = _mm256_permute2x128_si256(__t1, __t3, 0x31);
}

void sort_columns_64i(__m256i& row0, __m256i& row1, 
    __m256i& row2, __m256i& row3) {
  minmax(row0,row1);
  minmax(row2,row3);
  minmax(row0,row2);
  minmax(row1,row3);
  minmax(row1,row2);
}

void merge8_64i(__m256i *rows) {
  minmax(rows[0], rows[4]);
  minmax(rows[1], rows[5]);
  minmax(rows[2], rows[6]);
  minmax(rows[3], rows[7]);

  minmax(rows[2], rows[4]);
  minmax(rows[3], rows[5]);

  minmax(rows[1], rows[2]);
  minmax(rows[3], rows[4]);
  minmax(rows[5], rows[6]);
}

void sort16_64i(__m256i *rows) {
  sort_columns_64i(rows[0], rows[1], rows[2], rows[3]);
  transpose4_64i(rows);
}

void sort32_64i(__m256i *rows) {
  sort_columns_64i(rows[0], rows[1], rows[2], rows[3]);
  sort_columns_64i(rows[4], rows[5], rows[6], rows[7]);

  merge8_64i(rows);
  transpose8_64i(rows);
}

// 8-by-8 merge
void bitonic_merge(__m256i& a, __m256i& b, __m256i& c, __m256i& d) {
  __m256i a1, b1, c1, d1;
  // 8-by-8 minmax
  auto cr = reverse(c);
  auto dr = reverse(d);
  minmax(a, dr, a1, c1);
  minmax(b, cr, b1, d1);

  // 4-by-4 minmax
  minmax(a1, b1, a, b);
  minmax(c1, d1, c, d);

  // intra-register minmax
  intra_register_sort(a);
  intra_register_sort(b);
  intra_register_sort(c);
  intra_register_sort(d);
}

void intra_register_sort(__m256i& a) {
  __m256i b, c, d;

  // 2-by-2 merge
  b = _mm256_permute4x64_epi64(a, 0x4e);
  minmax(a, b, c, d);
  // pick top-2 and last-2 64 bit elements from
  // corresponding registers
  a = _mm256_blend_epi32(c, d, 0xf0);

  // 1-by-1 merge
  b = _mm256_shuffle_epi32(a, 0x4e);
  minmax(a, b, c, d);
  // pick alternate elements from registers
  a = _mm256_blend_epi32(c, d, 0xcc);
}

void initialize() {
  global_masks.load_store_mask = _mm256_set1_epi64x((int64)1<<63);
}

void test_basic() {
  __m256i min, max;
  int64 test_arr1[4] = {1,2,7,8};
  int64 test_arr2[4] = {3,4,5,6};
  __m256i test1 = load_reg256((int64 *)&test_arr1[0]);
  __m256i test2 = load_reg256((int64 *)&test_arr2[0]);
  print_register(test1, "test1");
  print_register(test2, "test2");
  print_register(reverse(test1), "Reverse Output");
  minmax(test1, test2, min, max);
  print_register(min, "Minimum");
  print_register(max, "Maximum");
}
