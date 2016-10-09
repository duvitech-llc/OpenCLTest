/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/
#ifndef _CLPROGRAM_H
#define _CLPROGRAM_H
#include <clobject.h>
#include <clcontext.h>

class ClProgram : public ClObject {
private:
protected:
	size_t source_size;
	char *source_str;
	const char *program_name;
	cl_program program;
public:
	ClProgram() : ClObject() {
		clear();
	};
	ClProgram(ClContext &context, const char *fileName, const char *progname) : ClObject() {
		clear();
		char *src = load(fileName);
		open(context, src);
		program_name = progname;
		TRACE("kernelname = %s\n", program_name);
	};
	~ClProgram() {
		close();
	};
	char *load(const char *fileName) {
		/* Load the source code containing the kernel*/
		FILE *fp;
		fp = fopen(fileName, "r");
		if (!fp) {
			TRACE("Failed to load kernel source.\n");
			exit(1);
		}

		source_str = (char*)malloc(MAX_SOURCE_SIZE);
		source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
		fclose(fp);
		INFO("Load Source code (size=%zd)\n", source_size);
		//TRACE("src = %s\n", source_str);
		return source_str;
	};
	const char *name(const char *progname = 0) {
		if(progname != 0) {
			program_name = progname;
		}
		return program_name;
	};
	void clear() {
		source_str = NULL;
		source_size = 0;
		program_name = 0;
	};
	void close() {
		if(!is_open()) {
			return;
		}
		lastError = clReleaseProgram(program);
		if(source_str) {
			free(source_str);
		}
		clear();
		TRACE("lastError = %d\n", set_close());
	};
	void open(ClContext &context, const char *source_str) {
		if(is_open()) {
			return;
		}
		if(!context.is_open()) {
			return;
		}
		//TRACE("source_str = %s\n", source_str);	
		PERF_START("clCreateProgramWithSource");
		/* Create Kernel Program from the source */
		program = clCreateProgramWithSource(context.get(), 1, (const char **)&source_str,
				(const size_t *)&source_size, &lastError);
		PERF_END("clCreateProgramWithSource");
		TRACE("lastError = %d\n", set_open());
	};
	bool build(ClPlatform &platform) {
		if(!is_open()) {
			return false;
		}
		if(!platform.is_open()) {
			return false;
		}

#ifdef _FREESCALE
		const char *options = " -D_FREESCALE";
#endif //_FREESCALE

#ifdef _INTEL
		const char *options = " -D_INTEL -cl-mad-enable";
#endif //_FREESCALE

#ifdef _QUALCOMM
		const char *options = " -D_QUALCOMM -cl-mad-enable";
#endif //_FREESCALE

		void *user_data = NULL;
		cl_device_id device_id = platform.get();

		PERF_START("clBuildProgram");
		lastError = clBuildProgram(program, 1, &device_id, options, NULL, user_data);
		PERF_END("clBuildProgram");

		if(CL_SUCCESS != lastError) {
			TRACE("lastError = %d\n", lastError);
		}
		if(lastError == CL_SUCCESS) {
			return true;
		}

		char buildLog[16384];
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
				sizeof(buildLog), buildLog, NULL);
		TRACE("%s\n", buildLog);
		return false;
	};
	cl_program& get() {
		return program;
	};
};
#endif // _CLPROGRAM_H

