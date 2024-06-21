#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum {
    DIR_ERR,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_NO,
} Direction;

typedef enum {
    STMT_INVALID = 0,
    STMT_EOF,
    STMT_CASE,
    STMT_RUN,
    STMT_TRACE
} Statement_Type;

typedef struct {
    Token state;
    Token read;
    Direction direction;
    Token write;
    Token next_state;
} Case_Stmt;

typedef struct {
    Token *tape;
    size_t tape_size;
} Run_Stmt;

typedef struct {
    Statement_Type type;
    void *parameters;
} Statement;

Statement parser_next_statement(Lexer *l);
Direction parser_parse_arrow(Lexer *l);
Token parser_parse_symbol(Lexer *l);
char *parser_direction_pretty(Direction d);

#endif