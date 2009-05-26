#include <arcos.h>
#include <kd.h>
#include <rtl.h>
#include <api.h>
#include <apps.h>

VOID
AppWaiter(){
    HANDLE philosophers[5];
    BOOL forks[5];
    ULONG forksinuse, i, status;
    CHAR args[25];
    PMESSAGE myMessage;


    for(i=0;i<5;i++){
        RtlFormatString(args, 25, "%d %d %d", i , (i+1)%5, GetProcessId());
        KdPrint(args);
        status = CreateProcess("philosopher", 15, &philosophers[i] ,args);
        KdPrint("%d", status);
    }
    //while(!(myMessage=ReceiveFirst(1000)));

    KillMe();
}
