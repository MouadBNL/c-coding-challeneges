#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <wctype.h>
#include <stddef.h>
#include <stdio.h>
#include "../include/stb_ds.h"


// -------- Helper function [Declaration] --------
static JsonToken generate_token(JsonTokenizer *tokenizer, JsonTokenKind kind, size_t value_size);
static const char *get_token_kind_name(JsonTokenKind kind);

JsonTokenizer	*jlx_tokenizer_create(char *buffer, size_t buffer_size)
{
	JsonTokenizer *tokenizer = malloc(sizeof(JsonTokenizer));
	tokenizer->buffer = buffer;
	tokenizer->buffer_size = buffer_size;
	tokenizer->tokens = NULL;
	tokenizer->cursor = 0;
	tokenizer->line	= 1;
	tokenizer->col	= 1;


	return tokenizer;
}

void			jlx_tokenizer_destroy(JsonTokenizer *tokenizer)
{
	arrfree(tokenizer->tokens);
	free(tokenizer);
}

bool			jlx_tokenizer_process(JsonTokenizer *tokenizer)
{
	while(tokenizer->cursor < tokenizer->buffer_size) {
		bool ok = jlx_next_token(tokenizer);
		if(!ok) {
			fprintf(stderr, "Stopping the tokenizer\n");
			return false;
		}
	}
	arrput(tokenizer->tokens, generate_token(tokenizer, Token_EOF, 0));
	return 1;
}

bool			jlx_next_token(JsonTokenizer *tokenizer)
{
	while(iswspace(tokenizer->buffer[tokenizer->cursor])) {
		tokenizer->cursor++;
		tokenizer->col++;
		if(tokenizer->buffer[tokenizer->cursor-1] == '\n') {
			tokenizer->line++;
			tokenizer->col=1;
		}
	}
	switch (tokenizer->buffer[tokenizer->cursor]) {
		case '{':	
			arrput(tokenizer->tokens, generate_token(tokenizer, Token_CurlyBraketOpen, 1));
			break;
		case '}':
			arrput(tokenizer->tokens, generate_token(tokenizer, Token_CurlyBraketClose, 1));
			break;
		default:
			fprintf(stderr, "Unexpected input at %u:%u\n", tokenizer->line, tokenizer->col);
			return 0;
	}
	return 1;
}


void			jlx_token_print(JsonToken token)
{
	printf("%-10s{value=%3.*s, line=%u, .col=%u}", 
			get_token_kind_name(token.kind), (int) token.value_size, token.value_content, token.line, token.col);
}


// -------- Helper function [Implementation] --------
static JsonToken generate_token(JsonTokenizer *tokenizer, JsonTokenKind kind, size_t value_size)
{
	JsonToken token = {
		.kind = kind,
		.value_content = tokenizer->buffer,
		.value_size = value_size,
		.col = tokenizer->col,
		.line = tokenizer->line,
	};

	tokenizer->cursor += value_size;
	tokenizer->col += value_size;

	return token;
}


static const char *get_token_kind_name(JsonTokenKind kind)
{
	switch (kind) {
		case Token_CurlyBraketOpen:
			return "{";
		case Token_CurlyBraketClose:
			return "}";
		case Token_EOF:
			return "EOF";
	}
	return "UNKNOWN";
}
