/*
 *  scroller.c
 *
 *  Created on: May 13, 2009
 *  Author: Sam Eskandari
 */

#include <arcos.h>
#include <io.h>
#include <rtl.h>

HANDLE handle;
PCHAR text = "Denna scrollas...";	// Test text.
PCHAR pek;				// Point on same stuff.
ULONG length, moreLength;

void scrollerInit()
{
	handle = IoCreateFile('l');
	moreLength = length = RtlStringLength(text);
	pek = text;
}

void scrollText()
{

	IoWriteFile(handle,"hej",3);			
	/*
	pek++;
	length--;

	if (length == 0)	// Back to the beginning.
	{
		length = moreLength;		
		pek = text;
	}
	*/
}

/****************************

//#include <stdio.h>
#include <arcos.h>
#include <io.h>

int textLength( char text[] ) //Returns the length of string
{
	ULONG textLength=0;
	while ( text[textLength] != '\0' )
		textLength++;
	return textLength;
}

void printToLCD(CHAR text[],ULONG position) //Prints out 8 characters
{
	ULONG i;
	ULONG index = 0;
	CHAR buffer[8];
	HANDLE handle;
	for(i=position;i<position+8;i++)
	{
		buffer[index] = text[i];
		index++;
	}
	//printf("\n%s",buffer);
	//HalDisplayString(buffer);
	handle = IoCreateFile('l');
	ASSERT(handle);
	IoWriteFile(handle,buffer,8);
	//IoWriteFile(handle,"test",4);
}

void scrollText()
{
	ULONG i;
	//CHAR inputText[100]; //The string to show on the LED
	//PCHAR finalString;
	PCHAR inputText = "                          ";
	CHAR begin[7]; //To use seven spaces in the beginning of the string
	CHAR end[7]; //To use seven spaces at the enf of the string
	ULONG stringLength;
	//gets(inputText); //Reads string from keyboard
	inputText = "       hej       \0";
	//finalString = "       hej       ";	// Added by Olle
	
	stringLength = textLength(inputText);
	stringLength = 14+stringLength;
	CHAR finalString[stringLength]; //THe original string plus 7 spaces in the beginning and 7 spaces at the end.
	for(i=0;i<=7;i++)
	{
		begin[i] = ' ';
		end[i] = ' ';
	}
	for(i=0;i<=stringLength;i++) //clean the buffer (do not delete it)
		finalString[i] = '\0';
	i = 0;
	while(i < stringLength) //Adds 7 spaces in the beginning and at the end of the string
	{
		if (i<7) //Adds 7 spaces in the beginning of the string
		{
			finalString[i] = begin[i];
			i++;
		}
		else if ((6 < i) && (i < stringLength -7 )) //Reads the original string and put it on the 8th place
		{
			finalString[i] = inputText[i-7];
			i++;
		}
		else //Adds 7 spaces at the end of the edited string
		{
			finalString[i] = end[stringLength - i];
			i++;
		}
	}
	

	i = 0;
	
	while(1) //Runs until the end of the universe BABY
	{
		printToLCD(finalString,i);
		i++;
		if((finalString[i+7])=='\0')
			i=0;
	}
}

*************************/