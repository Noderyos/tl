#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"

#define MAX_CASE_COUNT 256
#define TAPE_SIZE 100

int parse(Lexer *l);
void generate(Case_Stmt *cases[MAX_CASE_COUNT], int cases_count, int *tape, int tape_size);

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

    Case_Stmt *cases[MAX_CASE_COUNT] = {0};
    int case_count = 0;

    while (statement.type != STMT_EOF){
        if(statement.type == STMT_CASE){
            if(case_count == MAX_CASE_COUNT){
                fprintf(stderr, "ERROR: Too many cases");
                exit(1);
            }
            if(((Case_Stmt*)statement.parameters)->state == 0){
                fprintf(stderr, "ERROR: Found Halt in reading state");
                exit(1);
            }

            cases[case_count] = malloc(sizeof(Case_Stmt));
            memcpy(cases[case_count], statement.parameters, sizeof(Case_Stmt));
            case_count++;
        }else if (statement.type == STMT_RUN){
            Run_Stmt *runStmt = statement.parameters;

            generate(cases, case_count, runStmt->tape, runStmt->tape_size);
            free(runStmt->tape);
        }else if (statement.type == STMT_TRACE){
            Run_Stmt *runStmt = statement.parameters;
            generate(cases, case_count, runStmt->tape, runStmt->tape_size);
            free(runStmt->tape);
        }else {
            INVALID_TOKEN("WTF HOW DID YOU GET HERE ??", (Token){0});
        }

        free(statement.parameters);
        statement = parser_next_statement(l);
    }
    return 1;
}

void generate(Case_Stmt *cases[MAX_CASE_COUNT], int cases_count, int *tape, int tape_size){
    printf("int main(){\n");

    printf("  char t[%d] = {", TAPE_SIZE);
    for (int i = 0; i < tape_size; ++i) {
        printf("%d,", tape[i]);
    }
    printf("};\n");


    int max_state = 0;
    for (int i = 0; i < cases_count; ++i) {
        Case_Stmt *caseStmt = cases[i];
        if(caseStmt->state > max_state) max_state = caseStmt->state;
    }

    printf("  int i = 0;\n"
           "  int s = 1;\n"
           "  while (s != 0){\n"
           "    i = (i+%d)%%%d;\n", TAPE_SIZE, TAPE_SIZE);

    int max_value = 0;

    for (int state = 1; state <= max_state; ++state) {
        printf("    if(s == %d){\n", state);
        for (int j = 0; j < cases_count; ++j) {
            Case_Stmt *caseStmt = cases[j];
            if(caseStmt->read > max_value) max_value = caseStmt->read;
            if(caseStmt->state == state){
                printf("      if(t[i] == %d){\n", caseStmt->read);
                if(caseStmt->read != caseStmt->write){
                    printf("        t[i] = %d;\n", caseStmt->write);
                }
                if(caseStmt->state != caseStmt->next_state){
                    printf("        s = %d;\n", caseStmt->next_state);
                }
                if (caseStmt->direction == DIR_RIGHT){
                    printf("        i++;\n");
                }else if (caseStmt->direction == DIR_LEFT){
                    printf("        i--;\n");
                }
                printf("      }\n");
            }
        }
        printf("    }else ");
    }
    printf("{\n"
           "      printf(\"Invalid state (%%d)\", s);\n"
           "      return 1;\n"
           "    }\n"
           "  }\n");

    printf("  char *v[%d] = {", max_value+2);
    for (int i = 0; i <= max_value; ++i) {
        printf("\"%s\", ", get_value_by_id(i));
    }
    printf("};\n");

    printf("  printf(\"Tape : \");\n");
    printf("  for(int i = 0; i < %d;i++){\n", TAPE_SIZE);
    printf("    printf(\"%%s \", v[t[i]]);\n");
    printf("  }\n"
           "  printf(\"\\n\");\n"
           "  return 0;\n"
           "}\n");
}
