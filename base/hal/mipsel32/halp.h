/*

Module Name:

    halp.h

Abstract:

    MIPS32 HAL private include file.

Author:

    Michal Strehovsky

Revision History:
	2009-05-08: Olle added struct fifo.

*/

#include <arcos.h>

//
// base address of the NS16550 UART controller
//
#define UART_BASE           0xb80003f8

//
// base address of the Intel 8259A master interrupt controller
//
#define I8259A_MASTER_BASE  0xb8000020

//
// base address of the Intel 8259A slave interrupt controller
//
#define I8259A_SLAVE_BASE   0xb80000a0

// HACKHACK: this structure is just ripped off of the sample sources, naming is very poor
typedef struct
{
  union {
    UCHAR rbr; /* RBR: Receiver Buffer Register     */
    UCHAR thr; /* THR: Transmitter Holding Register */
    UCHAR dll; /* DLL: Divisor Latch (LS)           */
  };
  union {
    /* IER: Interrupt Enable Register */
    union {
        UCHAR reg;
        struct {
            UCHAR erbfi : 1;  /* bit 0: Enable RBR Data Available Interr */
            UCHAR etbei : 1;  /* bit 1: Enable THR Empty Interrupt       */
            UCHAR elsi  : 1;  /* bit 2: Enable Receiver Line Status Intr */
            UCHAR edssi : 1;  /* bit 3: Enable MODEM Status Interrupt    */
        } field;
    } ier;

    /* DLM: Divisor Latch (MS) */
    UCHAR dlm;
  };
  union {
    /* IIR: Interrupt Identity Register */
    union {
        UCHAR reg;
        struct {
            UCHAR pending : 1;  /* bit 0:   Interrupt Pending */
            UCHAR id      : 3;  /* bit 1-3: Interrupt ID      */
        } field;
    } iir;

    /* FCR: FIFO Control Register */
    union {
        UCHAR reg;
        struct {
            UCHAR fe  : 1;  /* bit 0: FIFO Enable     */
            UCHAR rfr : 1;  /* bit 1: RCVR FIFO Reset */
            UCHAR xfr : 1;  /* bit 2: XMIT FIFO Reset */
            UCHAR dms : 1;  /* bit 3: DMA Mode Select */
        } field;
    } fcr;
  };
  /* LCR: Line Control Register */
  union {
      UCHAR reg;
      struct {
          UCHAR wls : 2;  /* bit 0-1: Word Length Select  */
          UCHAR stb : 1;  /* bit 2:   Number of Stop Bits */
          UCHAR pen : 1;  /* bit 3:   Parity Enable       */
          UCHAR eps : 1;  /* bit 4:   Even Parity Select  */
      } field;
  } lcr;

  /* MCR: MODEM Control Register */
  union {
      UCHAR reg;
      struct {
          UCHAR dtr  : 1;  /* bit 0: Data Terminal Ready */
          UCHAR rts  : 1;  /* bit 1: Request To Send     */
          UCHAR out1 : 1;  /* bit 2: Out 1               */
          UCHAR out2 : 1;  /* bit 3: Out 2               */
      } field;
  } mcr;

  /* LSR: Line Status Register */
  union {
      UCHAR reg;
      struct {
          UCHAR dr   : 1;  /* bit 0: Data Ready                   */
          UCHAR oe   : 1;  /* bit 1: Overrun Error                */
          UCHAR pe   : 1;  /* bit 2: Parity Error                 */
          UCHAR fe   : 1;  /* bit 3: Framing Error                */
          UCHAR bi   : 1;  /* bit 4: Break Interrupt              */
          UCHAR thre : 1;  /* bit 5: Transmitter Holding Register */
          UCHAR temt : 1;  /* bit 6: Transmitter Empty            */
      } field;
  } lsr;

  /* MSR: MODEM Status Register */
  union {
      UCHAR reg;
      struct {
          UCHAR dcts : 1;  /* bit 0: Delta Clear to Send          */
          UCHAR ddsr : 1;  /* bit 1: Delta Data Set Ready         */
          UCHAR teri : 1;  /* bit 2: Trailing Edge Ring Indicator */
          UCHAR ddcd : 1;  /* bit 3: Delta Data Carrier Detect    */
          UCHAR cts  : 1;  /* bit 4: Clear to Send                */
          UCHAR dsr  : 1;  /* bit 5: Data Set Ready               */
          UCHAR ri   : 1;  /* bit 6: Ring Indicator               */
          UCHAR dcd  : 1;  /* bit 7: Data Carrier Detect          */
      } field;
  } msr;

  /* Scratch Register */
  UCHAR scratch;

} NS16550, *PNS16550;

//
// Intel 8259A interrupt controller
//
typedef struct _I8259A {
    UCHAR Reg0;
    UCHAR Reg1;
} I8259A, *PI8259A;


//
// mask used to get exception code from cause register
//
#define CP0_CAUSE_EXCEPTION_CODE_MASK   0x7C

//
// shift required to get the exception code from cause register
//
#define CP0_CAUSE_EXCEPTION_CODE_SHIFT  2

//
// if this bit is set in the cause register, timer was the reason for interrupt
//
#define CP0_CAUSE_TIMER                 0x8000

//
// if this bit is set in the cause register, UART (serial port) was the reason for interrupt
//
#define CP0_CAUSE_UART                  0x0400

//
// exception code for the syscall exception
//
#define CP0_CAUSE_SYSCALL               8

//
// exception frame used to pass data between high level and low level handler
// assembly sources use this too, so be careful to modify it right
//
typedef struct _EXCEPTION_FRAME {
    CONTEXT Context;
    ULONG Cause;
    ULONG Status;
} EXCEPTION_FRAME, *PEXCEPTION_FRAME;

//
// set timer to fire after specified period of time
//
VOID HalResetTimer(ULONG ticks);

