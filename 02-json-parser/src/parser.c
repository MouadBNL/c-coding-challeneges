#include "parser.h"
#include "lexer.h"
#include <stdbool.h>
#include <stdio.h>


// -------- Parser Helepers [Declaration] --------
static bool consume_token(JsonParser *parser, JsonTokenKind kind);
static JsonToken peek(JsonParser *parser);

JsonParser	jsp_parser_create(JsonToken *tokens, size_t len)
{
	return (JsonParser) {
		.tokens = tokens,
		.len = len,
		.currsor = 0,
	};
}

bool	jsp_parse(JsonParser *parser)
{
	if(!jsp_parse_value(parser)) {
		return false;
	}

	return consume_token(parser, Token_EOF);
}

bool		jsp_parse_value(JsonParser *parser)
{
	if(peek(parser).kind == Token_CurlyBraketOpen) {
		return jsp_parse_object(parser);
	}

	return false;
}

bool		jsp_parse_object(JsonParser *parser)
{
	if(!consume_token(parser,Token_CurlyBraketOpen)){
		return false;
	}

	if(!consume_token(parser, Token_CurlyBraketClose)) {
		return false;
	}

	return true;
}


// -------- Parser Helepers [Declaration] --------
static bool consume_token(JsonParser *parser, JsonTokenKind kind)
{
	if(parser->tokens[parser->currsor].kind != kind) {
		JsonToken tk = parser->tokens[parser->currsor];
		printf("Unexpected token ");
		jlx_token_print(tk);
		printf(" at %u:%u\n", tk.line, tk.col);
		return false;
	}
	parser->currsor++;
	return true;
}

static JsonToken peek(JsonParser *parser)
{
	return parser->tokens[parser->currsor];
}
