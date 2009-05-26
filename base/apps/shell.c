/*
 *  AppShell.c
 *
 *  Created on: May 21, 2009
 *  Author: Sam Eskandari
 */

#include <kd.h>
#include <api.h>
#include <arcos.h>
#include <apps.h>
#include <rtl.h>


void split(char *string, char **pCommand, char **pArgument) 
{
    ULONG i;
    *pCommand = string;
    for (i = 0; string[i] != '\0'; i++) 
	{
        if (string[i] == ' ') 
		{
            string[i] = '\0';
            *pArgument = &string[i+1];
            break;
        } else
            *pArgument = NULL;
    }
}

void AppShell() 
{
    CHAR input[100], ch[2]; 
    PCHAR c, command, argument;
    HANDLE handle, commandProcess;
	ULONG i, n;
    STATUS status;
    ch[1] = NULL;
    c = ch;
	while(1)
	{
		handle = CreateFile('s');
		WriteFile(handle, "ARCOS:\\>", 9); 
		i = 0;
		while (*c != '\r') 
		{
			command = NULL;
			argument = NULL;
			ReadFile(handle, c, 1); 
			if (*c == '\b' || *c == 127) 
			{
				if (i > 0) 
				{
					i = i - 1;
					input[i] = '\0';
					WriteFile(handle, "\e[D \e[D", 9); 
				}
			} 
			else if (*c == '\r')
				break;
			else 
			{
				input[i] = ch[0];
				WriteFile(handle, c, 1);  
				i++;
			}
			if (i > 99)
				i = 0;
		}
		split(input, &command, &argument);
		if (RtlCompareStrings(command,"Exit") == 0)
			KillMe();
		else
		{
			status = CreateProcess(command, 9, &commandProcess, argument);
			if (status != 0) 
			{
				WriteFile(handle, "\n\rARCOS:\\>Unknown command. Try again.", 38);
				//CloseHandle(commandProcess);				
			}
			else
			{
				status = WaitForSingleObject(commandProcess);
				//CloseHandle(commandProcess);
			}
			for (n = 0; n < i; n++)
				input[n] = '\0';
			ch[0] = '\0';
			WriteFile(handle, "\n\r", 1); 
			//CloseHandle(handle);
		}
	}
}