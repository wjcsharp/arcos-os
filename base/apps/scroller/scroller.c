/*
 *  scroller.c
 *
 *  Created on: May 13, 2009
 *  Author: Sam Eskandari
 */

#include <stdio.h>

int textLength( char text[] ) //Returns the length of string
{
	int textLength=0;
	while ( text[textLength] != '\0' )
		textLength++;
	return textLength;
}

void printToLED(char text[],int position) //Prints out 8 characters
{
	int i;
	int index = 0;
	char buffer[8];
	for(i=position;i<position+8;i++)
	{
		buffer[index] = text[i];
		index++;
	}
	printf("\n%s",buffer);
}

void scrollText()
{
	int i;
	char inputText[100]; //The string to show on the LED
	char begin[7]; //To use seven spaces in the beginning of the string
	char end[7]; //To use seven spaces at the enf of the string
	int stringLength;
	gets(inputText); //Reads string from keyboard
	stringLength = textLength(inputText);
	stringLength = 14+stringLength;
	char finalString[stringLength]; //THe original string plus 7 spaces in the beginning and 7 spaces at the end.
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
		printToLED(finalString,i);
		i++;
		if((finalString[i+7])=='\0')
			i=0;
	}
}

int main() //just to test the functions. Delete it if you don't like it.
{
	scrollText();
	return 0;
}