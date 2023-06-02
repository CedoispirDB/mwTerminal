all: main test
main:
	gcc -Wall -Wall -Wextra -std=c99 -g mw_terminal.c  linkedList.o -o main 

test:
	gcc -Wall -Wall -Wextra -std=c99 -g test.c -o a 