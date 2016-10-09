/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/
#ifndef _CLBUFFER_H
#define _CLBUFFER_H
#include <clobject.h>
//#include <cldevice.h>

class ClBuffer : public ClObject {
private:
	enum {
		B_NONE
		, B_BUFFER
		, B_IMAGE
		, B_SAMPLER
		, B_MAX
	};
protected:
	cl_mem m_buffer;
	cl_sampler m_sampler;
	cl_mem_flags m_flags;
	int m_type;
	unsigned int m_nsize;
	unsigned int m_length;
	int m_index;
public:
	ClBuffer() {
		set_close();
		m_type = B_NONE;
		m_index = -1;
	};
	ClBuffer(ClContext &context, size_t nSize, cl_mem_flags flags) {
		set_close();
		m_type = B_NONE;
		m_nsize = sizeof(cl_mem);
		m_length = 0;
		m_index = -1;
		open(context, nSize, flags);
	};
	ClBuffer(cl_mem buffer) {
		set_close();
		m_nsize = sizeof(cl_mem);
		m_type = B_IMAGE;
		m_buffer = buffer;
		m_length = 0;
		m_index = -1;
		set_open();
	};
	ClBuffer(cl_sampler sampler) {
		m_type = B_SAMPLER;
		m_nsize = sizeof(cl_sampler);
		m_sampler = sampler;
		m_length = 0;
		m_index = -1;
		set_open();
	};
	~ClBuffer() {
		close();
	};
	void release() {
		if(m_type == B_IMAGE) {
			if(m_buffer != 0) 
				lastError = clReleaseMemObject(m_buffer);
			m_buffer = 0;
		}
		m_index = -1;
	}
	void close() {
		if(!is_open())
			return;
		if(m_type == B_BUFFER) {
			lastError = clReleaseMemObject(m_buffer);
			if(CL_SUCCESS != lastError) {
				TRACE("lastError = %d\n", lastError);
			}
		}
		m_type = B_NONE;
		m_nsize = 0;
		m_index = -1;
	};
	bool open(ClContext &context, size_t nsize, cl_mem_flags flags, void *host_ptr = NULL) {
		if(is_open())
			return false;
		if(!context.is_open())
			return false;

		m_buffer = clCreateBuffer(context.get(), flags, nsize, host_ptr, &lastError);
		TRACE("lastError = %d\n", set_open());
		TRACE("nsize = %zu\n", nsize);
		if(CL_SUCCESS != lastError) {
			TRACE("%s\n", "failed");
			return false;
		}
		m_type = B_BUFFER;
		m_length = nsize;
		m_flags = flags;
		return true;
	};
	cl_mem& get() {
		if(m_type == B_SAMPLER) {
			//TRACE("sampler mem = %p\n", m_sampler);
			return (cl_mem&)m_sampler;
		} 
		return m_buffer;
	};
	unsigned int length() {
		return m_length;
	}
	unsigned int size() {
		return m_nsize;
	};
	inline bool is_image() {
		if(m_type == B_IMAGE)
			return true;
		TRACE("invalid buffer type\n");
		return false;
	};
	inline bool is_sampler() {
		if(m_type == B_SAMPLER) 
			return true;
		TRACE("invalid buffer type\n");
		return false;
	};
	inline bool is_buffer() {
		if(m_type == B_BUFFER)
			return true;
		TRACE("invalid buffer type\n");
		return false;
	};
	void index(int index) {
		if(m_index == -1)
			m_index = index;
	};
	int index() {
		return m_index;
	}
};
#endif // _CLBUFFER_H

