#include "lexer.h"
#include "parser.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define STB_DS_IMPLEMENTATION
#include "../include/stb_ds.h"


char *load_file_in_buffer(char *path);

int main(int argc, char **argv)
{
	if(argc == 1) {
		printf("Please provide a file path.\n");
		return 0;
	}

	char* buff = load_file_in_buffer(argv[1]);

	bool ok = false;
	JsonTokenizer *tokenizer = jlx_tokenizer_create(buff, strlen(buff));

	printf("-------- Buffer --------\n");
	printf("Buffer = %s\n", buff);
	printf("Length = %d\n", (int) strlen(buff));

	ok = jlx_tokenizer_process(tokenizer);
	if(!ok) {
		goto cleanup;
	}

	printf("-------- Tokens --------\n");
	for(int i = 0; i < arrlen(tokenizer->tokens); i++) {
		jlx_token_print(tokenizer->tokens[i]);
		printf("\n");
	}
	printf("-------- ------ --------\n");

	JsonParser parser = jsp_parser_create(tokenizer->tokens, arrlen(tokenizer->tokens));
	ok = jsp_parse(&parser);

cleanup:
	jlx_tokenizer_destroy(tokenizer);
	free(buff);

	if(ok) {
		printf("Content parsed sucessfully\n");
		return 0;
	} else {
		printf("Error parssing.\n");
		return 1;
	}
}


char *load_file_in_buffer(char *path)
{
	FILE *file = fopen(path, "rb");
	if(file == NULL) {
		fprintf(stderr, "Could not open the file: %s.\n", path);
		return NULL;
	}
	fseek(file, 0, SEEK_END);
	uint64_t file_size = ftell(file);
	rewind(file);

	char *buffer = malloc(file_size + 1);
	if(buffer == NULL) {
		fprintf(stderr, "Could not allocate memory of size %zu.\n", file_size);
		fclose(file);
		return NULL;
	}

	size_t bytes_read = fread(buffer, 1, file_size, file);
	if(bytes_read < file_size) {
		if(ferror(file)) {
			fprintf(stderr, "Error reading file.\n");
			free(buffer);
			fclose(file);
			return NULL;
		}
	}
	buffer[bytes_read] = '\0';

	fclose(file);
	return buffer;
}
