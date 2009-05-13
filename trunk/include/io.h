#ifndef __IO_H__
#define __IO_H__

#include <arcos.h>
#include <ob.h>

#define FIFO_SIZE	16

POBJECT_TYPE    serialType;
POBJECT_TYPE    ledType;

// Definitions of pointers to methods WriteFile and readFile, used in struct below.
//typedef VOID (*IOWRITEFILE_METHOD)(HANDLE file, PVOID buffer, ULONG bufferSize);
//typedef VOID (*IOREADFILE_METHOD)(HANDLE file, PVOID buffer, ULONG bufferSize);
typedef VOID (*IOWRITEFILE_METHOD)(PVOID buffer, ULONG bufferSize);
typedef VOID (*IOREADFILE_METHOD)(PVOID buffer, ULONG bufferSize);

typedef struct _FILE {
        //ULONG (*IoWriteFile)(HANDLE file, PVOID buffer, ULONG bufferSize);
        //ULONG (*IoReadFile)(HANDLE file, PVOID buffer, ULONG bufferSize);
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
IoInterreuptHandler();

#endif
