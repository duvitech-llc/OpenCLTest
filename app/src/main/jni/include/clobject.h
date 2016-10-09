/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/
#ifndef _CLOBJECT_H
#define _CLOBJECT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <clperformace.h>

#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <mutex>

#include <android/log.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)
#define APPNAME "OCL_VIDEOPROC"

#ifdef _DEBUG
//#define TRACE(format, ...) fprintf(stdout, "trace[%s](%s:%d) " format, __FUNCTION__, __FILE__, __LINE__, ## __VA_ARGS__ )
#define TRACE(format, ...) \
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "trace[%s](%s:%d) " format, __FUNCTION__, __FILE__, __LINE__, ## __VA_ARGS__ );
#else
#define TRACE(format, ...) // no output
#endif //_DEBUG
#define INFO(format, ...) fprintf(stdout, "info[%s](%s:%d) " format, __FUNCTION__, __FILE__, __LINE__, ## __VA_ARGS__ )

#ifdef _DEBUG
extern perf _perfdata;
#define PERF_INIT(VAR_A) perf _perfdata(VAR_A)
#define PERF_START(VAR_A) _perfdata.start(__FILE__, __FUNCTION__, VAR_A)
#define PERF_END(VAR_A) _perfdata.end(__FILE__, __FUNCTION__, VAR_A)
#else
#define PERF_INIT(VAR_A) //perf _perfdata(VAR_A);
#define PERF_START(VAR_A) //_perfdata.start(__FILE__, __FUNCTION__, VAR_A);
#define PERF_END(VAR_A) 0 //_perfdata.end(__FILE__, __FUNCTION__, VAR_A);
#endif //_DEBUG

typedef std::lock_guard<std::mutex> klock;

class Oneshot {
public:
	bool m_shot;
	int m_cnt;
	bool m_note;
	Oneshot() {
		reset();
	};
	void set() {
		m_shot = true;
		TRACE("shot = %d\n", m_shot);
	};
	void reset() {
		m_shot = false;
		m_note = false;
	}
	bool is_set() {
		if(m_note == false) {
			m_note = true;
			TRACE("oneshot %s\n", "active");
		}
		return m_shot;
	}
};

class ClObject {
private:
	//std::fstream m_dump;
protected:
	int m_open;
	cl_int lastError;
public:
	ClObject() {
		//m_dump.open(, std::fstream::in | std::fstream::out | std::fstream::trunc);
		set_close();
	};
	~ClObject() {
		//m_dump.close();
	}
	inline int is_open() {
		return m_open;
	};
	int set_open() {
		if(lastError == CL_SUCCESS)
			m_open = true;
		return lastError;
	};
	int set_close() {
		int lastErr = lastError;
		m_open = false; 
		lastError = CL_SUCCESS; 
		return lastErr;
	}
	void save(const char *filename, char *buffer, size_t nsize) {
#ifdef _DEBUG
		TRACE("obj save file(%s) size(%zu)\n", filename, nsize);
		std::fstream ofile;
		ofile.open(filename, std::fstream::out|std::fstream::binary|std::fstream::trunc); 
		if(ofile) {
			ofile.write(buffer, nsize);
			ofile.flush();
			ofile.close();
		} else {
			TRACE("%s failed\n", "open");
		}
#endif //_DEBUG
	}
};

#endif //_CLOBJECT_H
