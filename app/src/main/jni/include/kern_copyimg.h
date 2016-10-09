/*
                              _                       
                     __ _  __| |_   _____ _ __   __ _ 
                    / _` |/ _` \ \ / / _ \ '_ \ / _` |
                   | (_| | (_| |\ V /  __/ | | | (_| |
                    \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                |___/ 
*/
#ifndef _COPYIMG_H
#define _COPYIMG_H

#include <map>
#include <fstream>
#include <clobject.h>

class ClOpProc : public ClOperationGpu {
private:
protected:
	ClBuffer iImage;
	ClBuffer oImage;
	ClBuffer sampler;
	size_t m_width;
	size_t m_height;
public:
	ClOpProc() {
	};
	~ClOpProc() {
	};
	bool proc(ClFrame &in_frame, ClFrame &out_frame) {
		if(!is_open()) {
			if(open("./hello-img.cl", "copyimg") == false)
				return false;
			m_width = in_frame.width();
			m_height = in_frame.height();
		}

		PERF_START("copyimg_proc");
		clear();

		iImage = create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, in_frame);
		oImage = create_buffer(CL_MEM_WRITE_ONLY, NULL, m_width, m_height);
		sampler = create_buffer_sampler(CL_FALSE, CL_ADDRESS_CLAMP_TO_EDGE, CL_FILTER_NEAREST);

		arg(iImage);
		arg(oImage);
		arg(sampler);

		size_t local[2] = {8, 8};
		size_t global[2] = {in_frame.width(), in_frame.height()};
		run(2, local, global);
		//run(m_width, m_height);

		in_frame.get_origin(origin);
		in_frame.get_region(region);

		read(oImage, out_frame, origin, region);
		PERF_END("copyimg_proc");
		return true;
	};
};

// copyimg
#endif //_COPYIMG_H

