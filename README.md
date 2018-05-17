As in Objective-C's MRC environment, retain and release can be done manually, and the life of memory securing can be managed with reference count.

how to use
By including c_reference_counter.h, you can use the full name version of memory allocation and management functions.
By including c_ref_name.h as in test.cpp you can use with short name.
Below simple usage example

// When the reference counter and memory allocation are performed at the same time
const char* test_ref() {
  long* lp = (long*)refAlloc(sizeof(long)); // allocate a reference counter area and allocate memory
  auto ref = getRefCount(lp); // Extract the reference counter structure from the secured memory
  mu_assert("error, lp == null", lp != NULL);// check memory allocation
  mu_assert("error, lp cnt != 1", ref->ref_count == 1); // The initial state reference count is 1
  refRetain(lp);// Increase reference count
  mu_assert("error, lp cnt != 2", ref->ref_count == 2);// reference count 2
  mu_assert("error, lp cnt != 1", refRelease(lp) == 1);// Reduce the reference count (memory is not freed at this point)
  mu_assert("error, lp cnt != 0", refRelease(lp) == 0);// When the reference count reaches 0, the area secured by refAlloc is freed
  return 0;
}

// Example of securing reference count and memory separately
const char * test_ref_another() {
  st_Refcounter ref_st = { 0 }; // securing reference counter
  long * lp = (long *)malloc(sizeof(long)); // allocate memory managed by reference counter
  link_obj(lp, &ref_st); // Link reference counter and memory
  mu_assert("error, lp cnt! = 1", ref_st.ref_count == 1);// Reference counter 1
  refCounterRetain(&ref_st);// Reference counter 1 increase
  mu_assert("error, lp cnt! = 2", ref_st.ref_count == 2);// reference counter 2
  mu_assert("error, lp cnt! = 1", refCounterRelease(&ref_st) == 1);// Reference counter 1
  // reference counter 0 (release of managed memory lp)
  mu_assert("error, lp cnt! = 0", refCounterRelease(&ref_st) == 0);
  return 0;
}


## License

These codes are licensed under CC0.

[![CC0](http://i.creativecommons.org/p/zero/1.0/88x31.png "CC0")](http://creativecommons.org/publicdomain/zero/1.0/deed.ja)