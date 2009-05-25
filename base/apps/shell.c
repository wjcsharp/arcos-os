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

void split(char *string, char *pCommand, char *pArgument) {
    ULONG i;
    HANDLE outp, commandProcess;
    STATUS status;
    outp = CreateFile('s');
    pCommand = string;
    for (i = 0; string[i] != '\0'; i++) {
        if (string[i] == ' ') {
            string[i] = '\0';
            pArgument = &string[i + 1];
            break;
        } else
            pArgument = NULL;
    }
    // if (RtlCompareStrings(pCommand, "Exit") == 0)
    //   KillMe();

    status = CreateProcess(pCommand, 9, &commandProcess, pArgument);
    if (status != 0) {
        WriteFile(outp, "\n\rARCOS:\\>Unknown command. Try again.", 38);
        ObCloseHandle(outp);
    }

    if (status == 0) {
        ASSERT(commandProcess);
        ObCloseHandle(commandProcess);
    }
    ObCloseHandle(outp);
}

void AppShell() {
    CHAR input[100]; //buffer
    PCHAR c;
    CHAR ch[2];
    PCHAR command;
    PCHAR argument;
    HANDLE serialinout;
    ULONG i;
    ULONG n;
    ch[1] = NULL;
    c = ch;
    serialinout = CreateFile('s');
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
    split(input, command, argument);
    for (n = 0; n < i; n++)
        input[n] = '\0';
    ch[0] = '\0';
    WriteFile(serialinout, "\n\r", 1);
    ObCloseHandle(serialinout);
}