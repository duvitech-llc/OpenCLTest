/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/
#ifndef _CLKERNEL_H
#define _CLKERNEL_H
#include <clobject.h>
#include <clprogram.h>
#include <climage.h>
//#include <clbuffer.h>

class ClKernel : public ClObject {
private:
protected:
	cl_kernel kernel;
public:
	ClKernel() {
	};
	ClKernel(ClProgram &program) {
		open(program);
	};
	~ClKernel() {
		close();
	};
	void close() {
		if(!is_open()) {
			return;
		}
		lastError = clReleaseKernel(kernel);
		TRACE("lastError = %d\n", set_close());
	};
	void open(ClProgram &program) {	 
		if(is_open()) {
			return;
		}
		if(!program.is_open()) {
			return;
		}
		/* Create OpenCL Kernel */
		kernel = clCreateKernel(program.get(), program.name(), &lastError);
		TRACE("lastError = %d\n", set_open());
	};
	int arg(ClBuffer &buffer, int index) {	 
		if(!is_open()) {
			return -1;
		}
		if(!buffer.is_open()) {
			return -1;
		}
		buffer.index(index);
		cl_mem memobj = buffer.get();
		// Set OpenCL Kernel Parameters 
		lastError = clSetKernelArg(kernel, index, buffer.size(), (void *)&memobj);
		if(CL_SUCCESS != lastError) {
			TRACE("lastError = %d, index = %d\n", lastError, index);
		}
		return index + 1;
	};
	int arg(unsigned int &buffer, int index) {	 
		if(!is_open()) {
			return -1;
		}
		// Set OpenCL Kernel Parameters 
		lastError = clSetKernelArg(kernel, index, sizeof(unsigned int), (void *)&buffer);
		if(CL_SUCCESS != lastError) {
			TRACE("lastError = %d, index = %d\n", lastError, index);
		}
		return index + 1;
	};
	int arg(float &buffer, int index) {	 
		if(!is_open()) {
			return -1;
		}
		// Set OpenCL Kernel Parameters 
		lastError = clSetKernelArg(kernel, index, sizeof(float), (void *)&buffer);
		if(CL_SUCCESS != lastError) {
			TRACE("lastError = %d, index = %d\n", lastError, index);
		}
		return index + 1;
	};
	cl_kernel& get() {
		return kernel;
	};
};

#endif // _CLKERNEL_H
