/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/
#ifndef _CLPLATFORM_H
#define _CLPLATFORM_H
#include <clobject.h>

enum {
	D_NONE
	, D_CHAR
	, D_SIZET
	, D_UINT
	, D_ULONG
	, D_BOOL
} E_DEVINFO;

#define PLOG(index, retSize, cBuffer) \
m_plog << dev[index].name << "[" << dev[index].info << "]: ("; \
m_plog << sizeof(cBuffer) << "," << retSize << ") value "; \
m_plog << cBuffer << std::endl

struct dev_info {
	const char *name;
	cl_device_info info;
	int type;
	char buffer[1024];
};
dev_info dev[] = {
	{"CL_DEVICE_ADDRESS_BITS", CL_DEVICE_ADDRESS_BITS, D_UINT, 0}
	, {"CL_DEVICE_AVAILABLE", CL_DEVICE_AVAILABLE, D_UINT, 0}
	, {"CL_DEVICE_COMPILER_AVAILABLE", CL_DEVICE_COMPILER_AVAILABLE, D_BOOL, 0}
//	, {"CL_DEVICE_DOUBLE_FP_CONFIG", CL_DEVICE_DOUBLE_FP_CONFIG, D_NONE, 0}
	, {"CL_DEVICE_ENDIAN_LITTLE", CL_DEVICE_ENDIAN_LITTLE, D_BOOL, 0}
	, {"CL_DEVICE_ERROR_CORRECTION_SUPPORT", CL_DEVICE_ERROR_CORRECTION_SUPPORT, D_BOOL, 0}
	, {"CL_DEVICE_EXECUTION_CAPABILITIES", CL_DEVICE_EXECUTION_CAPABILITIES, D_BOOL, 0}
	, {"CL_DEVICE_EXTENSIONS", CL_DEVICE_EXTENSIONS, D_CHAR, 0}
	, {"CL_DEVICE_GLOBAL_MEM_CACHE_SIZE", CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, D_ULONG, 0}
	, {"CL_DEVICE_GLOBAL_MEM_CACHE_TYPE", CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, D_NONE, 0}
	, {"CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE", CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, D_UINT, 0}
	, {"CL_DEVICE_GLOBAL_MEM_SIZE", CL_DEVICE_GLOBAL_MEM_SIZE, D_ULONG, 0}
//	, {"CL_DEVICE_HALF_FP_CONFIG", CL_DEVICE_HALF_FP_CONFIG, D_NONE, 0}
	, {"CL_DEVICE_IMAGE_SUPPORT", CL_DEVICE_IMAGE_SUPPORT, D_BOOL, 0}
	, {"CL_DEVICE_IMAGE2D_MAX_HEIGHT", CL_DEVICE_IMAGE2D_MAX_HEIGHT, D_SIZET, 0}
	, {"CL_DEVICE_IMAGE2D_MAX_WIDTH", CL_DEVICE_IMAGE2D_MAX_WIDTH, D_SIZET, 0}
	, {"CL_DEVICE_IMAGE3D_MAX_DEPTH", CL_DEVICE_IMAGE3D_MAX_DEPTH, D_SIZET, 0}
	, {"CL_DEVICE_IMAGE3D_MAX_HEIGHT", CL_DEVICE_IMAGE3D_MAX_HEIGHT, D_SIZET, 0}
	, {"CL_DEVICE_IMAGE3D_MAX_WIDTH", CL_DEVICE_IMAGE3D_MAX_WIDTH, D_SIZET, 0}
	, {"CL_DEVICE_LOCAL_MEM_SIZE", CL_DEVICE_LOCAL_MEM_SIZE, D_ULONG, 0}
	, {"CL_DEVICE_LOCAL_MEM_TYPE", CL_DEVICE_LOCAL_MEM_TYPE, D_NONE, 0}
	, {"CL_DEVICE_MAX_CLOCK_FREQUENCY",CL_DEVICE_MAX_CLOCK_FREQUENCY,D_UINT, 0}
	, {"CL_DEVICE_MAX_COMPUTE_UNITS",CL_DEVICE_MAX_COMPUTE_UNITS,D_UINT, 0}
	, {"CL_DEVICE_MAX_CONSTANT_ARGS"	,CL_DEVICE_MAX_CONSTANT_ARGS,D_UINT, 0}
	, {"CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE",CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, D_ULONG, 0}
	, {"CL_DEVICE_MAX_MEM_ALLOC_SIZE",CL_DEVICE_MAX_MEM_ALLOC_SIZE, D_ULONG, 0}
	, {"CL_DEVICE_MAX_PARAMETER_SIZE",CL_DEVICE_MAX_PARAMETER_SIZE, D_SIZET, 0}
	, {"CL_DEVICE_MAX_READ_IMAGE_ARGS"	,CL_DEVICE_MAX_READ_IMAGE_ARGS,D_UINT, 0}
	, {"CL_DEVICE_MAX_SAMPLERS"	,CL_DEVICE_MAX_SAMPLERS,D_UINT, 0}
	, {"CL_DEVICE_MAX_WORK_GROUP_SIZE",CL_DEVICE_MAX_WORK_GROUP_SIZE,D_SIZET, 0}
	, {"CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS",CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,D_UINT, 0}
	, {"CL_DEVICE_MAX_WORK_ITEM_SIZES"	,CL_DEVICE_MAX_WORK_ITEM_SIZES,D_NONE, 0}
	, {"CL_DEVICE_MAX_WRITE_IMAGE_ARGS"	,CL_DEVICE_MAX_WRITE_IMAGE_ARGS,D_UINT, 0}
	, {"CL_DEVICE_MEM_BASE_ADDR_ALIGN"	,CL_DEVICE_MEM_BASE_ADDR_ALIGN,D_UINT, 0}
	, {"CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE",CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE,D_UINT, 0}
	, {"CL_DEVICE_NAME", CL_DEVICE_NAME, D_CHAR, 0}
	, {"CL_DEVICE_PLATFORM", CL_DEVICE_PLATFORM, D_NONE, 0}
	/*CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR
	 CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT
	 CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT
	 CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG
	 CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT
	 CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE*/
	, {"CL_DEVICE_PROFILE", CL_DEVICE_PROFILE, D_CHAR, 0}
	, {"CL_DEVICE_PROFILING_TIMER_RESOLUTION", CL_DEVICE_PROFILING_TIMER_RESOLUTION,D_SIZET, 0}
	, {"CL_DEVICE_QUEUE_PROPERTIES", CL_DEVICE_QUEUE_PROPERTIES,	D_NONE, 0}
//	, {"CL_DEVICE_SINGLE_FP_CONFIG", CL_DEVICE_SINGLE_FP_CONFIG	, D_NONE, 0}
	, {"CL_DEVICE_TYPE", CL_DEVICE_TYPE	, D_NONE, 0}
	, {"CL_DEVICE_VENDOR", CL_DEVICE_VENDOR, D_CHAR, 0}	
	, {"CL_DEVICE_VENDOR_ID", CL_DEVICE_VENDOR_ID, D_UINT, 0}	
	, {"CL_DEVICE_VERSION", CL_DEVICE_VERSION	, D_CHAR, 0}
	, {"CL_DRIVER_VERSION", CL_DRIVER_VERSION, D_CHAR, 0}
	, {0, 0, D_NONE, 0}
};

