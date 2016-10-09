/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/
#ifndef _CLVIDEO_H
#define _CLVIDEO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <fstream>

#include <clplatform.h>
#include <clcontext.h>
#include <clcommandqueue.h>
#include <clprogram.h>
#include <clkernel.h>
#include <clbuffer.h>
#include <clhost.h>
#include <cldevice.h>
#include <climage.h>
#include <clfps.h>
#include <cltarget.h>
#include <clsdl.h>
#include <clvideohelper.h>
#include <clvideodevice.h>
#include <clvideocapability.h>
#include <clframe.h>
#include <clkey.h>

#include <linux/videodev2.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

class ClVideo : public ClVideoHelper {
private:
protected:
	ClKey m_thekey;
	ClFps m_fps;
	Oneshot os;

	ClVideoCapability m_cap;
	ClSDL m_sdl;
	ClTarget m_target;
	bool m_ocl_image;
	bool m_motion;
	bool m_crop;
	ClFrame rgb_frame;
	ClFrame bg_img;
public:
	ClVideo() {
		m_motion = false;
		m_crop = false;
		m_ocl_image = false;
		m_fps.m_drop = 7.5;

		m_target.open(m_width, m_height, 5);
		rgb_frame.open(m_width, m_height, 4);

		bg_img.open(m_width, m_height, 4);
		bg_img.fill_color(128, 128, 128, 255);

		m_sdl.open("Camera input using OpenCL", m_width, m_height);
		m_sdl.set_max(m_width - 1, m_height - 1);
	};
	~ClVideo() {
		close();
		m_sdl.close();
		rgb_frame.close();
		bg_img.close();
	}
	/*
		============================================================================
		Vidoe IN
		============================================================================
	 */
	bool open(const char *devname) {
		if(m_cap.open(devname) == false)
			return false;
		n_buffers = m_cap.buffer_count();
		buffers = m_cap.get_buffers();
		set_open();
		return true;
	};
	void close() {
		m_cap.close();
		set_close();
	};
	bool stop(void) {
		return m_cap.stream_off();
	};
	bool start() {
		if(is_open() == false)
			return false;
		if(m_cap.queue_buffers(n_buffers) == false)
			return false;
		return m_cap.stream_on();
	};
	void frame_select() {
		for (;;) {
			fd_set fds;
			struct timeval tv;
			int ret;

			FD_ZERO(&fds);
			FD_SET(m_cap.get(), &fds);

			tv.tv_sec = 0; // timeout
			tv.tv_usec = 100;

			ret = select(m_cap.get() + 1, &fds, NULL, NULL, &tv);

			if(-1 == ret) {
				if(EINTR == errno)
					continue;
				break;
			}

			if(0 == ret) {
				break;
			}

			if(read_frame() == true)
				break;
			/* EAGAIN - continue select loop. */
		}
	};
	void calc_fsp(unsigned long delay) {
#ifdef _WITH_FPS
		m_fps.set(delay);
		m_fps.log(STR_BOOL(m_motion), STR_BOOL(m_ocl_image), delay);
#else
		m_fps.set(1);
#endif //_WITH_FPS
	};
	inline void yuv2rgb(unsigned char *tmp_img, int offset, int valb, int valc, int vald, int val_v, int val_u) {
		tmp_img[0 + offset] = m_thekey.m_mx[4] * (valb - vald) +                                     m_thekey.m_mx[0] * (val_v - valc);
		tmp_img[1 + offset] = m_thekey.m_mx[4] * (valb - vald) - m_thekey.m_mx[1] * (val_u - valc) - m_thekey.m_mx[2] * (val_v - valc);
		tmp_img[2 + offset] = m_thekey.m_mx[4] * (valb - vald) + m_thekey.m_mx[3] * (val_u - valc);
		tmp_img[3 + offset] = 255; // a
	};
	bool read_frame() {
		struct v4l2_buffer buf;

		if(buffers == NULL) {
			TRACE("%s failed\n", "read frame buffers");
			return false;
		}

		if(r_frames++ > 99999) {
			r_frames = 1;
		}

		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;

		if(-1 == m_cap.read(VIDIOC_DQBUF, &buf)) {
			switch (errno) {
				case EAGAIN:
					return false;

				case EIO:
					/* Could ignore EIO, see spec. */

					/* fall through */

				default:
					TRACE("errno = %d (%s)\n", errno, strerror(errno));
					exit(1);
					return false;
			}
		}

		PERF_START("process_image");
		if(m_fps.is_slow() == false) {
			ClFrame frame((unsigned char *)buffers[buf.index].start, m_width, m_height, 4);
			process_image(frame);
#ifdef _WITH_FPS
		} else {
			m_fps.log_slow();
#endif //_WITH_FPS
		}
		unsigned long delay = PERF_END("process_image");
#ifdef _WITH_FPS
		calc_fsp(delay);
#endif //_WITH_FPS

		if(-1 == m_cap.read(VIDIOC_QBUF, &buf)) {
			TRACE("%s failed\n", "VIDIOC_QBUF");
			return false;
		}

		return true;
	};
	/*
		============================================================================
		Vidoe OUT
		============================================================================
	 */
	/*
	void find_target(unsigned char *p) {
		int chk[] = {30, 60, 0, 0};
		int px = 0;
		int psize = m_width * m_height * 4;
		for(int y = 0; y < m_height; y++) {
			for(int x = 0; x < m_width; x++) {
				if((p[0] > chk[0]) && (p[1] > chk[1]) && (p[2] > chk[2])) {
					if(m_target.set(x, y, 5)) {
						m_target.search(false);
						return;
					}
				}
				p = p + 4;
			}
		}
	};
	*/
	void set_background_image(ClFrame &bg) {
#ifdef _WITH_FRAMECPY
		// direct copy of image
		m_sdl.set_frame(bg);
#else
		int x, y;
		char r, b, g, a;
		pixel_cl pix;
		for(y = 0; y < m_height; y++) {
			for(x = 0; x < m_width; x++) {
				bg.get_pixel(x, y, pix);
				m_sdl.draw_rgb(x, y, pix.r, pix.g, pix.b);
			}
		}
#endif //_WITH_FRAMECPY
	}
	void display_target() {
		if(m_target.is_enabled() == true) {
			m_sdl.draw_circle(m_target.get_x(), m_target.get_y(), m_target.get_r(), 0, 0, 255);
		}
	}
	void crop_pixels(size_t x, size_t y, unsigned char *img, size_t origin[3], size_t region[3]) {
		if(m_crop) {
			if((x < origin[0]) || (x > (region[0] + origin[0])))
				return;
			if((y < origin[1]) || (y > (region[1] + origin[1]))) 
				return;
		}
		m_sdl.draw_rgb(x, y, img[0], img[1], img[2]);
	}
	void offset_pixels(int x, int y, unsigned char *img, size_t origin[3], size_t region[3]) {
		if(m_crop) {
			x = x + origin[0];
			y = y + origin[1] - region[1];
		} else {
			x = x + origin[0];
			y = y + origin[1];
		}
		m_sdl.draw_rgb(x, y, img[0], img[1], img[2]);
	}
	void display_image_ocl_crop(ClFrame &frame) {
		/*
		size_t origin[3];
		size_t region[3];
		frame.get_origin(origin);
		frame.get_region(region);
		unsigned char *img = frame.get();
		for(int y = region[1]; y > 0; y--) {
			for(int x = 0; x < region[0]; x++) { 
				offset_pixels(x, y, img, origin, region);
				img = img + 4;
			}
		}
		*/
		m_sdl.set_frame_crop(frame);
		display_target();
		m_sdl.update_screen();
	}
	void display_image(ClFrame &frame) {
		size_t origin[3];
		size_t region[3];
		frame.get_origin(origin);
		frame.get_region(region);
#ifdef _WITH_FRAMECPY
		// direct copy of image
		m_sdl.set_frame(frame);
#else
		unsigned char *img = frame.get();
		int x, y;
		int ox, oy;
		for(y = m_height; y > 0; y--) {
			for(x = 0; x < m_width; x++) { 
				crop_pixels(x, y, img, origin, region);
				img = img + 4;
			}
		}
#endif //_WITH_FRAMECPY
		display_target();
		m_sdl.update_screen();
	}
	/*
		============================================================================
		Vidoe PROC
		============================================================================
	 */
	virtual void process_image(ClFrame &in_frame) {
		unsigned char *p = in_frame.get();
		PERF_START("imageSave");
		if(m_trip == false) {
			fflush(stderr);
			TRACE("image = %p, size = %zu\n", p, in_frame.length());
			fflush(stdout);

			//save("cap.dat", (char *)p, size);
			m_trip = true;
#if(0)
			ClImage image(m_width, m_height);
			image.store("cap.png", (unsigned char *)p, m_save_bits, m_bbp);
			image.close();
#endif
		}
		PERF_END("imageSave");
	};
	void run() {
		PERF_START("video_run_loop");
		run(-1);
		PERF_END("video_run_loop");
	};
	void run(int frame_count) {
		if(start() == false)
			return;

		bool loop = true;
		int count;

		count = frame_count;
		if(-1 == count) {
			TRACE("frame count = %s\n", "continuous");
		} else {
			TRACE("frame count = %d\n", count);
		}
		while (loop) {
#ifdef _WITH_SDL
			if(m_sdl.poll() == false) {
				loop = false;
				continue;
			} 
#ifdef _DEBUG
			if(m_sdl.m_event.type == SDL_KEYDOWN) {
				switch(m_sdl.m_event.key.keysym.sym) {
					KEY_MX_INC(SDLK_KP7, 0)
					KEY_MX_DEC(SDLK_KP1, 0)

					KEY_MX_INC(SDLK_KP8, 1)
					KEY_MX_DEC(SDLK_KP2, 1)

					KEY_MX_INC(SDLK_KP9, 2)
					KEY_MX_DEC(SDLK_KP3, 2)

					KEY_MX_INC(SDLK_KP4, 3)
					KEY_MX_DEC(SDLK_KP6, 3)

					KEY_MX_INC(SDLK_KP_PLUS, 4)
					KEY_MX_DEC(SDLK_KP_MINUS, 4)

					case SDLK_KP5:
						m_ocl_image = !m_ocl_image;
						KEYLOG(SDLK_KP5, m_ocl_image)
						break;
					case SDLK_KP0:
						m_motion = !m_motion;
						KEYLOG(SDLK_KP0, m_motion)
						break;
					case SDLK_KP_PERIOD:
						m_target.is_enabled() ? m_target.disable() : m_target.enable();
						KEYLOG(SDLK_KP_PERIOD, m_target.is_enabled())
						break;
					case SDLK_KP_DIVIDE:
						m_target.dec();
						KEYLOG(SDLK_KP_DIVIDE, m_target.get_r())
						break;
					case SDLK_KP_MULTIPLY:
						m_target.inc();
						KEYLOG(SDLK_KP_MULTIPLY, m_target.get_r())
						break;
					case SDLK_DOWN:
						m_target.down();
						KEYLOG(SDLK_DOWN, m_target.get_y())
						break;
					case SDLK_UP:
						m_target.up();
						KEYLOG(SDLK_UP, m_target.get_y())
						break;
					case SDLK_RIGHT:
						m_target.right();
						KEYLOG(SDLK_RIGHT, m_target.get_x())
						break;
					case SDLK_LEFT:
						m_target.left();
						KEYLOG(SDLK_LEFT, m_target.get_x())
						break;
					case SDLK_F7:
						m_target.search(true);
						KEYLOG(SDLK_F7, m_target.is_search())
						break;
					case SDLK_F8:
						m_target.search(false);
						KEYLOG(SDLK_F8, m_target.is_search())
						break;
					case SDLK_F9:
						INC_MAX(m_thekey.color_map[0], 1, 255)
						break;
					case SDLK_F10:
						INC_MAX(m_thekey.color_map[1], 1, 255)
						break;
					case SDLK_F11:
						INC_MAX(m_thekey.color_map[2], 1, 255)
						break;
					case SDLK_F2:
						DEC_MIN(m_thekey.color_map[0], 1, 0)
						break;
					case SDLK_F3:
						DEC_MIN(m_thekey.color_map[1], 1, 0)
						break;
					case SDLK_F4:
						DEC_MIN(m_thekey.color_map[2], 1, 0)
						break;
					case SDLK_F5:
						m_crop = true;
						KEYLOG(SDLK_F5, m_crop)
						break;
					case SDLK_F6:
						m_crop = false;
						KEYLOG(SDLK_F6, m_crop)
						break;
					case SDLK_UNKNOWN:
					case SDLK_BACKSPACE:
					case SDLK_TAB:
						break;
				}
			}
#endif //_DEBUG
#else
			if(count-- < 0) {
				stop();
				return;
			}
#endif //_WITH_SDL
			frame_select();
		}

		stop();
	};
};

#endif // _CLVIDEO_H

