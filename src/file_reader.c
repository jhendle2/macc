#include "file_reader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "macros.h"
#include "safe.h"
#include "debug.h"

FileLine newFileLine(const size_t line_number, const char line_buf[MAX_LINE_BUF_SZ], const char file_name[MAX_FILE_NAME_SZ]) {
    FileLine fl = {
        .line_number = line_number
    };
    strncpy(fl.line_buf , line_buf , MAX_LINE_BUF_SZ);
    strncpy(fl.file_name, file_name, MAX_FILE_NAME_SZ);
    return fl;
}

#define FIND_OF_CHAR_W_BOOL_W_HITS(FN_NAME, BOOLEAN, HITS) \
    static ssize_t FN_NAME(char* s, const size_t len, const char to_find) { \
        size_t hit_counter = 0; \
        for (ssize_t i = 0; i<len; i++) { \
            if (!s[i]) break; \
            if ( (s[i] == to_find) == BOOLEAN) { \
                hit_counter++; \
                if (hit_counter == HITS) return i; \
                continue; \
            } \
        } \
        return -1; \
    }
FIND_OF_CHAR_W_BOOL_W_HITS(findFirstOf    , true , 1)
FIND_OF_CHAR_W_BOOL_W_HITS(findSecondOf   , true , 2)
FIND_OF_CHAR_W_BOOL_W_HITS(findFirstNotOf , false, 1)
FIND_OF_CHAR_W_BOOL_W_HITS(findSecondNotOf, false, 2)

static void lstrip(char* s, const size_t len) {
    char* t_src = (char*)malloc(sizeof(char)*len);
    char* t = t_src;
    strncpy(t, s, len);
    const size_t first_not_of_space = findFirstNotOf(t, MAX_LINE_BUF_SZ, ' ');
    if (first_not_of_space > 0) t += first_not_of_space;
    strncpy(s, t, len);
    free(t_src);
}

bool isEmptyFileLine(const FileLine fl) {
    for (size_t i = 0; i<strlen(fl.line_buf); i++)
        if (!isSpace(fl.line_buf[i])) return false;
    return true;
}

const char* strFileLine(const FileLine fl) {
    static char buf[MAX_STR_FILELINE_SZ];
    char* stripped = (char*)malloc(sizeof(char)*MAX_LINE_BUF_SZ);
    strncpy(stripped, fl.line_buf, MAX_LINE_BUF_SZ);
    lstrip(stripped, MAX_LINE_BUF_SZ);
    snprintf(buf, MAX_STR_FILELINE_SZ, "%s:%zu: %s", fl.file_name, fl.line_number, stripped);
    free(stripped);
    return buf;
}
bool copyFileLine(FileLine* a, const FileLine b) {
    assert(a);
    a->line_number = b.line_number;
    memcpy(a->line_buf, b.line_buf, MAX_FILE_NAME_SZ);
    memcpy(a->file_name, b.file_name, MAX_FILE_NAME_SZ);
    return true;
}


static size_t sanitizeLine(char** line_buf) {
    printf_dbg("Sanitizing Line `%s`\n", *line_buf);

    char* new_buf = (char*)malloc(sizeof(char)*MAX_LINE_BUF_SZ);
    char* temp = new_buf;
    memcpy(temp, *line_buf, MAX_LINE_BUF_SZ);

    /* Ignore preprocessor commands */
    const ssize_t first_not_space = findFirstNotOf(temp, MAX_LINE_BUF_SZ, ' ');
    if (first_not_space >= 0 && first_not_space < MAX_LINE_BUF_SZ-2) {
        if (temp[first_not_space] == '#') {
            printf_dbg("[!] Found a preproc -> Ignoring it (for now)...\n");
            temp[first_not_space] = 0;
            goto END;
        }
    }

    /* Strip line comments */
    const ssize_t first_slash_index  = findFirstOf (temp, MAX_LINE_BUF_SZ, '/');
    const ssize_t second_slash_index = findSecondOf(temp, MAX_LINE_BUF_SZ, '/');
    if ( (second_slash_index > first_slash_index) && (second_slash_index == first_slash_index+1)) {
        if (first_slash_index >= 0 && first_slash_index < MAX_LINE_BUF_SZ-1) temp[first_slash_index] = 0;
    }

    /* Strip newline and return escape characters */
    const ssize_t newline_index = findFirstOf(temp, MAX_LINE_BUF_SZ, '\n');
    const ssize_t return_index  = findFirstOf(temp, MAX_LINE_BUF_SZ, '\r');

    /* Double checks here ensure that valgrind doesn't cry about invalid write */
    if (newline_index >= 0 && newline_index < MAX_LINE_BUF_SZ-1) temp[newline_index] = 0;
    if (return_index  >= 0 && return_index < MAX_LINE_BUF_SZ-1) temp[return_index]  = 0;

END:
    strncpy(*line_buf, temp, MAX_LINE_BUF_SZ);
    free(new_buf);

    return strlen(*line_buf);
}

size_t readFileAsLines(const char file_name[MAX_FILE_NAME_SZ], FileLine** file_as_lines) {
    printf_dbg("Reading file `%s` as lines...\n", file_name);
    FILE* in_file = fopen(file_name, "r");
    if (!in_file) NOTICE_EXIT("RuntimeError", "File Not Found", "File with name `%s` could not be found", file_name);

    size_t len = MAX_LINE_BUF_SZ;
    char* line_buf = (char*)malloc(sizeof(char)*len);
    size_t line_number = 0;
    size_t num_lines = 0;

    while (getline(&line_buf, &len, in_file) != -1) {
        line_number++;
        size_t sanitized_len = sanitizeLine(&line_buf);
        if (sanitized_len == 0) continue;

        FileLine fl = newFileLine(line_number, line_buf, file_name);

        DebugLastFileLine = &fl;
        copyFileLine(&(*file_as_lines)[num_lines++], fl);
    }

    free(line_buf);
    fclose(in_file);

    return num_lines;
}
