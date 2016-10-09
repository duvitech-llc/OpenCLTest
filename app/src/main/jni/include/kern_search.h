/*
                     _    _ _       
           __ _  ___| | _| (_)_ __  
          / _` |/ __| |/ / | | '_ \
         | (_| | (__|   <| | | | | |
          \__,_|\___|_|\_\_|_|_| |_|
                                    
*/
#ifndef _KERN_SEARCH_H
#define _KERN_SEARCH_H

#include <map>
#include <fstream>
#include <clposition.h>

class ClOpSearch : public ClOperationGpu {
private:
protected:
	ClFrame last_frame;
	ClBuffer in_image;
	ClBuffer last_image;
	ClBuffer out_image;
	ClBuffer pos;
	ClBuffer sampler;
	ClBuffer width;
	size_t m_width;
	size_t m_height;
	char *m_pos;
	int m_counter;
	size_t y_top;
	size_t y_bottom;
	size_t x_left;
	size_t x_right;
public:
	ClOpSearch() {
		m_pos = 0;
		m_counter = 0;
		y_top = 0;
		y_bottom = 0;
		x_left = 0;
		x_right = 0;
	};
	~ClOpSearch() {
		pos.close();
		width.close();
		last_frame.close();
		if(m_pos != 0) {
			delete[] m_pos;
			m_pos = 0;
		}
	};
	inline bool proc(ClFrame &in_frame, ClPosition &postion) {
#ifdef _INTEL
		return proc_find_motion(in_frame, postion);
#else
		return proc_find_target(in_frame, postion);
		//return proc_find_motion(in_frame, postion);
#endif 
	}
protected:
	void search(ClPosition &postion) {
		size_t z = 15;
		for(size_t y = z*2; y < m_height; y++) {
			for(size_t x = z*2; x < m_width; x++) {
				if(m_pos[(y * m_width) + x] > 0) {
					postion.set(x, y, z);
					return;
				}
			}
		}
	};
	void find_edges(ClPosition &postion) {
		size_t z = 15;
		size_t a, b;
		bool found = false;
		x_right = 0;
		y_bottom = 0;

		x_left = m_width;
		y_top = m_height;
		for(size_t y = z*2; y < m_height; y++) {
			for(size_t x = z*2; x < m_width; x++) {
				if(m_pos[(y * m_width) + x] > 0) {
					if(x < x_left) // find left edge
						x_left = x;
					if(y < y_top) // find opt edge
						y_top = y;

					if(x > x_right) // find right edge
						x_right = x;
					if(y > y_bottom) // find bottom edge
						y_bottom = y;
					found = true;
				}
			}
		}
		if(found == false)
			return;
		//find center
		a = (x_right + x_left)/2;
		b = (y_bottom + y_top)/2;
		z = x_right - x_left;
		if(z > 100) // limit radius
			z = 100;
		if(z < 10)
			z = 5;
		postion.direction(a, b, z);
		if((a > 1) && (b > 1)) // protect from inversion
			postion.set(a, b, z);
	};
	bool init_proc(ClFrame &in_frame) {
		m_width = in_frame.width();
		m_height = in_frame.height();
		m_pos = new char[m_width * m_height + 2];
		memset(m_pos, 0, sizeof(char) * (m_width * m_height + 2)); // clear buffer
		int flags = CL_MEM_USE_HOST_PTR;

#if(0)
		if(init_buffer(pos, sizeof(char) * (m_width * m_height + 2), CL_MEM_READ_ONLY) == false)
			return false
		if(init_buffer(width, sizeof(size_t) * 1, CL_MEM_READ_WRITE) == false)
			return false;
#else
		if(init_buffer(pos, sizeof(char) * (m_width * m_height + 2), CL_MEM_READ_ONLY) == false)
			return false;
		if(init_buffer(width, sizeof(size_t) * 1, CL_MEM_READ_WRITE|flags, &m_width) == false)
			return false;
#endif //_INTEL

	}
	inline bool proc_find_target(ClFrame &in_frame, ClPosition &postion) {
		if(!is_open()) {
			if(open("./search.cl", "find_target") == false)
				return false;
			if(init_proc(in_frame) == false)
				return false;
		}

		PERF_START("search_proc");
		clear();

		in_image = create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, in_frame);

		arg(in_image);
		arg(pos);
		arg(width);
#ifdef _FREESCALE
		arg(in_image); // once the index is set in the buffer it will not change
#endif

		write(pos, m_pos);
		write(width, &m_width);
		//pin(pos, m_pos);
		//pin(width, &m_width);

		run(m_width, m_height);

		in_frame.get_origin(origin);
		in_frame.get_region(region);

#ifndef _INTEL
		read(in_image, in_frame, origin, region);
		read(pos, m_pos);
#else
		//m_device->read(in_image.get_index(), in_frame, origin, region); // ok seem read/write for image is ocl2.0 feature
		read(pos, m_pos);
		m_device->read(4, in_frame, origin, region);
#endif //_INTEL

		PERF_END("search_proc");

		search(postion);

		return true;
	};
	inline bool proc_find_motion(ClFrame &in_frame, ClPosition &postion) {
		if(!is_open()) {
			if(open("./search.cl", "find_motion") == false)
				return false;
			init_proc(in_frame);
		}

		if(last_frame.is_open() == false) {
			last_frame.open(in_frame);
		}

#if(1)
		if(m_counter++ > 30) {
			last_frame.framecopy(in_frame);
			m_counter = 0;
		}
#endif

		PERF_START("search_proc");
		clear();

		in_image = create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, in_frame);
		last_image = create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, last_frame);

		out_image = create_buffer(CL_MEM_WRITE_ONLY, NULL, m_width, m_height);
		sampler = create_buffer_sampler(CL_FALSE, CL_ADDRESS_CLAMP_TO_EDGE, CL_FILTER_NEAREST);

		arg(in_image);
		arg(last_image);
		arg(out_image);
		arg(sampler);
		arg(pos);
		arg(width);

		write(pos, m_pos);
		write(width, &m_width);

		run(m_width, m_height);

		in_frame.get_origin(origin);
		in_frame.get_region(region);

		read(out_image, in_frame, origin, region);
		read(pos, m_pos);
		PERF_END("search_proc");

		PERF_START("find_edges");
		find_edges(postion);
		PERF_END("find_edges");

		return true;
	};
};

// kern_search
#endif //_KERN_SEARCH_H

