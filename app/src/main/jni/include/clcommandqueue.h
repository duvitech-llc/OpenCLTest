/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/
#ifndef _CLCOMMANDQUEUE_H
#define _CLCOMMANDQUEUE_H
#include <clobject.h>
#include <clbuffer.h>
#include <clkernel.h>

class ClCommandQueue : public ClObject {
private:
	bool m_bTrace;
protected:
	cl_command_queue command_queue;
#ifdef _WITH_QEVENT
   cl_event m_cmdevent;
#endif //_WITH_QEVENT
public:
	ClCommandQueue() {
		m_bTrace = false;
	};
	ClCommandQueue(ClContext &context, ClPlatform &platform) {
		m_bTrace = false;
		open(context, platform);
	};
	~ClCommandQueue() {
		close();
	};
	void close() {
		if(!is_open()) {
			return;
		}
#ifdef _WITH_QEVENT
   	lastError = clReleaseEvent(m_cmdevent);
#endif //_WITH_QEVENT
		lastError = clFlush(command_queue);
		lastError = clFinish(command_queue);
		lastError = clReleaseCommandQueue(command_queue);
		TRACE("lastError = %d\n", set_close());
	};
	void open(ClContext &context, ClPlatform &platform) {
		if(is_open()) {
			return;
		}

#ifdef _FREESCALE
		//clCreateCommandQueue is deprecated
		command_queue = clCreateCommandQueue(context.get(), platform.get(), 0, &lastError);
#endif //_FREESCALE

#ifdef _INTEL
	//	command_queue = clCreateCommandQueueWithProperties(context.get(), platform.get(), 0, &lastError);
		command_queue = clCreateCommandQueue(context.get(), platform.get(), 0, &lastError);
#endif //_INTEL

#ifdef _NVIDIA
	//	command_queue = clCreateCommandQueueWithProperties(context.get(), platform.get(), 0, &lastError);
		command_queue = clCreateCommandQueue(context.get(), platform.get(), 0, &lastError);
#endif //_NVIDIA

		TRACE("lastError = %d\n", set_open());
	};
	cl_command_queue& get() {
		return command_queue;
	};
	inline void enqueueKernel(ClKernel &kernel) {
		size_t global_work_size[1] = {1}; //get_global_id
		size_t local_work_size[1] = {1}; //get_local_size()
		enqueueKernel(kernel, 1, global_work_size, local_work_size);
	};
	inline void enqueueKernel(ClKernel &kernel, int dimCount, const size_t *global, const size_t *local) {
		const size_t *global_work_offset = NULL; // must be NULL
		if(!is_open()) {
			return;
		}
		if(!kernel.is_open()) {
			return;
		}

		//TRACE("dimCount = %d\n", dimCount);
#if(0)
		lastError = clEnqueueTask(command_queue, kernel.get(), 0, NULL,NULL);
#else
		PERF_START("clEnqueueNDRangeKernel");
#ifdef _WITH_QEVENT
		lastError = clEnqueueNDRangeKernel(command_queue, kernel.get(), dimCount, 
				global_work_offset, global, local, 
				0, NULL, &m_cmdevent);
#else
		lastError = clEnqueueNDRangeKernel(command_queue, kernel.get(), dimCount, 
				global_work_offset, global, local, 
				0, NULL, NULL);
#endif //_WITH_QEVENT
		PERF_END("clEnqueueNDRangeKernel");
#endif
		if(CL_SUCCESS != lastError) {
			TRACE("lastError = %d\n", lastError);
		}
		//clFinish(command_queue);
	};
	size_t getWorkGroupInfo(ClContext &context, ClKernel &kernel) {
		size_t local;
		cl_device_id device_id = context.getId();
		size_t param_value_size_ret;

		PERF_START("clGetKernelWorkGroupInfo");
		lastError = clGetKernelWorkGroupInfo(kernel.get(), device_id, 
				CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), (void *)&local, &param_value_size_ret);
		PERF_END("clGetKernelWorkGroupInfo");
		if(CL_SUCCESS != lastError) {
			TRACE("lastError = %d\n", lastError);
		}

		//TRACE("local = %zu, device_id = %p, param_value_size_ret = %zu\n", local
		//		, device_id, param_value_size_ret);
		return local;
	};
	void write(ClBuffer &buffer, const void *val, size_t nsize) {
		if(!is_open()) {
			return;
		}
		if(!buffer.is_open()) {
			return;
		}

		PERF_START("clEnqueueWriteBuffer");
		lastError = clEnqueueWriteBuffer(command_queue, buffer.get(), CL_TRUE, 0,
				nsize, val, 0, NULL, NULL);
		PERF_END("clEnqueueWriteBuffer");
		if(CL_SUCCESS != lastError) {
			TRACE("lastError = %d\n", lastError);
		}
	};
	void read(ClBuffer &buffer, void *string, size_t nsize) {
		if(!is_open()) {
			return;
		}
		if(!buffer.is_open()) {
			return;
		}

		PERF_START("clEnqueueReadBuffer");
		lastError = clEnqueueReadBuffer(command_queue, buffer.get(), CL_TRUE, 0,
				nsize, string, 0, NULL, NULL);
		PERF_END("clEnqueueReadBuffer");
		if(CL_SUCCESS != lastError) {
			TRACE("lastError = %d\n", lastError);
		}
	};
	void trace(bool onoff) {
		m_bTrace = onoff;
	};
	void *pin(ClBuffer &buffer, size_t nsize) {
		void *host_ptr = clEnqueueMapBuffer(command_queue, buffer.get(), 
				CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 
				0, nsize, 0, NULL, NULL, &lastError); 
		if(CL_SUCCESS != lastError) {
			TRACE("lastError = %d\n", lastError);
		}
		return host_ptr;
	};
	//clEnqueueCopyBufferToImage, clEnqueueCopyImageToBuffer
	//clEnqueueMapImage, clEnqueueUnmapMemObject
	void finish() {
#ifdef _WITH_QEVENT
   	cl_ulong time_start;
		clFinish(command_queue);
		clGetEventProfilingInfo(m_cmdevent, CL_PROFILING_COMMAND_START,
				sizeof(time_start), &time_start, NULL);
#endif //_WITH_QEVENT
	}
};


#endif // _CLCOMMANDQUEUE_H

