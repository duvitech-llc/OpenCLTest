/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/
#ifndef _CLHOST_H
#define _CLHOST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>

#include <clplatform.h>
#include <clcontext.h>
#include <clcommandqueue.h>
#include <clprogram.h>
#include <clkernel.h>
#include <clbuffer.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
 
class ClHost 
{
private:
protected:
public:
	ClPlatform *platform;
	ClContext *context;
	ClCommandQueue *command_queue;
	ClHost(cl_device_type type) {
		platform = new ClPlatform(type);
		context = new ClContext(*platform);
		command_queue = new ClCommandQueue(*context, *platform);
	};
	~ClHost() {
		delete platform;
		delete context;
		delete command_queue;
	};
	ClContext& get() {
		return *context;
	};
};


#endif // _CLHOST_H

