LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

#opencv
include $(LOCAL_PATH)/open_cv/jni/OpenCV.mk
OPENCV_CAMERA_MODULES:=off
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=STATIC

LOCAL_MODULE        := libplayer_jni
LOCAL_CFLAGS        := -Werror

LOCAL_C_INCLUDES    :=  $(LOCAL_PATH)                       \
                        $(LOCAL_PATH)/open_cv/jni/include   \
                        $(LOCAL_PATH)/ffmpeg

LOCAL_SRC_FILES     :=  file/file.cpp               \
                        security/aes.cpp            \
                        security/licence.cpp        \
                        transform/touch.cpp         \
                        transform/sensor.cpp        \
                        transform/transform.cpp     \
                        compose/LaplacianBlending.cpp       \
                        compose/mycv.cpp                    \
                        compose/Generate_fusion_area.cpp    \
                        gl/gl_matrix.cpp            \
                        gl/gl_base.cpp              \
                        gl/gl_renderer.cpp          \
                        gl/gl_picture.cpp           \
                        gl/gl_video.cpp             \
                        gl/gl_yuv.cpp               \
                        gl/gl_play_yuv.cpp          \
                        bean/float_buffer.cpp       \
                        bean/double_buffer.cpp      \
                        bean/bean.cpp               \
                        bean/region.cpp             \
                        jni_api.cpp

LOCAL_LDLIBS        += -llog -lGLESv3 -lEGL -ljnigraphics -lm

include $(BUILD_SHARED_LIBRARY)
