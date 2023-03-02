#ifndef FILE_READER_H
#define FILE_READER_H

#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct file_line_s {
    #ifndef FILE_LINE_S
    #define FILE_LINE_S
        #define MAX_FILE_NAME_SZ 64
        #define MAX_LINE_BUF_SZ 128

        #define MAX_LINES_IN_FILE 512
        // #define MAX_LINES_IN_FILE 1024
        // #define MAX_LINES_IN_FILE 2048
        // #define MAX_LINES_IN_FILE 4096

        #define MAX_STR_FILELINE_SZ 256
        // #define MAX_STR_FILELINE_SZ 200
    #endif /* FILE_LINE_S */
    
    size_t line_number;
    char line_buf[MAX_LINE_BUF_SZ];
    char file_name[MAX_FILE_NAME_SZ];
} FileLine;

FileLine newFileLine(const size_t line_number, const char line_buf[MAX_LINE_BUF_SZ], const char file_name[MAX_FILE_NAME_SZ]);

const char* strFileLine(const FileLine fl);
bool copyFileLine(FileLine* a, const FileLine b);
bool isEmptyFileLine(const FileLine fl);

size_t readFileAsLines(const char file_name[MAX_FILE_NAME_SZ], FileLine** file_as_lines);

static inline bool isSpace(const char c) {
    return (
        c == ' '  ||
        c == '\t' ||
        c == '\n' ||
        c == '\r' ||
        0
    );
}

#endif /* FILE_READER_H */