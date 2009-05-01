/*

Module Name:

    status.h

Abstract:

    Constant definitions for the STATUS values.

Author:

    Michal Strehovsky

Revision History:

*/

#ifndef __STATUS_H__
#define __STATUS_H__

#include <types.h>

typedef ULONG STATUS;

#define STATUS_SUCCESS                  ((STATUS)0x00000000)

#define STATUS_TIMEOUT                  ((STATUS)0x00000001)

#define STATUS_INSUFFICIENT_RESOURCES   ((STATUS)0x00000002)

#define STATUS_INVALID_HANDLE           ((STATUS)0x00000003)

#define STATUS_INVALID_DEVICE_REQUEST   ((STATUS)0x00000004)

#define STATUS_NO_MEMORY                ((STATUS)0x00000005)

#define STATUS_BUFFER_TOO_SMALL         ((STATUS)0x00000006)

#define STATUS_SEMAPHORE_LIMIT_EXCEEDED ((STATUS)0x00000007)

#define STATUS_MUTEX_NOT_OWNED          ((STATUS)0x00000008)

#define STATUS_INVALID_PARAMETER_MIX    ((STATUS)0x00000009)

#define STATUS_INVALID_PARAMETER        ((STATUS)0x0000000A)

#define STATUS_OBJECT_TYPE_MISMATCH     ((STATUS)0x0000000B)

#define STATUS_HANDLE_LIMIT_REACHED     ((STATUS)0x0000000C)

#endif
