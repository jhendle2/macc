#include "safe.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "macros.h"
#include "file_reader.h"
#include "lexer.h"

FileLine* file_as_lines     = NULL;
Token* line_as_tokens       = NULL;

/* Debug variables for printing errors, warnings, etc. */
FileLine const* DebugLastFileLine = NULL;
Token const*    DebugLastToken    = NULL;

void safeFreeAll() {
    /* In the event the program crashes early... Let's hope this works :) */

    /* main.c */
    safeFree(file_as_lines);

    /* lexer.c */
    safeFree(line_as_tokens);
}

int safeExit(const int exit_code) {
    safeFreeAll();
    exit(exit_code);
}

int main(int argc, char** argv) {
    printf("macc starting up...\n");

    if (argc == 1) NOTICE_EXIT("RuntimeError", "No Compiler Arguments", "Compiler cannot evaluate zero arguments");

    file_as_lines = (FileLine*)malloc(sizeof(FileLine)*MAX_LINES_IN_FILE);
    const size_t num_lines = readFileAsLines(argv[1], &file_as_lines);
    printf_dbg("\n");

    const LexNode master_node = buildLexTree(file_as_lines, num_lines);
    printLexTree(master_node);
    deleteLexTree(master_node);

    safeFreeAll();
    
    printf("All done.\n");
    return EXIT_SUCCESS;
}