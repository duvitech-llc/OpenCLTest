/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/
#ifndef _CLIMAGE_H
#define _CLIMAGE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <fstream>

#include <clobject.h>
//#include <clhost.h>
//#include <cldevice.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#ifdef _WITH_FREEIMAGE
#include <FreeImage.h>
#endif //_WITH_FREEIMAGE

class ClImage : public ClObject
{
private:
protected:
	std::fstream file;
	int m_length;
	int m_nsize;
	unsigned char *m_header;
	size_t m_width;
	size_t m_height;
public:
	unsigned char *m_buffer;
	ClImage() {
		init();
	};
	ClImage(const char *fileName) {
		init();
		load(fileName);
	};
	ClImage(size_t width, size_t height) {
		init();
		m_width = width;
		m_height = height;
		TRACE("m_width = %zu\n", m_width);
		TRACE("m_height = %zu\n", m_height);
	};
	~ClImage() {
		close();
#ifdef _WITH_FREEIMAGE
		freeimage_deinit();
#endif //_WITH_FREEIMAGE
	};
	void init() {
		m_buffer = 0;
#ifdef _WITH_FREEIMAGE
		freeimage_init();
#endif //_WITH_FREEIMAGE
	};
#ifdef _WITH_FREEIMAGE
	FIBITMAP* m_bitmap;
	void freeimage_init() {
		m_bitmap = NULL;
		FreeImage_Initialise();
	};
	void freeimage_deinit() {
		FreeImage_DeInitialise();
	};
	void freeimage_alloc(size_t width, size_t height, int bpp) {
		m_bitmap = FreeImage_Allocate(width, height, bpp);
	}
	void freeimage_draw(size_t width, size_t height) {
		RGBQUAD color;
		for(int i = 0; i < width; i++) {
			for(int j = 0; j < height; j++) {
				color.rgbRed = 0;
				color.rgbGreen = (double)i / width * 255.0;
				color.rgbBlue = (double)j / height * 255.0;
				FreeImage_SetPixelColor(m_bitmap, i, j, &color);
			}
		}
	}
	void freeimage_save(const char *file) {
		int ret = FreeImage_Save(FIF_PNG, m_bitmap, file, 0);
		if(ret) {
			TRACE("file %s\n", "saved");
		}
	}
	bool open(const char *fileName) {
		TRACE("filename = %s\n", fileName);
		FREE_IMAGE_FORMAT format = FreeImage_GetFileType(fileName, 0);
		if(format == FIF_UNKNOWN) {
			TRACE("%s\n", "FIF_UNKNOWN");
			return false;
		}
		FIBITMAP* image = FreeImage_Load(format, fileName, BMP_DEFAULT);
		if(!image) {
			TRACE("error: %s\n", "FreeImage_Load");
			return false;
		}
		TRACE("pointer = %p\n", image);
		FIBITMAP* temp = image;
		image = FreeImage_ConvertTo32Bits(image);
		TRACE("pointer = %p\n", image);
		FreeImage_Unload(temp);
		TRACE("pointer = %p\n", image);
		m_width = FreeImage_GetWidth(image);
		m_height = FreeImage_GetHeight(image);
		m_nsize = 4 * m_width * m_height;
		TRACE("m_nsize = %d\n", m_nsize);
		m_buffer = new unsigned char[m_nsize];
		memset(m_buffer, 0, m_nsize);
		memcpy(m_buffer, FreeImage_GetBits(image), m_nsize);
		FreeImage_Unload(image);
		TRACE("m_width = %zu\n", m_width);
		TRACE("m_height = %zu\n", m_height);
		return true;
	};
	void store(const char *fileName) {
		store(fileName, m_buffer);
	};
	void store(const char *fileName, unsigned char *data) {
		store(fileName, data, 4, 32);
	};
	void store(const char *fileName, unsigned char *data, int bits, int bpp) {
		TRACE("freeimage save = %s, bits = %d, bpp = %d\n", fileName, bits, bpp);
		FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(fileName);
		FIBITMAP *image = FreeImage_ConvertFromRawBits((BYTE*)data, m_width,
				 m_height, m_width * bits, bpp,
				 0xFF000000, 0x00FF0000, 0x0000FF00);
		TRACE("images %p\n", image);
		FreeImage_Save(format, image, fileName);
	};
	void load(const char *fileName) {
		open(fileName);
	};
#else

	void open(const char *fileName) {
		file.open(fileName, std::fstream::in);
		TRACE("filename = %s\n", fileName);
		file.seekg (0, file.end);
		m_length = file.tellg();
		file.seekg (0, file.beg);
		TRACE("image m_length = %d\n", m_length);
	};
	void load(const char *fileName) {
		open(fileName);
		m_header = new unsigned char[54];
		file.read((char *)m_header, 54);
		m_width = (size_t)*(int*)&m_header[18];
		m_height = (size_t)*(int*)&m_header[22];
		TRACE("image m_width = %zu, m_height = %zu\n", m_width, m_height);
		m_nsize = 4 * m_width * m_height;
		TRACE("image nsize = %d\n", m_nsize);
		m_buffer = new unsigned char[m_nsize];
		file.read((char *)m_buffer, m_nsize);
	};
	void store(const char *fileName) {
		TRACE("%s store\n", "ClImage");
		std::fstream ofile;
		ofile.open(fileName, std::fstream::out);
		ofile.write((char *)m_header, 54); // use same header
		ofile.write((char *)m_buffer, m_nsize); 
		ofile.close();
		TRACE("store image = %s\n", fileName);
		TRACE("size = (%d,%d)\n", m_nsize, m_length);
	};
	void store(const char *fileName, unsigned char *data) {
		std::fstream ofile;
		ofile.open(fileName, std::fstream::out);
		ofile.write((char *)m_header, 54); // use same header
		ofile.write((char *)data, m_nsize); 
		ofile.close();
		TRACE("store image = %s\n", fileName);
		TRACE("size = (%d,%d)\n", m_nsize, m_length);
	};
#endif //_WITH_FREEIMAGE
	void close() {
		if(file.is_open()) {
			file.close();
		}
		if(m_buffer != 0) {
			delete m_buffer;
		}
		m_buffer = 0;
	};
	int Size() {
		return m_nsize;
	};
	char *Data() {
		return (char *)m_buffer;
	};
	char *buffer() {
		return (char *)m_buffer;
	};
	char *image() {
		return (char *)m_buffer;
	};
	size_t width() {
		return m_width;
	};
	size_t height() {
		return m_height;
	}
};
#endif // _CLIMAGE_H
