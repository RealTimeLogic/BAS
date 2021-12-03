
/* See examples/misc/LeakDetector.cpp */
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
void* baLeakDetectMalloc(size_t n, const char* file, int line);
void* baLeakDetectRealloc(void* p, size_t n, const char* file, int line);
void baLeakDetectFree(void* p, const char* file, int line);
#ifdef __cplusplus
}
#endif
#define baMalloc(n) baLeakDetectMalloc(n, __FILE__, __LINE__)
#define baRealloc(p,n) baLeakDetectRealloc(p, n, __FILE__, __LINE__)
#define baFree(p) baLeakDetectFree(p, __FILE__, __LINE__)
