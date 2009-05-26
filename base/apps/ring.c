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

#include <arcos.h>
#include <ke.h>

 void AppRingNode()
 {
 
                //KdPrint("I'm alive!");
                //KillMe();
 
                HANDLE handle;
                ULONG recievedPID;
                PMESSAGE message;
                CHAR c[100];
                PCHAR s;
                ULONG PID;
		
		
                handle = CreateFile('s');
                s = c;
                PID = 7;
		
		KdPrint("babyring pid: %d", GetProcessId());

                Sleep(1000);

                message = ReceiveFirst(5000);
                
		//ASSERT(message);
                /*
		recievedPID = *((PULONG)message->buffer);
                WriteFile(handle,"\n\rI recieved message from ",20);
                RtlFormatString(s,100,"%d",PID);
                s = "7";
                WriteFile(handle,s,20);
                WriteFile(handle,", my own PID is ",20);
                RtlFormatString(s,100,"%d",GetProcessId());
                WriteFile(handle,s,20);
                
		
		message = ReceiveFirst(10000);
                WriteFile(handle,"\n\rRecieved ",20);
                ASSERT(message);
                WriteFile(handle,message->buffer,20); //BUGGGGGGGGGGG
                KdPrint("Real");
                RtlFormatString(s,100,", I'm sending message to %d.",recievedPID);
                WriteFile(handle,s,20);
                SendMessage(recievedPID,0,"hej! Jag är meddelande",23);
                WriteFile(handle,"\n\rMessage sent. I'm killing myself....",20);
                KillMe();
                ObCloseHandle(handle);
		*/
                KdPrint("Babynode not waiting anymore, killing myself.");
	KillMe();
 }
 
 void AppRing()
 {
     KillMe();
                ULONG recievedPid, i, myPid;
                PMESSAGE message;
                HANDLE ring, handle;
                STATUS status;
		CHAR argBuffer[20];
		CHAR args[25];
		CHAR c;
		ULONG nr;

		// How many babyrings? Check args (only two digits).
		CopyArgs(args, 25);
		nr = args[0] - '0';		
		if(args[1]) nr = nr + (args[1] - '0') * 10;

		// Fix array to store babyring pids.
		ULONG pids[nr];

                handle = CreateFile('s');

                myPid = GetProcessId();

		KdPrint("nr = %d",nr);

                // Create all nodes.
                for(i = 0;i < nr;i++)
                {
                                status = CreateProcess("ringnode",10,&ring,NULL);
                                if(status != 0) KdPrint("Problem when creating process");
                                PsGetPid(ring,&pids[i]);
				KdPrint("pid from ring: %d", pids[i]);
                }
                
		ASSERT(nr > 1);
                for(i = 0;i < nr-2;i++) SendMessage(pids[i],0,&pids[i+1],4);
		KdPrint("All messages sent");

		/*                
                SendMessage(PIDs[n-1],0,&myPID,4);
                
                message = ReceiveFirst(10000);
                
                ASSERT(message);
                recievedPid = *((PULONG)message->buffer);   // Or is this PCHAR?
                //KdPrint("Here");
                //DeleteMessage(message);
                KdPrint("Here");
                WriteFile(handle,"Message deleted",25);
		*/
		KillMe();
}