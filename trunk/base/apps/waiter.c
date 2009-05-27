#include <arcos.h>
#include <kd.h>
#include <rtl.h>
#include <api.h>
#include <apps.h>

VOID
AppWaiter() {
    HANDLE philos;
    BOOL freeForks[5];
    ULONG forksInUse, forkNr, messType, philPid, i, j, status;
    CHAR args[25], myMessage[50]; //BUGBUGBUG but it works
    PMESSAGE message, tmpmess;
    PCHAR finalMess;
    ULONG hungryl[5][2], hungryr[4][2];

    forksInUse = 0;
    for (i = 0; i < 5; i++) {
        freeForks[i] = TRUE;
    }
    //init hungry pids to 0
    for (i = 0; i < 5; i++) {
        hungryl[i][1] = 0;
    }
    for (i = 0; i < 4; i++) {
        hungryr[i][1] = 0;
    }


    message = NULL;

    for (i = 0; i < 5; i++) {
        RtlFormatString(args, 25, "%d %d %d", i, (i + 1) % 5, GetProcessId());
        KdPrint(args);
        status = CreateProcess("philosopher", 5, &philos, args);
        if (status == 0)
            CloseHandle(philos);
    }

    for (i = 0; i < 500; i++) {
        //KdPrint("before receive");
        while (!(message = ReceiveFirst(5000)));
        //Store message locally
        status = CopyMessage(myMessage, message, GetMessageSize(message));
        if (0 != status)
            KdPrint("to small buffer in waiter:%d", status);

        //KdPrint("%c%c%c", myMessage[0],myMessage[1],myMessage[2]);
        tmpmess = (PMESSAGE) message;
        finalMess = tmpmess->buffer;
        //L/R/D fork:finalMess[0], forknr:finalMess[1]-'0' sendPID:
        KdPrint("MessType:%cNUM:%dPID:%d", finalMess[0], finalMess[1] - '0', tmpmess->senderPid);
        Sleep(3000);
        messType = finalMess[0];
        forkNr = (ULONG) (finalMess[1] - '0');
        philPid = tmpmess->senderPid;

        //IF a drop message
        if (messType == 'D') {
            forksInUse--;
            freeForks[forkNr] = TRUE;

            for (j = 0; j < 4; j++) {
                if ((hungryr[j][0] == (ULONG) (forkNr)) && (hungryr[j][1] != 0)) {
                    SendMessage(hungryr[j][1], 0, "fork", 4);
                    forksInUse++;
                    freeForks[forkNr] = FALSE;
                    hungryr[j][0] = 0;
                    hungryr[j][1] = 0;
                    break;
                }
            }
            if (forksInUse < 4) {
                for (j = 0; j < 5; j++) {
                    if ((hungryl[j][0] == (ULONG) (forkNr)) && (hungryl[j][1] != 0)) {
                        SendMessage(hungryr[j][1], 0, "fork", 4);
                        forksInUse++;
                        freeForks[forkNr] = FALSE;
                        hungryl[j][0] = 0;
                        hungryl[j][1] = 0;
                        break;
                    }
                }
            }
        }

        if (messType == 'R') {
            KdPrint("in R");
            if (freeForks[forkNr]) {
                forksInUse++;
                freeForks[forkNr] = FALSE;
                KdPrint("sending R to:%d", philPid);
                SendMessage(philPid, 0, "fork", 4);
                //KdPrint("sending R END");
            } else {
                for (j = 0; j < 4; j++) {
                    if (hungryr[j][1] == 0)
                        hungryr[j][0] = forkNr;
                    hungryr[j][1] = philPid;
                    break;
                }

            }

        }

        if (messType == 'L') {
             KdPrint("in L");
            if ((forksInUse < 4) && freeForks[forkNr]) {
                forksInUse++;
                freeForks[forkNr] = FALSE;
                SendMessage(philPid, 0, "fork", 4);
            } else {
                for (j = 0; j < 5; j++) {
                    if (hungryl[j][1] == 0) {
                        hungryl[j][0] = forkNr;
                        hungryl[j][1] = philPid;
                        break;
                    }

                }

            }

        }

        DeleteMessage(message);
        //KdPrint("Waiter killing itself");
    }
    KillMe();
}
