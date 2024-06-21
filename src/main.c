#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"


int parse(Lexer *l);

char *read_file(const char *filename){
    FILE *f = fopen(filename, "r");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);
    string[fsize] = 0;

    return string;
}

int main(int argc, char *argv[]){

    if(argc != 2){
        fprintf(stderr, "Usage %s [file to run]", argv[0]);
        exit(1);
    }

    char *text = read_file(argv[1]);

    Lexer lexer = lexer_init(text, strlen(text));
    parse(&lexer);

    free(text);
}

int parse(Lexer *l){

    Statement statement = parser_next_statement(l);
    while (statement.type != STMT_EOF){
        if(statement.type == STMT_CASE){
            Case_Stmt *caseStmt = statement.parameters;
            printf("Case :\n"
                   "  Reading %.*s in %.*s state\n"
                   "  Moving to %s\n"
                   "  Writing %.*s and going to %.*s state\n",
                   TOKEN_PRINT(caseStmt->read), TOKEN_PRINT(caseStmt->state),
                   parser_direction_pretty(caseStmt->direction),
                   TOKEN_PRINT(caseStmt->write), TOKEN_PRINT(caseStmt->next_state));
        }else if (statement.type == STMT_RUN){
            Run_Stmt *runStmt = statement.parameters;
            printf("Run with");
            for (size_t i = 0; i < runStmt->tape_size; ++i) {
                printf(" %.*s", TOKEN_PRINT(runStmt->tape[i]));
            }
            printf("\n");
            free(runStmt->tape);
        }else if (statement.type == STMT_TRACE){
            Run_Stmt *runStmt = statement.parameters;
            printf("Trace with");
            for (size_t i = 0; i < runStmt->tape_size; ++i) {
                printf(" %.*s", TOKEN_PRINT(runStmt->tape[i]));
            }
            printf("\n");
            free(runStmt->tape);
        }else {
            printf("INVALID_TOKEN: WTF HOW DID YOU GET HERE ??\n");
            exit(1);
        }

        free(statement.parameters);
        statement = parser_next_statement(l);
    }

    return 1;
}