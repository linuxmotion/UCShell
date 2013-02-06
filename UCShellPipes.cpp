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


vector<vector<string> >  UCShellPipes::stripPipes(const vector<string>& Tokens){

	vector<string>::const_iterator bter = Tokens.begin();
	vector<string>::const_iterator eter = Tokens.end();

	vector<string> command;
	vector<vector<string> > Pipes;
	for(int i = 0; bter < eter; i++, bter++ ){

		if(Tokens[i] == "|"){
			Pipes.push_back(command);
			command.clear();
			continue;
		}
		//	log("Pushing command token " << Tokens[i])
		command.push_back(Tokens[i]);

	}
	// Dont forget the last command string, or the first if no |
	// is present
	Pipes.push_back(command);
	//log("The amount of pipes to connect is " << Pipes.size())

	return Pipes;
}

/**
 * Only allows one execution process.
 * If other work must be done, the the handlers must
 * create the needed process and initialize the streams.
 */
void UCShellPipes::startChildExecution(const vector<string>& Tokens){

	vector<string> leftHandSide;
	vector<string> rightHandSide;
	vector<string>::const_iterator bter = Tokens.begin();
	vector<string>::const_iterator eter = Tokens.end();

	vector<vector<string> > pipes = stripPipes(Tokens);


	for(int i = 0; bter < eter; i++, bter++ ){

		if(Tokens[i] == "2>"){
			log("2>");
			rightHandSide = vector<string>(++bter, eter);
			handleSTDErrRedirect(leftHandSide, rightHandSide);
			pipes[0] = leftHandSide;
			continue;
			//exit(EXIT_FAILURE);

		}

		if(Tokens[i] == ">>"){
			log(">>");
			rightHandSide = vector<string>(++bter, eter);
			handleRightCatRedirect(leftHandSide, rightHandSide);
			exit(EXIT_FAILURE);

		}
		if(Tokens[i] == "<"){
			log("Handling left redirect");
			rightHandSide = vector<string>(++bter, eter);
			handleLeftRedirect(leftHandSide, rightHandSide);
			pipes = stripPipes(Tokens);
			string tmp = Tokens[0];
			pipes[0].clear();
			pipes[0].push_back(tmp);
			continue;
		}
		if(Tokens[i] == ">"){
			log("Handling right redirect");
			rightHandSide = vector<string>(++bter, eter);
			handleRightRedirect(leftHandSide, rightHandSide);
			exit(EXIT_FAILURE);
		}
		if(Tokens[i] == "|"){
			log("Handling pipe redirect");
			handlePipe(pipes[0], pipes[1]);
			exit(EXIT_FAILURE);
		}

		log("Pushing back " << Tokens[i] << " for the left hand side")
		leftHandSide.push_back(Tokens[i]);
	}

	// If the program reached this point
	// Just execute the orignal function
	// Nothing else has been done
	log("Nothing special to execute")
	UCShell::startChildExecution(leftHandSide);

}

/**
 * Redirects STDOUT and concatenates the file in rightHandSide[0].
 * The function should not return, if it does then
 * redirection was unsuccessful
 */
bool UCShellPipes::handleSTDErrRedirect(vector<string>& leftHandSide,
		vector<string>& rightHandSide){
	//First, we're going to open a file
	log("Redirecting to " << rightHandSide[0])
	char * st = new char[BUFFER_SIZE];
	strcpy(st,mCurrentDir);
	strcat(st, "/");
	strcat(st, rightHandSide[0].c_str());
	if(!freopen(st, "w", stderr))
		return false;
	log("Opened error redirection pipe to " << st)


	log("Redirecting " << rightHandSide[0].c_str() )
	//Now standard error has been redirected, we can write to
	// the file
	return true;


}
/**
 * The function should never return
 */
bool UCShellPipes::handleLeftRedirect(vector<string>& leftHandSide,
		vector<string>& rightHandSide){


	log("Redirecting from " << rightHandSide[0] << " into " << leftHandSide[0] )
										char * st = new char[BUFFER_SIZE];
	SimpleGlibPipe myPipe;

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


#ifdef DEBUG
	for(unsigned int i = 0; i < rightHandSide.size(); i++){
		log("rightHandSide[" << i <<"] = " << rightHandSide[i])
	}
#ifdef DEBUG
	for(unsigned int i = 0; i < leftHandSide.size(); i++){
		log("leftHandSide[" << i <<"] = " << leftHandSide[i])
	}
#endif
#endif



	return true;

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
	if(file < 0)
		return false;

	log("Setting the output")
	//Now we redirect standard output to the file using dup2
	dup2(file,STDOUT_FILENO);

	//close(file,STDOUT_FILENO)

	log("Redirecting " << rightHandSide[0].c_str() )
	//Now standard out has been redirected, we can write to
	// the file
	executeSingleCommand(leftHandSide);
	return false;


}
/**
 * Redirects STDOUT to the file in rightHandSide[0].
 * The function should not return, if it does then
 * redirection was unsuccessful
 */
bool UCShellPipes::handleRightRedirect(vector<string>& leftHandSide,
		vector<string>& rightHandSide){

	//First, we're going to open a file
	log("Redirecting to " << rightHandSide[0])
									char * st = new char[BUFFER_SIZE];
	strcpy(st,mCurrentDir);
	strcat(st, "/");
	strcat(st, rightHandSide[0].c_str());
	int file = open(st, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	log("Opened redirection pipe to " << st)
	if(file < 0)
		return false;

	log("Setting the output")
	//Now we redirect standard output to the file using dup2
	dup2(file,STDOUT_FILENO);

	//close(file);

	log("Redirecting " << rightHandSide[0].c_str() )
	//Now standard out has been redirected, we can write to
	// the file
	executeSingleCommand(leftHandSide);
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


			vector<string>::const_iterator bter = Tokens.begin();
			vector<string>::const_iterator eter = Tokens.end();
			for(int i = 0; bter < eter; i++, bter++ ){

				if(Tokens[i] == ">>"){
					log(">>")
									rightHandSide = vector<string>(++bter, eter);
					handleRightCatRedirect(leftHandSide, rightHandSide);
					exit(EXIT_FAILURE);

				}else if(Tokens[i] == ">"){
					log(">")
											rightHandSide= vector<string>(++bter, eter);
					handleRightRedirect(leftHandSide, rightHandSide);
					exit(EXIT_FAILURE);
				}

			}

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

