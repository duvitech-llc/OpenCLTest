/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/
#ifndef _CLFREEIMAGE_H
#define _CLFREEIMAGE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <fstream>

#include <FreeImage.h>

#ifdef _DEBUG
#define verbose(format, ...) fprintf(stdout, "(%s/%s:%d) " format, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__ )
#else
#define verbose(format, ...) //fprintf(stdout, "(%s/%s:%d) " format, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__ )
#endif //_DEBUG
#define INFO(format, ...) fprintf(stdout, "(%s/%s:%d) " format, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__ )

#define retError(VAR_A) ((VAR_A != 0) ? retString(__FUNCTION__, VAR_A, __LINE__) : true)
#define retAssert(VAR_A) if (VAR_A == 0) return retString(__FUNCTION__, -1, __LINE__)

#define FREEIMAGE_BPP 24

class CFreeImage 
{
protected:
	FIBITMAP* m_bitmap;
	FREE_IMAGE_FORMAT m_format;
	FIBITMAP* m_image;
	size_t m_width;
	size_t m_height;
public:
	bool retString(const char *funcName, int value, int line) {
		verbose("%s failed (%d) [line: %d]\n", funcName, value, line);
		return false;
	}
	void freeimage_init() {
		FreeImage_Initialise();
		verbose("%s\n", "FreeImage_Initialise called");
	};
	void freeimage_deinit() {
		FreeImage_DeInitialise();
		verbose("%s\n", "FreeImage_DeInitialise called");
	};
	bool freeimage_alloc(size_t width, size_t height, int bpp) {
		m_bitmap = FreeImage_Allocate(width, height, bpp);
		verbose("%s\n", "FreeImage_Allocate called");
		if(m_bitmap) {
			verbose("freeimage_alloc %s\n", "success");
			return true;
		}
		verbose("freeimage_alloc %s\n", "failed");
		return false;
	};
	bool freeimage_load(const char *fileName) {
		m_image = FreeImage_Load(m_format, fileName, BMP_DEFAULT);
		if(!m_image) {
			verbose("error: %s\n", "FreeImage_Load");
			return false;
		}
		verbose("pointer = %p\n", m_image);
		return true;
	};
	bool freeimage_draw(size_t width, size_t height) {
		verbose("freeimage_draw %p\n", "called");
		verbose("freeimage_draw (%zu, %zu)\n", width, height);
		retAssert(m_bitmap);
		RGBQUAD color;
		for(size_t i = 0; i < width; i++) {
			for(size_t j = 0; j < height; j++) {
				color.rgbRed = 0;
				color.rgbGreen = (double)i / width * 255.0;
				color.rgbBlue = (double)j / height * 255.0;
				FreeImage_SetPixelColor(m_bitmap, i, j, &color);
			}
		}
		return true;
	};
	bool freeimage_getfiletype(const char *fileName) {
		m_format = FreeImage_GetFileType(fileName, 0);
		if(m_format == FIF_UNKNOWN) {
			verbose("%s\n", "FIF_UNKNOWN");
			return false;
		}
		return true;
	};
	void freeimage_convert32bits() {
		FIBITMAP* temp = m_image;
		verbose("pointer = %p\n", m_image);
		m_image = FreeImage_ConvertTo32Bits(m_image);
		FreeImage_Unload(temp);
		verbose("pointer = %p\n", m_image);
	};
	void freeimage_getwidth() {
		m_width = FreeImage_GetWidth(m_image);
		verbose("m_width = %zu\n", m_width);
	};
	void freeimage_getheight() {
		m_height = FreeImage_GetHeight(m_image);
		verbose("m_height = %zu\n", m_height);
	};
	bool freeimage_store(const char *file, unsigned char *data, size_t width, size_t height) {
		verbose("freeimage store = %s\n", file);
		FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(file);
		FIBITMAP *image = FreeImage_ConvertFromRawBits((BYTE*)data, width,
				 height, width * 4, 32,
				 0xFF000000, 0x00FF0000, 0x0000FF00);
		int lastError = FreeImage_Save(format, image, file);
		return retError(lastError);
	};
	bool freeimage_save(const char *file) {
		verbose("freeimage save = %s\n", file);
		retAssert(m_bitmap);
		return FreeImage_Save(FIF_PNG, m_bitmap, file, 0);
	};
};

class ClFreeImage : protected CFreeImage
{
private:
	bool retString(const char *funcName, int value, int line) {
		verbose("%s failed (%d) [line: %d]\n", funcName, value, line);
		return false;
	}
protected:
	int m_length;
	int m_nsize;
	unsigned char *m_buffer;

	void init() {
		m_buffer = 0;
		freeimage_init();
	};
public:
	ClFreeImage() {
		init();
	};
	~ClFreeImage() {
		close();
	};
	bool open(const char *fileName) {
		verbose("filename = %s\n", fileName);
		if(freeimage_getfiletype(fileName) == false)
			return false;
		return true;
	};
	bool open(size_t width, size_t height) {
		m_width = width;
		m_height = height;
		return freeimage_alloc(width, height, FREEIMAGE_BPP);
	}
	bool write(const char *fileName) {
		freeimage_store(fileName, m_buffer, m_width, m_height);
		return true;
	};
	bool write(const char *fileName, unsigned char *data) {
		freeimage_store(fileName, data, m_width, m_height);
		return true;
	};
	bool save(const char *file) {
		return freeimage_save(file);
	}
	bool read(const char *fileName) {
		open(fileName);
		if(freeimage_load(fileName) == false)
			return false;

		freeimage_convert32bits();
		freeimage_getwidth();
		freeimage_getheight();

		m_nsize = 4 * m_width * m_height;
		verbose("m_nsize = %d\n", m_nsize);

		m_buffer = new unsigned char[m_nsize];
		memset(m_buffer, 0, m_nsize);

		memcpy(m_buffer, FreeImage_GetBits(m_image), m_nsize);
		FreeImage_Unload(m_image);
		return true;
	};
	void close() {
		freeimage_deinit();
		if(m_buffer != 0) {
			delete m_buffer;
		}
		m_buffer = 0;
	};
	void draw() {
		freeimage_draw(m_width, m_height);
	}
	int size() {
		return m_nsize;
	};
	char *buffer() {
		return (char *)m_buffer;
	};
	int width() {
		return m_width;
	}
	int height() {
		return m_height;
	}
};
#endif // _CLFREEIMAGE_H

