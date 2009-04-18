#ifndef __ARCOS_H__
#define __ARCOS_H__

#include <types.h>
#include <status.h>

//
// helper macro for conversion to string literals
//
#define STRINGIFY2(x)   #x
#define STRINGIFY(x)    STRINGIFY2(x)

//
// kernel mode assert macro definition
//
#ifndef NDEBUG
#define ASSERT(X) \
    do { if (!(X)) KeBugCheck("Assertion failed: " __FILE__ "(" STRINGIFY(__LINE__) "): " #x "\n" ); } while (0)
#else
#define ASSERT(X) ((void)0)
#endif

#ifdef MIPS32

//
// MIPS32 processor context
// DO NOT REORDER, assembler sources use this order
//
typedef struct _CONTEXT {
    ULONG At;
    ULONG V0;
    ULONG V1;

    ULONG A0;
    ULONG A1;
    ULONG A2;
    ULONG A3;

    ULONG T0;
    ULONG T1;
    ULONG T2;
    ULONG T3;
    ULONG T4;
    ULONG T5;
    ULONG T6;
    ULONG T7;

    ULONG S0;
    ULONG S1;
    ULONG S2;
    ULONG S3;
    ULONG S4;
    ULONG S5;
    ULONG S6;
    ULONG S7;

    ULONG T8;
    ULONG T9;

    ULONG K0;
    ULONG K1;

    ULONG Gp;
    ULONG Sp;
    ULONG Fp;
    ULONG Ra;

    ULONG Hi;
    ULONG Lo;
    ULONG Pc;
} CONTEXT, *PCONTEXT;

#endif // MIPS32

typedef struct _PROCESS {
    CONTEXT Context;
    
} PROCESS, *PPROCESS;


#endif
