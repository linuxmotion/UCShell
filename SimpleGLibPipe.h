/*
 * SimpleGLibPipe.h
 *
 *  Created on: Feb 1, 2013
 *      Author: john
 */

#ifndef SIMPLEGLIBPIPE_H_
#define SIMPLEGLIBPIPE_H_


#include <unistd.h>
#include <cstdio>

class SimpleGlibPipe{

private:
	int mPipes[2];
	int mOpenPipes[2];

public:
	SimpleGlibPipe() throw(int){
		openPipe();
	}

	void openPipe() throw (int){

		// Open a pipe
		int status = pipe(mPipes);
		//
		if(status == -1)
			throw status;
		// Set a status flag so that
		// we dont double close
		mOpenPipes[0] = 1;
		mOpenPipes[1] = 1;
	}



	int getWritePipe(){
		return mPipes[1];
	}

	int getReadPipe(){
		return mPipes[0];
	}


	void writeToPipe(){


	}

	void readFromPipe(){



	}

	void reopenPipe() throw(int){
		closeWritePipe();
		closeReadPipe();
		openPipe();

	}




	void closeWritePipe(){

		if(mOpenPipes[1]){
			close(mPipes[1]);
			mOpenPipes[1] = 0;
		}
	}

	void closeReadPipe(){
		if(mOpenPipes[0]){
			close(mPipes[0]);
			mOpenPipes[0] = 0;
		}
	}

	~SimpleGlibPipe(){
		closeWritePipe();
		closeReadPipe();

	}


};





#endif /* SIMPLEGLIBPIPE_H_ */
