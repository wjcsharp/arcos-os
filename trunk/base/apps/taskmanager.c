#include <arcos.h>
#include <kd.h>
#include <rtl.h>
#include <api.h>
#include <apps.h>

#define TASKM_BUFFER_SIZE 20

ULONG
min(ULONG A, ULONG B) {
    if (A < B)
        return A;
    return B;
}

VOID
AppTaskManager() {
    //PPROCESS_INFO pinfo;
    ULONG numprocess, i, j, cputmp, tend;
    CHAR strbuff[250];
    HANDLE tmout;
    PCHAR timend[] = {"ms", "s"};
    PROCESS_INFO pinfo[TASKM_BUFFER_SIZE];

    tmout = CreateFile('s');

    for (j = 0; j < 1; j++) {
        //KdPrint("GetProcessInfo BEGIN");
        GetProcessInfo(pinfo, TASKM_BUFFER_SIZE, &numprocess);
        //KdPrint("GetProcessInfo END");
        WriteString(tmout, "\n\r---------TASKMANAGER---------\n\r");
        //KdPrint("---taskm DONE");
        //Sleep(1000);
        for (i = 0; i < (min(numprocess, TASKM_BUFFER_SIZE)); i++) {
            if (!pinfo[i].RunningProgram)
                pinfo[i].RunningProgram = "Unnamed";
        }
        for (i = 0; i < (min(numprocess, TASKM_BUFFER_SIZE)); i++) {
            //Check how big cputime is
            cputmp = pinfo[i].CPUTime;
            if (cputmp < 10000)
                tend = 0;
            else {
                cputmp = cputmp / 1000;
                tend = 1;
            }
            RtlFormatString(strbuff, 250, "%s PID: %d, STATE:%d CPU TIME: %d %s\n\r", pinfo[i].RunningProgram, pinfo[i].PID, pinfo[i].State, cputmp, timend[tend]);
            //KdPrint("Before WriteFile");
            WriteString(tmout, strbuff);
            //  Sleep(1000);
        }
        //Sleep(5000);
    }
    //KdPrint("Before tmgoodbye");
    WriteString(tmout, "--------Task Manager says godbye--------\n\r");
    //ObCloseHandle(tmout);//BUGBUGBUG should be syscall? is this needed?
    KillMe();
}

