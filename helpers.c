#include "helpers.h"

int parseCharToInt(char* string) {
	char currentChar = string[0];
	int negative = 0;
	int startingCounter = 0, returnNumber = 0;
	if(currentChar == '-') {
		negative = 1;
		startingCounter++;
		currentChar = string[1];
	}

	while(currentChar != '\0') {
		returnNumber = returnNumber * 10 + ((int)currentChar-48);
		startingCounter++;
		currentChar = string[startingCounter];
	}

	if(negative == 1) returnNumber = -returnNumber;
	
	return returnNumber;
}
