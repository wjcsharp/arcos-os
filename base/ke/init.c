#include <arcos.h>
#include <hal.h>
#include <ke.h>
#include <rtl.h>
#include <mm.h>
#include <io.h>
#include <ps.h>

#include <kd.h>

VOID
KeInitialize(VOID) {
    CHAR temp[260];
    PROCESS initProcess;

    HalInitialize();

#ifdef HAVE_KD
    KdInitialize();
#endif

    MmInitialize();

    //IoInitialize();

    PsInitialize();

    KeRestoreInterrupts(TRUE);

    KdPrint("Say %s to %s!", "hello", "ARCOS kernel debugger");

    HalDisplayString("Hello world!\n");

    RtlFormatString(temp, sizeof (temp), "First usable address: 0x%x\n", HalGetFirstUsableMemoryAddress());

    HalDisplayString(temp);

    HalDisplayString("\x1B[D"); // ANSI cursor back by one character

    KeCurrentProcess = &initProcess;

//#define PSTEST
#ifdef PSTEST
    //BUGBUGBUGBUGBUGBUGBUGBUG
    //REMOVE REMOVE REMOVE REMOVE
    //Temporary to test PsCreate
    HANDLE handtag;
    HANDLE handtag2;
    PVOID mstempp;
    PPROCESS gurkproc1;
    PPROCESS gurkproc2;
    PsCreateProcess(MyFirstProgram, 14, &handtag, NULL);
    ObReferenceObjectByHandle(handtag, processType, &mstempp);
    gurkproc1 = (PPROCESS) mstempp;
    
    PsCreateProcess(MyFirstProgram, 24, &handtag2, NULL);
    ObReferenceObjectByHandle(handtag2, processType, &mstempp);
    gurkproc2 = (PPROCESS) mstempp;



    KdPrint("prio1: %d\n", gurkproc1->Priority);
    KdPrint("prio2: %d\n", gurkproc2->Priority);
    KdPrint("PID1: %d\n", gurkproc1->PID);
    KdPrint("PID2: %d\n", gurkproc2->PID);
    KdPrint("ExitStatus1: %d\n", gurkproc1->ExitStatus);
    KdPrint("ExitStatus2: %d\n", gurkproc2->ExitStatus);
    //End test ps
#endif
    //HANDLE handle = IoCreateFile('serial');

    while (1) {
        // wait for something interesting to happen
        KeYieldProcessor();
    }
}

