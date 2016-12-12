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

int compare_entry_qsort (const void *a, const void *b) {
  return (((entry *)a)->key - ((entry *)b)->key);
}

int compare (const void *a, const void *b) {
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
      std::cout << "i:" << a[i].key << ",i-1: " << a[i-1].key << std::endl;
      return false;
    }
  }
  return true;
}

bool check_reference(entry *a, std::vector<entry>& ref, size_t len) {
  for (size_t i=0; i<len; i++) {
    if (a[i].key != ref[i].key) {
      std::cout << "a[i]:" << a[i].key << "ref[i]: " << ref[i].key << std::endl;
      return false;
    }
  }
  return true;
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


// bool test_sort32_64i() {
//   alignas(32) entry a[8][4];
//   entry result[32];
//   __m256i rows[8];

//   for(int i = 0; i< 8; i++) {
//     generate_rand_entry_array(a[i], 4);
//     rows[i] = load_reg256((sort_ele_type *)&(a[i][0]));
//   }

//   sort32_64i(rows);

//   for (int i=0; i<8; i++) {
//     store_reg256((sort_ele_type *)&result[i*4], rows[i]);
//   }

//   for(int i = 0; i<4; i++){
//     if (!is_key_sorted((entry *) &result[i*8], 8)) {
//       print_array(&result[i*8], "ResultRow", 8);
//       return false;
//     }
//   }
//   return true;
// }

// bool test_bitonic_merge() {
//   alignas(32) entry a[8];
//   entry result[16];
//   __m256i rows[4];
//   for (int i=0; i<2; i++) {
//     generate_rand_sorted_entry_array(a, 8);
//     rows[2*i] = load_reg256((sort_ele_type *) &a[0]);
//     rows[2*i+1] = load_reg256((sort_ele_type *) &a[4]);
//   }

//   bitonic_merge(rows[0], rows[1], rows[2], rows[3]);
  
//   for (int i=0; i<4; i++) {
//     store_reg256((sort_ele_type *) &result[i*4], rows[i]);
//   }

//   if (!is_key_sorted(result, 16)) {
//     for (int i=0; i<4; i++)
//       print_register(rows[i], "Register");
//     print_array(result, "Result", 16);
//     return false;
//   }

//   return true;
// }

// bool test_merge_phase(int size) {
//   alignas(32) std::vector<entry> a(2*size), res(2*size);
//   generate_rand_sorted_entry_array(&a[0], size);
//   generate_rand_sorted_entry_array(&a[size], size);
//   merge_phase((sort_ele_type *)&a[0], (sort_ele_type *)&res[0], 0, size-1, 2*size-1);
//   if (!is_key_sorted(&res[0], 2*size)) {
//     print_array(&a[0], "A", size);
//     print_array(&a[size], "B", size);
//     print_array(&res[0], "Result", 2*size);
//     return false;
//   }
//   return true;
// }

void generate_reference(std::vector<entry>& ref) {
  std::sort(ref.begin(), ref.end(), compare_entry);
  // qsort(&ref[0], ref.size(), sizeof(entry), compare_entry_qsort); 
}

// bool test_merge(int len) {
//   std::vector<entry> a(len), temp(len), ref(len);
//   for (int i=0; i<len; i+=8) {
//     generate_rand_sorted_entry_array(&a[i], 8);
//   }
//   // print_array(&a[0], "A", len);
//   std::copy(&a[0], &a[0]+len, &ref[0]);
//   generate_reference(ref);
//   auto res = merge(a, temp);

//   if (!check_reference(res.first, ref, len)) {
//     print_array(&ref[0], "Ref", len);
//     print_array(&res.first[0], "Out", len);
//     return false;
//   }
//   return true;
// }

inline double get_time() {
  return 
    static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::steady_clock::now().time_since_epoch()).count())/1000;
}

bool test_merge_sort(int len) {
  bool status = true;
  entry *a = nullptr, *temp = nullptr;

  // 32-byte aligned allocation
  if (posix_memalign((void **)&a, 32, len*sizeof(entry)) != 0) {
    throw std::bad_alloc();
  }

  if (posix_memalign((void **)&temp, 32, len*sizeof(entry)) != 0) {
    throw std::bad_alloc();
  }

  std::vector<entry> ref(len);
  generate_rand_entry_array(a, len);
  std::copy(a, a+len, &ref[0]);

  auto std_start = get_time();
  generate_reference(ref);
  auto std_end = get_time();

  auto res = merge_sort((sort_ele_type*)a, (sort_ele_type*)temp, len);
  auto merge_end = get_time();
  
  if (!check_reference((entry*)res.first, ref, len)) {
    print_array(&ref[0], "Ref", len);
    print_array((entry*)res.first, "Out", len);
    status = false;
  } else {
    status = true;
    std::cout << "std time: " << std_end-std_start << "\nMerge time:" 
      << merge_end - std_end << std::endl;
  }
  return status;
}

int main(int argc, char *argv[]) {
  int num_iters = 1, pow_2 = 24;
  int start, end;
  srand(time(NULL));

  if (argc==2) {
    pow_2 = atoi(argv[1]);
  } else {
    pow_2 = 24;
  }

  start = 1 << pow_2;
  end = 1 << pow_2;

  for(int i=start; i<=end;i++) {
    for (int j=0; j<num_iters; j++) {
      if(!test_merge_sort(i))
        return 1;
    }
    std::cout << "Passed:( " << i << " ," << num_iters << ")" << std::endl;
  }
  return 0;
}