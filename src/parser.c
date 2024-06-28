#include "parser.h"

#define CHECK_TYPE(t, ty, exp) \
    if(t.type != ty){          \
        printf("INVALID_TOKEN at %ld:%ld %s\n", t.row + 1, t.col+1, exp); \
        return statement;                                                 \
    }

#define MAX_STATE_COUNT 256
#define MAX_STATE_NAME_SIZE 256
#define MAX_VALUE_COUNT 256
#define MAX_VALUE_NAME_SIZE 256

char states[MAX_STATE_NAME_SIZE][MAX_STATE_COUNT] = {
        [0] = "Halt"
};
char values[MAX_VALUE_NAME_SIZE][MAX_VALUE_COUNT] = {
        [0] = "B"
};


int get_state_id(Token t){
    for (int i = 0; i < MAX_STATE_COUNT; ++i) {
        if(states[i][0] == 0){
            memcpy(states[i], t.text, t.text_len);
            return i;
        }
        if(memcmp(states[i], t.text, t.text_len) == 0){
            return i;
        }
    }
    return -1;
}

int get_value_id(Token t){
    for (int i = 0; i < MAX_STATE_COUNT; ++i) {
        if(values[i][0] == 0){
            memcpy(values[i], t.text, t.text_len);
            return i;
        }
        if(memcmp(values[i], t.text, t.text_len) == 0){
            return i;
        }
    }
    return -1;
}


char* get_value_by_id(int id){
    return values[id];
}

char* get_state_by_id(int id){
    return states[id];
}

Statement parser_next_statement(Lexer *l){
    Statement statement = {0};

    Token t = lexer_next(l);

    if(t.type == TOKEN_END){
        statement.type = STMT_EOF;
        return statement;
    }

    if(t.type != TOKEN_KEYWORD){
        INVALID_TOKEN("Expecting keyword", t);
    }

    if(t.text_len == 4 && memcmp(t.text, "case", 4) == 0){
        statement.type = STMT_CASE;
        Case_Stmt caseStmt = {0};

        caseStmt.state = get_state_id(parser_parse_symbol(l));
        caseStmt.read = get_value_id(parser_parse_symbol(l));
        caseStmt.direction = parser_parse_arrow(l);
        caseStmt.write = get_value_id(parser_parse_symbol(l));
        caseStmt.next_state = get_state_id(parser_parse_symbol(l));

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
            runStmt.tape = realloc(runStmt.tape, runStmt.tape_size * sizeof(int));

            ASSERT(runStmt.tape != NULL, "Please buy more RAM");
            runStmt.tape[runStmt.tape_size-1] = get_value_id(t);
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
            traceStmt.tape = realloc(traceStmt.tape, traceStmt.tape_size * sizeof(int));
            ASSERT(traceStmt.tape != NULL, "Please buy more RAM")
            traceStmt.tape[traceStmt.tape_size - 1] = get_value_id(t);
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