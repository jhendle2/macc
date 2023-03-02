#ifndef MACROS_H
#define MACROS_H

#include <stdio.h>
#include <stdbool.h>

#ifdef ENABLE_DEBUG_FLAG
    static bool debug_flag = true;
#else
    static bool debug_flag = false;
#endif /* ENABLE_DEBUG_FLAG */

#define printf_dbg(...) if (debug_flag) printf(__VA_ARGS__)


#define NOTICE(TYPE, NAME, ...) {printf("\n[" TYPE " - " NAME "]\n" __VA_ARGS__); printf("\n");}
#define NOTICE_EXIT(TYPE, NAME, ...) {NOTICE(TYPE, NAME, __VA_ARGS__); safeExit(EXIT_FAILURE);}
#define NOTICE_EXIT_CODE(EXIT_CODE, TYPE, NAME, ...) {NOTICE(TYPE, NAME, __VA_ARGS__); safeExit(EXIT_CODE);}

#endif /* MACROS_H */