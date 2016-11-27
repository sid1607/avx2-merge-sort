#include <time.h>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <iterator>
#include "merge_sort.h"

void print_array(entry *a, const std::string& msg, const int size) {
  std::cout << msg << std::endl;
  for (int i=0; i<size; i++) {
    std::cout << "(" << a[i].key << ", " << a[i].oid << ")" <<  ",";
  }
  std::cout << std::endl;
}

void print_register(const __m256i& a, const std::string& msg) {
  std::cout << msg << std::endl;
  for (int i=0; i<4; i++) {
    auto ele = (entry *)&a[i];
    std::cout << "(" << ele->key << "," << ele->oid << ")" <<  ",";
  }
  std::cout << std::endl;
}

int get_rand_1000() {
  return ((rand()%2000));
}

inline bool compare_entry (entry a, entry b)
{
  return a.key < b.key;
}

int compare (const void *a, const void *b)
{
  return (*(int *)a - *(int *)b);
}

void generate_rand_entry_array(entry *a, int size) {
  for (int i=0; i<size; i++) {
    a[i].key = get_rand_1000();
    a[i].oid = get_rand_1000();
  }
}

void generate_rand_sorted_entry_array(entry *a, int size) {
  for (int i=0; i<size; i++) {
    a[i].key = get_rand_1000();
    a[i].oid = get_rand_1000();
  }
  std::sort(a, a+size, compare_entry);
}

// checks if the lower 32 bits of 64-bit elements are sorted
bool is_key_sorted(entry *a, int size) {
  for (int i=1; i<size; i++) {
    if (a[i].key < a[i-1].key) {
      return false;
    }
  }
  return true;
}

bool test_sort32_64i() {
  alignas(32) entry a[8][4];
  __m256i rows[8];

  for(int i = 0; i< 8; i++) {
    generate_rand_entry_array(a[i], 4);
    rows[i] = load_reg256((int64 *)&(a[i][0]));
  }

  sort32_64i(rows);

  for(int i = 0; i<8; i++){
    if (!is_key_sorted((entry *) &rows[i], 4)) {
      for(int j = 0; j<8; j+=2) {
        std::cout.width(10);
        std::cout 
          << ((int *) &rows[i])[j] 
          << "|" << ((int *) &rows[i])[j+1] << "\t";
      }
      std::cout <<std::endl;
      return false;
    }
  }
  return true;
}

bool test_bitonic_merge() {
  alignas(32) entry a[8];
  entry result[16];
  __m256i rows[4];
  for (int i=0; i<2; i++) {
    generate_rand_sorted_entry_array(a, 8);
    rows[2*i] = load_reg256((int64 *) &a[0]);
    rows[2*i+1] = load_reg256((int64 *) &a[4]);
  }

  bitonic_merge(rows[0], rows[1], rows[2], rows[3]);
  
  for (int i=0; i<4; i++) {
    store_reg256((int64 *) &result[i*4], rows[i]);
  }

  if (!is_key_sorted(result, 16)) {
    for (int i=0; i<4; i++)
      print_register(rows[i], "Register");
    print_array(result, "Result", 16);
    return false;
  }

  return true;
}

// bool check_bitonic_result(int *a, int *b, 
//     std::pair<__m256i, __m256i>& result) {
//   int prev = 0;
//   if (is_sorted_register(result.first, &prev)) {
//     if (is_sorted_register(result.second, &prev)) {
//       return true;
//     }
//   }
//   // print failure report
//   print_array(a, "A");
//   print_array(b, "B");
//   print_register(result.first, "Sorted_A");
//   print_register(result.second, "Sorted_B");
//   return false;
// }

// bool test_bitonic_merge() {
//   int a[8], b[8];
//   generate_random_sorted_array(a);
//   generate_random_sorted_array(b);
//   auto ymm0 = load_reg256(&a[0]);
//   auto ymm1 = load_reg256(&b[0]);
//   auto result = bitonic_merge(ymm0, ymm1);
//   return check_bitonic_result(a, b, result);
// }

