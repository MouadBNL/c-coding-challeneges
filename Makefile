CC		= gcc
CFLAGS 	= -Wall -Wextra -Wpedantic -fanalyzer -Iinclude -g -O0 -fno-omit-frame-pointer -fsanitize=address,undefined
wc: 
	$(CC) $(CFLAGS) 01-wc-tool/main.c -o build/wc
