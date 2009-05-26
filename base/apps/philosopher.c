/****************************************
	philosopher.c

	author: Hugo Heyman, 2009-05-25

	Philosopher node process handling

*****************************************/


#include <arcos.h>
#include <rtl.h>
#include <api.h>
#include <kd.h>


VOID AppPhilosopher() {
	
	ULONG LeftForkNum, RightForkNum, WaiterPID;
	PCHAR LeftForkMess, RightForkMess, LeftDropMess, RightDropMess;

	PVOID NewMess;

	// Get args from waiter process
	CHAR WaiterArgs[30]; //= KeCurrentProcess->Args;

	CopyArgs(WaiterArgs, 30);

	// Make separate ULONGs from the WaiterArgs string
	//
	LeftForkNum = WaiterArgs[0];
	RightForkNum = WaiterArgs[2];


	WaiterPID = RtlAtoUL(WaiterArgs);


	// Some debugging

	KdPrint("LeftFork: %d", LeftForkNum);

	KdPrint("RightFork: %d", RightForkNum);

	KdPrint("WaiterPID: %d", WaiterPID);


	// Make left fork message
	LeftForkMess = (PCHAR)'L';
	LeftForkMess++;
	LeftForkMess = (PCHAR)LeftForkNum;
	LeftForkMess++;
	LeftForkMess = '\0';

	// Make right fork message
	RightForkMess = (PCHAR)'R';
	RightForkMess++;
	RightForkMess = (PCHAR)RightForkNum;
	RightForkMess++;
	RightForkMess = '\0';
	
	// Make left fork drop message
	LeftDropMess = (PCHAR)'D';
	LeftDropMess++;
	LeftDropMess = (PCHAR)LeftForkNum;
	LeftDropMess++;
	LeftDropMess = '\0';

	// Make left fork drop message
	RightDropMess = (PCHAR)'D';
	RightDropMess++;
	RightDropMess = (PCHAR)RightForkNum;
	RightDropMess++;
	RightDropMess = '\0';

	/*********** Philosopher loop ******************/
	//
	while(1) {


		// Think
		Sleep(2000);


		// Get first chopstick
			SendMessage(WaiterPID, 0, LeftForkMess, 3);
			NewMess = NULL;
			while(!NewMess) {
				NewMess = ReceiveFirst(1000);
			}
			DeleteMessage(NewMess);


		// Get second chopstick
			SendMessage(WaiterPID, 0, RightForkMess, 3);

			NewMess = NULL;
			while(!NewMess) {
				ReceiveFirst(1000);
			}
			DeleteMessage(NewMess);


		// Eat
		Sleep(3000);


		// Drop left chopstick
		SendMessage(WaiterPID, 0, LeftDropMess, 3);


		// Drop right chopstick
		SendMessage(WaiterPID, 0, RightDropMess, 3);

	}
	
	/************************************************/
	

}