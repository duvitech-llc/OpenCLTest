/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/
#ifndef _CLVIDEODEVICE_H
#define _CLVIDEODEVICE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <fstream>

#include <clvideohelper.h>

#include <linux/videodev2.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

class ClVideoDevice : public ClVideoHelper {
private:
protected:
	int m_dev;
public:
	ClVideoDevice() {
		m_dev = -1;
	};
	~ClVideoDevice() {
		close();
	};
	bool open(const char *devname) {
		m_dev = ::open(devname, O_RDWR | O_NONBLOCK, 0);
		if(m_dev == -1)
			return false;
		set_open();
		return true;
	};
	void close() {
		if(is_open() == false)
			return;
		::close(m_dev);
		set_close();
		m_dev = -1;
	};
	int get() {
		return m_dev;
	};
	inline int read(int request, void *arg) {
		return xioctl(m_dev, request, arg);
	}
};


#endif //_CLVIDEODEVICE_H
