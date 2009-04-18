#include <arcos.h>
#include <hal.h>
#include <ke.h>
#include <rtl.h>

VOID
KeInitialize(VOID)
{
    CHAR temp[260];
    PROCESS initProcess;
    
    HalInitialize();

    KeRestoreInterrupts(TRUE);

    HalDisplayString("Hello world!\n");

    RtlFormatString(temp, sizeof(temp), "First usable address: 0x%x", HalGetFirstUsableMemoryAddress());

    HalDisplayString(temp);

    HalDisplayString("\x1B[D"); // ANSI cursor back by one character

    KeCurrentProcess = &initProcess;

    while (1) {
        // wait for something interesting to happen
        KeYieldProcessor();
    }
}

