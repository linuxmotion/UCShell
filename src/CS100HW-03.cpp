//============================================================================
// Name        : CS100HW-03.cpp
// Author      : 
// Version     :
// Copyright   :
// Description : UCShell, ashell program that behaves like a subset of
//				 bash, the standard Linux shell.
//============================================================================

#include <iostream>
#include <sys/wait.h>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;


// dir must be manually changed
void callCD(char* cwd, vector<string>);
string handleDotandTilde(const string commandToken, char cwd[]) ;
static inline string tildeExpansion();

// Comment this out to disable debuging
//#define DEBUG

#ifdef DEBUG
#define log(String) \
		cout << __LINE__ << " " << String << endl;
#else
#define log(String)
#endif

const int BUFFER_SIZE = 288;


int main(int argc, char* argv[], char* envp[]) {


	char cwd[1024];

	vector<string> Args;
	vector<string> Tokens;

	for(int i = 0; i < argc; i++){

		Args.push_back(argv[i]);
	}

	char *buffer = new char[BUFFER_SIZE];

	do{
		// get the current dir
		getcwd(cwd, sizeof(cwd));
		// clear all the tokens from the last input
		Tokens.clear();
		// Print the command line
		printf("%s$ ", cwd);
		//Get the line that was entered
		cin.getline(buffer, BUFFER_SIZE);
		// clean stdin
		cin.clear();
		// Break it up by tokens
		char* prog = strtok(buffer, " " );
		// Put all the tokens into a vector
		while (prog != NULL)
		{
			Tokens.push_back(prog);
			//printf ("%s\n",prog);
			prog = strtok (NULL, " ");
		}

		if(!strcmp(buffer, "exit"))break;				// stop program if exit entered
		if(!strcmp(buffer, ""))continue;			// Start a new enter wait
		if(!strcmp(buffer, "cd")){
			// Change the current dir
			callCD(cwd, Tokens);
			continue;			// Start a new enter wait
		}
	    int status;
		pid_t kidpid = fork();	 // we entered a command, fork
		if(kidpid){ //i Am  in the parent process?
			log("in Parent");
			// wait for child to exit
			if ( waitpid( kidpid, &status, 0 ) < 0 )
			{
				perror( "An error occured while waiting for the child process" );
				return -1;
			}

			if (WIFEXITED(status)) {
				log("Child exit status = " << WEXITSTATUS(status))
			}

			log("Child finished");
		}else if(kidpid == 0){ //  i am in the child process
			log("in Child");
			char** arg = new char*[Tokens.size()+1];
			char* buffer = new char[BUFFER_SIZE];
			for(unsigned int i = 0; i < Tokens.size(); i++){
				strcpy(buffer, Tokens[i].c_str());
				arg[i] = buffer;
				log(arg[i]);
			}
			arg[Tokens.size()] = NULL;

			string unExpandedCommandToken = Tokens[0];
			const char *command;
			string expansion = handleDotandTilde(Tokens[0], cwd);
			// There was an expansion
			if(expansion != ""){
				command = expansion.c_str();
			}
			else{
				command = unExpandedCommandToken.c_str();
			}

			// execute the command in Tokens
			log("about to execute command " << command )
			int status = execvp(command, arg);
			log("execvp failure has occured")
			printf("The command %s failed with\n %s", command,  strerror(status));
			// Say that we exited
			exit(EXIT_FAILURE);

		}else{ // some error must have occurred

			perror( "Internal error: could not fork process." );
			return -1;

		}



	}while(true);
	return 0;

}

/**
 * Exits if a syntax error occured
 * Returns "" if a no expansion was needed
 * or the expanded path and the command
 *
 */
string inline handleDotandTilde(const string commandToken, char cwd[]){
	// Check to see if it is a dotr case
	if (commandToken[0] == '.') {
		//Is the second character a dot
		if (commandToken[1] == '.') {
			// Do we have a slash
			if (commandToken[2] == '/') {
				// A command was invoked in
				// the manner of
				// ../PATH_TO_COMMAND
				// Get the the current working
				// directory
				string tmp = string(cwd);
				tmp += "/" + commandToken;
				return tmp;

			}else{
				// Improperly formed command call
				exit(EXIT_FAILURE);
			}
			// is the second char a /
		} else  {
			if(commandToken[1] == '/'){
				// A command was invoked in
				// the manner of
				// ./COMMAND
				// Get the the current working
				// directory
				string pwd = string(cwd);
				// strip the . away
				string sub = commandToken.substr(1);
				// returns the CWD/COMMAND, ie,
				// the full path, not nessicarly absolute
				return pwd + sub;
			}else{
				// Improperly formed command call
				exit(EXIT_FAILURE);
			}
		}
	} else if (commandToken[0] == '~') {

		if (commandToken[1] == '/') {
			// Strip the ~ off the command
			string sub = commandToken.substr(2);
			return tildeExpansion() + sub;
		} else {
			exit(EXIT_FAILURE);
		}
	}
	return "";
}

static inline string tildeExpansion(){
	return string(getenv("HOME")) + "/";
}

void callCD(char* cwd, vector<string> tokens){
	log("Changing the current dir");
	string newDir = (cwd);
	if(tokens.size() > 1){

		// it was change to root
		if(tokens[1][0] == '/')
			newDir = tokens[1];
		// it was a tilde expansion
		else if(tokens[1][0] == '~'){
			newDir = tildeExpansion() + tokens[1].substr(1);
		}
		else
			//
			newDir += "/" + tokens[1];
	}else{
		// no Directory was supplied, return to HOME
		newDir = getenv("HOME");

	}
	log("The directory to change from is " << newDir)
	// chdir return 0 on success
	if(chdir(newDir.c_str())){
		chdir(cwd);
	}

	//getcwd(cwd, sizeof(cwd));
	//printf("%s$ ", cwd);

}
