/*
 * UCShell.cpp
 *
 *  Created on: Feb 1, 2013
 *      Author: john
 */

#include "UCShell.h"
#include <sys/wait.h>
#include <vector>
#include <stdlib.h>
#include <cstdio>

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;

UCShell::UCShell() {
	log("Shell object created");
	mInputBuffer = new char[BUFFER_SIZE];

}



/**
 * Executes a single command using execvp().
 * Handles the tilde and dot expansion.
 * Exits with EXIT_FAILURE if execvp fails
 *
 */
void UCShell::singleExecution(const vector<string>& Tokens, char* cwd) {
	char** arg = stripArgs(Tokens);
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
	log("Executing command " << command  << " at " __TIME__)
	int status = execvp(command, arg);
	log("execvp failure has occured")
	printf("The command %s failed with\n %s", command,  strerror(status));
	// Say that we exited
	exit(EXIT_FAILURE);
}

/**
 * Extracts the global tokens when the shell starts
 */
void inline UCShell::extractGlobalTokens(int argc, char* argv[]) {
	for (int i = 0; i < argc; i++) {
		mGlobalTokens.push_back(argv[i]);
	}
}

/**
 * Extracts the tokens from the inputbuffer buffer
 * and places each into a vector. Tokens are
 * determined by spaces
 */
void UCShell::extractLocalTokens(char* buffer, vector<string>& Tokens) {
	// Break it up by tokens
	char* prog = strtok(buffer, " ");
	// Put all the tokens into a vector

	while (prog != NULL) {
		Tokens.push_back(prog);
		log(prog)
		// stop taking tokens if a & is encountered
		if(Tokens[Tokens.size()-1] == "&"){
			break;
		}

		prog = strtok(NULL, " ");
	}
}

void UCShell::executeSingleCommand(const vector<string>& Tokens) {
	char** arg = UCShell::stripArgs(Tokens);
	string unExpandedCommandToken = Tokens[0];
	const char* command;
	string expansion = handleDotandTilde(Tokens[0], mCurrentDir);
	// There was an expansion
	if (expansion != "") {
		command = expansion.c_str();
	} else {
		command = unExpandedCommandToken.c_str();
	}
	// execute the command in Tokens
	log("Executing command " << command << " at " __TIME__)
	int status = execvp(command, arg);
	log("execvp failure has occured")
	printf("The command %s failed with\n %s", command, strerror(status));
	// Say that we exited
	exit(EXIT_FAILURE);
}

/**
 * Finishes execution of the parent process
 * after it has been forked. If Tokes contains
 * an & the the parent process will not wait.
 * Return true on success and false on failure
 *
 */
bool UCShell::parentExecutionAfterFork(pid_t kidpid, vector<string> Tokens, bool wait) {
	log("Parent execution after fork started");
	/**
	 * if somewhere there is a &
	 * ie. command -flags &
	 * the the parent should not wait on the child
	 *
	 */
	//wait = true;
	if(wait){
		log("Waiting on child process");
		int status;
		// wait for child to exit
		if (waitpid(kidpid, &status, 0) < 0) {
			log("An error occured while waiting for the child process");
			perror("An error occured while waiting for the child process");
			return false;
		}
		if (WIFEXITED(status)) {
			log("Child exit status = " << WEXITSTATUS(status))
		}
	}
	else{
		log("Not waiting on child execution")

	}
	log("Parent execution after fork finished successfully");

	return true;
}

void UCShell::startChildExecution(const vector<string>& Tokens) {
	//  i am in the child process
	log("Child process started at " << __TIME__);
	singleExecution(Tokens, mCurrentDir);
}

/**
 * Start the main execution of the program.
 * Fork will be called and both the parents'
 * and child executions will be called
 */
bool UCShell::startExecution(const vector<string>& Tokens) {

	bool wait = true;
	if(Tokens[Tokens.size()-1] == "&"){
		wait = false;

	}
	pid_t kidpid = fork(); // we entered a command, fork
	if (kidpid) {
		if(!parentExecutionAfterFork(kidpid, Tokens, wait))
			return false;
	} else if (kidpid == 0) {
		//  I am in the child process
		// This function never returns
		// Rather it will exit
		startChildExecution(Tokens);
	} else {
		// some error must have occurred
		perror("Internal error: could not fork process.");
		return false;
	}

	return true;
}

/**
 * Present the command prompt to the user
 */
void UCShell::presentCommandPrompt(vector<string>& Tokens) {
	// get the current dir
	getcwd(mCurrentDir, sizeof(mCurrentDir));
	// clear all the tokens from the last input
	Tokens.clear();
	// Print the command line
	printf("%s$ ", mCurrentDir);
	//Get the line that was entered
	cin.getline(mInputBuffer, BUFFER_SIZE);
	// clean stdin
	cin.clear();
}

char **UCShell::stripArgs(vector<string> Tokens){
		char **arg = new char*[Tokens.size() + 1];
		char* buffer = new char[BUFFER_SIZE];
		int size = (Tokens[Tokens.size()-1] == "&")?(Tokens.size()-1):Tokens.size();
		for(unsigned int i = 0;i < size; i++){
			strcpy(buffer, Tokens[i].c_str());
			arg[i] = buffer;
			log(arg[i]);
		}
		arg[Tokens.size()] = NULL;
		return arg;
	}

// Starts the shell
int UCShell::StartShell(int argc, char* argv[]) {

	extractGlobalTokens(argc, argv);

	vector<string> Tokens;
	do{
		// get the current dir
		presentCommandPrompt(Tokens);
		// Break it up by tokens
		extractLocalTokens(mInputBuffer, Tokens);

		if(!strcmp(mInputBuffer, "exit"))break;				// stop program if exit entered
		if(!strcmp(mInputBuffer, ""))continue;			// Start a new enter wait
		if(!strcmp(mInputBuffer, "cd")){
			// Change the current dir
			callCD(mCurrentDir, Tokens);
			continue;			// Start a new enter wait
		}

		if(!startExecution(Tokens))
			return -EXIT_FAILURE;

	}while(true);

	return 0;

}


/**
 * Exits if a syntax error occured
 * Returns "" if a no expansion was needed
 * or the expanded path and the command
 *
 */
string inline UCShell::handleDotandTilde(const string commandToken, char cwd[]){
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

string UCShell::tildeExpansion(){
	return string(getenv("HOME")) + "/";
}

void UCShell::callCD(char* cwd, vector<string> tokens){
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


UCShell::~UCShell() {
	log("Shell object being destroyed");
	// Free the input buffer memory
	delete mInputBuffer;
	mInputBuffer = NULL;

}

