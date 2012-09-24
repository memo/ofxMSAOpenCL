#pragma once

#include "ofMain.h"
#include <OpenCL/Opencl.h>

namespace msa { 
	
	class OpenCL;
	class OpenCLKernel;
	
	class OpenCLProgram {
	public:
		OpenCLProgram();
		~OpenCLProgram();
		
		void loadFromFile(string filename, bool isBinary = false);
		void loadFromSource(string source);
		
		OpenCLKernel* loadKernel(string kernelName);
		
		void getBinary();
		
		cl_program& getCLProgram();
		
	protected:	
		OpenCL*		pOpenCL;
		cl_program		clProgram;
		
		void			build();
		
	};
	
}