#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "macros.h"
#include "safe.h"

void clearBuf(char* s, const size_t max_len) {
    assert(s);
    for (size_t i = 0; i<max_len; i++) s[i] = 0;
}

void appendChar(char* s, const size_t max_len, const char c) {
    assert(s);
    const size_t current_len = strlen(s);
    if ( (current_len + 1) < (max_len-1) ) { /* Need space for '\0' so -1 */
        s[current_len  ] = c;
        s[current_len+1] = 0;
    }
}

Token newToken(const size_t space_offset, const char text[MAX_TOKEN_TEXT_SIZE], const FileLine parent_line) {
    Token token = {
        .space_offset = space_offset
    };
    strncpy(token.text, text, MAX_TOKEN_TEXT_SIZE);
    copyFileLine(&token.parent_line, parent_line);
    return token;
}
bool copyToken(Token* a, const Token b) {
    assert(a);
    a->space_offset = b.space_offset;
    strncpy(a->text, b.text, MAX_TOKEN_TEXT_SIZE);
    copyFileLine(&(a->parent_line), b.parent_line);
    return true;
}
size_t copyTokens(Token** as, const Token* bs, const size_t bn) {
    assert(as); assert(*as); assert(bs);
    for (size_t bi = 0; bi<bn; bi++)
        copyToken(&((*as)[bi]), bs[bi]);
    return bn;
}
const char* strToken(const Token token) {
    static char buf[MAX_STR_FILELINE_SZ];
    snprintf(buf, MAX_STR_FILELINE_SZ, "%s:%zu:%zu: %s", token.parent_line.file_name, token.parent_line.line_number, token.space_offset, token.text);
    return buf;
}

static void appendToTokens(Token** tokens, size_t* num_tokens, const Token token) {
    assert(tokens);
    assert(*tokens);

    if (!strlen(token.text)) return; /* Obviously ignore empty tokens */
    copyToken(&((*tokens)[*num_tokens]), token);
    (*num_tokens)++;
}

static inline bool isOp(const char c, const char d) {
    char op[3] = {c, d, 0};
    return (
        strcmp(op, "==")==0 ||
        strcmp(op, "!=")==0 ||
        strcmp(op, "<=")==0 ||
        strcmp(op, ">=")==0 ||
        strcmp(op, "<<")==0 ||
        strcmp(op, ">>")==0 ||
        strcmp(op, "->")==0 ||
        strcmp(op, "++")==0 ||
        strcmp(op, "--")==0 ||
        strcmp(op, "+=")==0 ||
        strcmp(op, "-=")==0 ||
        strcmp(op, "*=")==0 ||
        strcmp(op, "/=")==0 ||
        strcmp(op, "%%=")==0 ||
        strcmp(op, "&=")==0 ||
        strcmp(op, "|=")==0 ||
        strcmp(op, "^=")==0 ||
        strcmp(op, "&&")==0 ||
        strcmp(op, "||")==0 ||
        // strcmp(op, "^^")==0 ||
        0
    );
}

static inline bool isDelim(const char c) {
    switch (c) {
        /* Arithmetic Operators */
        case '+': case '-':
        case '*': case '/':
        case '%': case '=':

        /* Bitwise Operators */
        case '&': case '|':
        case '^': case '~':

        /* Boolean Operators */
        case '!':

        /* Ternary Operators */
        case '?': case ':':

        /* Pairs */
        case '{': case '}':
        case '(': case ')':
        case '[': case ']':
        case '<': case '>':

        /* Quotes */
        case '\'': case '\"':

        /* Misc */
        case '.': case ',':
        case ';':
        
        /* Preproc */ // FIXME: Probably removable b/c preproc should be before tokenizer?
        case '#':
            
            return true;

        default: break;
    }
    return false;
}