class ClPlatform : public ClObject {
private:
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	std::fstream m_plog;
protected:
	cl_platform_id platform_id;
	cl_device_type type;
	cl_device_id device_id; 
public:
	ClPlatform() : ClObject() {
		platform_id = 0;
		type = 0;
		device_id = 0; 
		open(CL_DEVICE_TYPE_DEFAULT);
	}
	ClPlatform(cl_device_type type) : ClObject() {
		platform_id = 0;
		type = 0;
		device_id = 0; 
		open(type);
	};
	void close() {
		if(!is_open()) {
			return;
		}
		platform_id = 0;
		set_close();
		m_plog.close();
	};
	bool open(cl_device_type type) {
		if(is_open()) {
			return false;
		}
		m_plog.open("platform.log", std::fstream::in | std::fstream::out | std::fstream::trunc);

		/* Get Platform and Device Info */
		lastError = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
		TRACE("lastError = %d\n", lastError);
		TRACE("ret_num_platforms = %d\n", ret_num_platforms);

		lastError = clGetDeviceIDs(platform_id, type, 1, &device_id, &ret_num_devices);
		//lastError = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ACCELERATOR, 1, &device_id, &ret_num_devices);
		if(CL_SUCCESS != lastError) {
			TRACE("lastError = %d\n", lastError);
		}

		if(lastError != CL_SUCCESS) {
			TRACE("failed on clGetDeviceIDs on GPU retry default\n");
//			open(CL_DEVICE_TYPE_DEFAULT);
			lastError = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, 
					&ret_num_devices);
			if(lastError != CL_SUCCESS) {
				exit(1);
			}
		}
		set_open();
#if _DEBUG
		info();
#endif //_DEBUG
		return true;
	};
	void info() {
		if(!is_open()) {
			return;
		}
		TRACE("clGetDeviceIDs ret_num_devices = %d, for device_id = %p\n", 
				ret_num_devices, (int*)device_id);

		size_t retSize;
		int index = sizeof(dev)/sizeof(dev[0]);
		while(index--) {
			switch(dev[index].type) {
			case D_CHAR: 
				{
				char cBuffer[1024];
				lastError = clGetDeviceInfo(device_id, dev[index].info, sizeof(cBuffer), &cBuffer, &retSize);
				PLOG(index, retSize, cBuffer);
				}
				break;
			case D_BOOL:
				{
				bool cBuffer;
				lastError = clGetDeviceInfo(device_id, dev[index].info, sizeof(cBuffer), &cBuffer, &retSize);
				PLOG(index, retSize, cBuffer);
				}
				break;
			case D_UINT:
				{
				unsigned int cBuffer;
				lastError = clGetDeviceInfo(device_id, dev[index].info, sizeof(cBuffer), &cBuffer, &retSize);
				PLOG(index, retSize, cBuffer);
				}
				break;
			case D_ULONG:
				{
				unsigned long cBuffer;
				lastError = clGetDeviceInfo(device_id, dev[index].info, sizeof(cBuffer), &cBuffer, &retSize);
				PLOG(index, retSize, cBuffer);
				}
				break;
			case D_SIZET:
				{
				size_t cBuffer;
				lastError = clGetDeviceInfo(device_id, dev[index].info, sizeof(cBuffer), &cBuffer, &retSize);
				PLOG(index, retSize, cBuffer);
				}
				break;

			};
			//TRACE("lastError = %d\n", lastError);
		}
		//clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(cBuffer), &cBuffer, NULL);
		//INFO("CL_DEVICE_NAME: %s\n", cBuffer);
		//clGetDeviceInfo(device_id, CL_DRIVER_VERSION, sizeof(cBuffer), &cBuffer, NULL);
		//INFO("CL_DRIVER_VERSION: %s\n", cBuffer);
	};
	~ClPlatform() {
		close();
	};
	cl_device_id& get() {
		return device_id;
	};
};

#endif // _CLPLATFORM_H
