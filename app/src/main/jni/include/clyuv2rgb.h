/*
                          _    _ _       
                __ _  ___| | _| (_)_ __  
               / _` |/ __| |/ / | | '_ \
              | (_| | (__|   <| | | | | |
               \__,_|\___|_|\_\_|_|_| |_|
                                         
*/
#ifndef _CLYUV2RGB_H
#define _CLYUV2RGB_H

#include <map>
#include <fstream>
#include <clobject.h>

class ClOpConvert : public ClOperationGpu {
private:
protected:
	ClBuffer iImage;
	ClBuffer oImage;
	ClBuffer sampler;
#ifdef _INTEL
	ClBuffer m_mx;
#endif //_INTEL
	size_t m_width;
	size_t m_height;
public:
	ClOpConvert() {
#ifdef _INTEL
		init_buffer(m_mx, sizeof(float) * 5, CL_MEM_READ_ONLY);
#endif //_INTEL
	};
	~ClOpConvert() {
#ifdef _INTEL
		m_mx.close();
#endif //_INTEL
	};
	bool proc(ClFrame &in_frame, ClFrame &out_frame, float *m_fmx) {
		if(!is_open()) {
			if(open("./yuv2rgb.cl", "convert") == false) {
				return false;
			}
			m_width = in_frame.width();
			m_height = in_frame.height();
		}

		PERF_START("convert_proc");
		clear();

		in_frame.resize(in_frame.width()/2, in_frame.height(), 4);

		iImage = create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, in_frame);
		oImage = create_buffer(CL_MEM_WRITE_ONLY, NULL, m_width, m_height);
		sampler = create_buffer_sampler(CL_FALSE, CL_ADDRESS_CLAMP_TO_EDGE, CL_FILTER_NEAREST);

		arg(iImage);
		arg(oImage);
		arg(sampler);

#ifdef _INTEL
		arg(m_mx);
		write(m_mx, m_fmx);
#endif //_INTEL

		//run(in_frame.width(), in_frame.height()); // TODO: slow
		size_t local[2] = {8, 8};
		size_t global[2] = {in_frame.width(), in_frame.height()};
		run(2, local, global);

		in_frame.get_origin(origin);
		in_frame.get_region(region);

		read(oImage, out_frame, origin, region);
		PERF_END("convert_proc");
		return true;
	};
};

// clyuv2rgb
#endif //_CLYUV2RGB_H

