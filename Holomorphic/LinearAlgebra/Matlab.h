#ifndef _DEBUG_TOOLS_H
#define _DEBUG_TOOLS_H

#include <stdio.h>
#include <string.h>

#ifndef NO_MATLAB
	#include "Engine.h"	// MatLab 6.5+ required
	// #include"I:\MATLAB6p5\extern\include\Engine.h"
#else
	typedef void* Engine;
	typedef void* mxArray;
#endif


bool InitMatlabEngine();
void CloseMatlabEngine();
Engine* GetEngine();
mxArray* _CreateDoubleMatrix(int nRow, int nCol, double* pData, char* szVarName);

typedef size_t mwIndex;

#endif
