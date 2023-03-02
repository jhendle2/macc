#include "safe.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "macros.h"
#include "file_reader.h"
#include "lexer.h"

FileLine* file_as_lines  = NULL;

void safeFreeAll() {
    /* main.c */
    safeFree(file_as_lines);

    /* lexer.c */
    extern Token* line_as_tokens;
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
    deleteLexTree(master_node);

    safeFreeAll();
    
    printf("All done.\n");
    return EXIT_SUCCESS;
}