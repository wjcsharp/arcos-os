//
// File:   waiter.c
// Author: Magnus Söderling
//
//


#include <arcos.h>
#include <kd.h>
#include <api.h>
#include <apps.h>

VOID
AppKeepAlive() {
    HANDLE proc;
    ULONG dronePid = 0;
    PMESSAGE pmessage = NULL;

    CreateProcess("drone", 15, &proc, NULL);
    GetPid(proc, &dronePid);
    //KdPrint("dronepid:%d:", dronePid);
    CloseHandle(proc);
    //Supervise drone.
    SuperviseProc(GetProcessId(), dronePid);

    while (1) {
        KdPrint("after while1");
        while (!(pmessage = ReceiveFirst(100000)));
        KdPrint("after while mess");
        CreateProcess("drone", 5, &proc, NULL);
        GetPid(proc, &dronePid);
        //KdPrint("dronepid:%d:", dronePid);
        CloseHandle(proc);
        //Supervise drone.
        SuperviseProc(GetProcessId(), dronePid);
        //Remove message fro mqueue
        KdPrint("removing message");
        DeleteMessage(pmessage);
        pmessage = NULL;
        KdPrint("after removing message");
    }
    KillMe(); //Wil never happen but follow apps convention
}