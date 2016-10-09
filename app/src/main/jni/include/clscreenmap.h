/*
                              _    _ _       
                    __ _  ___| | _| (_)_ __  
                   / _` |/ __| |/ / | | '_ \
                  | (_| | (__|   <| | | | | |
                   \__,_|\___|_|\_\_|_|_| |_|
                                             
*/
#ifndef _CLSCREENMAP_H
#define _CLSCREENMAP_H

#include <map>
#include <fstream>
#include <clobject.h>
#include <clscreeninfo.h>

class ClScreenMap : public ClObject { 
private:
	unsigned int m_bytes_per_pixel;
protected:
	unsigned char *fbp;
	long int mapped_length;
	unsigned int m_yoffset;
	unsigned int m_line_length;
	unsigned int m_xoffset;
public:
	ClScreenMap() {
	};
	~ClScreenMap() {
	};
	bool open(ClScreenInfo &info) {
		if(info.is_open() == false)
			return false;

		m_bytes_per_pixel = info.bytes_per_pixel();
		mapped_length = info.length();
		m_yoffset = info.yoffset();
		m_line_length = info.line_length();
		m_xoffset = info.xoffset();

		fbp = (unsigned char *)mmap(0, mapped_length, PROT_READ | PROT_WRITE, MAP_SHARED
				, info.get(), 0);
		if (fbp == MAP_FAILED) {
			TRACE("mmap %s\n", "failed"); 
			return false;
		}
		set_open();
		return true;
	};
	void close() {
		if(is_open()) {
			munmap(fbp, mapped_length);
		}
		set_close();
	};
	long int length() {
		return mapped_length;
	}
	unsigned char *get() {
		if(is_open() == false)
			return NULL;
		return fbp;
	}
	void dump(const char *file) {
		if(is_open())
			save(file, (char *)fbp, (size_t)mapped_length);
	}
	unsigned int bytes_per_pixel() {
		return m_bytes_per_pixel;
	};
	unsigned int yoffset() {
		return m_yoffset;
	};
	unsigned int line_length() {
		return m_line_length;
	};
	unsigned int xoffset() {
		return m_xoffset;
	};
};


// clscreenmap
#endif //_CLSCREENMAP_H

