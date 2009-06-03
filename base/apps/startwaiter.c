//
// File:   startwaiter.c
// Author: Magnus Söderling
//
//


#include <arcos.h>
//#include <rtl.h>
#include <api.h>
//#include <apps.h>

VOID
AppStartWaiter(){
    STATUS status;
    HANDLE proc;
    
    status = CreateProcess("waiter", 15, &proc, NULL);
    CloseHandle(proc);
    KillMe();
}
