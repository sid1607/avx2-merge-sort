#include <time.h>
#include <stdlib.h>
#include <iostream>
#include "merge_sort.h"

void print_array(int *a, const std::string& msg, int size=8) {
  std::cout << msg << std::endl;
  for (int i=0; i<size; i++) {
    std::cout << a[i] <<  ",";
  }
  std::cout << std::endl;
}

void print_register(__m256i a, const std::string& msg) {
  std::cout << msg << std::endl;
  for (int i=0; i<8; i++) {
    std::cout << ((int *) &a)[i] <<  ",";
  }
  std::cout << std::endl;
}

int get_rand_1000() {
  return ((rand()%2000));
}

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

void test_sort() {
  int odd[8] = {1,5,8,12,17,21,26,37};
  int even[8] = {2,3,4,5,10,13,14,80};
  __m256i a = load_reg256(&odd[0]);
  __m256i b = load_reg256(&even[0]);
  auto result = bitonic_merge(a, b);
  print_register(result.first, "Upper");
  print_register(result.second, "Lower");
}

void generate_random_array(int *a, int size=8) {
  for (int i=0; i<size; i++) {
    a[i] = get_rand_1000();
  }
  qsort(a, size, sizeof(int), compare);
}

bool is_sorted(__m256i a, int *prev) {
  for (int i=0; i<8; i++) {
    auto curr = ((int *) &a)[i];
    if (curr < *prev) return false;
    *prev = curr;
  }
  return true;
}

bool check_bitonic_result(int *a, int *b, 
    std::pair<__m256i, __m256i>& result) {
  int prev = 0;
  if (is_sorted(result.first, &prev)) {
    if (is_sorted(result.second, &prev)) {
      return true;
    }
  }
  // print failure report
  print_array(a, "A");
  print_array(b, "B");
  print_register(result.first, "Sorted_A");
  print_register(result.second, "Sorted_B");
  return false;
}

bool test_bitonic_merge() {
  int a[8], b[8];
  generate_random_array(a);
  generate_random_array(b);
  auto ymm0 = load_reg256(&a[0]);
  auto ymm1 = load_reg256(&b[0]);
  auto result = bitonic_merge(ymm0, ymm1);
  return check_bitonic_result(a, b, result);
}

void test_sort_column() {
  int a[8], b[8], c[8], d[8], e[8], f[8], g[8], h[8];
  generate_random_array(a);
  generate_random_array(b);
  generate_random_array(c);
  generate_random_array(d);
  generate_random_array(e);
  generate_random_array(f);
  generate_random_array(g);
  generate_random_array(h);
  __m256i y0 = load_reg256(&a[0]);
  __m256i y1 = load_reg256(&b[0]);
  __m256i y2 = load_reg256(&c[0]);
  __m256i y3 = load_reg256(&d[0]);
  __m256i y4 = load_reg256(&e[0]);
  __m256i y5 = load_reg256(&f[0]);
  __m256i y6 = load_reg256(&g[0]);
  __m256i y7 = load_reg256(&h[0]);


  sort_columns(y0, y1, y2,y3, y4, y5,y6, y7);

  for(int i = 0; i<8; i++){
    std::cout
      << ((int *) &y0)[i] <<  " " << ((int *) &y1)[i] << " "
      << ((int *) &y2)[i] <<  " " << ((int *) &y3)[i] << " "
      << ((int *) &y4)[i] <<  " " << ((int *) &y5)[i] << " "
      << ((int *) &y6)[i] <<  " " << ((int *) &y7)[i] 
      << std::endl;
  }
}

bool check_merge_phase(int *out, int len) {
  int prev = out[0];
  for (int i=1; i<len; i++) {
    if (out[i] < prev)
      return false;
  }
  return true;
}

bool test_merge_phase() {
  int a[16], b[16], c[32];
  //int c[32];
  generate_random_array(a, 16);
  generate_random_array(b, 16);
  merge_phase(a, b, c, 16);
  if (check_merge_phase(c, 16) == false) {
    print_array(a, "A", 16);
    print_array(b, "B", 16);
    print_array(c, "Out", 16);
    return false;
  }
  return true;
}

int main() {
  test_sort64();
  int num_iters = 100000;
  srand(time(NULL));
  initialize();
  for (int i=0; i<num_iters; i++) {
    if (!test_bitonic_merge())
      return 1;
    if (!test_merge_phase())
      return 1;
  }
  std::cout << "Passed:" << num_iters << std::endl;
  return 0;
}