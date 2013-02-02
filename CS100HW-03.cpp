//============================================================================
// Name        : CS100HW-03.cpp
// Author      : 
// Version     :
// Copyright   :
// Description : UCShell, ashell program that behaves like a subset of
//				 bash, the standard Linux shell.
//============================================================================
#include "UCShellPipes.h"


int main(int argc, char* argv[], char* envp[]) {

	UCShellPipes mShell;

	return mShell.StartShell(argc, argv);

	return 0;

}

