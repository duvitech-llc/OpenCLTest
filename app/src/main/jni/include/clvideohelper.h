/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/
#ifndef _CLVIDEOHELPER_H
#define _CLVIDEOHELPER_H
#include <map>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <fstream>

#include <clobject.h>

#include <linux/videodev2.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define CLAMP(VAR_A) VAR_A = (VAR_A < 0) ? 0 : (VAR_A >= 255) ? 255 : VAR_A;
#define CMAP(VAR_A, VAR_B, VAR_C, VAR_D) \
if((VAR_A >= VAR_B) && (VAR_A <= VAR_C)) VAR_A = VAR_D

#define INC_MAX(VAR_A, VAR_B, VAR_C) VAR_A = VAR_A + VAR_B; \
															//if(VAR_A > VAR_C) VAR_A = VAR_C;
#define DEC_MIN(VAR_A, VAR_B, VAR_C) VAR_A = VAR_A - VAR_B; \
															//if(VAR_A < VAR_C) VAR_A = VAR_C;
#define VAL(VAR_A) TRACE("val %s = %d\n", #VAR_A, VAR_A);
#define VAL2(VAR_A, VAR_B) TRACE("val2d %s = %d, %s = %d\n", #VAR_A, VAR_A, #VAR_B, VAR_B);
#define VAL3(VAR_A, VAR_B, VAR_C) TRACE("val3d %s = %d, %s = %d, %s = %d\n", #VAR_A, VAR_A, #VAR_B, VAR_B, #VAR_C, VAR_C);
#define CDIFF(VAR_A, VAR_B) \
	var = VAR_B - 5; \
	CLAMP(var) \
	VAR_B = var;\
	if(VAR_A < VAR_B) {\
		VAR_B = VAR_A; \
	} else {\
		var = VAR_A - VAR_B; \
		CLAMP(var) \
		VAR_B = var;\
	}

#define STR_BOOL(VAR_A) (VAR_A ? "ON" : "OFF")

#define WORKDIM 2
#define CLEAR(x) memset(&(x), 0, sizeof(x))
struct buffer {
	void   *start;
	size_t  length;
};

class ClVideoHelper : public ClObject {
private:
protected:
	enum io_method {
		IO_METHOD_READ,
		IO_METHOD_MMAP,
		IO_METHOD_USERPTR,
	};
	unsigned int n_buffers;
	buffer *buffers;
	bool m_force_format;
	bool m_trip;
	int m_width;
	int m_height;
	int m_min_width;
	int m_save_bits;
	int m_bbp;
	int r_frames;
public:
	ClVideoHelper() {
		n_buffers = 0;
		m_force_format = true;
		m_trip = false;
		m_width = 640;
		m_height = 480;
		m_min_width = 2;
		m_save_bits = 2;
		m_bbp = 16;
		r_frames = 0;
	};
	~ClVideoHelper() {
	};
	inline int xioctl(int fh, int request, void *arg) {
		int r;
		do {
			r = ioctl(fh, request, arg);
		} while (-1 == r && EINTR == errno);
		return r;
	}
};

#endif //_CLVIDEOHELPER_H
