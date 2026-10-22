#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

#define STB_DS_IMPLEMENTATION
#include "../include/stb_ds.h"


int main(void)
{
	bool ok = false;
	char *buff = "{}      f ";
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

	if(ok) {
		printf("Content parsed sucessfully\n");
		return 0;
	} else {
		printf("Error parssing.\n");
		return 1;
	}
}
