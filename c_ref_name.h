/*
c_reference_counter.h
- Reference counter short name

Written in 2018/05/17 by purinnohito

To the extent possible under law, the author(s)
have dedicated all copyright and related and neighboring
rights to this software to the public domain worldwide.
This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain
Dedication along with this software.If not, see
<https://creativecommons.org/publicdomain/zero/1.0/deed.ja>.
*/
#ifndef _C_REF_NAME_H_
#define _C_REF_NAME_H_

#include "c_reference_counter.h"


//-------------------------------------------------------
// alloceter

// alloc pointer(reference_counter self management)
#define alloc_self(_size)                         c_ref_counter__alloc_self(_size)
#define custom_alloc_self(_size, _alloc, _free)   c_ref_counter__custom_alloc_self(_size, _alloc, _free)

// alloc pointer(reference_counter outside management)
#define alloc_another(_size, _ref_obj)           c_ref_counter__alloc_another(_size, _ref_obj)
#define custom_alloc_another(_size, _ref_obj, _alloc, _free) \
 c_ref_counter__custom_alloc_another(_size, _ref_obj, _alloc, _free)

//-------------------------------------------------------
// get Reference Countera

// only alloc_self
#define getSelfRefcounter(_ptr)   c_ref_counter__getSelfRefcounter(_ptr)

//-------------------------------------------------------
// retain

// reference count up(only alloc_self)
#define selfRefRetain(_ptr)     c_ref_counter__selfRefRetain(_ptr)

// reference count up(direct c_reference_counter)
#define refCounterRetain(_object)    c_ref_counter__refCounterRetain(_object)

//-------------------------------------------------------
// release

// reference count release(only alloc_self)
#define selfRefRelease(_ptr)      c_ref_counter__selfRefRelease(_ptr)

// reference count release(direct c_ref_counter__st_Refcounter)
#define refCounterRelease(_ptr)   c_ref_counter__refCounterRelease(_ptr)




#endif//_C_REF_NAME_H_