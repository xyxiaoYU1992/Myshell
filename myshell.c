

#include "myshell.h"


#define TRUE 1
#define FALSE 0
#define MAX_NAME_LEN 256
#define MAX_PATH_LEN 1024
#define MAXLINE 4096



void shellProgram(){

	char promptInfo[ MAX_PATH_LEN + MAX_NAME_LEN + 20];
	char *command;
    char **parameter;
    int parameterNumber;
    int childPID1, childPID2;
    int pipe_fd[2];
    int status;

    /*printf("%d\n", checkInfo->checkPoint);*/
	inputBuffer = malloc(sizeof(char) * MAXLINE);
	command = NULL;
	parameter = malloc(sizeof(char*) * 30);
		/*repeat forever to let user use*/ 
	while(TRUE){			
		/* Print a command prompt that also displays the current working directory */
		commandPrompt(promptInfo); 
		parameterNumber = readCommand(promptInfo, &command, parameter);
		if(parameterNumber == -1){
			continue;
		}
		/*modify the parameter number to avoid segamentfault
		 */
		parameterNumber--;
		checkPipline(parameter, parameterNumber, &checkInfo);
		if(strcmp(command,"cd") == 0 || strcmp(command,"exit") == 0 || strcmp(command,"quit") == 0){
			builtInCommand(command, parameter);
			continue;
		}
		/* check the command have pipline or not*/

		/*check pipe failed or not*/
		if(checkInfo.checkPoint){
			if(pipe(pipe_fd) < 0){
				printf("error: pipe failed.\n");
				exit(0);
			}
		}
		/*if it is parent process*/
		if((childPID1 = fork()) != 0){
			/*if piped*/
			if(checkInfo.checkPoint ){
				if ((childPID2 = fork()) == 0) /*command after pipeline*/
				{
					close(pipe_fd[1]);
					close(fileno(stdin));
					dup2(pipe_fd[0], fileno(stdin));
					close(pipe_fd[0]);
					execMyself(checkInfo.commandAfterPipe,checkInfo.parameterAfterPipe);
				}
				/*wait for the command after pipe oherwise*/
				else{
					close(pipe_fd[0]);
					close(pipe_fd[1]);
					waitpid(childPID2, &status, 0);
				}
			}
			else{
				waitpid(childPID1, &status, 0);
			}
		}
		/* command before pipeline*/

		else{
			if(checkInfo.checkPoint){
				close(pipe_fd[0]);
				close(fileno(stdout));
				dup2(pipe_fd[1], fileno(stdout));
				close(pipe_fd[1]);
			}
			execMyself(command, parameter);

		}

	}
	free(parameter);
	free(inputBuffer);
}

int main(){	


	/* call the shell program */
	shellProgram(); 
	return 0;
}
void execMyself(char* command, char **parameter){
	char *pathEnviroment;
	pathEnviroment = getenv("PATH");
	if(NULL == pathEnviroment){
		printf("couldn't get path variable\n");
	}
	else{
		if(execvp(command, parameter) == -1){
			printf("wrong input command\n");
		}
	}
}
void checkInfoAssign(struct checkPipeInfo *checkInfo){
	checkInfo->checkPoint = 0;
	checkInfo->commandAfterPipe = NULL;
	checkInfo->parameterAfterPipe = NULL;

	return;
}
/*a function check if the command contain a pipline or not*/
void checkPipline(char **parameter, int parameterNumber, struct checkPipeInfo *checkInfo){
	int i;
	/*initialized the checkInfo*/
	checkInfoAssign(checkInfo);
	for(i = 0; i< parameterNumber;){
		if(0 == strcmp(parameter[i], "|")){
			char *saveTheInfo;
			checkInfo->checkPoint = 1;
			parameter[i] = NULL;
			checkInfo->commandAfterPipe = parameter[i + 1];
			checkInfo->parameterAfterPipe = &parameter[i + 1];
			for(saveTheInfo = checkInfo->parameterAfterPipe[0] + strlen(checkInfo->parameterAfterPipe[0]);
				saveTheInfo != &(checkInfo->parameterAfterPipe[0][0]) && *saveTheInfo != '/';
				saveTheInfo--);
			if(*saveTheInfo == '/'){
				saveTheInfo++;
			}
			checkInfo->parameterAfterPipe[0] = saveTheInfo;
			break;
		}
		else{
			i++;
		}
	}
/*	printf("pipe check: ");
		checkInfo->checkPoint?printf("yes, command: %s %s %s\n", checkInfo->commandAfterPipe,checkInfo->parameterAfterPipe[0],checkInfo->parameterAfterPipe[1]):printf("no\n");*/
	return;
} 

