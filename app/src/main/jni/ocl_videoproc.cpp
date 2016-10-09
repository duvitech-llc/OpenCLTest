
/* sign: Robert Acklin <acklinr@us.panasonic.com> [2016-04-12] */
/* file: ocl_videoproc.cpp [2016-04-12] acklinr-ae */

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
#include <clvideo.h>
#include <cloperation.h>
#include <kern_copyimg.h>
#include <kern_transform.h>
#include <cldms.h>

#include <linux/videodev2.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

class MyDMS : public ClDMS {
private:
protected:
	ClOpProc m_imgcopy;
	ClOpTransform m_trans;
public:
	void dms_frame(ClFrame &in_frame, ClFrame &out_frame) {
		if(m_ocl_image) {
			if(m_imgcopy.proc(in_frame, out_frame) == false) {
				return;
			}
		}

		if(m_motion) {
			if(m_trans.proc(5, in_frame, out_frame) == false) {
				return;
			}
		} else {
			m_trans.clear_last(in_frame);
		}

		if(!m_ocl_image && !m_motion) {
			out_frame.framecopy(in_frame);
		}
	}
};

#ifdef _WITH_STANDALONE_ANDROID
PERF_INIT("./ocl_videoproc.csv");

int main(int argc, char **argv) 
{
	MyDMS dms;

#ifdef _INTEL
	char *dev = dms.cmd_option(argv, argv + argc, "-v", "/dev/video1");
#else
	char *dev = dms.cmd_option(argv, argv + argc, "-v", "/dev/video4");
#endif //_INTEL

	if(dms.open(dev) == false) {
		TRACE("%s failed\n", "video open");
		return false;
	}

	dms.run();

	dms.close();

	free(dev);
}
#endif //_WITH_STANDALONE_ANDROID
