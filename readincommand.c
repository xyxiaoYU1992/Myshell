#include "myshell.h"

/*
return value is the number of paramaeters
0 represents only command without any paramaeters
-1 represents wrong input
*/
int readCommand(char *promptInfo, char **command, char **parameter){
	/*command get as a string and store in this variable*/
	char *commandGet;
	int cursor;
	int prevspace;
	int lvc;
	char *bufferCountStart;
	char *bufferCountEnd;
	int parameterCount;
	int finished;
	cursor = 0;
	prevspace = 0;
	printf("%s", promptInfo);
	commandGet = fgets(inputBuffer, MAXLINE, stdin);
	/*
	if pointer point to null just exit
	*/
	if(commandGet == NULL){
		printf("\n");
		exit(0);
	}
	/*wrong input*/
	if(inputBuffer[0] == '\0'){
		return -1;
	}

/*check get the input correctly*/
/*
	for(cursor = 0; cursor < strlen(inputBuffer); cursor++){
	printf("%c\n", inputBuffer[cursor]);
	}
*/
	/*shrink inputbuffer's spaces(more than two) into one space*/
	do{
		if((inputBuffer[cursor] == ' ') && prevspace){
			for(lvc = cursor; inputBuffer[lvc]; lvc++){
				inputBuffer[lvc] = inputBuffer[lvc + 1];
			}
		}
		else{
			prevspace = (inputBuffer[cursor++] == ' ');
		}
	}while(inputBuffer[cursor]);
/*check the shrink*/
	/*puts(inputBuffer);*/

/*divide the buff by space 
use double pointer to save command and parameter safely
use delimier is space*/
	parameterCount = 0;
	finished = FALSE;
	bufferCountStart = inputBuffer;
	bufferCountEnd = inputBuffer;
	while(finished == FALSE){
		/*check they meet space or not*/ 
		while(*bufferCountStart == ' ' && *bufferCountEnd == ' '){
			bufferCountStart++;
			bufferCountEnd++;
		}
		/*check the buffer count reach the end or not and if no command return -1*/
		if(*bufferCountEnd == '\0' || *bufferCountEnd == '\n'){
			/*reach end at first means wrong*/
			if(parameterCount == 0){
				return -1;
			}
			break;
		}
		/* endcount keep going if didn't reach end or space*/
		while(*bufferCountEnd != ' ' && *bufferCountEnd != '\0' && *bufferCountEnd != '\n'){
			bufferCountEnd++;
		}
		/* if no parameter and command now*/
		if(parameterCount == 0){
			char *afterCommand;
			afterCommand = bufferCountEnd;
			*command = bufferCountStart;
			/* also need check sometimes we type such as ./ as a command */
			while(afterCommand != bufferCountStart && *afterCommand != '/'){
				afterCommand--;
			}
			if(*afterCommand == '/'){
				afterCommand++;
			}
			parameter[0] = afterCommand;
			parameterCount = parameterCount + 2;
		}
		/*if they already have command and parameter*/
		else{
			parameter[parameterCount - 1] = bufferCountStart;
			parameterCount++;
		}
		/*if reach end now then finish*/
		if(*bufferCountEnd == '\0' || * bufferCountEnd == '\n'){
			*bufferCountEnd = '\0';
			finished = TRUE;
		}
		else{
			*bufferCountEnd = '\0';
			bufferCountEnd++;
			bufferCountStart = bufferCountEnd;
		}
		/*otherwise let start = end and start from end */
	}
	/*let the last argument = null*/
parameter[parameterCount - 1] = NULL;

/*test for the command pararameter function*/
/*printf("commandwith/:%s\ncommand:%s\nparameters:\n", *command, parameter[0] );
int testi;
for(testi = 0; testi < parameterCount - 1; testi++){
	printf("%s\n", parameter[testi]);
}*/
	return parameterCount;
}