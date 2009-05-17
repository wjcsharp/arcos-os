#include <arcos.h> 
#include <hal.h>
#include <ke.h>
#include <rtl.h>
#include <mm.h>
#include <io.h>
#include <ps.h>
#include <kd.h>

VOID
TestProcess(PCHAR args)
{
    ULONG i;
    UNREFERENCED_PARAMETER(args);
    
    KdPrint("Hello from testprocess");
    
    while (1) {
        for (i = 0; i < 0xFFFFFF; i++);
        KdPrint("testprocess heartbeat");        
    }
}

VOID
TestProcess2(PCHAR args)
{
    ULONG i;
    UNREFERENCED_PARAMETER(args);

    KdPrint("Hello from testprocess2");
    
    while (1) {
        for (i = 0; i < 0x1FFFFFF; i++);
        KdPrint("testprocess2 heartbeat");        
    }
}

VOID
KeInitialize(VOID) {
    HANDLE testProcess, testProcess2, testProcess3;

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
    PsCreateProcessByName("TestProcess3", 1, &testProcess3, NULL);

    KeRestoreInterrupts(TRUE);

    while (1) {
        // wait for something interesting to happen
        KeYieldProcessor();
    }
}

