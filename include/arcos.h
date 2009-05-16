#ifndef __ARCOS_H__
#define __ARCOS_H__

#include <types.h>
#include <status.h>
#include <mess.h>

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
    do { if (!(X)) KeBugCheck("Assertion failed: " __FILE__ "(" STRINGIFY(__LINE__) "): " #X "\n" ); } while (0)
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

#ifdef WIN32

#define DWORD                           ULONG
#define BYTE                            UCHAR

#define MAXIMUM_SUPPORTED_EXTENSION     512
#define SIZE_OF_80387_REGISTERS         80

typedef struct _FLOATING_SAVE_AREA {
    DWORD   ControlWord;
    DWORD   StatusWord;
    DWORD   TagWord;
    DWORD   ErrorOffset;
    DWORD   ErrorSelector;
    DWORD   DataOffset;
    DWORD   DataSelector;
    BYTE    RegisterArea[SIZE_OF_80387_REGISTERS];
    DWORD   Cr0NpxState;
} FLOATING_SAVE_AREA;

//
// again, do not reorder, this order is used by Windows API
//
typedef struct _CONTEXT {
    DWORD ContextFlags;

    DWORD   Dr0;
    DWORD   Dr1;
    DWORD   Dr2;
    DWORD   Dr3;
    DWORD   Dr6;
    DWORD   Dr7;

    FLOATING_SAVE_AREA FloatSave;

    DWORD   SegGs;
    DWORD   SegFs;
    DWORD   SegEs;
    DWORD   SegDs;

    DWORD   Edi;
    DWORD   Esi;
    DWORD   Ebx;
    DWORD   Edx;
    DWORD   Ecx;
    DWORD   Eax;

    DWORD   Ebp;
    DWORD   Eip;
    DWORD   SegCs;
    DWORD   EFlags;
    DWORD   Esp;
    DWORD   SegSs;

    BYTE    ExtendedRegisters[MAXIMUM_SUPPORTED_EXTENSION];
} CONTEXT, *PCONTEXT;

#endif

/* PROCESS CONTROL BLOCK
 * PROCESS and datastructures of PROCESS
 */
//Defines possible states of a process.
typedef enum {
    created, running, ready,  blocked, done
}PROCESS_STATE, *PPROCESS_STATE;


typedef struct _HANDLE_TABLE_ENTRY {
    ULONG Attributes;
    PVOID Object;
} HANDLE_TABLE_ENTRY, *PHANDLE_TABLE_ENTRY;

//
// specifies the maximum number of open handles in a process
//
#define MAX_HANDLE_COUNT 256

typedef struct _PROCESS {
    CONTEXT Context;
    HANDLE_TABLE_ENTRY HandleTable[MAX_HANDLE_COUNT];
    PROCESS_STATE State;
    ULONG RunningProgram;
    ULONG PID;
    ULONG Priority;
    ULONG ExitStatus;
    PVOID AllocatedMemory;//Assume only one allocation per process
    ULONG CPUTime;
    ULONG WakeUpTime;
    ULONG Quantum;
    PMESSAGE MessageQueue; 	// Added by Olle
    PMESSAGE Supervisor;	// Added by Lars Karlsson från Kättismeåla
    struct _PROCESS *NextPCB;
} PROCESS, *PPROCESS;



#endif
