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
	switch (peek(parser).kind) {
		case Token_CurlyBraketOpen:
			return jsp_parse_object(parser);
		case Token_SquareBraketOpen:
			return jsp_parse_array(parser);
		case Token_String:
		case Token_Number:
		case Token_False:
		case Token_True:
		case Token_Null:
			parser->currsor++;
			return true;
		default:
			fprintf(stderr, "Unexpected Token: ");
			jlx_token_print(peek(parser));
			printf("\n");
			return false;
	}
}

bool		jsp_parse_object(JsonParser *parser)
{
	if(!consume_token(parser,Token_CurlyBraketOpen)){
		return false;
	}

	if(peek(parser).kind != Token_CurlyBraketClose) {
		do {
			if(!consume_token(parser, Token_String)) {
				return false;
			}

			if(!consume_token(parser, Token_Colon)) {
				return false;
			}

			if(!jsp_parse_value(parser)) {
				return false;
			}
		} while(consume_token(parser, Token_Comma));
	}

	if(!consume_token(parser, Token_CurlyBraketClose)) {
		return false;
	}

	return true;
}

bool		jsp_parse_array(JsonParser *parser)
{
	if(!consume_token(parser, Token_SquareBraketOpen)) {
		return false;
	}

	if(peek(parser).kind != Token_SquareBraketClose) {
		do {
			if(!jsp_parse_value(parser)) {
				return false;
			}
		} while(consume_token(parser, Token_Comma));
	}

	if(!consume_token(parser, Token_SquareBraketClose)) {
		return false;
	}

	return true;
}


// -------- Parser Helepers [Declaration] --------
static bool consume_token(JsonParser *parser, JsonTokenKind kind)
{
	if(parser->tokens[parser->currsor].kind != kind) {
		// JsonToken tk = parser->tokens[parser->currsor];
		// printf("Unexpected token ");
		// jlx_token_print(tk);
		// printf(" at %u:%u\n", tk.line, tk.col);
		return false;
	}
	parser->currsor++;
	return true;
}

static JsonToken peek(JsonParser *parser)
{
	return parser->tokens[parser->currsor];
}
