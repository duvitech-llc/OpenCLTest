/*
                               _    _ _       
                     __ _  ___| | _| (_)_ __  
                    / _` |/ __| |/ / | | '_ \
                   | (_| | (__|   <| | | | | |
                    \__,_|\___|_|\_\_|_|_| |_|
*/
#ifndef _CLSCREENDRAW_H
#define _CLSCREENDRAW_H

#include <map>
#include <fstream>
#include <clobject.h>
#include <clscreenmap.h>
#include <clscreeninfo.h>

class ClScreenDraw : public ClObject {
private:
protected:
	unsigned int x;
	unsigned int y;
	int step;
	unsigned char *current;
	unsigned int m_bytes_per_pixel;
	unsigned char *video_memory;
	unsigned int m_yoffset;
	unsigned int m_line_length;
	unsigned int m_xoffset;
	bool m_new_buffer;
public:
	ClScreenDraw() {
		m_new_buffer = false;
		step = 4;
		current = NULL;
		video_memory = NULL;
	};
	~ClScreenDraw() {
		if(m_new_buffer)
			delete[] video_memory;
	};
	bool open(ClScreenMap &smap, bool new_buffer) {
		if(smap.is_open() == false)
			return false;

		m_bytes_per_pixel = smap.bytes_per_pixel();
		m_yoffset = smap.yoffset();
		m_line_length = smap.line_length();
		m_xoffset = smap.xoffset();

		if(new_buffer) {
			m_new_buffer = new_buffer;
			video_memory = new unsigned char[smap.length()];
			memcpy(video_memory, smap.get(), smap.length());
		} else {
			video_memory = smap.get();
		}
		set_open();
		return true;
	};
	void close() {
		set_close();
	};
	void set_pixel(char r, char g, char b, char a) {
		if(is_open() == false)
			return;
		if(current != NULL) {
			current[0] = b;
			current[1] = g;
			current[2] = r;
			current[3] = a;
		}
	};
	unsigned char *next_row(int y_row) {
		current = video_memory + (y_row + m_yoffset) * m_line_length + m_xoffset * m_bytes_per_pixel;
		return current;
	}
	void next_pixel() {
		unsigned int i;
		unsigned int pixel = 0;
		switch (m_bytes_per_pixel)
		{
			case 4:
				pixel = le32toh(*((uint32_t *) current));
				current += step;
				break;
			case 2:
				TRACE("%s\n", "here");
				pixel = le16toh(*((uint16_t *) current));
				current += 2;
				break;
			default:
				TRACE("%s\n", "here");
				for (i = 0; i < m_bytes_per_pixel; i++)
				{
					pixel |= current[0] << (i * 8);
					current++;
				}
				break;
		}
	};
	unsigned char *get() {
		return video_memory;
	};
	void fill(ClScreenInfo &info, char r, char g, char b, char a) {
		for (y = 0; y < info.yres() - 1; y++)
		{
			next_row(y);
			for (x = 0; x < info.xres() - 1; x++)
			{
				next_pixel();
				set_pixel(r, g, b, a);
			}
		}
	}
};

// clscreendraw
#endif //_CLSCREENDRAW_H
