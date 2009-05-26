/*
 *  Author:     Olle
 *  Date:       2009-05-26
 *  Function:   Used in user programs to get a string as input.
 *  Doc:        Must be called from user mode - not kernel!
 */

#include <arcos.h>
#include <io.h>
#include <api.h>
#include <mm.h>

PCHAR
GetLine(){

    ULONG i, length;
    PCHAR s, iterator;
    HANDLE handle;

    handle = IoCreateFile('s');
    length = 0;
    // Allocate 100 bytes/chars.
    iterator = s = MmAlloc(100);
    if(!s) { KdPrint("Memory out in GetLine"); return NULL; }
    // Just check...
    ASSERT(s); ASSERT(iterator);
    // Make null with s. This is so WriteFile will work properly.
    for(i = 0; i < 99; i++)
        *s = '\0';

    // Loop some input.
    do{
        ReadFile(handle,iterator,1);
        KdPrint("Not waiting as it should?");
        WriteFile(handle,iterator,1);
        length++; 
        //if(*iterator == '\r') break;        // Add '\r', or don't.
    } while(*iterator++ != '\r');

    // CloseFile;

    return s;
}
