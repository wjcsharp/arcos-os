#ifndef __OB_H__
#define __OB_H__

#include <arcos.h>

//
// object attibutes
//
#define OBJ_PERMANENT   1
#define OBJ_INHERIT     2

// forward declaration of _OBJECT_TYPE
struct _OBJECT_TYPE;

//
// object header definition
//
typedef struct _OBJECT_HEADER {
    ULONG Magic;
    ULONG HandleCount;
    ULONG PointerCount;
    ULONG Attributes;
    struct _OBJECT_TYPE *Type;
    
    struct _OBJECT_HEADER *NextObjectOfType;

    // placeholder for raw object data
    ULONG Body;
} OBJECT_HEADER, *POBJECT_HEADER;

//
// object type specific dump method pointer
//
typedef VOID (*DUMP_METHOD)(PVOID object, PCHAR buffer, ULONG bufferSize);

//
// object type specific delete method pointer
//
typedef VOID (*DELETE_METHOD)(PVOID object);

//
// type object definition
//
typedef struct _OBJECT_TYPE {
    ULONG Name;
    POBJECT_HEADER FirstObjectOfType;
    struct _OBJECT_TYPE *NextTypeObject;
    DUMP_METHOD Dump;
    DELETE_METHOD Delete;
} OBJECT_TYPE, *POBJECT_TYPE;

//
// object type initializer definition
//
typedef struct _OBJECT_TYPE_INITIALIZER {
    DUMP_METHOD DumpMethod;
    DELETE_METHOD DeleteMethod;
} OBJECT_TYPE_INITIALIZER, *POBJECT_TYPE_INITIALIZER;

STATUS
ObCreateObjectType(
    ULONG TypeName,
    POBJECT_TYPE_INITIALIZER ObjectTypeInitializer,
    POBJECT_TYPE *ObjectType
    );

STATUS
ObCreateObject(
    POBJECT_TYPE ObjectType,
    ULONG ObjectAttributes,
    ULONG ObjectBodySize,
    PVOID *Object
    );

STATUS
ObOpenObjectByPointer(
    PVOID Object,
    ULONG HandleAttributes,
    POBJECT_TYPE ObjectType,
    PHANDLE Handle
    );

STATUS
ObReferenceObjectByHandle(
    HANDLE Handle,
    POBJECT_TYPE ObjectType,
    PVOID *Object
    );

STATUS
ObReferenceObject(
    PVOID Object,
    POBJECT_TYPE ObjectType
);

VOID
ObDereferenceObject(
    PVOID Object
    );

STATUS
ObInitProcess(
    PPROCESS ParentProcess,
    PPROCESS NewProcess
    );

VOID
ObKillProcess(
    PPROCESS Process
    );

STATUS
ObWaitForSingleObject(
    HANDLE Handle,
    ULONG TimeOut
    );

STATUS
ObCloseHandle(
    HANDLE Handle
    );

PVOID
ObGetFirstObjectOfType(
    POBJECT_TYPE ObjectType
    );

PVOID
ObGetNextObjectOfType(
    PVOID Object
    );

VOID
ObDumpObject(
    PVOID Object,
    PCHAR Buffer,
    ULONG BufferSize
    );

#endif
