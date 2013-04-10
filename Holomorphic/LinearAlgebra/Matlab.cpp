#include "Matlab.h"

Engine* gpMLEngine = NULL;
FILE* gDebugLogFile = NULL;
bool gEnableLineDebugger = false;



bool InitMatlabEngine()
{
#ifdef NO_MATLAB
	assert(false);		// build without Matlab supports!

	return (false);
#else
	if (gpMLEngine)
		return (true);	// no need

	gpMLEngine = engOpen(NULL);	
	if (gpMLEngine == NULL)
	{
		printf("Starting Engine Fails");
		return(false);
	}

	return (true);
#endif
}

void CloseMatlabEngine()
{
#ifdef NO_MATLAB
	assert(false);		// build without Matlab supports!

	return;
#else
	if (gpMLEngine)
		engClose(gpMLEngine);
#endif
}

Engine* GetEngine()
{
	return (gpMLEngine);
}


mxArray* _CreateDoubleMatrix(int nRow, int nCol, double* pData, char * szVarName)
{
	if (!InitMatlabEngine())
		return NULL;
#ifndef NO_MATLAB
	Engine* ep = GetEngine();

	mxArray* pArr = mxCreateDoubleMatrix(nRow, nCol, mxREAL);
	double* xr = mxGetPr(pArr);
	memcpy(xr, pData, sizeof(double) * nRow * nCol);
	engPutVariable(ep, szVarName, pArr);
	return pArr;	
#else
	return NULL;
#endif
}

