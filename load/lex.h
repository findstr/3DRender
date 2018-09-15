#ifndef _LEX_H
#define _LEX_H

#define LEX_STRING	(1)
#define LEX_DIGIT	(2)

struct lex;

struct lex *lex_create(const char *str);
void lex_free(struct lex *l);
int lex_lookhead(struct lex *l);
float lex_nexttoken(struct lex *l, int type, char *buff, int sz);

#endif

