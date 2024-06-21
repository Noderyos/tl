

all: src/main.c src/lexer.c src/parser.c
	$(CC) -Wall -Wextra -Werror -Iinclude -o main src/main.c src/lexer.c src/parser.c -g -ggdb

clean:
	rm -f main