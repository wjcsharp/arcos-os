/****************************************
        philosopher.c

        author: Hugo Heyman, 2009-05-25

        Philosopher node process handling

 *****************************************/


#include <arcos.h>
#include <rtl.h>
#include <api.h>
#include <kd.h>

VOID
AppPhilosopher() {

    ULONG WaiterPID;
    CHAR LeftForkMess[3], RightForkMess[3], LeftDropMess[3], RightDropMess[3], LeftForkNum, RightForkNum;
    HANDLE outp;
    PVOID NewMess;
    CHAR WaiterArgs[30]; //= KeCurrentProcess->Args;
    CHAR outstring[80];
    
    outp = CreateFile('s');
    // Get args from waiter process
    CopyArgs(WaiterArgs, 30);

    // Make separate ULONGs from the WaiterArgs string
    //
    LeftForkNum = WaiterArgs[0];
    RightForkNum = WaiterArgs[2];


    WaiterPID = RtlAtoUL(&WaiterArgs[4]);


    // Some debugging

    //KdPrint("LeftFork: %c", LeftForkNum);
    //KdPrint("RightFork: %c", RightForkNum);
    //KdPrint("WaiterPID: %d", WaiterPID);


    // Make left fork message
    LeftForkMess[0] = 'L';
    LeftForkMess[1] = LeftForkNum;
    LeftForkMess[2] = '\0';

    // Make right fork message
    RightForkMess[0] = 'R';
    RightForkMess[1] = RightForkNum;
    RightForkMess[2] = '\0';

    // Make left fork drop message
    LeftDropMess[0] = 'D';
    LeftDropMess[1] = LeftForkNum;
    LeftDropMess[2] = '\0';

    // Make left fork drop message
    RightDropMess[0] = 'D';
    RightDropMess[1] = RightForkNum;
    RightDropMess[2] = '\0';


    //KdPrint("LeftForkMess: %c%c", LeftForkMess[0], LeftForkMess[1]);

    while (1) {


        // Think
        RtlFormatString(outstring,80,"PID:%d THINKING\r\n", GetProcessId());
        WriteString(outp, outstring);
        //KdPrint("Philosopher[%c]: Thinking..", LeftForkNum);
        Sleep(2000);


        // Get first chopstick
        //SendMessage(WaiterPID, 0, LeftForkMess, 3);
        NewMess = NULL;
        while (!NewMess) {
            SendMessage(WaiterPID, 0, LeftForkMess, 3);
            NewMess = ReceiveFirst(3000);
        }
        DeleteMessage(NewMess);

        //        KdPrint("Philosopher[%c]: I got the left fork!", LeftForkNum);
        RtlFormatString(outstring,80,"PID:%d Got Left Fork\r\n", GetProcessId());
        WriteString(outp, outstring);
        // Get second chopstick


        NewMess = NULL;
        while (!NewMess) {
            SendMessage(WaiterPID, 0, RightForkMess, 3);
            NewMess = ReceiveFirst(3000);
        }
        DeleteMessage(NewMess);

        //KdPrint("Philosopher[%c]: I got the right fork!", LeftForkNum);
        RtlFormatString(outstring,80,"PID:%d Got Right Fork EATING\r\n", GetProcessId());
        WriteString(outp, outstring);

        // Eat
        Sleep(3000);


        //KdPrint("Philosopher[%c]: Dropping left fork (%c)", LeftForkNum, LeftForkNum);
        // Drop left chopstick

        RtlFormatString(outstring,80,"PID:%d Dropping Left fork\r\n", GetProcessId());
        WriteString(outp, outstring);
        //KdPrint("LDM:%s", LeftDropMess);
        SendMessage(WaiterPID, 0, LeftDropMess, 3);


        //KdPrint("Philosopher[%c]: Dropping right fork (%c)", LeftForkNum, RightForkNum);
        // Drop right chopstick
        RtlFormatString(outstring,80,"PID:%d Dropping right fork\r\n", GetProcessId());
        WriteString(outp, outstring);
        //KdPrint("RDM%s", RightDropMess);
        SendMessage(WaiterPID, 0, RightDropMess, 3);

    }

    /************************************************/


}