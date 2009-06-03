#include <arcos.h> 
#include <hal.h>
#include <ke.h>
#include <rtl.h>
#include <mm.h>
#include <io.h>
#include <ps.h>
#include <kd.h>
#include <mess.h>
#include <api.h>
#include <apps.h>

VOID
TestScrollerProcess() {
    KdPrint("SCROLLER PID is %d", GetProcessId());

    scrollerInit();

    while (1) {
        //KdPrint("Hello from text scroller");
        scrollText();
        Sleep(500);
    }
}

VOID
KeInitialize(VOID) {
    HANDLE testScrollerProcess, testShell;

    HalInitialize();

#ifdef HAVE_KD
    KdInitialize();
#endif

    MmInitialize();

    IoInitialize();

    //Initialize ps manager, create initial process
    PsInitialize();
    ASSERT(KeCurrentProcess);

    //Initialize message passing
    MessInitialize();



    KdPrint("Say %s to %s!", "hello", "ARCOS kernel debugger");


    PsCreateProcess(TestScrollerProcess, 10, &testScrollerProcess, NULL);
    ObCloseHandle(testScrollerProcess);

    CreateProcess("shell", 31, &testShell, NULL);
    ObCloseHandle(testShell);




    KeRestoreInterrupts(TRUE);

    while (1) {
        // wait for something interesting to happen
        KeYieldProcessor();
    }
}

