#ifndef PIC_LEFT_H
#define PIC_LEFT_H

#include <jni.h>
#include "gl/gl_renderer.h"
#include "gl/gl_picture.h"
#include "gl/gl_yuv.h"
#include "gl/gl_play_yuv.h"
#include "bean/bean.h"
#include "transform/transform.h"
#include "security/licence.h"

extern "C" {
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved);
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved);
};
#endif //PIC_LEFT_H
