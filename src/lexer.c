#include "lexer.h"

struct {
    char c;
    Token_Type type;
} literals[] = {
        {'>', TOKEN_GREATER},
        {'<', TOKEN_LOWER},
        {'-', TOKEN_DASH},
        {'{', TOKEN_OPEN_CURLY},
        {'}', TOKEN_CLOSE_CURLY}
};

#define literals_count (sizeof(literals)/sizeof(literals[0]))

const char *keywords[] = {
        "case", "run", "trace"
};

size_t keywords_count = 3;


Lexer lexer_init(char *content, size_t content_len){
    Lexer l = {0};
    l.content = content;
    l.content_len = content_len;
    return l;
}

char lexer_chop_char(Lexer *l){
    ASSERT(l->cursor < l->content_len, "Unexpected EOF")

    char x = l->content[l->cursor];
    l->cursor++;
    if(x == '\n'){
        l->line++;
        l->bol = l->cursor;
    }
    return x;
}


void lexer_trim_left(Lexer *l){
    while (l->cursor < l->content_len && isspace(l->content[l->cursor])){
        UNUSED(lexer_chop_char(l));
    }
}

int is_symbol(char c){
    return isalnum(c);
}


Token lexer_next(Lexer *l){
    lexer_trim_left(l);
    Token token = {
            .text = &l->content[l->cursor],
            .row = l->line,
            .col = l->cursor - l->bol
    };

    if (l->cursor >= l->content_len) return token;

    if(is_symbol(l->content[l->cursor])){
        token.type = TOKEN_SYMBOL;
        while (l->cursor < l->content_len && is_symbol(l->content[l->cursor])){
            UNUSED(lexer_chop_char(l));
            token.text_len++;
        }
        for (size_t i = 0; i < keywords_count; ++i) {
            size_t keyword_len = strlen(keywords[i]);

            if(token.text_len == keyword_len && memcmp(token.text, keywords[i], keyword_len) == 0){
                token.type = TOKEN_KEYWORD;
                break;
            }
        }
        return token;
    }

    for (size_t i = 0; i < literals_count; ++i) {
        if(l->content[l->cursor] == literals[i].c){
            lexer_chop_char(l);
            token.type = literals[i].type;
            token.text_len = 1;
            return token;
        }
    }

    UNUSED(lexer_chop_char(l));
    token.type = TOKEN_INVALID;
    token.text_len = 1;
    return token;
}

char *lexer_pretty(Token t){
    switch (t.type) {
        case TOKEN_END: return "end";
        case TOKEN_SYMBOL: return "symbol";
        case TOKEN_KEYWORD: return "keyword";
        case TOKEN_INVALID: return "invalid";
        case TOKEN_GREATER: return "greater";
        case TOKEN_LOWER: return "lower";
        case TOKEN_DASH: return "dash";
        case TOKEN_OPEN_CURLY: return "open curly";
        case TOKEN_CLOSE_CURLY: return "close curly";
        default: INVALID_TOKEN("Invalid Token", t);
    }
}