#include <stdlib.h>
#include <string.h>
#include "matrial.h"

static struct matrial *M;

struct matrial *
matrial_create()
{
	struct matrial *m = malloc(sizeof(*m));
	memset(m, 0, sizeof(*m));
	m->next = M;
	M = m;
	m->color = RGBA(0, 0, 255, 255);
	m->ka = 1.0f;
	m->kd = 1.0f;
	m->ks = 1.0f;
	m->power = 0.0f;
	m->ra = rgba_mul(m->color, m->ka);
	m->rd = rgba_mul(m->color, m->kd);
	m->ks = rgba_mul(m->color, m->ks);
	return m;
}

