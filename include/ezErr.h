#ifndef _EZERR_H_
#define _EZERR_H_

#include <stdio.h>
#include <stdlib.h>

void ERR_stamp_val(const char* file, const char* func, unsigned int line);

#define ERR_stamp() ERR_stamp_val(__FILE__, __func__, __LINE__)

/* https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html */
#define ERR_alert(format, ...) {ERR_stamp(); fprintf(stderr, format "\n" __VA_OPT__(,) __VA_ARGS__);}
#define ERR_throw(format, ...) {ERR_alert(format, __VA_ARGS__); exit(EXIT_FAILURE);}

#define ERR_warning(format, ...) ERR_alert("Warning : " format, __VA_ARGS__)
#define ERR_error(  format, ...) ERR_throw("Error   : " format, __VA_ARGS__)

#define ERR_assert( test, message) if (!(test)) ERR_error  ("Failed assertion " #test " : %s", message);
#define ERR_assertw(test, message) if (!(test)) ERR_warning("Failed assertion " #test " : %s", message);




#endif /* ezErr_h */