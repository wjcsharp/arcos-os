#ifndef __IO_H__
#define __IO_H__

#include <arcos.h>
#include <ob.h>

#define FIFO_SIZE	16

// Used by IoInterruptHandler to store chars in buffer. Fixed size.
typedef struct _FIFO 
{
	CHAR buffer[FIFO_SIZE];
	ULONG length;
} FIFO;

POBJECT_TYPE    serialType;
POBJECT_TYPE    ledType;

// Definitions of pointers to methods WriteFile and readFile, used in struct below.
typedef VOID (*IOWRITEFILE_METHOD)(PVOID buffer, ULONG bufferSize);
typedef VOID (*IOREADFILE_METHOD)(PVOID buffer, ULONG bufferSize);

typedef struct _FILE {
	IOWRITEFILE_METHOD write;
	IOREADFILE_METHOD read;
} FILE, *PFILE;

ULONG
IoWriteFile(
        HANDLE file,
        PVOID buffer,
        ULONG bufferSize
        );

ULONG
IoReadFile(
        HANDLE file,
        PVOID buffer,
        ULONG bufferSize
        );

HANDLE
IoCreateFile(
        ULONG filename           // Hm.
        );

VOID
IoInterreuptHandler(CHAR c);	// Changed, but it matters only to me.

#endif
