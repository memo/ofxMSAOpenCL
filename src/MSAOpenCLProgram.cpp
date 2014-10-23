#include "MSAOpenCL.h"
#include "MSAOpenCLProgram.h"
#include "MSAOpenCLKernel.h"

namespace msa { 
	
	char *OpenCL_textFileRead(char *fn);
	
	
	OpenCLProgram::OpenCLProgram() {
		ofLog(OF_LOG_VERBOSE, "OpenCLProgram::OpenCLProgram");
		pOpenCL = NULL;
		clProgram = NULL;
	}
	
	
	OpenCLProgram::~OpenCLProgram() {
		ofLog(OF_LOG_VERBOSE, "OpenCLProgram::~OpenCLProgram");
		pOpenCL->finish();	// tig: make sure all CL ops have ended before dropping the program.
		if (clProgram != NULL) clReleaseProgram(clProgram);
	}
	
	
	void OpenCLProgram::loadFromFile(std::string filename, bool isBinary) { 
		ofLog(OF_LOG_VERBOSE, "OpenCLProgram::loadFromFile " + filename + ", isBinary: " + ofToString(isBinary));
		
		string fullPath = ofToDataPath(filename.c_str());
		
		if(isBinary) {
			//		clCreateProgramWithBinary
			ofLog(OF_LOG_ERROR, "Binary programs not implemented yet\n");
			assert(false);
			
		} else {
			
			char *source = OpenCL_textFileRead((char*)fullPath.c_str());
			if(source == NULL) {
				ofLog(OF_LOG_ERROR, "Error loading program file: " + fullPath);
			}
			
			loadFromSource(source);
			
			free(source);
		}
	}
	
	
	
	void OpenCLProgram::loadFromSource(std::string source) {
		ofLog(OF_LOG_VERBOSE, "OpenCLProgram::loadFromSource ");// + source);
		
		cl_int err;
		
		pOpenCL = OpenCL::currentOpenCL;
		
		const char* csource = source.c_str();
		clProgram = clCreateProgramWithSource(pOpenCL->getContext(), 1, &csource, NULL, &err);
		


		build();
	} 
	
	
	OpenCLKernelPtr OpenCLProgram::loadKernel(string kernelName) {
		ofLog(OF_LOG_VERBOSE, "OpenCLProgram::loadKernel " + kernelName);
		assert(clProgram);
		
		cl_int err;
		
		OpenCLKernelPtr k = OpenCLKernelPtr(new OpenCLKernel(pOpenCL, clCreateKernel(clProgram, kernelName.c_str(), &err), kernelName));
		
		if(err != CL_SUCCESS) {
			ofLog(OF_LOG_ERROR, string("Error creating kernel: ") + kernelName);
//			assert(false);
		}
		
		return k;
	}
	
	
	void OpenCLProgram::getBinary()
	{
		cl_uint program_num_devices;
		cl_int err;
		err = clGetProgramInfo(clProgram, CL_PROGRAM_NUM_DEVICES, sizeof(cl_uint), &program_num_devices, NULL);
		assert(err == CL_SUCCESS);
		
		if (program_num_devices == 0) {
			std::cerr << "no valid binary was found" << std::endl;
			return;
		}
		
		vector<size_t> binaries_sizes(program_num_devices);
		
		err = clGetProgramInfo(clProgram, CL_PROGRAM_BINARY_SIZES, program_num_devices*sizeof(size_t), binaries_sizes.data(), NULL);
		assert(err = CL_SUCCESS);
		
		char **binaries = new char*[program_num_devices];
		
		for (size_t i = 0; i < program_num_devices; i++)
			binaries[i] = new char[binaries_sizes[i]+1];
		
		err = clGetProgramInfo(clProgram, CL_PROGRAM_BINARIES, program_num_devices*sizeof(size_t), binaries, NULL);
		assert(err = CL_SUCCESS);
		
		for (size_t i = 0; i < program_num_devices; i++) {
			binaries[i][binaries_sizes[i]] = '\0';
			std::cout << "Program " << i << ":" << std::endl;
			std::cout << binaries[i];
		}
		
		for (size_t i = 0; i < program_num_devices; i++)
			delete [] binaries[i];
		
		delete [] binaries;
	}
	
	
	void OpenCLProgram::build() {
		if(clProgram == NULL) {
			ofLog(OF_LOG_ERROR, "Error creating program object.");
			assert(false); 
		}	
		
		string Options;
		Options += "-I \"" + ofToDataPath("") + "\" ";
		cl_int err = clBuildProgram(clProgram, 0, NULL, Options.c_str(), NULL, NULL);
		if(err != CL_SUCCESS) {
			ofLog(OF_LOG_ERROR, "\n\n ***** Error building program. ***** \n ***********************************\n\n");
			//	get build log size first so we always have errors to display 
			size_t len = 0;
			int BuildInfoErr = clGetProgramBuildInfo(clProgram, pOpenCL->getDevice(), CL_PROGRAM_BUILD_LOG, 0, NULL, &len );
			vector<char> buffer( len+1 );
			BuildInfoErr = clGetProgramBuildInfo(clProgram, pOpenCL->getDevice(), CL_PROGRAM_BUILD_LOG, buffer.size(), &buffer.at(0), NULL );
			buffer[len] = '\0';
			
			const char* bufferString = &buffer[0];
			ofLog(OF_LOG_ERROR, bufferString );
//			assert(false);
		}
	}
	
	cl_program& OpenCLProgram::getCLProgram(){
		return clProgram;	
	}
	
	//---------------------------------------------------------
	// below is from: www.lighthouse3d.com
	// you may use these functions freely. they are provided as is, and no warranties, either implicit, or explicit are given
	//---------------------------------------------------------
	
	char *OpenCL_textFileRead(char *fn) {
		
		FILE *fp;
		char *content 	= 	NULL;
		int count		=	0;
		
		if (fn != NULL) {
			fp = fopen(fn,"rt");
			if (fp != NULL) {
				
				fseek(fp, 0, SEEK_END);
				count = ftell(fp);
				rewind(fp);
				
				if (count > 0) {
					content = (char *)malloc(sizeof(char) * (count+1));
					count = fread(content,sizeof(char),count,fp);
					content[count] = '\0';
				}
				fclose(fp);
			}
		}
		
		return content;
	}
}
