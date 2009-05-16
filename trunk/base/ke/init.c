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

    //HANDLE handle = IoCreateFile('serial');

    while (1) {
        // wait for something interesting to happen
        KeYieldProcessor();
    }
}

