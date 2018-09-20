#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "file.h"
#include "lex.h"
#include "primitive.h"

#define TOKENLEN (128)

int
asc_load(const char *path, struct object *obj)
{
	const char *cname[] = {"Red", "Green"};
	char token[TOKENLEN];
	char *buff = file_load(path);
	struct lex *l = lex_create(buff);
	lex_nexttoken(l, LEX_STRING, token, TOKENLEN);	//Ambient
	lex_nexttoken(l, LEX_STRING, token, TOKENLEN);	//light
	lex_nexttoken(l, LEX_STRING, token, TOKENLEN);	//color
	lex_nexttoken(l, ':', NULL, 0);
	lex_nexttoken(l, LEX_STRING, token, TOKENLEN);	//Red
	assert(strcmp(token, cname[0]) == 0);
	lex_nexttoken(l, '=', NULL, 0);
	lex_nexttoken(l, LEX_STRING, token, TOKENLEN);	//Red
	printf("%s\n", token);
	return 0;
}
