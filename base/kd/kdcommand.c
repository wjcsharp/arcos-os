/*

Module Name:

    kdcommand.c

Abstract:

    Kernel debugger command processor.

Author:

    Michal Strehovsky

Revision History:

*/

#include <arcos.h>
#include <ke.h>
#include <rtl.h>
#include <ob.h>

#include "kdp.h"

typedef struct _KD_COMMAND {
    PCHAR Name;
    PCHAR HelpText;
    VOID (*Execute)(PCHAR Parameters);
} KD_COMMAND, *PKD_COMMAND;

PCHAR
KdpSplitParameter(
    PCHAR command
    )
{
    while (*command != 0) {

        if (*command == ' ') {

            *command = 0;
            command++;
            break;
        }
        
        command++;
    }

    return command;
}

VOID
KdpCmdTicks(
    PCHAR Parameter
    )
{
    UNREFERENCED_PARAMETER(Parameter);
    
    KdpPrint("%d milliseconds\n", KeGetTickCount());
}

VOID
KdpCmdObjects(
    PCHAR Parameter
    )
{
    // HACKHACK :)
    extern POBJECT_TYPE ObTypeObjectListHead;

    POBJECT_TYPE currentType = ObTypeObjectListHead;

    UNREFERENCED_PARAMETER(Parameter);

    while (currentType) {
        PVOID currentObject = ObGetFirstObjectOfType(currentType);

        KdpPrint("OBJECT_TYPE 0x%x Name: 0x%x\n", currentType, currentType->Name);

        while (currentObject) {
            CHAR dump[320];
            ObDumpObject(currentObject, dump, sizeof(dump));
    
            KdpPrint("0x%x %s\n", currentObject, dump);
            currentObject = ObGetNextObjectOfType(currentObject);
        }

        currentType = currentType->NextTypeObject;

        if (currentType) KdpPrintChar('\n');
    }
}

// neccessary forward declaration...
VOID
KdpCmdHelp(PCHAR Parameter);

KD_COMMAND KdpCommands[] = {
    { "help", "Prints this help", KdpCmdHelp },
    { "ticks", "Gets number of milliseconds that elapsed since the system started", KdpCmdTicks },
    { "objects", "Prints a list of objects present in the system", KdpCmdObjects }
};

VOID
KdpCmdHelp(
    PCHAR Parameter
    )
{
    ULONG i;

    UNREFERENCED_PARAMETER(Parameter);

    for (i = 0; i < COUNTOF(KdpCommands); i++) {

        KdpPrint("%s - %s\n", KdpCommands[i].Name, KdpCommands[i].HelpText);
    }
}

VOID
KdpExecuteCommand(
    PCHAR command
    )
{
    PCHAR parameter = KdpSplitParameter(command);
    ULONG i;

    for (i = 0; i < COUNTOF(KdpCommands); i++) {

        if (RtlCompareStrings(KdpCommands[i].Name, command) == 0) {

            KdpCommands[i].Execute(parameter);
            return;
        }
    }

    KdpPrint("%s: command not recognized\n", command);
}

