/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/
#ifndef _CLCONTEXT_H
#define _CLCONTEXT_H

#include <clobject.h>

class ClContext : public ClObject {
private:
protected:
	cl_device_id device_id;
public:
	cl_context context;
	ClContext() {
		clear();
	};
	ClContext(ClPlatform &platform) {
		clear();
		open(platform);
	};
	~ClContext() {
		close();
	};
	void close() {
		if(!is_open()) {
			return;
		}
		lastError = clReleaseContext(context);
		set_close();
	};
	void open(ClPlatform &platform) {
		if(is_open()) {
			return;
		}
		if(!platform.is_open()) {
			return;
		}

		device_id = platform.get();
		context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &lastError);
		TRACE("lastError = %d\n", set_open());
	};
	void clear() {
		device_id = 0;
		context = 0;
	};
	cl_context& get() {
		return context;
	};
	cl_device_id getId() {
		return device_id;
	};
#ifndef _NVIDIA
	void getEvent(cl_event &evt) {
		if(!is_open()) {
			return;
		}
		evt = clCreateUserEvent(context, &lastError);
		if(CL_SUCCESS != lastError) {
			TRACE("lastError = %d\n", lastError);
		}
	};
#endif //_NVIDIA
	cl_sampler createSampler(cl_bool normalized_coords, cl_addressing_mode addressing_mode
			, cl_filter_mode filter_mode) {
		cl_sampler ret = clCreateSampler(context, normalized_coords, addressing_mode, filter_mode, &lastError);
		if(CL_SUCCESS != lastError) {
			TRACE("lastError = %d\n", lastError);
		}
		return ret;
	};
};


#endif // _CLCONTEXT_H

