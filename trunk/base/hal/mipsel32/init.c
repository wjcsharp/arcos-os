/*

Module Name:

    init.c

Abstract:

    MIPS32 HAL initialization and high level exception handling code.

Author:

    Michal Strehovsky

Revision History:

*/

#include <arcos.h>
#include <rtl.h>
#include <ke.h>
#include <hal.h>
#include <io.h>
#include <kd.h>
#include "halp.h"


PVOID
HalGetFirstUsableMemoryAddress(
    VOID
    )
{
    //
    // symbol _end gets defined by linker
    // it's address is the end of the last section of the executable in memory
    //
    extern int _end;
    return &_end;
}


//
// these two are defined in exception.S
//
extern CHAR HalExceptionHandlerVector;
extern CHAR HalExceptionHandlerVectorEnd;


VOID
HalInitialize(
    VOID
    )
{
    //
    // initialize UART for HalDisplayString to work
    //
    volatile PNS16550 tty = (PNS16550)UART_BASE;
    tty->lcr.field.wls = 3; // set bit width to 8 bits
    tty->ier.field.erbfi = 1; // we want interrupts
    tty->mcr.field.out2 = 1;

#ifdef HAVE_KD
    volatile PNS16550 tty2 = (PNS16550)0xb80002f8;
    tty2->lcr.field.wls = 3; // set bit width to 8 bits
    tty2->ier.field.erbfi = 1; // we want interrupts
    tty2->mcr.field.out2 = 1;
#endif
    
    //
    // initialize exception handling by copying exception handling vector
    // to a hardwired memory location
    //
    RtlCopyMemory((PVOID)0xa0000180, &HalExceptionHandlerVector,
        &HalExceptionHandlerVectorEnd - &HalExceptionHandlerVector);
    
    //
    // reset the system timer to fire in specified time
    //
    HalResetTimer(67000 * 5000);

    //
    // set buffer fifo length (IO)
    //
}

VOID
HalHandleException(
    PEXCEPTION_FRAME pFrame
    )
{
    //
    // fetch exception code from the exception frame
    //
    ULONG exceptionCode = (pFrame->Cause & CP0_CAUSE_EXCEPTION_CODE_MASK) >> CP0_CAUSE_EXCEPTION_CODE_SHIFT;

    if (exceptionCode) {
        
        // syscall exception?
        if (exceptionCode == CP0_CAUSE_SYSCALL) {
            HalDisplayString("SYSCALL\n");
        } else {
            KeBugCheck("Unexpected exception occured");
        }
    } else {

        // timer interrupt?
        if (pFrame->Cause & CP0_CAUSE_TIMER) {

            HalDisplayString("TICK\n");
            //
            // reset timer so that it fires next time again
            //
            HalResetTimer(67000 * 5000);
        } 

        // serial device interrupt?
        if (pFrame->Cause & CP0_CAUSE_UART) {

            volatile PNS16550 tty = (PNS16550)UART_BASE;

#ifdef HAVE_KD
            volatile PNS16550 tty2 = (PNS16550)0xb80002f8;
            if (tty2->lsr.field.dr) KdCharacterReceived(tty2->rbr);
#endif

            // check if any data is ready in the buffer
            // (if it's not, this interrupt just acknowledges that we finished writing data - ignore it)
            if (tty->lsr.field.dr) {
		IoInterruptHandler(tty->rbr);
                //HalAddCharToBuffer(tty->rbr);	// Old, fifo moved to IO
                //
                // HACKHACK: Simics voodoo to acknowledge interrupt
                //
                pFrame->Cause &= ~0x1000;
            } else return;
        }
    }
}

