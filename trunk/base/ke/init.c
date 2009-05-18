#include <arcos.h> 
#include <hal.h>
#include <ke.h>
#include <rtl.h>
#include <mm.h>
#include <io.h>
#include <ps.h>
#include <kd.h>


// remove this when removing the test processes
#include <api.h>
#include <apps.h>

VOID
TestProcess(PCHAR args) {
    UNREFERENCED_PARAMETER(args);

    KdPrint("Hello from testprocess. My PID is %d.", GetProcessId());

    while (1) {
        Sleep(5000);
        KdPrint("testprocess heartbeat");
    }
}

VOID
TestProcess2(PCHAR args) {
    UNREFERENCED_PARAMETER(args);

    KdPrint("Hello from testprocess2. My PID is %d.", GetProcessId());

    while (1) {
        Sleep(10000);
        KdPrint("testprocess2 heartbeat");
    }
}

VOID
TestScrollerProcess()
{
    scrollerInit(); 

    while (1) {
	//KdPrint("Hello from text scroller");
	scrollText();
        Sleep(1000);
    }
}

VOID
KeInitialize(VOID) {
    HANDLE testProcess, testProcess2, testProcess3, testScrollerProcess;

    HalInitialize();

#ifdef HAVE_KD
    KdInitialize();
#endif

    MmInitialize();

    IoInitialize();

    //Initialize ps manager, create initial process
    PsInitialize();
    ASSERT(KeCurrentProcess);


    KdPrint("Say %s to %s!", "hello", "ARCOS kernel debugger");

    HalDisplayString("Hello world!\n");

    //HANDLE handle = IoCreateFile('serial');


    PsCreateProcess(TestProcess, 1, &testProcess, NULL);
    PsCreateProcess(TestProcess2, 1, &testProcess2, NULL);
    PsCreateProcess(TestScrollerProcess, 1, &testScrollerProcess, NULL);

    //CRASHES with MmFree revision 103
    //PsCreateProcessByName("TestProcess3", 5, &testProcess3, NULL);

    KeRestoreInterrupts(TRUE);

    while (1) {
        // wait for something interesting to happen
        KeYieldProcessor();
    }
}

