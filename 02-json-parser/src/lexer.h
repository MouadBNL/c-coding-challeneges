#ifndef JSONP_LEXER_H
#define JSONP_LEXER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum  {
	Token_CurlyBraketOpen,
	Token_CurlyBraketClose,
	Token_String,
	Token_Number,
	Token_Colon,
	Token_Comma,
	Token_False,
	Token_True,
	Token_Null,
	Token_EOF
} JsonTokenKind;

typedef struct {
	JsonTokenKind kind;
	char* value_content;
	size_t value_size;
	uint32_t line;
	uint32_t col;
} JsonToken;

typedef struct {
	char *buffer;
	size_t buffer_size;
	JsonToken *tokens;
	size_t cursor;
	uint32_t line;
	uint32_t col;
} JsonTokenizer;


JsonTokenizer	*jlx_tokenizer_create(char *buffer, size_t buffer_size);
void			jlx_tokenizer_destroy(JsonTokenizer *tokenizer);
bool			jlx_tokenizer_process(JsonTokenizer *tokenizer);
bool			jlx_next_token(JsonTokenizer *tokenizer);

void			jlx_token_print(JsonToken token);


static const struct { const char *key; JsonTokenKind kind; } json_keywords_table[] = {
	{ "false"	, Token_False	},
	{ "true"	, Token_True	},
	{ "null"	, Token_Null	},
};

#endif
