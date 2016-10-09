# A simple test for the minimal standard C++ library
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
#for opencl.h
LOCAL_C_INCLUDES := $(ADRENO_SDK)/Development/Inc/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/
#LOCAL_CPPFLAGS += -I$(ADRENO_SDK)/Development/Inc
LOCAL_CFLAGS := -DUSE_GPU -c -g -O0  -W  -std=c++11 -D_QUALCOMM -D_DEBUG -D_WITH_IMAGEDESC -fexceptions
LOCAL_LDLIBS := -llog -landroid -ljnigraphics $(LOCAL_PATH)/libs/libOpenCL.so
#LOCAL_LDLIBS := -llog -landroid -lOpenCL -ljnigraphics
LOCAL_MODULE := ocl_videoproc
LOCAL_SRC_FILES := processor.cpp
LOCAL_SRC_FILES += ocl_videoproc.cpp
LOCAL_SHARED_LIBRARIES += libcutils libOpenCL
include $(BUILD_SHARED_LIBRARY)
