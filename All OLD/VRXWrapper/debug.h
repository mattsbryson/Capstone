#ifndef DEBUG_H
#define DEBUG_H

void AssertFailed(const char * check, const char * file, int line);

#define ASSERT(x) if (!(x)) { AssertFailed(#x, __FILE__, __LINE__); }

#endif//DEBUG_H
