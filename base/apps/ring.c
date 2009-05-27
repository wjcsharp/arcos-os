/*
 *  ring.c
 *
 *  Created on: May 25, 2009
 *  Author: 	Sam Eskandari & Olle Harstedt
 *  Doc:	Maximum of 99 babyrings.
 *
 *
 */
 
 #include <kd.h>
 #include <api.h>
#include <rtl.h>

// Delete this.
#include <arcos.h>
#include <ke.h>

 void AppRingNode()
 {
                HANDLE handle;
                ULONG receivedPID;
                PMESSAGE message;
                CHAR stringBuffer[100];
                PCHAR s;
				
                handle = CreateFile('s');
		
                message = ReceiveFirst(5000);
                if(message) KdPrint("Baby found message! myPid = %d, received pid = %d", GetProcessId(),*(PULONG)(message->buffer));
                else { KdPrint("BIP BIP BIP! Babynode not finding pid message! Commiting suicide."); KillMe(); }

                receivedPID = *((PULONG)message->buffer);
                DeleteMessage(message);

                s = stringBuffer;
                RtlFormatString(s, 100, "\n\rBip! I received target pid nr %d, my own pid is %d. Z z z...", receivedPID, GetProcessId());
                WriteFile(handle,s,0);

                // Eternal babyring loop.
                while(1){
                    // Wait for start message and sleep a second.
                    Sleep(2000);
                    message = ReceiveFirst(1);
                    Sleep(2000);
                    if(message){
                        RtlFormatString(s,100,"\n\rYey! I got start message! My pid is %d, sending start message to %d. Z z z...", GetProcessId(),receivedPID);
                        WriteFile(handle,s,0);
                        DeleteMessage(message);
                        SendMessage(receivedPID,0,&s,4);    // Message type and content doesn't matter here.
                    }
                }
                KillMe();
 }
 
 void AppRing()
 {

                ULONG i, myPid;
                HANDLE ring, handle;
                STATUS status;
		CHAR args[25];
		ULONG nr = 3;       // 3 = default.

		// How many babyrings? Check args (only two digits).
                if(CopyArgs(args,25) != 0){
                    nr = args[0] - '0';
                    if(args[1]) nr = nr + (args[1] - '0') * 10;     // What if second number is zero?
                }
                if(nr < 3) nr = 3;      // Must be atleast three, otherwise for-loops will crash.
                
                KdPrint("nr = %d", nr);
		// Fix array to store babyring pids.
		ULONG pids[10];

                handle = CreateFile('s');

                myPid = GetProcessId();

		KdPrint("nr = %d",nr);  // Check check.

                // Create all nodes.
                for(i = 0;i < nr;i++)
                {
                                status = CreateProcess("ringnode",10,&ring,NULL);
                                if(status != 0) KdPrint("Problem when creating process");
                                GetPid(ring,&pids[i]);
				KdPrint("pid from ring: %d", pids[i]);
                }
                KdPrint("pids = ");
                for(i = 0; i < nr; i++) KdPrint("%d", pids[i]);
                // Sending target pid message. Last node is special case - its pid should be the first node.
                for(i = 0;i < nr-1;i++) SendMessage(pids[i],0,&pids[i+1],4);
                SendMessage(pids[nr-1],0,&pids[0],4);

                Sleep(1000);
                WriteFile(handle,"\n\rRing master sending start message.",0);

                // Send start message.
                SendMessage(pids[0],0,&pids[0],4);      // Message type and content doesn't matter here.
                //while(1);
                KdPrint("RING_MASTER: All messages sent, killing myself.");
                KillMe();
}