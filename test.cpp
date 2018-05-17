/*
c_reference_counter.h
- Reference counter test & sample

Written in 2018/05/17 by purinnohito

To the extent possible under law, the author(s)
have dedicated all copyright and related and neighboring
rights to this software to the public domain worldwide.
This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain
Dedication along with this software.If not, see
<https://creativecommons.org/publicdomain/zero/1.0/deed.ja>.
*/
#include "c_ref_name.h"
#include "minunit.h"
#include <sstream>
#include <assert.h>

int tests_run = 0;
int expect_cnt = 0;


const char* test_ref() {
  long* lp = (long*)refAlloc(sizeof(long));
  auto ref = getRefCount(lp);
  mu_assert("error, lp == null", lp != NULL);
  mu_assert("error, lp cnt != 1", ref->ref_count == 1);
  refRetain(lp);
  mu_assert("error, lp cnt != 2", ref->ref_count == 2);
  mu_assert("error, lp cnt != 1", refRelease(lp) == 1);
  mu_assert("error, lp cnt != 0", refRelease(lp) == 0);
  return 0;
}

const char* test_ref_parallel() {
  static std::string output;
  long* lp = (long*)refAlloc(sizeof(long));
  *lp = 0;
  auto ref = getRefCount(lp);
  mu_assert(u8"error, lp == null", lp != NULL);
  {
    {
#pragma omp parallel
#pragma omp for
      for (int i = 0; i < 100; i++)
      {
        refRetain(lp);
        std::stringstream ss(u8"error, lp cnt != ");
        ss << (i + 1) << std::endl;
#pragma omp critical
        {
          output = ss.str();
          mu_expect(output.c_str(), ref->ref_count, printf);
        }
      }
      mu_expect(u8"error, lp cnt != ", ref->ref_count == 101, printf);
#pragma omp flush
    }
    mu_assert(u8"error, lp cnt != 101", ref->ref_count == 101);
    {

      for (int i = 0; i < 100; i++)
      {
        {
          std::stringstream ss(u8"error, lp cnt != ");
          ss << (i + 1) << std::flush;
          output = ss.str();
          mu_assert(output.c_str(), refRelease(lp));
        }
      }
    }
  }
  mu_assert(u8"error, lp cnt != 0", refRelease(lp) == 0);
  return 0;
}

const char * all_tests() {
  mu_run_test(test_ref);
  mu_run_test(test_ref_parallel);
  return 0;
}

int main()
{
  const char *result = NULL;
  try {
    result = all_tests();
    if (result != 0) {
      printf("%s\n", result);
    }
    else if (expect_cnt) {
      printf("expect %d\n", expect_cnt);
    }
    else {
      printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);
  }
  catch (...) {
    printf("reigai\n");
  }
  getchar();
  return result != 0;
}

