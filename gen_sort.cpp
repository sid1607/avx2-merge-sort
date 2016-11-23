#include "merge_sort.h"
#include <iostream>
#include <algorithm>
#include <cstring>
#include <climits>
#include <cassert>

// the two input arrays are a[start, mid] and a[mid+1, end]
void merge_phase(int *a, int *out, int start, int mid, int end) {
  int i=start, j=mid+1, k=start;
  int i_end = i + mid - start + 1;
  int j_end = j + end - mid;

  auto ra = load_reg256(&a[i]);
  auto rb = load_reg256(&a[j]);

  i += SIMD_SIZE;
  j += SIMD_SIZE;

  // 8-by-8 merge
  if (mid-start+1 == SIMD_SIZE) {
    auto result = bitonic_merge(ra, rb);
    // save the smaller half
    store_reg256(&out[k], result.first);
    k += SIMD_SIZE;
    // then save the larger half
    store_reg256(&out[k], result.second);
    k += SIMD_SIZE;
    return;
  }

  do {
    auto result = bitonic_merge(ra, rb);
    
    // save the smaller half
    store_reg256(&out[k], result.first);
    k += SIMD_SIZE;
    
    // use the larger half for the next comparison
    ra = result.second;

    // select the input with the lowest value at the current pointer
    if (a[i] < a[j]) {
      rb = load_reg256(&a[i]);
      i += SIMD_SIZE;
    } else {
      rb = load_reg256(&a[j]);
      j += SIMD_SIZE;
    }
  } while (i < i_end && j < j_end);

  // merge the final pair of registers from each input
  auto result = bitonic_merge(ra, rb);
  store_reg256(&out[k], result.first);
  k += SIMD_SIZE;
  ra = result.second;

  // consume remaining data from a, if left
  while (i < i_end) {
    rb = load_reg256(&a[i]);
    i += SIMD_SIZE;
    auto result = bitonic_merge(ra, rb);
    store_reg256(&out[k], result.first);
    k += SIMD_SIZE;
    ra = result.second;
  }

  // consume remaining data from b, if left
  while (j < j_end) {
    rb = load_reg256(&a[j]);
    j += SIMD_SIZE;
    auto result = bitonic_merge(ra, rb);
    store_reg256(&out[k], result.first);
    k += SIMD_SIZE;
    ra = result.second;
  }

  // store the final batch
  store_reg256(&out[k], ra);
  k += SIMD_SIZE;
}

// minimum merge_size=16, minimum n=2*merge_size
void merge_pass(int *in, int *out, int n, int merge_size) {
  for (int i=0; i < n-1; i+=2*merge_size) {
    auto mid = i + merge_size - 1;
    auto end = std::min(i+2*merge_size-1, n-1);
    // check if there are 2 sub-arrays to merge
    if (mid < end) {
      // merge two merge_size arrays per iteration
      merge_phase(in, out, i, mid, end);
    } else {
      // copy the leftover data to output
      std::memcpy(out+i, in+i, (n-i)*sizeof(int));
    }
  }
}

// assume first sort phase has finished
std::pair<std::vector<int>, std::vector<int>> 
    merge(std::vector<int>& a, std::vector<int>& b) {
  int i=0;
  size_t len = a.size();
  /*
   * even iterations: a->b
   * odd iterations: b->a
   */
  // start from 16-16 merge
  for (size_t pass_size=SIMD_SIZE; pass_size<len; pass_size*=2, i++) {
    if (i%2 == 0) {
      merge_pass(&a[0], &b[0], len, pass_size);
    } else {
      merge_pass(&b[0], &a[0], len, pass_size);
    }
  }

  if (i%2 == 0)
    return std::make_pair(a,b);
  return std::make_pair(b,a);
}

std::pair<std::vector<int>, std::vector<int>> 
  merge_sort(std::vector<int>& a, std::vector<int>& b) {
    __m256i rows[SIMD_SIZE];
  if (a.size()%64!=0) {
    // add padding
    auto i = a.size();
    auto end = ((i+64)/64)*64;
    while (i<end) {
      a.push_back(INT_MAX);
      i++;
    }
    // adjust b's size as well
    b.resize(a.size());
  }

  assert(a.size()%64 == 0);
  assert(b.size() == a.size());

  for (size_t i=0; i < a.size(); i+=SORT_SIZE) {
    for (int j=0; j<SORT_SIZE/SIMD_SIZE; j++) {
      rows[j] = load_reg256(&a[i+j*SIMD_SIZE]);
    }
    sort64(rows);
    for (int j=0; j<SORT_SIZE/SIMD_SIZE; j++) {
      store_reg256(&a[i+j*SIMD_SIZE], rows[j]);
    }
  }

  return merge(a, b);
}