// void test_sort_column() {
//   int a[8], b[8], c[8], d[8], e[8], f[8], g[8], h[8];
//   generate_random_sorted_array(a);
//   generate_random_sorted_array(b);
//   generate_random_sorted_array(c);
//   generate_random_sorted_array(d);
//   generate_random_sorted_array(e);
//   generate_random_sorted_array(f);
//   generate_random_sorted_array(g);
//   generate_random_sorted_array(h);
//   __m256i y0 = load_reg256(&a[0]);
//   __m256i y1 = load_reg256(&b[0]);
//   __m256i y2 = load_reg256(&c[0]);
//   __m256i y3 = load_reg256(&d[0]);
//   __m256i y4 = load_reg256(&e[0]);
//   __m256i y5 = load_reg256(&f[0]);
//   __m256i y6 = load_reg256(&g[0]);
//   __m256i y7 = load_reg256(&h[0]);


//   sort_columns(y0, y1, y2,y3, y4, y5,y6, y7);

//   for(int i = 0; i<8; i++){
//     std::cout
//       << ((int *) &y0)[i] <<  " " << ((int *) &y1)[i] << " "
//       << ((int *) &y2)[i] <<  " " << ((int *) &y3)[i] << " "
//       << ((int *) &y4)[i] <<  " " << ((int *) &y5)[i] << " "
//       << ((int *) &y6)[i] <<  " " << ((int *) &y7)[i] 
//       << std::endl;
//   }
// }

// bool test_sort16_64i() {
//   int64_t a[4][4];
//   __m256i row[4];

//   for(int i = 0; i< 4; i++) {
//     generate_random_array_withptr(a[i], 4);
//     row[i] =  load_reg256(&(a[i][0]));
//   }

//   for(int i = 0; i<4; i++){
//     for(int j = 0; j<8; j+=2) {
//       std::cout.width(10);
//       std::cout << ((int *) &row[i])[j] << "|" << ((int *) &row[i])[j+1] << "\t";
//     }
//     std::cout <<std::endl;
//   }

//   sort16_64i(row[0], row[1], row[2], row[3]);

//   std::cout << "Sort64 result\n";
//   for(int i = 0; i<4; i++){
//     for(int j = 0; j<8; j+=2) {
//       std::cout.width(10);
//       std::cout << ((int *) &row[i])[j] << "|" << ((int *) &row[i])[j+1] << "\t";
//     }
//     std::cout <<std::endl;
//   }

//   return true;
// }



// bool test_sort64() {
//   int a[8][8];
//   __m256i row[8];

//   for(int i = 0; i< 8; i++) {
//     generate_random_array(a[i], 8);
//     row[i] =  load_reg256(&(a[i][0]));
//   }

//   sort64(row);

//   bool succeed = true;
//   for(int i = 0; i<8; i++){
//     for(int j = 1; j<8; j++) {
//       if(((int *) &row[i])[j] < ((int *) &row[i])[j-1]){
//         succeed = false;
//         break;
//       }
//     }
//   }

//   if(!succeed){
//     std::cout << "Sort64 test failed\n";
//     for(int i = 0; i<8; i++){
//       for(int j = 1; j<8; j++) {
//         std::cout << ((int *) &row[i])[j] << " ";
//       }
//       std::cout <<std::endl;
//     }
//   }

//   return succeed;
// }

// bool is_sorted_array(int *out, int len) {
//   int prev = out[0];
//   for (int i=1; i<len; i++) {
//     if (out[i] < prev)
//       return false;
//     prev = out[i];
//   }
//   return true;
// }

// bool is_sorted_array(int *out, int *ref, int len) {
//   int prev = out[0];
//   for (int i=0; i<len; i++) {
//     if (out[i] < prev)
//       return false;
//     if (out[i] != ref[i])
//       return false;
//     prev = out[i];
//   }
//   return true;
// }

