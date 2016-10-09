/*
                              _                       
                     __ _  __| |_   _____ _ __   __ _ 
                    / _` |/ _` \ \ / / _ \ '_ \ / _` |
                   | (_| | (_| |\ V /  __/ | | | (_| |
                    \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                |___/ 
*/
#ifndef _CLOPERATION_H
#define _CLOPERATION_H
#include <map>
#include <fstream>
#include <clobject.h>

class ClOperations : public ClObject {
private:
protected:
	ClHost *m_host;
	ClDevice *m_device;
	size_t dev_local;
	size_t origin[3];
	size_t region[3];
	bool is_clear;
public:
	ClOperations(cl_device_type dev_type = CL_DEVICE_TYPE_GPU) {
		m_host = new ClHost(dev_type);
		m_device = new ClDevice(m_host);
		dev_local = 0;
		origin[0] = 0;
		origin[1] = 0;
		origin[2] = 0;
		region[0] = 0; //m_width;
		region[1] = 0; //m_height;
		region[2] = 1;
		is_clear = true;
	};
	~ClOperations() {
		delete m_device;
		delete m_host;
	};
	bool open(const char *file, const char *kernel) {
		if(m_device->open(file, kernel) == false) {
			TRACE("%s failed\n", "open");
			return false;
		}
		set_open();
		return true;
	};
	void close() {
		m_device->close();
		set_close();
	};
	void run(size_t width, size_t height) {
		PERF_START("operation_run");
		if(dev_local < 1) {
			dev_local = m_device->getWorkGroupInfo();
		}
		if((dev_local > width) && (dev_local > height)) {
			size_t local[WORKDIM] = {1, 1};
			size_t global[WORKDIM] = {(size_t)width, (size_t)height};
			PERF_END("operation_run");
			m_device->run(WORKDIM, global, local);
		} else {
#ifdef _FREESCALE
			size_t local[WORKDIM] = {8, 8}; // 8*8=64 < 176/2
			size_t global[WORKDIM] = {(size_t)width, (size_t)height};
#else
			size_t local[WORKDIM] = {dev_local, dev_local};
			size_t global[WORKDIM] = {m_device->roundup(local[0], width), m_device->roundup(local[1], height)};
#endif //_FREESCALE
			PERF_END("operation_run");
			m_device->run(WORKDIM, global, local);
		}
	};
	void run(int dim, const size_t *local, const size_t *global) {
		m_device->run(dim, global, local);
	};
	void clear() {
		m_device->arg(); //clear arg index
	};
	void read(ClBuffer &buffer, ClFrame &frame, const size_t *origin
			, const size_t *region) {
		m_device->read(buffer.index(), frame, origin, region);
	};
	void read(ClBuffer &buffer, void *val) {
		m_device->read(buffer.index(), val);
	};
	void write(ClBuffer &buffer, const void *val) {
		m_device->write(buffer.index(), val);
	};
	void pin(ClBuffer &buffer, void *val) {
		m_device->pin(buffer.index(), val);
	};
	cl_mem create_buffer(cl_mem_flags flags, ClFrame &frame) {
		return m_device->image(flags, frame);
	};
	cl_mem create_buffer(cl_mem_flags flags, unsigned char *buffer, size_t width, size_t height) {
		return m_device->image(flags, buffer, width, height);
	};
	cl_sampler create_buffer_sampler(cl_bool normalized_coords, cl_addressing_mode addressing_mode
			, cl_filter_mode filter_mode) {
		return m_device->sampler(normalized_coords, addressing_mode, filter_mode);
	};
	bool init_buffer(ClBuffer &buffer, size_t nSize, cl_mem_flags flags, void *ptr = NULL) {
		return buffer.open(m_device->context(), nSize, flags, ptr);
	}
	inline void arg(ClBuffer &buffer) {
		m_device->arg(buffer);
	};
};

class ClOperationGpu : public ClOperations {
public:
	ClOperationGpu() : ClOperations(CL_DEVICE_TYPE_GPU)
  	{
	};
};

class ClOperationDsp : public ClOperations {
public:
	ClOperationDsp() : ClOperations(CL_DEVICE_TYPE_ACCELERATOR)
  	{
	};
};

// cloperation
#endif //_CLOPERATION_H

