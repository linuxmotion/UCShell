/*
 * UCShell.h
 *
 *  Created on: Feb 1, 2013
 *      Author: john
 */

#ifndef UCSHELL_H_
#define UCSHELL_H_

#include "UCShell.h"
#include "SimpleGLibPipe.h"
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

// Comment this out to disable debuging
#define DEBUG

#ifdef DEBUG
#define log(String) \
		cout << __FILE__ << "::"<< __FUNCTION__ << "::"<< __LINE__ << " " << String << endl;
#else
#define log(String)
#endif




class UCShell {

private:
	vector<string> mGlobalTokens;
	char *mInputBuffer;

	void extractGlobalTokens(int argc, char* argv[]);
	void extractLocalTokens(char* buffer, vector<string>& Tokens);
	void presentCommandPrompt(vector<string>& Tokens);
	// Execution functions
	bool startExecution(const vector<string>& Tokens);
	void singleExecution(const vector<string>& Tokens, char* cwd);
	bool parentExecutionAfterFork(pid_t kidpid, vector<string> Tokens, bool wait);

protected:
	void executeSingleCommand(const vector<string>& Tokens);
	char mCurrentDir[1024];
	static const int BUFFER_SIZE = 288;
	string handleDotandTilde(const string commandToken, char cwd[]);
	string inline tildeExpansion();
	void callCD(char* cwd, vector<string> tokens);
	char **stripArgs(vector<string> Tokens);
	virtual void startChildExecution(const vector<string>& Tokens);

public:
	UCShell();
	int StartShell(int argc, char* argv[]);

	virtual ~UCShell();
};

#endif /* UCSHELL_H_ */
