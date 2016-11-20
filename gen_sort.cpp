#include "merge_sort.h"
#include <iostream>

// add sort phase

void merge_phase(int *a, int *b, int *out, int merge_size) {
  int i=0, j=0, k=0; // index ptrs for a and b
  int i_end = i+merge_size, j_end = j+merge_size;

  auto ra = load_reg256(&a[i]);
  auto rb = load_reg256(&b[j]);

  // std::cout << "reached" << std::endl;

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
    if (a[i] < b[j]) {
      rb = load_reg256(&a[i]);
      i += SIMD_SIZE;
    } else {
      rb = load_reg256(&b[j]);
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
    rb = load_reg256(&b[j]);
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

void merge_pass(int *in, int *out, int n, int merge_size) {
  for (int i=0; i < n-merge_size; i+=2*merge_size) {
    // merge two merge_size arrays per iteration
    merge_phase(in+i, in+i+merge_size, out+i, merge_size);
  }
}

// assume first sort phase has finished
void merge(int *a, int *out) {

}