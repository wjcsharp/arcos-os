/*
 *  shell.c
 *
 *  Created on: May 21, 2009
 *  Author: Sam Eskandari
 */

#include <stdio.h>

void split(char *string,char *pCommand,char *pArgument)
{
	pCommand = string;
	int i;
	for(i=0;string[i] != '\0';i++)
	{
		if(string[i] == ' ')
		{
			string[i] = '\0';
			pArgument = &string[i+1];
			break;
		}
	}
	printf("\nCommand is: %s",pCommand); //delete it
	printf("\nArgument is %s",pArgument); //delete it
}

int main()
{
	char input[256]; //buffer
	char *command;
	char *argument;
	printf("\nWrite a string: "); //delete it
	gets(input); //reads from keyboard
	split(input,command,argument);
	return 0;
}
