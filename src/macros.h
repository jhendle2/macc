#ifndef MACROS_H
#define MACROS_H

#include <stdio.h>
#include <stdbool.h>

#ifdef ENABLE_DEBUG_FLAG
    static bool debug_flag = true;
#else
    static bool debug_flag = false;
#endif /* ENABLE_DEBUG_FLAG */

#define printf_dbg(...) if (debug_flag) { printf(__VA_ARGS__); }

#define ERROR_GENERIC 1
#define ERROR_UNEXPECTED_COMPILER 2
#define ERROR_MISMATCHED_BRACES 3

#include "debug.h"

#define NOTICE(TYPE, NAME, ...) {\
        printf("\n[" TYPE " - " NAME "]\n" __VA_ARGS__); printf("\n");\
        if (DebugLastFileLine) printf("%s", strFileLine(*DebugLastFileLine));\
        if (DebugLastToken)    printf("%s", strToken   (*DebugLastToken)   );\
    }
#define NOTICE_EXIT(TYPE, NAME, ...) {NOTICE(TYPE, NAME, __VA_ARGS__); safeExit(EXIT_FAILURE);}
#define NOTICE_EXIT_CODE(EXIT_CODE, TYPE, NAME, ...) {NOTICE(TYPE, NAME, __VA_ARGS__); safeExit(EXIT_CODE);}

#endif /* MACROS_H */