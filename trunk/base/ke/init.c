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

/*
VOID
TestShell() {
    KdPrint("Shell PID is %d", GetProcessId());

   // while (1) {
        AppShell();
        //Sleep(0);
    //}
}
*/
/*
VOID
TestProcess() {
    ULONG i;

    KdPrint("Blubb from testprocess. My PID is %d.", GetProcessId());

    HANDLE handle = CreateFile('s');
    CHAR s[] = "Test1:     \n";
    //s[1] = NULL;
    PCHAR c = &s[6];

    //Sleep(5000);

    while (1) {
        for (i = 0; i < 0x00FFFF; i++);
        //Sleep(500);
        ReadFile(handle, c, 1);
        WriteFile(handle, s, 1);
        //KdPrint("testprocess heartbeat");
    }
}

VOID
TestProcess2() {
    ULONG i;

    KdPrint("Hello from testprocess2. My PID is %d.", GetProcessId());

    HANDLE handle = CreateFile('s');
    CHAR s[] = "Test2:     \n";
    //s[1] = NULL;
    PCHAR c = &s[6];

    //Sleep(15000);

    while (1) {
        for (i = 0; i < 0x00FFFF; i++);
        //Sleep(250);
        ReadFile(handle, c, 1);
        WriteFile(handle, s, 1);

        //KdPrint("testprocess2 heartbeat");
    }
}
 */
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
    HANDLE testProcess, testProcess2, testProcess3, testScrollerProcess, testShell;

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

    //HalDisplayString("Hello world!\n");

    //HANDLE handle = IoCreateFile('serial');

    CreateProcess("shell", 31, &testShell, NULL);
    ObCloseHandle(testShell);
    //PsCreateProcess(TestProcess, 30, &testProcess, NULL);
    //PsCreateProcess(TestProcess2, 5, &testProcess2, NULL);
    PsCreateProcess(TestScrollerProcess, 10, &testScrollerProcess, NULL);

    //PsCreateProcessByName("TestProcess3", 1, &testProcess3, NULL);
    //ObCloseHandle(testProcess3);

    KeRestoreInterrupts(TRUE);

    while (1) {
        // wait for something interesting to happen
        KeYieldProcessor();
    }
}

