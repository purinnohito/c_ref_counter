/*
minunit - for test

To the extent possible under law, the author(s) 
have dedicated all copyright and related and neighboring
rights to this software to the public domain worldwide.
This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain
Dedication along with this software.If not, see 
<https://creativecommons.org/publicdomain/zero/1.0/deed.ja>. 
*/
#ifndef _MINUNIT_MOD_H_
#define _MINUNIT_MOD_H_

/* file: minunit.h mod  */

#define mo_expect(message, test, ex_cnt, outputFunc) do { if (!(test)){ outputFunc(message);++ex_cnt; }} while (0)
#define mo_assert(message, test) do { if (!(test)) return message; } while (0)
#define mo_run_test(test, tests_cnt) do { const char *message = test(); ++tests_cnt; \
                                if (message) return message; } while (0)


#endif//_MINUNIT_MOD_H_