size_t tokenizeFileLine(const FileLine parent_line, Token** tokens) {
    printf_dbg("Tokenizing FileLine: %s\n", strFileLine(parent_line));
    size_t num_tokens = 0;

    char text[MAX_TOKEN_TEXT_SIZE] = {0};
    const size_t len = strlen(parent_line.line_buf);

    bool inString = false, inChar = false;

    #define LAMBDA_appendChar       {appendChar(text, MAX_TOKEN_TEXT_SIZE, c);}
    #define LAMBDA_appendToTokens   {appendToTokens(tokens, &num_tokens, newToken(i - strlen(text) + 1, text, parent_line)); clearBuf(text, MAX_TOKEN_TEXT_SIZE);}

    size_t i;
    for (i = 0; i<len; i++) {
        const char c = parent_line.line_buf[i];

        /*******************************/

        if (inString && !inChar) {
            if (c == '\"') {
                LAMBDA_appendChar;
                inString = false;
                continue;
            }
            LAMBDA_appendChar;
            continue;
        }
        if (inChar && !inString) {
            if (c == '\'') {
                LAMBDA_appendChar;
                inChar = false;
                continue;
            }
            LAMBDA_appendChar;
            continue;
        }

        /*******************************/

        if (c == '\"') {
            inString = true;
            LAMBDA_appendToTokens;
            LAMBDA_appendChar;
            continue;
        }

        if (c == '\'') {
            inChar = true;
            LAMBDA_appendToTokens;
            LAMBDA_appendChar;
            continue;
        }

        if (isSpace(c)) {
            LAMBDA_appendToTokens;
            continue;
        }

        const char d = (i < len-1) ? parent_line.line_buf[i+1] : 0;
        if (isOp(c, d) || isDelim(c)) {
            LAMBDA_appendToTokens;

            text[0] = c;
            if (isOp(c, d)) {
                i++; /* So we skip the second half of the op on the next pass */
                text[1] = d; text[2] = 0;   /* Copy operator in */
            }
            else text[1] = 0;               /* Copy delim in */

            LAMBDA_appendToTokens;

            continue;
        }

        LAMBDA_appendChar;
    }
    LAMBDA_appendToTokens;

    return num_tokens;
}

/******************************************/

LexNode newLexNode(const Token tokens[MAX_TOKENS_IN_LINE], const size_t num_tokens) {
    LexNode node = (LexNode)malloc(sizeof(struct lex_node_s));

    node->num_children = 0;
    // printf("here\n");
    memcpy(node->tokens, tokens, sizeof(Token) * num_tokens);
    // printf("there\n");
    
    node->parent = NULL;
    for (size_t i = 0; i<MAX_NUM_LEX_CHILDREN; i++)
        node->children[i] = NULL;

    return node;
}
bool deleteLexTree(LexNode node) {
    assert(node);
    printf_dbg("Deleting a LexNode...\n");

    node->parent = NULL;
    printf_dbg(" * Found %zu children\n", node->num_children);
    for (size_t i = 0; i<node->num_children; i++)
        deleteLexTree(node->children[i]);

    safeFree(node);
    printf("Successfully deleted 1 LexNode\n");
    return true;
}

void printLexNode(const LexNode node) {

}
void printLexTree(const LexNode node) {

}

static LexNode newMasterLexNode(const char file_name[MAX_FILE_NAME_SZ]) {
    const char master_file_line_buf[MAX_LINE_BUF_SZ] = "#MASTER";
    const char master_token_text[MAX_TOKEN_TEXT_SIZE] = "#MASTER";
    const FileLine master_file_line = newFileLine(0, master_file_line_buf, file_name);
    const Token master_tokens[MAX_TOKENS_IN_LINE] = { newToken(0, master_token_text, master_file_line) };
    return newLexNode(master_tokens, 1);
}

Token* line_as_tokens = NULL;
const LexNode buildLexTree(FileLine file_as_lines[MAX_LINES_IN_FILE], const size_t num_lines) {
    assert(num_lines > 0);

    line_as_tokens = (Token*)malloc(sizeof(Token)*MAX_TOKENS_IN_LINE);
    for (size_t i = 0; i<num_lines; i++) {
        if (isEmptyFileLine(file_as_lines[i])) continue;

        printf_dbg("%s\n", strFileLine(file_as_lines[i]));
        const size_t num_tokens = tokenizeFileLine(file_as_lines[i], &line_as_tokens);

        for (size_t j = 0; j<num_tokens; j++) {
            printf_dbg(" * %s\n", strToken(line_as_tokens[j]));
        } printf_dbg("\n");
    }
    printf_dbg("\n");


    LexNode master_node = newMasterLexNode(file_as_lines[0].file_name);
    return master_node;
}