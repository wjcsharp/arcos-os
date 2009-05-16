#ifndef _STDARG_H_
#define _STDARG_H_


#ifdef _MSC_VER

#include <vadefs.h>

#define va_start _crt_va_start
#define va_arg _crt_va_arg
#define va_end _crt_va_end

#else /* Visual C++ */

typedef __builtin_va_list __va_list;
typedef __va_list va_list;

// not sure when this got renamed - maybe it was earlier than 4
#if __GNUC__ >= 4
#define va_start(ap,fmt) __builtin_va_start(ap,fmt)
#else /* GCC version >= 4 */
#define va_start(ap,fmt) __builtin_stdarg_start(ap,fmt)
#endif /* GCC version < 4 */

#define va_arg(ap,t) __builtin_va_arg(ap,t)
#define va_copy(ap1,ap2) __builtin_va_copy(ap1,ap2)
#define va_end(ap) __builtin_va_end(ap)

#endif /* GCC */


#endif /* _STDARG_H_ */
