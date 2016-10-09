/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/
#ifndef _CLFPS_H
#define _CLFPS_H
#include <map>
#include <fstream>

class ClFpsLog {
private:
protected:
	std::fstream m_log;
public:
	ClFpsLog() {
		m_log.open("fps.log", std::fstream::in | std::fstream::out | std::fstream::trunc);
	};
	~ClFpsLog() {
		m_log.close();
	};
	void slow(float fps) {
		m_log << "fps = ";
		m_log << fps;
		m_log << " is too low; drop frame";
		m_log << std::endl;
	};
	void log(float fps, const char *motion, const char *ocl_image, unsigned long delay) {
		m_log << "fps = " << fps;
		m_log << " motion_" << motion;
		m_log << " opencl_" << ocl_image;
		if(delay > 0) {
			m_log << " abs( " << (float)(1000.0/delay) << " )";
		} else {
			m_log << " rel( " << delay << " )";
		}
		m_log << std::endl;
	};
	void log(float fps) {
		m_log << "total fps = " << fps;
		m_log << std::endl;
	};
};

class ClFps {
private:
protected:
	float m_fps[60];
	float m_avgfps;
	float m_avgfps_total;
	int m_len;
	ClFpsLog m_log;
public:
	float m_drop;
	ClFps() {
		m_len = 8;
		m_avgfps = m_len * 2.0;
		for(int ndx = 0; ndx < 60; ndx++) {
			m_fps[ndx] = m_avgfps;
		}
	};
	~ClFps() {
		m_log.log(m_avgfps_total);
	};
	inline bool is_slow() {
		return (m_avgfps > m_drop) ? false : true;
	}
	void avg() {
		for(int ndx = 0; ndx < m_len; ndx++) {
			m_avgfps = (m_avgfps + m_fps[ndx]) / 2.0;
		}
		m_avgfps_total = (m_avgfps_total + m_avgfps) / 2.0;
	}
	void set(unsigned long delay) {
		if(delay == 0)
			return;
		if(delay > 100000) {
			TRACE("%s\n", "fault");
		}
		int ndx = 0;
		for(ndx = 1; ndx < m_len; ndx++) {
			m_fps[ndx - 1] = m_fps[ndx];
		}
		m_fps[m_len - 1] = (1000.0/delay);
		avg();
	};
	void log_slow() {
		m_log.slow(m_avgfps);
	}
	void log(const char *motion, const char *ocl_image, unsigned long delay) {
		m_log.log(m_avgfps, motion, ocl_image, delay);
	}
};

#endif //_CLFPS_H

