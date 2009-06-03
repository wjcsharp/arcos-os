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
    ULONG dronePid;
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
        //Remove message from queue
        KdPrint("removing message");
        DeleteMessage(pmessage);
        pmessage = NULL;
        KdPrint("after removing message");
        //Create new process
        CreateProcess("drone", 5, &proc, NULL);
        //Supervise drone.
        GetPid(proc, &dronePid);
        SuperviseProc(GetProcessId(), dronePid);
        CloseHandle(proc);
    }
    KillMe(); //Wil never happen but follow apps convention
}