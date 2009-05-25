/*
 *  AppShell.c
 *
 *  Created on: May 21, 2009
 *  Author: Sam Eskandari
 */

#include <kd.h>
#include <api.h>

void split(char *string, char **pCommand, char **pArgument) {
    ULONG i;
    *pCommand = string;
    for (i = 0; string[i] != '\0'; i++) {
        if (string[i] == ' ') {
            string[i] = '\0';
            *pArgument = &string[i+1];
            break;
        } else
            *pArgument = NULL;
    }
}

void AppShell() {
    CHAR input[100], ch[2]; 
    PCHAR c, command, argument;
    HANDLE serialinout, outp, commandProcess;
    ULONG i, n;
    STATUS status;
    outp = CreateFile('s');
	serialinout = CreateFile('s');
    ch[1] = NULL;
    c = ch;
    WriteFile(serialinout, "ARCOS:\\>", 9);
    i = 0;
    while (*c != '\r') {
        command = NULL;
        argument = NULL;
        ReadFile(serialinout, c, 1);
        if (*c == '\b' || *c == 127) {
            if (i > 0) {
                i = i - 1;
                input[i] = '\0';
                WriteFile(serialinout, "\e[D \e[D", 9);
            }
        } else if (*c == '\r')
            break;
        else {
            input[i] = ch[0];
            WriteFile(serialinout, c, 1); //BUGBUGBUG?
            i++;
        }
        if (i > 99)
            i = 0;
    }
    split(input, &command, &argument);
		
	status = CreateProcess(command, 9, &commandProcess, argument);
    if (status != 0) {
        WriteFile(outp, "\n\rARCOS:\\>Unknown command. Try again.", 38);
        ObCloseHandle(outp);
    }

    if (status == 0) {
        ASSERT(commandProcess);
        ObCloseHandle(commandProcess);
    }
    ObCloseHandle(outp);
    for (n = 0; n < i; n++)
        input[n] = '\0';
    ch[0] = '\0';
    WriteFile(serialinout, "\n\r", 1);
    ObCloseHandle(serialinout);
}