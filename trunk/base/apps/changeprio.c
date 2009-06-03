//
// File:   changeprio.c
// Author: Magnus Söderling
//
//


#include <arcos.h>
#include <kd.h>
#include <rtl.h>
#include <api.h>

VOID
AppChangePrio() {
    ULONG pid, prio, argLength, status;
    PCHAR first, second;
    PCHAR pek;// = KeCurrentProcess->Args;
    CHAR Args[25];

    //Get function args
    argLength = CopyArgs(Args, 25);
    pek = Args;

    //THE FOLLOWING IS REALLY UGLY so look away!
    if (argLength == 0) {
        KdPrint("ChangePrio needs arguments e.g. ' 2 3'('PID NEWPRIO')"); //BUGBUGBUG
        KillMe();
    }
    //remove everything until first digit
    if (!Rtlappisdigit(*pek)) {
        while ((!Rtlappisdigit(*++pek)) && (*pek != 0));
        if (0 == *pek) {
            KdPrint("ChangePrio needs arguments e.g. ' 2 3'('PID NEWPRIO')"); //BUGBUGBUG
            KillMe();
        }
    }
    first = pek; //Start of first stringarg
    //Find end of first arg
    while (Rtlappisdigit(*++pek));
    if (0 == *pek) {
        KdPrint("ChangePrio needs 2 arguments e.g. ' 2 3'('PID NEWPRIO')"); //BUGBUGBUG
        KillMe();
    }
    *pek = 0; //Set end of first string
    //remove everything until next digit
    while ((!Rtlappisdigit(*++pek)) && (*pek != 0));
    if (0 == *pek) {
        KdPrint("ChangePrio needs 2 arguments e.g. ' 2 3'"); //BUGBUGBUG
        KillMe();
    }
    second = pek; //Start of second stringarg
    //Find end of second arg
    while (Rtlappisdigit(*++pek));
    if (0 != *pek)
        *pek = 0;

    pid = RtlAtoUL(first);
    prio = RtlAtoUL(second);

    status = ChangePriority(pid, prio);
    if (0 != status) {
        KdPrint("The pid:%d is not in use", pid); //BUGBUGBUG
        KillMe();
    }

    KdPrint("Changed Priority of PID = %s", first); //BUGBUGBUG
    KdPrint("Changed priority to: %s", second); //BUGBUGBUG
    KillMe();
}
