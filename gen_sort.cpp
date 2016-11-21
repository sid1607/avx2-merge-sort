#include "merge_sort.h"
#include <iostream>
#include <algorithm>

// add sort phase

// the two input arrays are a[start, mid] and a[mid+1, end]
void merge_phase(int *a, int *out, int start, int mid, int end) {
  int i=0, j=mid+1, k=0;
  int i_end = mid - start + 1;
  int j_end = end - mid + i_end;

  auto ra = load_reg256(&a[i]);
  auto rb = load_reg256(&a[j]);

  i += SIMD_SIZE;
  j += SIMD_SIZE;

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
    // merge two merge_size arrays per iteration
    merge_phase(in+i, out+i, i, mid, end);
  }
}

// assume first sort phase has finished
void merge(int *a, int *out) {

}