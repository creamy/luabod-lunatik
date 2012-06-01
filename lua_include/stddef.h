#include <sys/types.h>

#ifdef  _BSD_PTRDIFF_T_
typedef _BSD_PTRDIFF_T_		ptrdiff_t;
#undef  _BSD_PTRDIFF_T_
#endif

#include <sys/_null.h>

