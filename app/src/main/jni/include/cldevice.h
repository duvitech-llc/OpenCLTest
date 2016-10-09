/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/
#ifndef _CLDEVICE_H
#define _CLDEVICE_H

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
#include <clhost.h>
#include <climage.h>
#include <clobject.h>
#include <clframe.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
 
class ClDevice : public ClObject
{
private:
	int m_index;
protected:
	std::map<int, ClBuffer*> mapBuffer;
public:
	ClProgram *program;
	ClKernel *kernel;
	ClHost *m_context;
	ClDevice(ClHost *context) {
		clear();
		set_close();
		m_context = context;
	};
	~ClDevice() {
		close();
	};
	void close() {
		delete program;
		delete kernel;
		set_close();
		clear();
	};
	bool open(const char *fileName, const char *kernelName) {
		TRACE("%s\n", "create");
		program = new ClProgram(*m_context->context, fileName, kernelName);
		bool ret = program->build(*m_context->platform);
		kernel = new ClKernel(*program);
		if(ret == false) {
			TRACE("create failed for program kernel %s\n", kernelName);
			return false;
		}
		set_open();
		return true;
	};
	void clear() {
		m_index = 0;
		program = NULL;
		kernel = NULL;
		m_context = NULL;
	};
	ClContext& context() {
		return m_context->get();
	}
	void arg() {
		while(m_index--) {
			ClBuffer *buffer = mapBuffer[m_index];
			buffer->release();
		}
		m_index = 0;
		mapBuffer.clear();
	}
	void arg(ClBuffer &buffer) {
		if(!is_open()) {
			return;
		}
		mapBuffer[m_index] = &buffer;
		m_index = kernel->arg(buffer, m_index);
	};
	void arg(unsigned int &buffer) {
		if(!is_open()) {
			return;
		}
		m_index = kernel->arg(buffer, m_index);
	};
	void arg(float &buffer) {
		if(!is_open()) {
			return;
		}
		m_index = kernel->arg(buffer, m_index);
	};
	void write(int index, const void *istr) {
		if(!is_open()) {
			return;
		}	
		ClBuffer *buffer = mapBuffer[index];
		if(buffer->is_buffer() == false) {
			return;
		}
		m_context->command_queue->write(*buffer, istr, buffer->length());
	};
	void pin(int index, void *str) {
		if(!is_open()) {
			return;
		}
		ClBuffer *buffer = mapBuffer[index];
		void *ptr = m_context->command_queue->pin(*buffer, buffer->length());
		if(ptr == NULL) {
			TRACE("%s\n", "fault");
			return;
		}
		memcpy(ptr, str, buffer->length());
	};
	void read(int index, void *ostr) {
		if(!is_open()) {
			return;
		}	
		ClBuffer *buffer = mapBuffer[index];
		m_context->command_queue->read(*buffer, ostr, buffer->length());
	};
	void read(int index, ClFrame &frame, const size_t *origin
			, const size_t *region) {
		read(index, frame.get(), origin, region);
	}
	void read(int index, unsigned char *ret_img, const size_t *origin
			, const size_t *region) {
		if(!is_open()) {
			return;
		}
		ClBuffer *buffer = mapBuffer[index];
		if(buffer->is_image() == false) {
			return;
		}

		PERF_START("clEnqueueReadImage");
		lastError = clEnqueueReadImage(m_context->command_queue->get(), buffer->get(), CL_TRUE
				, origin, region, 0, 0, ret_img
				, 0, NULL, NULL);
		PERF_END("clEnqueueReadImage");

		if(CL_SUCCESS != lastError) {
			TRACE("lastError = %d\n", lastError);
		}
	};
	inline void run() {
		if(!is_open()) {
			return;
		}
		m_context->command_queue->enqueueKernel(*kernel);
	};
	void run(int dimCount, const size_t *global, const size_t *local) {
		if(!is_open()) {
			return;
		}
		//size_t Xlocal = getWorkGroupInfo();
		PERF_START(program->name());
		m_context->command_queue->enqueueKernel(*kernel, dimCount, global, local);
		PERF_END(program->name());
	};
	size_t getWorkGroupInfo() {
		size_t local = m_context->command_queue->getWorkGroupInfo(*m_context->context, *kernel);
		return local;
	};
	void trace(bool onoff) {
		m_context->command_queue->trace(onoff);	
	};
	size_t roundup(int groupSize, int globalSize)
	{
		int r = globalSize % groupSize;
#ifdef _WITH_SNAPSHOT
		TRACE("roundup yeilds r = %d (%d, %d) [%d]\n", 
				r, groupSize, globalSize, globalSize + groupSize - r);
#endif //_WITH_SNAPSHOT
		if(r == 0) {
			return globalSize;
		} else {
			return globalSize + groupSize - r;
		}
	}
	cl_sampler sampler(cl_bool normalized_coords, cl_addressing_mode addressing_mode
			, cl_filter_mode filter_mode) {
		return m_context->context->createSampler(normalized_coords, addressing_mode, filter_mode);
	};
	cl_mem image(cl_mem_flags flags, ClImage &img) {
		return image(flags, img.m_buffer, img.width(), img.height());
	}
	cl_mem image(cl_mem_flags flags, ClFrame &frame) {
		return image(flags, frame.get(), frame.width(), frame.height());
	}
	cl_mem image(cl_mem_flags flags, unsigned char *buffer, size_t width, size_t height) {
		cl_image_format format;
		format.image_channel_order = CL_RGBA;
		format.image_channel_data_type = CL_UNORM_INT8;
		if(m_context->context->is_open() == false) {
			TRACE("%s failed\n", "is_open");
			return NULL;
		}
#ifdef _WITH_IMAGEDESC
		cl_image_desc desc;
		desc.image_type = CL_MEM_OBJECT_IMAGE2D;
		desc.image_width = width;
		desc.image_height = height;
		desc.image_depth = 0;
		desc.image_array_size = 0;
		desc.image_row_pitch = 0;
		desc.image_slice_pitch = 0;
		desc.num_mip_levels = 0;
		desc.num_samples = 0;
		desc.buffer = NULL; //cl_mem 
		cl_mem img = clCreateImage(m_context->context->get(), flags, 
				&format, &desc, buffer, &lastError);
#else
		//clCreateImage2D is deprecated 
		cl_mem img = clCreateImage2D(m_context->context->get(), flags
				, &format, width, height, 0, buffer, &lastError);
#endif //_WITH_IMAGEDESC
		if(CL_SUCCESS != lastError) {
			TRACE("lastError = %d\n", lastError);
		}
		return img;
	};
};

#endif // _CLDEVICE_H

