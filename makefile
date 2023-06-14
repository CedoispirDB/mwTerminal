all: main test
main:
	gcc -Wall -Wall -Wextra -std=c99 -g mw_terminal.c  linkedList.o -o main
test:
	gcc -Wall -Wall -Wextra -std=c99 -g struct_test.c -o a 

# a:
# 	gcc -Wall -Wall -Wextra -std=c99 -c ./utils/linkedList.c -o linkedList.o