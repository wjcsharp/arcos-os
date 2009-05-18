#ifndef __APPS_H__
#define __APPS_H__
#include <arcos.h>
#include <api.h>
//ALL APPS SHOULD HAVE VOID RETURN AND USE KillMe() for graceful exit.
//Arguments is passed via PCHAR Args in PROCESS


VOID
MyFirstProgram();

VOID
PSTestProcess3();

VOID		// Should return status.
scrollerInit();

VOID
scrollText();

#endif
