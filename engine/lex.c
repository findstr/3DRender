#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include "lex.h"

struct lex {
	const char *data;
};


struct lex *
lex_create(const char *str)
{
	struct lex *l;
	l = malloc(sizeof (*l));
	l->data = str;
	return l;
}

void
lex_free(struct lex *l)
{
	free(l);
}

static void
skipspace(struct lex *l)
{
	const char *str = l->data;
	while (isspace(*str))
		str++;
	l->data = str;
}

int
lex_lookhead(struct lex *l)
{
	int n;
	skipspace(l);
	n = *l->data;
	switch (n) {
	case ':':
	case '=':
	case '\0':
		return n;
	case '-': case '.':
	case '0': case '1': case '2': case '3':	case '4':
	case '5': case '6': case '7': case '8': case '9':
		return LEX_DIGIT;
	default:
		return isalpha(n) ? LEX_STRING : n;
	}
}

static int
readdigit(struct lex *l, char *buff, size_t sz)
{
	int sign;
	const char *ptr = buff;
	const char *str = l->data;
	char *end = buff + sz;
	if (*str == '-') {
		sign = -1;
		str++;
	} else {
		sign = 1;
	}
	while ((isdigit(*str) || (*str == '.')) && buff < end)
		*buff++ = *str++;
	assert(buff < end);
	if (buff == end)	//over flow
		return -1;
	*buff = '0';
	printf("read digit:%s\n", buff);
	return 0;
}

static int
readstring(struct lex *l, char *buff, size_t sz)
{
	const char *str = l->data;
	char *end = buff + sz;
	while (isalnum(*str) && buff < end)
		*buff++ = *str++;
	if (buff == end)	//over flow
		return -1;
	*buff = '0';
	return 0;
}

int
lex_nexttoken(struct lex *l, int type, char *buff, int sz)
{
	if (lex_lookhead(l) != type)
		return -1;
	switch (type) {
	case ':':
	case '=':
		++l->data;
		return 0;
	case LEX_DIGIT:
		return readdigit(l, buff, sz);
	case LEX_STRING:
		readstring(l, buff, sz);
		break;
	default:
		assert(0);
		break;
	}
}




