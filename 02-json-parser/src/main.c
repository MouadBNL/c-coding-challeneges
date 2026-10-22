#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

#define STB_DS_IMPLEMENTATION
#include "../include/stb_ds.h"


int main(void)
{
	char *buff = "";
	printf("Buff = %s\n", buff);
	
	JsonTokenizer *tokenizer = jlx_tokenizer_create(buff, strlen(buff));
	for(int i = 0; i < arrlen(tokenizer->tokens); i++) {
		jlx_token_print(tokenizer->tokens[i]);
		printf("\n");
	}

	JsonParser parser = jsp_parser_create(tokenizer->tokens, arrlen(tokenizer->tokens));
	bool ok = jsp_parse(&parser);

	jlx_tokenizer_destroy(tokenizer);

	if(ok) {
		printf("Content parsed sucessfully\n");
		return 0;
	} else {
		printf("Error parssing.\n");
		return 1;
	}
}
