/*
 *  AppShell.c
 *
 *  Created on: May 21, 2009
 *  Author: Sam Eskandari
 */

#include <io.h>
#include <arcos.h>
#include <ps.h>
#include <kd.h>
#include <api.h>
#include <rtl.h>

void split(char *string,char *pCommand,char *pArgument)
{
	ULONG i;
	HANDLE handle;
	STATUS status;
	handle = CreateFile('s');
	pCommand = string;
	for(i=0;string[i] != '\0';i++)
	{
		if(string[i] == ' ')
		{
			string[i] = '\0';
			pArgument = &string[i+1];
			break;
		}
		else
			pArgument = NULL;
	}
	/*i = 0;
	while(pCommand[i])
	{
		KdPrint("%c",pCommand[i]);
		i++;
		if(pCommand[i] == NULL)
			KdPrint("%s","NULLLLLLLLLLLLLLLLLLLLLLL");
	}*/
	//KdPrint("Command:%s",pCommand);
	HANDLE commandProcess;
	
	//KdPrint("Argument is:%s11",pArgument);
	//KdPrint("Argument Length is: %d",RtlStringLength(pArgument));
	status = CreateProcess(pCommand,31,&commandProcess,pArgument);
	if (status != 0)
		WriteFile(handle,"\nARCOS:\\>Unknown comannd. Try again.",0);
	else
		ObCloseHandle(commandProcess);
	//KdPrint("Status is: %d",status);
    //KdPrint("Length is: %d",RtlStringLength(pCommand));
	//PsCreateProcessByName("TestProcess3", 1, &testProcess3, NULL);
    //ObCloseHandle(testProcess3);
	
}

void AppShell()
{
	CHAR input[100]; //buffer
	PCHAR c;
	CHAR ch[2];
	PCHAR command;
	PCHAR argument;
	HANDLE handle;
	ULONG i;
	ULONG n;
	ch[1] = NULL;
	c = ch;
	handle = CreateFile('s');
	WriteFile(handle,"ARCOS:\\>",0);
		i = 0;
		while(*c != '\r')
		{
			command = NULL;
			argument = NULL;
			ReadFile(handle,c,1);
			if (*c == '\b' || *c == 127) 
			{
				if (i > 0) 
				{
					i = i-1;
					input[i]='\0';
					WriteFile(handle,"\e[D \e[D",0);
				}
			}
			else if(*c == '\r')
				break;
			else
			{
				input[i] = ch[0];
				WriteFile(handle,c,1);
				i++;
			}
			if(i>99)
				i = 0;
		}
		split(input,command,argument);
		for(n=0;n<i;n++)
			input[n] = '\0';
		ch[0] = '\0';
		WriteFile(handle,"\n",0);
}
