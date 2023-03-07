

#define MAKO_VER "3.9"

#define MA_STR_HELPER(x) #x
#define MA_STR(x) MA_STR_HELPER(x)

#ifdef BASLIB_VER_NO
#define MAKO_DATE "BAS lib " MA_STR(BASLIB_VER_NO) ". Build date: " __DATE__
#else
#define MAKO_DATE "Build date: " __DATE__
#endif
