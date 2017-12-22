

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h> 
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h> 



	/*
	input save buffer
	*/
	char *inputBuffer;
	/*	
	struct for checkPipeline function
	*/ 
	struct checkPipeInfo checkInfo;
	/*	
	struct for getpwuid() function in <pwd.h>
	use for username
	*/ 
	struct passwd *myinfoPwd;
/*	
	struct for checkPipeline function
*/ 
struct checkPipeInfo
{
	int checkPoint;
	char *commandAfterPipe;
	char **parameterAfterPipe;
};




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
void commandPrompt(char *promptInfo);

/*
use fgets() function to get the input command into a buffer
The entered command string must be tokenized into an array of strings by removing the space delimiters. 
Also delimiters consisting of more than one space must be handled correctly.
*/
int readCommand(char *promptInfo, char **command, char **parameter);

/*build the builtin command for cd and exit command and the cd command 
should display error message if necessary*/
int builtInCommand(char *command, char **parameter);

/*a function check if the command contain a pipline or not*/
void checkPipline(char **parameter, int parameterNumber, struct checkPipeInfo *checkInfo);
/*a function to initialize the information of checkInfo */
void checkInfoAssign(struct checkPipeInfo *checkInfo);
/*write my own exec function use use getenv() to get the path and then call
execve() inside*/
void execMyself(char* command, char **parameter);



