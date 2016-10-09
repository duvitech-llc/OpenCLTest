/*
                                    _    _ _       
                          __ _  ___| | _| (_)_ __  
                         / _` |/ __| |/ / | | '_ \
                        | (_| | (__|   <| | | | | |
                         \__,_|\___|_|\_\_|_|_| |_|
                                                   
*/
#ifndef _CLFRAMEBUFFER_H
#define _CLFRAMEBUFFER_H

#include <map>
#include <fstream>
#include <clobject.h>

class ClFrameBuffer : public ClObject {
private:
protected:
	int m_dev;
public:
	ClFrameBuffer() {
		m_dev = 0;
	};
	~ClFrameBuffer() {
		close();
	};
	bool open(const char *dev) {
		m_dev = ::open(dev, O_RDWR);
		if(m_dev == 0)
			return false;
		set_open();
		return true;
	};
	void close() {
		if(m_dev != 0) {
			::close(m_dev);
		}
		set_close();
	};
	int get() {
		return m_dev;
	}
};

// clframebuffer
#endif //_CLFRAMEBUFFER_H