// void create_merge_array(int *a, int array_size, int merge_size) {
//   // created sorted runs of 8 elements
//   for (int i=0; i<array_size; i+=merge_size) {
//     generate_random_sorted_array(&a[i], merge_size);
//   }
// }

// bool test_merge_phase(int size) {
//   bool status;
//   int *a = new int[size], *out = new int[size];
//   create_merge_array(a, size, size/2);
//   merge_phase(a, out, 0, (size/2)-1, size-1);
//   if (is_sorted_array(out, size) == false) {
//     print_array(a, "A", size);
//     print_array(out, "Out", size);
//     status = false;
//   } else {
//     status = true;
//   }
//   delete[] a;
//   delete[] out;
//   return status;
// }

// bool test_merge_pass(int array_size, int merge_size) {
//   bool status;
//   int *a = new int[array_size];
//   int *out = new int[array_size];
//   create_merge_array(a, array_size, merge_size);
//   print_array(a, "A", array_size);

//   merge_pass(a, out, array_size, merge_size);
//     print_array(out, "Out", array_size);


//   if (is_sorted_array(out, array_size) == false) {
//     std::cout << "sorted" << std::endl;
//     print_array(a, "A", array_size);
//     print_array(out, "Out", array_size);
//     status = false;
//   } else {
//     status = true;
//   }

//   delete[] a;
//   delete[] out;
//   return status;
// }

// void generate_reference(std::vector<int>& ref) {
//   std::sort(ref.begin(), ref.end(), compare_vec);
// }

// bool test_merge(int len) {
//   bool status;
//   std::vector<int> a(len), temp(len), ref(len);
//   create_merge_array(&a[0], len, 16);
//   std::copy(&a[0], &a[0]+len, &ref[0]);

//   generate_reference(ref);
//   auto res = merge(a, temp);
//   if (is_sorted_array(&res.first[0], &ref[0], len) == false) {
//     print_array(&ref[0], "Ref", len);
//     print_array(&res.first[0], "Out", len);
//     status = false;
//   } else {
//     status = true;
//   }
//   return status;
// }

// inline double get_time() {
//   return 
//     static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(
//       std::chrono::steady_clock::now().time_since_epoch()).count())/1000;
// }

// bool test_merge_sort(int len) {
//   bool status = true;
//   std::vector<int> a(len), temp(len), ref(len);
//   generate_random_array(&a[0], len);
//   std::copy(&a[0], &a[0]+len, &ref[0]);

//   auto std_start = get_time();
//   generate_reference(ref);
//   auto std_end = get_time();

//   auto res = merge_sort(a,temp);
//   auto merge_end = get_time();
//   // print_array(&res.first[0], "Out", len);
//   if (is_sorted_array(&res.first[0], &ref[0], len) == false) {
//       print_array(&ref[0], "Ref", len);
//       print_array(&res.first[0], "Out", len);
//       status = false;
//   } else {
//     status = true;
//     std::cout << "std time:" << std_end-std_start << " Merge time:" 
//       << merge_end - std_end << std::endl;
//   }
//   return status;
// }

int main() {
  int num_iters = 1000;
  // int start = 1<<21, end = 1<<21;
  srand(time(NULL));

  initialize();

  for (int i=0; i<num_iters; i++) {
    if (!test_sort32_64i())
      return 1;
    if (!test_bitonic_merge())
      return 1;
  }
//  auto start32 = get_time();
//  test_minmax();
//  auto end32 = get_time();
//  test_minmax64();
//  auto end64 = get_time();
//
//  std::cout << "minmax time:" << end32-start32 << " minmax64 time:"
//  << end64 - end32 << std::endl;

  // test_sort32_64i();

//
//  for(int i=start; i<=end;i++) {
//    for (int j=0; j<num_iters; j++) {
//      if(!test_merge_sort(i))
//        return 1;
//    }
//    std::cout << "Passed:(" << i << "," << num_iters << ")" << std::endl;
//  }
  return 0;
}
