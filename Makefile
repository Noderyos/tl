

all: src/main.c src/lexer.c
	$(CC) -Iinclude -o main src/main.c src/lexer.c -Wall -Wextra -Werror

clean:
	rm -f main