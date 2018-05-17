/*
c_reference_counter.h
- Reference counter like Objective-C, implementation in C language

Written in 2018/05/17 by purinnohito

To the extent possible under law, the author(s)
have dedicated all copyright and related and neighboring
rights to this software to the public domain worldwide.
This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain
Dedication along with this software.If not, see
<https://creativecommons.org/publicdomain/zero/1.0/deed.ja>.
*/
#pragma once
#ifndef _C_REFERENCE_COUNTER_H_
#define _C_REFERENCE_COUNTER_H_

#ifdef _OPENMP
#include<omp.h>
#endif
#if !defined(__STDC_NO_THREADS__) && defined(__STDC_UTF_16__) && defined(__STDC_UTF_32__)
#include<threads.h>
#define USE_THREADS_H
#endif

#ifdef __cplusplus
extern "C"
{
#endif
#include<malloc.h>

#if defined(_OPENMP) && !defined(FORCE_THREADS_H) 
#define REF_MUTEX_OBJ                             omp_lock_t
#define REF_MUTEX_INIT_LOCK(_mutex)   omp_init_lock((_mutex))
#define REF_MUTEX_LOCK(_mutex)        omp_set_lock((_mutex))
#define REF_MUTEX_UNLOCK(_mutex)      omp_unset_lock((_mutex))
#define REF_MUTEX_DESTROY(_mutex)     omp_destroy_lock((_mutex))
#elif defined(FORCE_THREADS_H) || defined(USE_THREADS_H)
#define REF_MUTEX_OBJ                             mtx_t
#define REF_MUTEX_INIT_LOCK(_mutex)  mtx_init((_mutex))
#define REF_MUTEX_LOCK(_mutex)           mtx_lock((_mutex))
#define REF_MUTEX_UNLOCK(_mutex)      mtx_unlock((_mutex))
#define REF_MUTEX_DESTROY(_mutex)     mtx_destroy((_mutex))
#else
#ifndef REF_MUTEX_OBJ
#define REF_MUTEX_OBJ                             size_t
#endif
#ifndef REF_MUTEX_INIT_LOCK
#define REF_MUTEX_INIT_LOCK(_mutex)
#endif
#ifndef REF_MUTEX_LOCK
#define REF_MUTEX_LOCK(_mutex) 
#endif
#ifndef REF_MUTEX_UNLOCK
#define REF_MUTEX_UNLOCK(_mutex)
#endif
#ifndef REF_MUTEX_DESTROY
#define REF_MUTEX_DESTROY(_mutex)
#endif
#endif

enum c_reference_counter_alloc_state {
  outside_manage_c_reference_counter_alloc_state,
  self_manage_c_reference_counter_alloc_state,
  non_manage_c_reference_counter_alloc_state
};

typedef void * (*c_ref_counter__Malloc_Function)(size_t);
typedef void(*c_ref_counter__Free_Function)(void *);
#define Malloc_Function   c_ref_counter__Malloc_Function
#define Free_Function     c_ref_counter__Free_Function


typedef struct c_reference_counter_struct {
  const int       alloc_state;      //this manage state
  Free_Function   ref_freeFunc;     //ref_pointer free
  void*           ref_pointer;
  size_t          ref_count;
  REF_MUTEX_OBJ   writelock;
} c_ref_counter__st_Refcounter;
#define st_Refcounter  c_ref_counter__st_Refcounter

//-------------------------------------------------------
// alloceter

// alloc pointer(reference_counter self management)
static inline void * c_ref_counter__alloc_self(size_t size);
static inline void * c_ref_counter__custom_alloc_self(size_t size
                              , Malloc_Function mallocFunction
                              , Free_Function freeFunc);
// alias
#define refAlloc(_size)                           c_ref_counter__alloc_self(_size)
#define refCustomAlloc(_size, _alloc, _free)      c_ref_counter__custom_alloc_self(_size, _alloc, _free)

// alloc pointer(reference_counter outside management)
static inline void * c_ref_counter__alloc_another(size_t size, st_Refcounter* ref_obj);
static inline void * c_ref_counter__custom_alloc_another(size_t size
                              , st_Refcounter* ref_obj
                              , Malloc_Function mallocFunction
                              , Free_Function ref_freeFunc);
// link pointer(reference_counter outside management)
static inline void * c_ref_counter__link_obj(void* link_obj, st_Refcounter* ref_obj);
static inline void * c_ref_counter__custom_link_obj(void* link_obj
  , st_Refcounter* counter_obj
  , Free_Function ref_freeFunc);
//-------------------------------------------------------
// get Reference Countera

// only alloc_self
static inline st_Refcounter* c_ref_counter__getSelfRefcounter(void *ptr);
// alias
#define getRefCount(_ptr)         c_ref_counter__getSelfRefcounter(_ptr)

//-------------------------------------------------------
// retain

// reference count up(only alloc_self)
static inline void c_ref_counter__selfRefRetain(void * ptr);
// alias
#define refRetain(_ptr)         c_ref_counter__selfRefRetain(_ptr)

// reference count up(direct st_Refcounter)
static inline void c_ref_counter__refCounterRetain(st_Refcounter* mem_object);

//-------------------------------------------------------
// release

// reference count release(only alloc_self)
static inline size_t c_ref_counter__selfRefRelease(void * ptr);
// alias
#define refRelease(_ptr)          c_ref_counter__selfRefRelease(_ptr)

// reference count release(direct st_Refcounter)
static inline size_t c_ref_counter__refCounterRelease(st_Refcounter* mem_object);




// alloc pointer(reference_counter self management)
static inline void * c_ref_counter__custom_alloc_self(size_t size, Malloc_Function mallocFunction, Free_Function freeFunc)
{
  if (!mallocFunction){
    return NULL;
  }
  st_Refcounter *mem_object = (st_Refcounter*)mallocFunction(sizeof(st_Refcounter)+size);
  if (!mem_object) {
    return NULL;
  }
  char  *ptr = (char*)mem_object;
  ptr += sizeof(st_Refcounter);
  mem_object->ref_count = 1;
  mem_object->ref_pointer = ptr;
  mem_object->ref_freeFunc = freeFunc;
  REF_MUTEX_INIT_LOCK(&mem_object->writelock);

  *((int*)(&mem_object->alloc_state)) = self_manage_c_reference_counter_alloc_state;
  return (void *)ptr;
}
static inline void * c_ref_counter__alloc_self(size_t size) {
  return c_ref_counter__custom_alloc_self(size, malloc, free);
}

// link pointer(reference_counter outside management)
static inline void * c_ref_counter__custom_link_obj(void* link_obj
                              , st_Refcounter* counter_obj
                              , Free_Function ref_freeFunc)
{
  if (!counter_obj) {
    return NULL;
  }
  counter_obj->ref_count = 1;
  counter_obj->ref_pointer = link_obj;
  counter_obj->ref_freeFunc = ref_freeFunc;
  REF_MUTEX_INIT_LOCK(&counter_obj->writelock);


  if (counter_obj->alloc_state != outside_manage_c_reference_counter_alloc_state) {
    *((int*)(&counter_obj->alloc_state)) = outside_manage_c_reference_counter_alloc_state;
  }
  return link_obj;
}
static inline void * c_ref_counter__link_obj(void* link_obj, st_Refcounter* counter_obj) {
  return c_ref_counter__custom_link_obj(link_obj, counter_obj, free);
}

// alloc pointer(reference_counter outside management)
static inline void * c_ref_counter__custom_alloc_another(size_t size
                              , st_Refcounter* counter_obj
                              , Malloc_Function mallocFunction
                              , Free_Function ref_freeFunc)
{
  if (!mallocFunction || !counter_obj) {
    return NULL;
  }
  char  *ptr = (char*)mallocFunction(size);
  if (!ptr) {
    return NULL;
  }
  return (void *)c_ref_counter__custom_link_obj(ptr, counter_obj, ref_freeFunc);
}
static inline void * c_ref_counter__alloc_another(size_t size, st_Refcounter* counter_obj) {
  return c_ref_counter__custom_alloc_another(size, counter_obj, malloc, free);
}

// only alloc_self
static inline st_Refcounter* c_ref_counter__getSelfRefcounter(void *ptr) {
  char *refCntPtr = (char*)ptr;
  refCntPtr -= sizeof(st_Refcounter);
  return (st_Refcounter*)refCntPtr;
}

// reference count up
static inline void c_ref_counter__refCounterRetain(st_Refcounter* mem_object) {
  {
#pragma omp atomic
    ++(mem_object->ref_count);
  }
}

// reference count up(only alloc_self)
static inline void c_ref_counter__selfRefRetain(void * ptr)
{
  c_ref_counter__refCounterRetain(c_ref_counter__getSelfRefcounter(ptr));
}

// reference count release(direct st_Refcounter)
static inline size_t c_ref_counter__refCounterRelease(st_Refcounter* mem_object)
{
  REF_MUTEX_LOCK(&mem_object->writelock);
  REF_MUTEX_OBJ  writelock = mem_object->writelock;
  {
#pragma omp atomic
    --(mem_object->ref_count);
    if (mem_object->ref_count == 0)
    {
      Free_Function ref_freeFunc = mem_object->ref_freeFunc;
      if (mem_object->alloc_state == outside_manage_c_reference_counter_alloc_state) {
        ref_freeFunc(mem_object->ref_pointer);
        mem_object->ref_pointer = NULL;
        *((int*)(&mem_object->alloc_state)) = non_manage_c_reference_counter_alloc_state;
        REF_MUTEX_UNLOCK(&writelock);
        omp_destroy_lock(&writelock);
        return 0;
      }
      // if(mem_object->alloc_state == self_manage_c_reference_counter_alloc_state)
      else {
        ref_freeFunc(mem_object);
        REF_MUTEX_UNLOCK(&writelock);
        omp_destroy_lock(&writelock);
        return 0;
      }
    }
  }
  REF_MUTEX_UNLOCK(&writelock);
  return mem_object->ref_count;
}

// reference count release(only alloc_self)
static inline size_t c_ref_counter__selfRefRelease(void * ptr)
{
  {
    return c_ref_counter__refCounterRelease(c_ref_counter__getSelfRefcounter(ptr));
  }
}

#ifdef __cplusplus
}
#endif

#endif//_C_REFERENCE_COUNTER_H_