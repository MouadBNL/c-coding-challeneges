#include "lexer.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <wctype.h>
#include <stddef.h>
#include <stdio.h>
#include "../include/stb_ds.h"


// -------- Helper function [Declaration] --------
static JsonToken generate_token(JsonTokenizer *tokenizer, JsonTokenKind kind, size_t value_size);
static JsonToken generate_string_token(JsonTokenizer *tokenizer);
static JsonToken generate_number_token(JsonTokenizer *tokenizer);
static const char *get_token_kind_name(JsonTokenKind kind);
static bool generate_keyword(JsonTokenizer *tokenizer);

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
	size_t cursor = tokenizer->cursor;
	while(tokenizer->cursor < tokenizer->buffer_size) {
		// printf("Processing token %c at %zu - %u:%u\n", tokenizer->buffer[tokenizer->cursor], tokenizer->cursor, tokenizer->line, tokenizer->col);
		
		cursor = tokenizer->cursor;
		if(tokenizer->buffer[tokenizer->cursor] == '\0') break;
		bool ok = jlx_next_token(tokenizer);
		if(!ok) {
			fprintf(stderr, "Stopping the tokenizer\n");
			return false;
		}


		if(cursor == tokenizer->cursor) {
			fprintf(stderr, "Cursor did not move, infinit loop detected.");
			return 0;
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
	if(tokenizer->buffer[tokenizer->cursor] == '\0') return 1;
	switch (tokenizer->buffer[tokenizer->cursor]) {
		case '{':	
			arrput(tokenizer->tokens, generate_token(tokenizer, Token_CurlyBraketOpen, 1));
			return 1;
		case '}':
			arrput(tokenizer->tokens, generate_token(tokenizer, Token_CurlyBraketClose, 1));
			return 1;
		case '[':
			arrput(tokenizer->tokens, generate_token(tokenizer, Token_SquareBraketOpen, 1));
			return 1;
		case ']':
			arrput(tokenizer->tokens, generate_token(tokenizer, Token_SquareBraketClose, 1));
			return 1;
		case ':':
			arrput(tokenizer->tokens, generate_token(tokenizer, Token_Colon, 1));
			return 1;
		case ',':
			arrput(tokenizer->tokens, generate_token(tokenizer, Token_Comma, 1));
			return 1;
		case '"':
			arrput(tokenizer->tokens, generate_string_token(tokenizer));
			return 1;
	}

	char tk = tokenizer->buffer[tokenizer->cursor];

	if(isdigit(tk) || tk == '.' || tk == '-') {
		arrput(tokenizer->tokens, generate_number_token(tokenizer));
		return 1;
	}

	if(generate_keyword(tokenizer)) {
		return 1;
	}
	
	fprintf(stderr, "Unexpected input at %u:%u '%c' / %d\n", tokenizer->line, tokenizer->col, tk, (int) tk);
	return 0;
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
		.value_content = tokenizer->buffer + tokenizer->cursor,
		.value_size = value_size,
		.col = tokenizer->col,
		.line = tokenizer->line,
	};

	tokenizer->cursor += value_size;
	tokenizer->col += value_size;

	return token;
}

static JsonToken generate_string_token(JsonTokenizer *tokenizer)
{
	JsonToken token = {
		.kind = Token_String,
		.value_content = tokenizer->buffer + tokenizer->cursor,
		.value_size = 1,
		.col = tokenizer->col,
		.line = tokenizer->line,
	};

	tokenizer->cursor++;
	tokenizer->col++;

	while(tokenizer->buffer[tokenizer->cursor] != '"' && tokenizer->cursor < tokenizer->buffer_size) {
		token.value_size++;
		tokenizer->cursor++;
		tokenizer->col++;
	}
	token.value_size++;
	tokenizer->cursor++;
	tokenizer->col++;

	return token;
}

static JsonToken generate_number_token(JsonTokenizer *tokenizer)
{
	JsonToken token = {
		.kind = Token_Number,
		.value_content = tokenizer->buffer + tokenizer->cursor,
		.value_size = 0,
		.col = tokenizer->col,
		.line = tokenizer->line,
	};

	char tk = tokenizer->buffer[tokenizer->cursor];
	bool found_floating_point = false;

	do {
		if(tk == '.') {
			if(found_floating_point) break;
			else found_floating_point = true;
		}
		tokenizer->cursor++;
		tokenizer->col++;
		token.value_size++;
		tk = tokenizer->buffer[tokenizer->cursor];
	} while(isdigit(tk) || tk == '.');

	return token;
}

static const char *get_token_kind_name(JsonTokenKind kind)
{
	switch (kind) {
		case Token_CurlyBraketOpen:
			return "{";
		case Token_CurlyBraketClose:
			return "}";
		case Token_SquareBraketOpen:
			return "[";
		case Token_SquareBraketClose:
			return "]";
		case Token_Colon:
			return ":";
		case Token_Comma:
			return ",";
		case Token_String:
			return "String";
		case Token_Number:
			return "Number";
		case Token_True:
			return "B(True)";
		case Token_False:
			return "B(False)";
		case Token_Null:
			return "Null";
		case Token_EOF:
			return "EOF";
	}
	return "UNKNOWN";
}

static bool		generate_keyword(JsonTokenizer *tokenizer)
{
	char *buff = tokenizer->buffer + tokenizer->cursor;
	int len = 0;
	while(isalnum(tokenizer->buffer[tokenizer->cursor + len]) && tokenizer->cursor + len < tokenizer->buffer_size) {
		len++;
	}
	for(size_t i = 0; i < sizeof json_keywords_table / sizeof *json_keywords_table; i++) {
		if(len > 0 && strncmp(buff, json_keywords_table[i].key, len) == 0) {
			arrput(tokenizer->tokens, generate_token(tokenizer, json_keywords_table[i].kind, len));
			return 1;
		}
	}

	return  0;
}
