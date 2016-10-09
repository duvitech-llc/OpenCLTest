/*
               _                       
      __ _  __| |_   _____ _ __   __ _ 
     / _` |/ _` \ \ / / _ \ '_ \ / _` |
    | (_| | (_| |\ V /  __/ | | | (_| |
     \__,_|\__,_| \_/ \___|_| |_|\__, |
                                 |___/ 
*/
#ifndef _CLVIDEOCAPABILITY_H
#define _CLVIDEOCAPABILITY_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <fstream>

#include <linux/videodev2.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define BUF_COUNT 4

class ClVideoCapability : public ClVideoHelper {
private:
	ClVideoDevice m_device;
protected:
	struct v4l2_capability m_cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
public:
	bool open(const char *devname) {
		unsigned int min;

		if(m_device.open(devname) == false)
			return false;

		if(-1 == m_device.read(VIDIOC_QUERYCAP, &m_cap))
			return false;

		if(!(m_cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
			return false;

		if(!(m_cap.capabilities & V4L2_CAP_STREAMING))
			return false;

		CLEAR(cropcap);
		cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		TRACE("%s here\n", "VIDIOC_CROPCAP");
		if(0 == m_device.read(VIDIOC_CROPCAP, &cropcap)) {
			crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			crop.c = cropcap.defrect; /* reset to default */

			TRACE("%s failed; passed\n", "VIDIOC_CROPCAP");
			if(-1 == m_device.read(VIDIOC_S_CROP, &crop)) {
				TRACE("%s failed\n", "VIDIOC_CROP");
				//return false; // Errors ignored.
			}
		}

		TRACE("%s here\n", "V4L2_BUF_TYPE_VIDEO_CAPTURE");
		CLEAR(fmt);
		fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if(m_force_format) {
			fmt.fmt.pix.width       = m_width;
			fmt.fmt.pix.height      = m_height;
			fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
			//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
			//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
			//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV422P;
			fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

			if(-1 == m_device.read(VIDIOC_S_FMT, &fmt))
				return false;

		} else {
			TRACE("%s here\n", "X");
			if(-1 == m_device.read(VIDIOC_G_FMT, &fmt))
				return false;
		}

		TRACE("fmt.fmt.pix.width = %d\n", fmt.fmt.pix.width);
		TRACE("fmt.fmt.pix.height = %d\n", fmt.fmt.pix.height);

		min = fmt.fmt.pix.width * m_min_width;
		TRACE("min = %d\n", min);
		if(fmt.fmt.pix.bytesperline < min)
			fmt.fmt.pix.bytesperline = min;

		min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
		if(fmt.fmt.pix.sizeimage < min)
			fmt.fmt.pix.sizeimage = min;

		TRACE("fmt.fmt.pix.sizeimage = %d\n", fmt.fmt.pix.sizeimage);

		if(init_mmap() == false)
			return false;

		TRACE("min = %d\n", min);
		set_open();
		return true;
	};
	void close() {
		if(is_open() == false)
			return;
		unsigned int i = 0;
		for (i = 0; i < n_buffers; ++i)
			if(-1 == munmap(buffers[i].start, buffers[i].length))
				return;
		if(buffers != NULL)
			free(buffers);
		set_close();
	};
	int buffer_count() {
		return n_buffers;
	}
	buffer *get_buffers() {
		return buffers;
	}
	int get() {
		return m_device.get();
	};
	int read(int ask, void *arg) {
		return m_device.read(ask, arg);
	};
	bool stream_off() {
		enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if(-1 == read(VIDIOC_STREAMOFF, &type)) {
			TRACE("%s failed\n", "VIDIOC_STREAMOFF");
			return false;
		}
		return true;
	};
	bool stream_on() {
		enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if(-1 == read(VIDIOC_STREAMON, &type)) {
			TRACE("%s failed\n", "VIDIOC_STREAMON");
			return false;
		}
		return true;
	};
	bool queue_buffers(unsigned int buf_count) {
		unsigned int i;
		TRACE("index = %d\n", buf_count);
		for (i = 0; i < buf_count; ++i) {
			struct v4l2_buffer buf;

			CLEAR(buf);
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = i;

			if(-1 == read(VIDIOC_QBUF, &buf)) {
				TRACE("%s failed\n", "VIDIOC_QBUF");
				return false;
			}
		}
		return true;
	};
private:
	bool init_mmap() {
		struct v4l2_requestbuffers req;

		CLEAR(req);

		req.count = BUF_COUNT;
		req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		req.memory = V4L2_MEMORY_MMAP;

		if(-1 == m_device.read(VIDIOC_REQBUFS, &req))
			return false;

		if(req.count < 2) 
			return false;

		TRACE("req.count = %d\n", req.count);
		buffers = (buffer *)calloc(req.count, sizeof(*buffers));
		TRACE("sizeof(*buffers) = %zu\n", sizeof(*buffers));

		if(!buffers) 
			return false;

		TRACE("req.count = %d\n", req.count);
		for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
			struct v4l2_buffer buf;

			CLEAR(buf);

			buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory      = V4L2_MEMORY_MMAP;
			buf.index       = n_buffers;

			if(-1 == m_device.read(VIDIOC_QUERYBUF, &buf)) {
				TRACE("%s failed\n", "VIDIOC_QUERYBUF");
				return false;
			}

			int m_dev = m_device.get();
			buffers[n_buffers].length = buf.length;
			buffers[n_buffers].start =
				mmap(NULL /* start anywhere */,
						buf.length,
						PROT_READ | PROT_WRITE /* required */,
						MAP_SHARED /* recommended */,
						m_dev, buf.m.offset);

			if(MAP_FAILED == buffers[n_buffers].start) {
				TRACE("%s failed\n", "MAP_FAILED");
				return false;
			}
		}
		return true;
	};
};

#endif //_CLVIDEOCAPABILITY_H
