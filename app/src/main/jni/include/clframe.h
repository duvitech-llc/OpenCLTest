/*
                   _                       
          __ _  __| |_   _____ _ __   __ _ 
         / _` |/ _` \ \ / / _ \ '_ \ / _` |
        | (_| | (_| |\ V /  __/ | | | (_| |
         \__,_|\__,_| \_/ \___|_| |_|\__, |
                                     |___/ 
*/
#ifndef _CLFRAME_H
#define _CLFRAME_H

#include <clobject.h>
#include <clvideohelper.h>

#include <linux/videodev2.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

struct pixel_cl {
	char r; 
	char b; 
	char g; 
	char a;
};


class ClFrame : public ClObject {
private:
	unsigned char* m_ptmp;
protected:
	size_t m_width;
	size_t m_height;
	size_t m_bpp;
	unsigned char* m_pframe;
	size_t m_origin[3];
	size_t m_region[3];
public:
	ClFrame() {
		m_pframe = 0;
		m_ptmp = NULL;
	};
	ClFrame(ClFrame &in_frame) {
		m_pframe = in_frame.get();
		m_width = in_frame.width();
		m_height = in_frame.height();
		m_bpp = in_frame.bpp();
		m_ptmp = NULL;
		set_open();
	};
	ClFrame(unsigned char *frame, size_t width, size_t height, size_t bpp) {
		m_pframe = frame;
		m_width = width;
		m_height = height;
		m_bpp = bpp;
		m_ptmp = NULL;
		set_open();
	};
	~ClFrame() {
		if(m_ptmp != NULL) {
			delete[] m_ptmp;
		}
	};
	bool open(size_t width, size_t height, size_t bpp) {
		m_width = width;
		m_height = height;
		m_bpp = bpp;
		m_pframe = new unsigned char[m_width * m_height * m_bpp];
		set_open();
		return true;
	};
	bool open(ClFrame &frame) {
		if(open(frame.width(), frame.height(), frame.bpp()) == false)
			return false;
		framecopy(frame);
		return true;
	}
	void close() {
		if(!is_open())
			return;
		delete[] m_pframe;
	};
	inline void copy(ClFrame &orig) {
		open(orig.width(), orig.height(), orig.bpp());
		memcpy(m_pframe, orig.get(), orig.length());
		orig.get_origin(m_origin);
		orig.get_region(m_region);
	}
	//ClFrame& operator=(ClFrame& orig) {
	//	framecopy(orig); or copy(orig);
	//	return *this;
	//}
	inline void framecopy(ClFrame &orig) {
		if(!orig.is_open())
			return;
		if(m_pframe != 0)
			memcpy(m_pframe, orig.get(), orig.length());
	}
	inline size_t width() {
		return m_width;
	};
	inline size_t height() {
		return m_height;
	};
	inline size_t bpp() {
		return m_bpp;
	};
	void resize(size_t width, size_t height, size_t bpp) {
		m_width = width;
		m_height = height;
		m_bpp = bpp;
	}
	inline size_t length() {
		if(!is_open())
			return 0;
		return (m_width * m_height * m_bpp);
	};
	inline void set(char val, size_t len) {
		if(!is_open())
			return;
		memset(m_pframe, val, len);
	};
	inline unsigned char *get() {
		return m_pframe;
	};
	inline unsigned char *at(int x, int y) {
		size_t pos = (x + (y * m_width)) * m_bpp;
		return m_pframe + pos;
	};
	inline void set(unsigned char *frame, size_t len) {
		if(!is_open())
			return;
		memcpy(m_pframe, frame, len);
	};
	inline void get(unsigned char *frame, size_t len) {
		if(!is_open())
			return;
		memcpy(frame, m_pframe, len);
	};
	inline size_t offset(int x, int y) {
		size_t pos = (x + (y * m_width)) * m_bpp;
		return pos;
	}
	inline void set_pixel(int x, int y, pixel_cl pixel) {
		set_pixel(x, y, pixel.r, pixel.g, pixel.b, pixel.a);
	};
	inline void get_pixel(int x, int y, pixel_cl &pixel) {
		get_pixel(x, y, pixel.r, pixel.g, pixel.b, pixel.a);
	};
	inline void set_pixel(int x, int y, char r, char g, char b, char a) {
		if(!is_open())
			return;
		size_t pos = offset(x, y);
		m_pframe[pos + 0] = r;
		m_pframe[pos + 1] = g;
		m_pframe[pos + 2] = b;
		m_pframe[pos + 3] = a;
	};
	inline void get_pixel(int x, int y, char &r, char &g, char &b, char &a) {
		if(!is_open())
			return;
		size_t pos = offset(x, y);
		r = m_pframe[pos + 0];
		g = m_pframe[pos + 1];
		b = m_pframe[pos + 2];
		a = m_pframe[pos + 3];
	};
	inline void fill_color(char r, char g, char b, char a) {
		size_t x, y;
		for(y = 0; y < m_height; y++) {
			for(x = 0; x < m_width; x++) {
				set_pixel(x, y, r, g, b, a);
			}
		}
	};
	inline void add_color(char r, char g, char b, char a) {
		pixel_cl pix;
		size_t x, y;
		for(y = 0; y < m_height; y++) {
			for(x = 0; x < m_width; x++) {
				get_pixel(x, y, pix);
				pix.r = clamp(pix.r + r, pix.r, pix.r);
				pix.g = clamp(pix.g + g, pix.g, pix.g);
				pix.b = clamp(pix.b + b, pix.b, pix.b);
				pix.a = clamp(pix.a + a);
				set_pixel(x, y, pix);
			}
		}
	};
	inline void vflip() {
		int len = m_width * m_bpp;
	  	if(m_ptmp == NULL) 
			m_ptmp = new unsigned char[len];
		int bottom = (m_height/2);
		int top = (m_height/2) + 1;
		for(size_t ndx = 0; ndx < m_height/2; ndx++) {
			memcpy(m_ptmp, m_pframe + (bottom * len), len);
			memcpy(m_pframe + (bottom * len), m_pframe + (top * len), len);
			memcpy(m_pframe + (top * len), m_ptmp, len);
			bottom--;
			top++;
		}
	};
	inline void hflip() {
		//TODO:
	}
	inline int clamp(int val) {
		return clamp(val, 0, 255);
	};
	inline int clamp(int val, char low, char hi) {
		//max(min(a, b), c)
		if(val < 0)
			val = low;
		if(val > 255)
			val = hi;
		return val;
	};
	inline void set_region(int x, int y, int z) {
		m_region[0] = x;
		m_region[1] = y;
		m_region[2] = z;
	};
	void get_region(size_t region[3]) {
		region[0] = m_region[0];
		region[1] = m_region[1];
		region[2] = m_region[2];
	};
	inline void set_origin(int x, int y, int z) {
		m_origin[0] = x;
		m_origin[1] = y;
		m_origin[2] = z;
	};
	void get_origin(size_t origin[3]) {
		origin[0] = m_origin[0];
		origin[1] = m_origin[1];
		origin[2] = m_origin[2];
	};
};


#endif //_CLFRAME_H

