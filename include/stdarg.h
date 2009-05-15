#ifndef _STDARG_H_
#define _STDARG_H_


typedef __builtin_va_list __va_list;
typedef __va_list va_list;

// not sure when this got renamed - maybe it was earlier than 4
#if __GNUC__ >= 4
#define va_start(ap,fmt) __builtin_va_start(ap,fmt)
#else
#define va_start(ap,fmt) __builtin_stdarg_start(ap,fmt)
#endif

#define va_arg(ap,t) __builtin_va_arg(ap,t)
#define va_copy(ap1,ap2) __builtin_va_copy(ap1,ap2)
#define va_end(ap) __builtin_va_end(ap)

#endif /* _STDARG_H_ */
