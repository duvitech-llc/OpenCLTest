/*
                                _    _ _       
                      __ _  ___| | _| (_)_ __  
                     / _` |/ __| |/ / | | '_ \
                    | (_| | (__|   <| | | | | |
                     \__,_|\___|_|\_\_|_|_| |_|
                                               
*/
#ifndef _CLSCREENINFO_H
#define _CLSCREENINFO_H

#include <map>
#include <fstream>
#include <clobject.h>
#include <clframebuffer.h>

class ClScreenInfo : public ClObject {
private:
protected:
	int m_dev;
	struct fb_fix_screeninfo fix_info;
	struct fb_var_screeninfo var_info;
public:
	ClScreenInfo() {
	};
	~ClScreenInfo() {
		close();
	};
	bool open(ClFrameBuffer &fb) {
		if(fb.is_open() == false)
			return false;

		m_dev = fb.get();

		if (ioctl(m_dev, FBIOGET_FSCREENINFO, &fix_info) == -1) {
			TRACE("%s\n", "failed");
			return false;
		}

		if (fix_info.type != FB_TYPE_PACKED_PIXELS) {
			TRACE("%s\n", "failed");
			return false;
		}

		if (ioctl(m_dev, FBIOGET_VSCREENINFO, &var_info) == -1) {
			TRACE("%s\n", "failed");
			return false;
		}

		if (var_info.red.length > 8 || var_info.green.length > 8 || var_info.blue.length > 8) {
			TRACE("%s\n", "failed"); 
			return false;
		}

		TRACE("%dx%d, %dbpp\n", var_info.xres, var_info.yres, var_info.bits_per_pixel);
		set_open();
		return true;
	};
	void close() {
		set_close();
	};
	void get(struct fb_fix_screeninfo &fix, struct fb_var_screeninfo &var) {
		memcpy(&fix, &fix_info, sizeof(struct fb_fix_screeninfo));
		memcpy(&var, &var_info, sizeof(struct fb_var_screeninfo));
	};
	int get() {
		return m_dev;
	};
	size_t length() {
		if(is_open() == false)
			return 0;
		return fix_info.line_length * (var_info.yres + var_info.yoffset);
	};
	unsigned int row_x_bytes() {
		return (var_info.xres + 7) / 8; 
	};
	unsigned int bits_per_pixel() {
		return var_info.bits_per_pixel;
	}
	unsigned int bytes_per_pixel() {
		return (var_info.bits_per_pixel + 7) / 8;
	};
	unsigned int line_length() {
		return fix_info.line_length;
	};
	unsigned int xoffset() {
		return var_info.xoffset;
	};
	unsigned int yoffset() {
		return var_info.yoffset;
	};
	unsigned int xres() {
		return var_info.xres;
	};
	unsigned int yres() {
		return var_info.yres;
	};
};


// clscreeninfo
#endif //_CLSCREENINFO_H

