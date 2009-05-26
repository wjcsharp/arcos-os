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
	
	ULONG  WaiterPID;
	CHAR LeftForkMess[3], RightForkMess[3], LeftDropMess[3], RightDropMess[3], LeftForkNum, RightForkNum;

	PVOID NewMess;

	// Get args from waiter process
	CHAR WaiterArgs[30]; //= KeCurrentProcess->Args;

	// Alloc memory
	/*
	LeftForkMess = Malloc(4);
	RightForkMess = Malloc(4);
	LeftDropMess = Malloc(4);
	RightDropMess = Malloc(4);
	*/


	CopyArgs(WaiterArgs, 30);

	// Make separate ULONGs from the WaiterArgs string
	//
	LeftForkNum = WaiterArgs[0];
	RightForkNum = WaiterArgs[2];


	WaiterPID = RtlAtoUL(&WaiterArgs[4]);


	// Some debugging

	KdPrint("LeftFork: %c", LeftForkNum);

	KdPrint("RightFork: %c", RightForkNum);

	KdPrint("WaiterPID: %d", WaiterPID);

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
	LeftDropMess[0] = 'D';
	LeftDropMess[1] = RightForkNum;
	LeftDropMess[2] = '\0';

	
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