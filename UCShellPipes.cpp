/*
 * UCShellPipes.cpp
 *
 *  Created on: Feb 1, 2013
 *      Author: john
 */

#include "UCShellPipes.h"
#include "SimpleGLibPipe.h"
#include <cstdio>
#include <fcntl.h>
#include <stdlib.h>

UCShellPipes::UCShellPipes() {

}



void UCShellPipes::startChildExecution(const vector<string>& Tokens){

	vector<string> leftHandSide;
	vector<string> rightHandSide;
	vector<string>::const_iterator bter = Tokens.begin();
	vector<string>::const_iterator eter = Tokens.end();

	for(int i = 0; bter < eter; i++, bter++ ){


		if(Tokens[i] == ">>"){
			log(">>")
					rightHandSide = vector<string>(++bter, eter);
			handleRightCatRedirect(leftHandSide, rightHandSide);
			exit(EXIT_FAILURE);

		}
		if(Tokens[i] == "<"){
			log("<")
					rightHandSide = vector<string>(++bter, eter);
			handleLeftRedirect(leftHandSide, rightHandSide);
			exit(EXIT_FAILURE);
		}
		if(Tokens[i] == ">"){
			log(">")
					rightHandSide= vector<string>(++bter, eter);
			handleRightRedirect(leftHandSide, rightHandSide);
			exit(EXIT_FAILURE);
		}
		if(Tokens[i] == "|"){
			rightHandSide = vector<string>(++bter, eter);
			handlePipe(leftHandSide, rightHandSide);
			exit(EXIT_FAILURE);
		}

		leftHandSide.push_back(Tokens[i]);
	}

	// If the program reached this point
	// Just execute the orignal function
	// Nothing else has been done
	UCShell::startChildExecution(Tokens);

}

/**
 * The function should never return
 */
bool UCShellPipes::handleLeftRedirect(vector<string>& leftHandSide,
		vector<string>& rightHandSide){
	log("Redirecting from " << rightHandSide[0] << " into " << leftHandSide[0] )
				char * st = new char[BUFFER_SIZE];
	strcpy(st,mCurrentDir);
	strcat(st, "/");
	strcat(st, rightHandSide[0].c_str());
	int file = open(st, O_RDONLY);
	log("Opened redirection pipe to " << st)
	if(file < 0){
		perror("I/O Error: Failed to get a valid file descriptor");
		return false;
	}


	log("Connecting the input stream to " << leftHandSide[0])
	//Now we redirect standard output to the file using dup2
	dup2(file,STDIN_FILENO);

	//close(file,STDOUT_FILENO)

	log("Redirecting the contents of " << rightHandSide[0].c_str() )
	//Now standard in has been redirected, we can
	// execute the program
	vector<string>Tokens = leftHandSide;
	executeSingleCommand(Tokens);

	return false;

}
/**
 * Redirects STDOUT and concatenates the file in rightHandSide[0].
 * The function should not return, if it does then
 * redirection was unsuccessful
 */
bool UCShellPipes::handleRightCatRedirect(vector<string>& leftHandSide,
		vector<string>& rightHandSide){
	//First, we're going to open a file
	log("Redirecting to " << rightHandSide[0])
			char * st = new char[BUFFER_SIZE];
	strcpy(st,mCurrentDir);
	strcat(st, "/");
	strcat(st, rightHandSide[0].c_str());
	int file = open(st, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
	log("Opened redirection pipe to " << st)
	//if(file < 0)
	//	return 1;

	log("Setting the output")
	//Now we redirect standard output to the file using dup2
	dup2(file,STDOUT_FILENO);

	//close(file,STDOUT_FILENO)

	log("Redirecting " << rightHandSide[0].c_str() )
	//Now standard out has been redirected, we can write to
	// the file
	vector<string>Tokens = leftHandSide;
	executeSingleCommand(Tokens);
	return false;


}
/**
 * Redirects STDOUT to the file in rightHandSide[0].
 * The function should not return, if it does then
 * redirection was unsuccessful
 */
bool UCShellPipes::handleRightRedirect(vector<string>& leftHandSide,
		vector<string>& rightHandSide){

	//int Pipes[2];
	//pipe(Pipes);
	//First, we're going to open a file
	log("Redirecting to " << rightHandSide[0])
			char * st = new char[BUFFER_SIZE];
	strcpy(st,mCurrentDir);
	strcat(st, "/");
	strcat(st, rightHandSide[0].c_str());
	int file = open(st, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	log("Opened redirection pipe to " << st)
	//if(file < 0)
	//	return 1;

	log("Setting the output")
	//Now we redirect standard output to the file using dup2
	dup2(file,STDOUT_FILENO);

	//close(file,STDOUT_FILENO)

	log("Redirecting " << rightHandSide[0].c_str() )
	//Now standard out has been redirected, we can write to
	// the file
	vector<string>Tokens = leftHandSide;
	executeSingleCommand(Tokens);
	return false;

}

bool UCShellPipes::handlePipe(vector<string>& leftHandSide,
		vector<string>& rightHandSide){

	try {


		log("creating a pipe for " << leftHandSide[0] << " | " << rightHandSide[0]);
		SimpleGlibPipe myPipe;

		pid_t kidpid = fork(); // we entered a command, fork
		if (kidpid) {
			log("Connecting to pipe for " << leftHandSide[0]);
			myPipe.setwritePipe(STDOUT_FILENO);
			myPipe.closeReadPipe();
			vector<string>Tokens = leftHandSide;
			executeSingleCommand(Tokens);
		} else if (kidpid == 0) {

			log("Connecting from pipe for " << rightHandSide[0]);

			myPipe.setReadPipe(STDIN_FILENO);
			myPipe.closeWritePipe();
			vector<string>Tokens = rightHandSide;
			executeSingleCommand(Tokens);

		} else {
			// some error must have occurred
			perror("Internal error: could not fork process.");
			log("Error creating process");

		}




	} catch (int ex) {
		perror("Internal error: could not create pipe.");

	}

	return false;


}


UCShellPipes::~UCShellPipes() {
}

