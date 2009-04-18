#ifndef __TYPES_H__
#define __TYPES_H__

#define VOID void
typedef VOID *PVOID;
typedef char CHAR;
typedef CHAR *PCHAR;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;
typedef short SHORT;
typedef SHORT *PSHORT;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef long LONG;
typedef LONG *PLONG;
typedef unsigned long ULONG;
typedef ULONG *PULONG;

typedef UCHAR BOOL;
typedef BOOL *PBOOL;
#define TRUE    1
#define FALSE   0

typedef PVOID HANDLE;

//
// Calculate the address of the base of the structure given its type, and an
// address of a field within the structure.
//
#define CONTAINING_RECORD(address, type, field) \
    ((type *)( (PCHAR)(address) - (PCHAR)(&((type *)0)->field) ))
                                                  
typedef struct _STRING {
    PCHAR Buffer;
    USHORT MaximumLength;
    USHORT Length;
} STRING, *PSTRING;

#endif
