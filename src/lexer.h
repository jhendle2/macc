#ifndef LEXER_H
#define LEXER_H

#include "file_reader.h"

typedef struct token_s {
    #ifndef TOKEN_S
    #define TOKEN_S
        #define MAX_TOKEN_TEXT_SIZE 32
        #define MAX_TOKENS_IN_LINE MAX_LINE_BUF_SZ/2 /* Should be half of MAX_LINE_BUF_SZ to store half as chars and half as the spaces between them */
    
        #define MAX_STR_TOKEN_SZ 128
    #endif /* TOKEN_S */

    size_t space_offset; /* Index of first char of Token within the FileLine */
    char text[MAX_TOKEN_TEXT_SIZE];
    FileLine parent_line;
} Token;

Token newToken(const size_t space_offset, const char text[MAX_TOKEN_TEXT_SIZE], const FileLine parent_line);
bool copyToken(Token* a, const Token b);
size_t copyTokens(Token** as, const Token* bs, const size_t bn);
const char* strToken(const Token token);

size_t tokenizeFileLine(const FileLine parent_line, Token** tokens);

/******************************************/

typedef struct lex_node_s {
    size_t num_tokens, num_children;
    
    Token tokens[MAX_TOKENS_IN_LINE];

    #ifndef LEX_NODE_S
    #define LEX_NODE_S
        #define MAX_NUM_LEX_CHILDREN 256
        // #define MAX_NUM_LEX_CHILDREN 512
    #endif /* LEX_NODE_S */

    struct lex_node_s* children[MAX_NUM_LEX_CHILDREN];
    struct lex_node_s* parent;
}* LexNode;

LexNode newLexNode(const Token tokens[MAX_TOKENS_IN_LINE], const size_t num_tokens);
bool deleteLexTree(LexNode node);
bool addLexNodeChild(LexNode parent, LexNode child);

void printLexNode(const LexNode node, const size_t level);
void printLexTree(const LexNode node);

const LexNode buildLexTree(FileLine file_as_lines[MAX_LINES_IN_FILE], const size_t num_lines);

#endif /* LEXER_H */