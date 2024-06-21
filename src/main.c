#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"

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

int main(void){

    char *text = read_file("examples/inc.tula");

    Lexer lexer = lexer_init(text, strlen(text));

    Token t = lexer_next(&lexer);

    while (t.type != TOKEN_END){
        printf("%.*s (%s)\n", (int)t.text_len, t.text, lexer_pretty(t));
        t = lexer_next(&lexer);
    }

    free(text);
}