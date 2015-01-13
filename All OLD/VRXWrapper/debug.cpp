#include "common.h"

#include <stdlib.h>

#include "frontend.h"

void AssertFailed(const char * check, const char * file, int line)
{
	FrontendReleaseGrab();
	XFlush(g_frontend._dpy);

	fprintf(stderr, "%s:%d ASSERT FAILED %s\n", file, line, check);
	fflush(stderr);
	abort();
}


