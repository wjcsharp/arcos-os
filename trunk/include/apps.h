#ifndef __APPS_H__
#define __APPS_H__
#include <arcos.h>

//ALL APPS SHOULD HAVE VOID RETURN AND USE KillMe() for graceful exit.
//Arguments is passed via PCHAR Args in PROCESS as a string.

VOID
AppPhilosopher();

VOID
AppWaiter();

VOID
AppSupervise();

VOID
AppTaskManager();

VOID
AppPSTestProcess3();

VOID		// Should return status.
scrollerInit();

VOID
scrollText();

VOID
AppKill();

VOID
AppChangePrio();

VOID
AppShell();

VOID
AppRing();

VOID
AppRingNode();

#endif