/*
build the builtin command for cd and exit command and the cd command 
should display error message if necessary
use strcmp() function compare with the command check it is exit or cd
if it is exit or quit just exit(0);
if it is cd, use chdir() function
*/
int builtInCommand(char *command, char **parameter){
	char *cdNewPath;
	cdNewPath = NULL;
	/* cd function */
	if(strcmp(command, "cd") == 0){
		/*if just cd like zsh make it same with ~*/
		if(parameter[1] == NULL){
			printf("please input path!\n");

		}
		/*if cd to ~ means to the homedirectory*/
		if(parameter[1][0] == '~'){
			cdNewPath = malloc(strlen(myinfoPwd->pw_dir) + strlen(parameter[1]));
			strcpy(cdNewPath, myinfoPwd->pw_dir);
			strncpy(cdNewPath+strlen(myinfoPwd->pw_dir), parameter[1] + 1,strlen(parameter[1]));
/*			printf("path with ~:\n%s\n",cdNewPath);*/

		}
		/*other path*/
		else{
			cdNewPath = malloc(strlen(parameter[1] + 1));
			strcpy(cdNewPath,parameter[1]);
/*			printf("path with other:\n%s\n",cdNewPath);*/

		}
		if(chdir(cdNewPath) != 0){
			printf("wrong path!\n");
		}
		free(cdNewPath);


	}
	/*exit function*/
	else if(strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0){
		exit(0);
	}
	return 0;
}

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
/*	
	commdPrompt fuction display information
	It will display username, hostname and the current path
	in terminal.
	username use getpwuid(getuid())
	hostname use gethostname()
	path use getcwd()
	the display should like what bash display:
	username@hostname:path$(change $ to # if root)
*/
void commandPrompt(char *promptInfo){

	/*	variable for hostname ,username, length of promptInfo and path */
	char *userName;
	char hostName[MAX_NAME_LEN];
	char pathName[MAX_PATH_LEN];
	int lengthOfPrompt;
	/*	get the username information	*/
	myinfoPwd = getpwuid(getuid());
	userName = myinfoPwd->pw_name;

	/*	get the path information	*/
	getcwd(pathName, MAX_PATH_LEN);
	/*printf("%s\n", pathName);*/



	/*	get the hostname information
		if succeed, we should use sprintf() to 
		add username@hostname to a string to save them
		if fail just add the username information
	*/

	if(gethostname(hostName, MAX_NAME_LEN) == 0){
		sprintf(promptInfo,"[MyShell]%s@%s:", userName, hostName);
	}
	else{
		sprintf(promptInfo,"[MyShell]%s@unknown:", userName);
	}
	/*printf("%s\n", promptInfo);*/

/*	save the length of the prompt information, you can know where to add
	the path information. Because you should add the path infomation after 
	the hostname username information.Using sprintf() again.
	save the length of promptInfo first, and then check whether the pathname 
	is homedirectory. If path is homedirectory, use "~" to replace it. Use strlen()
	to get length and use strncmp to check the homedirectory.
*/


	lengthOfPrompt = strlen(promptInfo);
	/*
		pathname less than homedirectory or part of pathname not equal 
		to homedirectory display pathname otherwise replace homedirectory by ~
		you can get length by the return value of the sprintf
	*/	
	if(strlen(pathName) < strlen(myinfoPwd->pw_dir) || strncmp(pathName, myinfoPwd->pw_dir, strlen(myinfoPwd->pw_dir)) !=0){
		lengthOfPrompt = sprintf(promptInfo + lengthOfPrompt, "%s", pathName);

	}
	else{
		lengthOfPrompt = sprintf(promptInfo + lengthOfPrompt, "~%s", pathName + strlen(myinfoPwd->pw_dir));
	}
/*	printf("%s\n", promptInfo);*/
/*
	check whether user is root or not 

*/	
	lengthOfPrompt = strlen(promptInfo);
	if(getuid() == 0){
		sprintf(promptInfo + lengthOfPrompt, "#:");
	}
	else{
		sprintf(promptInfo + lengthOfPrompt, "$:");
	}
	return;
}
