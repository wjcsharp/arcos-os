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
#include "halp.h"

static FIFO fifo;	// fifo

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
    fifo.length = 0;	
}

// Added by Olle
// Stops adding when buffer is full. 
VOID
HalAddCharToBuffer(CHAR c)
{
	if (fifo.length + 1 < FIFO_SIZE)		// length + 1
	{
		fifo.buffer[fifo.length] = c;
		fifo.length++;
	}
}

// Added by Olle. 
// Returns first char in fifo buffer, which can be NULL.
CHAR
HalGetFirstCharFromBuffer()
{
	CHAR c;
	ULONG i;
	c = fifo.buffer[0];
	for (i = 0; i != fifo.length; i++)		// Move all chars one step. Tested, but not supertested.
		fifo.buffer[i] = fifo.buffer[i+1];	// Could be optimized. Later.
	if (fifo.length > 0)				// Decrease fifo length
		fifo.length--;
	return c;
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

            // check if any data is ready in the buffer
            // (if it's not, this interrupt just acknowledges that we finished writing data - ignore it)
            if (tty->lsr.field.dr) {
                HalAddCharToBuffer(tty->rbr);	// New code for IO
                //
                // HACKHACK: Simics voodoo to acknowledge interrupt
                //
                pFrame->Cause &= ~0x1000;
            } else return;
        }
    }
}

