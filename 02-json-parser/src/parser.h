#ifndef JSONP_PARSER_H
#define JSONP_PARSER_H
#include "lexer.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
	JsonToken *tokens;
	size_t len;
	size_t currsor;
} JsonParser;

JsonParser	jsp_parser_create(JsonToken *tokens, size_t len);

bool	jsp_parse(JsonParser *parser);
bool	jsp_parse_value(JsonParser *parser);

bool	jsp_parse_object(JsonParser *parser);

#endif
