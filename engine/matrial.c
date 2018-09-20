#include <stdlib.h>
#include <string.h>
#include "matrial.h"

struct matrial *
matrial_create()
{
	struct matrial *m = malloc(sizeof(*m));
	memset(m, 0, sizeof(*m));
	return m;
}

