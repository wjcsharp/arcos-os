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
    HANDLE handle, commandProcess;
    STATUS status;
    handle = CreateFile('s');
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

    status = CreateProcess(pCommand, 31, &commandProcess, pArgument);
    if (status != 0) {
        WriteFile(handle, "\nARCOS:\\>Unknown command. Try again.", 38);
        ObCloseHandle(handle);
    }

    if (status == 0) {
        ASSERT(commandProcess);
        ObCloseHandle(commandProcess);
    }
    ObCloseHandle(handle);
}

void AppShell() {
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
    WriteFile(handle, "ARCOS:\\>", 9);
    i = 0;
    while (*c != '\r') {
        command = NULL;
        argument = NULL;
        ReadFile(handle, c, 1);
        if (*c == '\b' || *c == 127) {
            if (i > 0) {
                i = i - 1;
                input[i] = '\0';
                WriteFile(handle, "\e[D \e[D", 9);
            }
        } else if (*c == '\r')
            break;
        else {
            input[i] = ch[0];
            WriteFile(handle, c, 1);
            i++;
        }
        if (i > 99)
            i = 0;
    }
    split(input, command, argument);
    for (n = 0; n < i; n++)
        input[n] = '\0';
    ch[0] = '\0';
    WriteFile(handle, "\n", 0);
    ObCloseHandle(handle);
}
