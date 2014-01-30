#pragma once

#include "ofMain.h"
#ifdef __APPLE__
	#include <OpenCL/Opencl.h>
#else
	#include <CL/opencl.h>
#endif


namespace msa { 
	
	class OpenCL;
	class OpenCLKernel;
	
	class OpenCLProgram {
	public:
		OpenCLProgram();
		~OpenCLProgram();
		
		void loadFromFile(string filename, bool isBinary = false);
		void loadFromSource(string source);
		
		std::shared_ptr<OpenCLKernel> loadKernel(string kernelName);
		
		void getBinary();
		
		cl_program& getCLProgram();
		
	protected:	
		OpenCL*		pOpenCL;
		cl_program		clProgram;
		
		void			build();
		
	};
	
}