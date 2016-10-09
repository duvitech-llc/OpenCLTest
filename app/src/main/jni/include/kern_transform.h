/*
                              _                       
                     __ _  __| |_   _____ _ __   __ _ 
                    / _` |/ _` \ \ / / _ \ '_ \ / _` |
                   | (_| | (_| |\ V /  __/ | | | (_| |
                    \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                |___/ 
*/
#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include <map>
#include <fstream>
#include <clobject.h>

class ClOpTransform : public ClOperationGpu {
private:
protected:
	ClFrame last_frame;
	ClBuffer iImage1;
	ClBuffer iImage2;
	ClBuffer oImage;
	ClBuffer sampler;
	size_t m_width;
	size_t m_height;
	const char *name[32] = {"trans1", "trans2", "blur3", 
		"blur4", "motion5", "trans6", "trans7"};
	int m_ndx;
	int cnt;
public:
	ClOpTransform() {
		m_ndx = 0;
		cnt = 0;
	};
	~ClOpTransform() {
		last_frame.close();
	};
	void clear_last(ClFrame &in_frame) {
		if(!is_open())
			return;
		if(is_clear == true) {
			return;
		}
		is_clear = true;
		last_frame.framecopy(in_frame);
	}
	inline bool proc(int ndx, ClFrame &in_frame, ClFrame &out_frame) {
		//int flags = CL_MEM_ALLOC_HOST_PTR;
		int flags = CL_MEM_COPY_HOST_PTR;

		if(!is_open()) {
			m_ndx = ndx - 1;
			if(open("./transform.cl", name[m_ndx]) == false)
				return false;
			m_width = in_frame.width();
			m_height = in_frame.height();
		}

		PERF_START("transimg_proc");

		if(last_frame.is_open() == false) {
			last_frame.open(in_frame);
		}

		clear();
		is_clear = false;

		iImage1 = create_buffer(CL_MEM_READ_ONLY | flags, in_frame);
		iImage2 = create_buffer(CL_MEM_READ_ONLY | flags, last_frame);

		oImage = create_buffer(CL_MEM_WRITE_ONLY, NULL, m_width, m_height);
		sampler = create_buffer_sampler(CL_FALSE, CL_ADDRESS_CLAMP_TO_EDGE, CL_FILTER_NEAREST);

		arg(iImage1);
		arg(iImage2);
		arg(oImage);
		arg(sampler);

		run(m_width, m_height);

		in_frame.get_origin(origin);
		in_frame.get_region(region);

		read(oImage, out_frame, origin, region);
		PERF_END("transimg_proc");

		if(cnt++ > 30) {
			last_frame.framecopy(in_frame);
			cnt = 0;
		} else {
			last_frame.framecopy(out_frame);
		}
		return true;
	};
};


// transform
#endif //_TRANSFORM_H

