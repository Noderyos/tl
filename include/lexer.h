#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define UNUSED(x) (void)(x)

#define INVALID_TOKEN(type, t)                                                   \
    do {                                                                         \
        fprintf(stderr, "INVALID_TOKEN: %s at %ld:%ld", type, t.row+1, t.col+1); \
        exit(1);                                                                 \
    } while(0)

#define isspace(c) (strchr(" \n\t", c) != NULL)
#define isalnum(c) (strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", c) != NULL)

#define ASSERT(cond, msg)           \
    if(!(cond)) {                   \
        fprintf(stderr, "%s", msg); \
        exit(1);                    \
    }

#define TOKEN_PRINT(t) (int)t.text_len, t.text

typedef enum {
    TOKEN_END = 0,
    TOKEN_SYMBOL,
    TOKEN_KEYWORD,
    TOKEN_GREATER,
    TOKEN_LOWER,
    TOKEN_DASH,
    TOKEN_OPEN_CURLY,
    TOKEN_CLOSE_CURLY,
    TOKEN_INVALID
} Token_Type;

typedef struct {
    Token_Type type;
    const char *text;
    size_t text_len;
    size_t row;
    size_t col;
} Token;

typedef struct {
    char *content;
    size_t content_len;
    size_t cursor;
    size_t line;
    size_t bol;
} Lexer;

Lexer lexer_init(char *content, size_t content_len);
Token lexer_next(Lexer *l);
char *lexer_pretty(Token t);

#endif