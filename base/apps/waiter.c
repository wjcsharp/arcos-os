//
// File:   waiter.c
// Author: Magnus Söderling
//
//


#include <arcos.h>
#include <kd.h>
#include <rtl.h>
#include <api.h>
#include <apps.h>

VOID
AppWaiter() {
    HANDLE philos;
    BOOL freeForks[5];
    ULONG forksInUse, forkNr, messType, philPid, i, status;
    CHAR args[25], myMessage[50]; //BUGBUGBUG but it works
    PMESSAGE message, tmpmess;
    PCHAR finalMess;
    //ULONG hungryl[5][2], hungryr[4][2];

    forksInUse = 0;
    for (i = 0; i < 5; i++) {
        freeForks[i] = TRUE;
    }
    /*
     //init hungry pids to 0
     for (i = 0; i < 5; i++) {
         hungryl[i][1] = 0;
     }
     for (i = 0; i < 4; i++) {
         hungryr[i][1] = 0;
     }
     */

    message = NULL;

    //Create philosophers
    for (i = 0; i < 5; i++) {
        RtlFormatString(args, 25, "%d %d %d", i, (i + 1) % 5, GetProcessId());
        status = CreateProcess("philosopher", 5, &philos, args);
        if (status == 0)
            CloseHandle(philos);
    }

    for (i = 0; i < 300; i++) {

        while (!(message = ReceiveFirst(5000)));
        //Store message locally
        status = CopyMessage(myMessage, message, GetMessageSize(message));
        if (0 != status)
            KdPrint("to small buffer in waiter:%d", status);

        tmpmess = (PMESSAGE) message;
        finalMess = tmpmess->buffer;

        messType = finalMess[0];
        forkNr = (ULONG) (finalMess[1] - '0');
        philPid = tmpmess->senderPid;

        //IF a drop message
        if (messType == 'D') {
            //KdPrint("InD:%d", forkNr);
            forksInUse--;
            freeForks[forkNr] = TRUE;

        }

        if (messType == 'R') {
            //KdPrint("InR:%d", forkNr);
            if (freeForks[forkNr]) {
                forksInUse++;
                freeForks[forkNr] = FALSE;
                //   KdPrint("sending R to:%d", philPid);
                SendMessage(philPid, 0, "fork", 5);
                //   KdPrint("sending R END");
            }
        }


        if (messType == 'L') {
            // KdPrint("InL:%d", forkNr);
            if ((forksInUse < 4) && freeForks[forkNr]) {
                forksInUse++;
                freeForks[forkNr] = FALSE;
                //   KdPrint("sending L to:%d", philPid);
                SendMessage(philPid, 0, "fork", 5);
                // KdPrint("after sendL");
            } /*else {
                for (j = 0; j < 5; j++) {
                    if (hungryl[j][1] == 0) {
                        hungryl[j][0] = forkNr;
                        hungryl[j][1] = philPid;
                        break;
                    }
                }
            }*/
        }
        DeleteMessage(message);
    }
    KdPrint("waiter kills itself");
    KillMe();
}