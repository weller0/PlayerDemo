#ifndef GL_PICTURE_YUV_H
#define GL_PICTURE_YUV_H

#include <GLES3/gl3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dlfcn.h>
#include "libavutil/frame.h"
#include "gl/gl_renderer.h"
#include "gl/gl_bitmap.h"
#include "bean/bean_base.h"
#include "file/file.h"
#include "log.h"

const char gYuvVertexShader[] =
        "#version 300 es                                                    \n"
        "layout (location = "STRV(SHADER_IN_POSITION)") in vec3 position;   \n"
        "layout (location = "STRV(SHADER_IN_TEX_COORDS)") in vec2 texCoord; \n"
        "out vec2 TexCoord;                                                 \n"
        "uniform mat4 projection;                                           \n"
        "uniform mat4 camera;                                               \n"
        "uniform mat4 transform;                                            \n"
        "void main() {                                                      \n"
        "  gl_Position = projection*camera*transform*vec4(position, 1.0);   \n"
        "  TexCoord = vec2(texCoord.s, 1.0-texCoord.t);                     \n"
        "}\n";

const char gYuvFragmentShader[] =
        "#version 300 es                        \n"
        "precision mediump float;               \n"
        "in vec2 TexCoord;                      \n"
        "uniform sampler2D tTexture;            \n"
        "uniform vec3 light;                    \n"
        "out vec4 color;                        \n"
        "void main() {                          \n"
        "  color = vec4(light, 1.0) * texture(tTexture, TexCoord); \n"
        "}\n";

class PictureYuv : public GLRenderer {
public:
    PictureYuv(TransformBean *transformBean, SettingsBean *settingsBean);

    ~PictureYuv();

protected:
    void loadShader();

    GLboolean prepareDraw(Bitmap *bmp);

    GLuint updateTextureAuto();

private:
    GLboolean bFirstFrame;
    SettingsBean *mSettingsBean;

    pthread_mutex_t mutex;
    void *pSO;
    AVFrame * (*funcGetFrame)(void);
    AVFrame *pFrame;

    GLuint mTextureY, mTextureU, mTextureV;

    void createTextureForYUV();
};

#endif //GL_PICTURE_YUV_H
