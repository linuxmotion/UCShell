/*
 * UCShellPipes.h
 *
 *  Created on: Feb 1, 2013
 *      Author: john
 */

#include "UCShell.h"

#ifndef UCSHELLPIPES_H_
#define UCSHELLPIPES_H_


using std::vector;
using std::string;


class UCShellPipes : public UCShell{
protected:

	bool handleRightCatRedirect(vector<string>& leftHandSide, vector<string>& rightHandSide);
	bool handleLeftRedirect(vector<string>& leftHandSide, vector<string>& rightHandSide);
	bool handleRightRedirect(vector<string>& leftHandSide, vector<string>& rightHandSide);
	bool handlePipe(vector<string>& leftHandSide, vector<string>& rightHandSide);


	virtual void startChildExecution(const vector<string>& Tokens);


public:
	UCShellPipes();

virtual ~UCShellPipes();



};

#endif /* UCSHELLPIPES_H_ */
