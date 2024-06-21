#include "parser.h"

#define CHECK_TYPE(t, ty, exp) \
    if(t.type != ty){          \
        printf("INVALID_TOKEN at %ld:%ld %s\n", t.row + 1, t.col+1, exp); \
        return statement;                           \
    }


Statement parser_next_statement(Lexer *l){
    Statement statement = {0};

    Token t = lexer_next(l);

    if(t.type == TOKEN_END){
        statement.type = STMT_EOF;
        return statement;
    }

    CHECK_TYPE(t, TOKEN_KEYWORD, "expecting keyword")

    if(t.text_len == 4 && memcmp(t.text, "case", 4) == 0){
        statement.type = STMT_CASE;
        Case_Stmt caseStmt = {0};

        caseStmt.state = parser_parse_symbol(l);
        caseStmt.read = parser_parse_symbol(l);
        caseStmt.direction = parser_parse_arrow(l);
        caseStmt.write = parser_parse_symbol(l);
        caseStmt.next_state = parser_parse_symbol(l);

        statement.parameters = malloc(sizeof(caseStmt));
        memcpy(statement.parameters, &caseStmt, sizeof(caseStmt));
    }

    if(t.text_len == 3 && memcmp(t.text, "run", 3) == 0){
        statement.type = STMT_RUN;
        Run_Stmt runStmt = {0};

        t = lexer_next(l);
        if(t.type != TOKEN_OPEN_CURLY){
            INVALID_TOKEN("Expecting open curly", t);
        }
        t = lexer_next(l);
        while (t.type != TOKEN_CLOSE_CURLY){
            if(t.type == TOKEN_END){
                INVALID_TOKEN("Unexpected EOF", t);
            }
            runStmt.tape_size++;
            runStmt.tape = realloc(runStmt.tape, runStmt.tape_size * sizeof(Token));
            assert(runStmt.tape != NULL);
            runStmt.tape[runStmt.tape_size-1] = t;
            t = lexer_next(l);
        }
        statement.parameters = malloc(sizeof(Run_Stmt));
        memcpy(statement.parameters, &runStmt, sizeof(Run_Stmt));
        return statement;
    }

    if(t.text_len == 5 && memcmp(t.text, "trace", 5) == 0){
        statement.type = STMT_TRACE;
        Run_Stmt traceStmt = {0};
        traceStmt.tape = malloc(1000);

        t = lexer_next(l);
        if(t.type != TOKEN_OPEN_CURLY){
            INVALID_TOKEN("Expecting open curly", t);
        }
        t = lexer_next(l);
        while (t.type != TOKEN_CLOSE_CURLY){
            if(t.type == TOKEN_END){
                INVALID_TOKEN("Unexpected EOF", t);
            }
            traceStmt.tape_size++;
            traceStmt.tape = realloc(traceStmt.tape, traceStmt.tape_size * sizeof(Token));
            assert(traceStmt.tape != NULL);
            traceStmt.tape[traceStmt.tape_size - 1] = t;
            t = lexer_next(l);
        }
        statement.parameters = malloc(sizeof(Run_Stmt));
        memcpy(statement.parameters, &traceStmt, sizeof(Run_Stmt));
        return statement;
    }

    return statement;
}

Direction parser_parse_arrow(Lexer *l){
    Direction dir;

    Token t = lexer_next(l);

    if(t.type == TOKEN_LOWER){
        dir = DIR_LEFT;
    }else if(t.type == TOKEN_DASH){
        dir = DIR_NO;
    }else {
        INVALID_TOKEN("Invalid direction", t);
    }

    t = lexer_next(l);

    if(t.type == TOKEN_DASH){
        return dir;
    }else if(t.type == TOKEN_GREATER && dir == DIR_NO){
        return DIR_RIGHT;
    }else{
        INVALID_TOKEN("Invalid direction", t);
    }
}

Token parser_parse_symbol(Lexer *l){
    Token t = lexer_next(l);
    if(t.type != TOKEN_SYMBOL){
        INVALID_TOKEN("Invalid symbol", t);
    }
    return t;
}

char *parser_direction_pretty(Direction d){
    switch (d) {
        case DIR_NO: return "same";
        case DIR_LEFT: return "left";
        case DIR_RIGHT: return "right";
        default: return "invalid";
    }
}