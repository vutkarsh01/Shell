// use -lreadline flag to run
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <readline/readline.h> 
#include <readline/history.h> 

#define clear() printf("\033[H\033[J")
#define MAXCOM 1000 //max no of characters in a command
#define MAXLIST 100 // max number of commands

void init_shell() 
{ 
    clear(); 
    printf("\n\n\n\n******************"
        "***********************************"); 
    printf("\n\n\n\t****UT's SHELL****"); 
    printf("\n\n\n\n*******************"
        "***********************"); 
    char* username = getenv("USER"); 
    printf("\n\n\n@%s What's Up??\n", username); 
} 

void printDirectory(){
   char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
       printf("%s:", cwd);
   }
   	else {
       perror("getcwd() error");
   }
}

void PipeCommmads(char **parsed,char **parsedpipe)
{
	int pipefd[2];
	if(pipe(pipefd) <0)
	{
		printf("Pipe could not be initialized. Please try again. :(\n");
		return;
	}
	int p1 = fork(); 
	if(p1<0){
		printf("Child process could not be initialized. Please try again. :(\n");
		return;
	}

	if(p1==0) //child enters
	{
		close(pipefd[0]); ///
		dup2(pipefd[1], STDIN_FILENO); 
		close(pipefd[1]);
		if (execvp(parsedpipe[0],parsedpipe) < 0){
			printf("Could not execute command 1... :(\n");
			exit(0);
		}
	}

	else 	//Parent
	{
		int p2 =fork();
		if(p2<0){
		printf("Child process could not be initialized. Please try again. :(\n");
		return;
		}	

		if(p2==0)
		{
			close(pipefd[1]);
			dup2(pipefd[0] ,STDIN_FILENO);
			close(pipefd[0]);
			if (execvp(parsedpipe[0], parsedpipe) < 0) { 
                printf("\nCould not execute command 2... :(\n"); 
                exit(0); 
            } 
		}

		else
		{
			wait(NULL);
			wait(NULL);
		}
	}
}

void SimpleCommands(char **parsed)
{
	int child = fork();
	if(child == -1)
		printf("Could not execute command. Please try again. :(\n");
	else if(child == 0)	////only child enters this arena.
		{
			if(execvp(parsed[0],parsed) < 0)		////all the fragments would be accessed until it reaches null
				printf("Could not execute command. Please try again. :)\n");
				exit(0);			 ////only child process stops
		}
	else
	{
		wait(NULL);
		return;
	}
}

int ownCmdHandler(char **parsed)
{
	char *ownCmds[5] = {"exit","cd","help","hello","clear"};
	int index;
	
	if(parsed[0]==NULL)
		return 1;
	
	for(int i=0;i<5; i++)
	{
		if(!(strcmp(parsed[0],ownCmds[i])))	//identical strings
		{
			index = i;
			break;  		
		}
	}

	if(index == 0)
	{	
		printf("\nGoodbye\n"); 
        exit(0); 
	}
	else if(index == 1)
	{
		chdir(parsed[1]);
		return 1;
	}
	else if(index == 2)
	{
		printf("List of Commands supported :\n");
		printf(
        "\n>cd"
        "\n>ls"
        "\n>exit"
        "\n>all other general commands available in UNIX shell"
   		"\n>clear"
        "\n>pipe handling"
        "\n>improper space handling\n");

	}
	else if(index == 3)
	{
		printf("Hello World!\n");
		return 1;	
	}
	else if(index ==4)
	{
		clear();
		return 1;
	}

	return 0;
}

int ParsingPipe(char* str, char** strpiped)
{
	for(int i=0;i<2;i++)
	{
		strpiped[i] = strsep(&str,"|");
		if (strpiped[i] == NULL)
			break;
	}
	if(strpiped[1]!=NULL)
		return 1;  				/// Pipe is found
	else
		return 0;				/// No pipe
}

void ParsingSpace(char* str,char** parsed)
{
	for(int i=0;i<MAXLIST;i++)
	{
		parsed[i] = strsep(&str," ");
		if(parsed[i] == NULL)
			break;
		if(strlen(parsed[i]) == 0)		
			i--;						////trimming extra spaces
	}
}

int func(char* pointertoinput,	char **parsed, char** parsedpipe)
{
	char *strpiped[2];  	 	//// will store the commands after parsing pipes;
	int pipes=0;				//// will be 1 if pipes will be found;
	pipes = ParsingPipe(pointertoinput, strpiped);	

	if(pipes){
		ParsingSpace(strpiped[0] , parsed); 			///Stores command before pipe
		ParsingSpace(strpiped[1] , parsedpipe);			///Stores command after pipe
	}
	else
		ParsingSpace(pointertoinput,parsed);
	
	if(ownCmdHandler(parsed))
		return 0;
	else if(pipes)
		return 2;
	else
		return 1;
}

int main() {

    init_shell();
    char str[MAXCOM];
    char *pointertoinput;
    int check_type;
    char *parsed[MAXLIST],*parsedpipe[MAXLIST];
    while(1)
    {
    	printDirectory();
    	pointertoinput = readline(">> ");
        if(strlen(pointertoinput)!=0)
        {
        	add_history(pointertoinput);
        	strcpy(&str[0],pointertoinput);
        }
        check_type = func(pointertoinput, parsed, parsedpipe); 
        /// check type = 0 // already implemented
        /// = 1 if simple commmand
        /// = 2 pipes
    	if(check_type==1)
    		SimpleCommands(parsed);
    	else if(check_type == 2)
    		PipeCommmads(parsed, parsedpipe);
    }
   return 0;
}