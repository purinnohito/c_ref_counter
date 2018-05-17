/*
minunit - for test minunit.h Compatibility

To the extent possible under law, the author(s)
have dedicated all copyright and related and neighboring
rights to this software to the public domain worldwide.
This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain
Dedication along with this software.If not, see
<https://creativecommons.org/publicdomain/zero/1.0/deed.ja>.
*/

#ifndef _MINUNIT_H_
#define _MINUNIT_H_

#include "minunit_mod.h"

/* file: minunit.h Compatibility  */

#define mu_expect(message, test, outputFunc) mo_expect(message, test, expect_cnt, outputFunc)
#define mu_assert(message, test) mo_assert(message, test) 
#define mu_run_test(test) mo_run_test(test, tests_run)

extern int tests_run;
extern int expect_cnt;


#endif//_MINUNIT_H_