#ifndef GL_PLAY_YUV_H
#define GL_PLAY_YUV_H

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
#include "compose/Generate_fusion_area.h"

const char gYuvAVertexShader[] = "#version 300 es                           \n"
        "layout (location = "STRV(SHADER_IN_POSITION)") in vec3 position;   \n"
        "layout (location = "STRV(SHADER_IN_TEX_COORDS)") in vec3 texCoord; \n"
        "out vec2 TexCoord;                                                 \n"
        "out vec2 LightFlag;                                                \n"
        "uniform mat4 projection;                                           \n"
        "uniform mat4 camera;                                               \n"
        "uniform mat4 transform;                                            \n"
        "void main() {                                                      \n"
        "  gl_Position = projection*camera*transform*vec4(position, 1.0);   \n"
        "  TexCoord = vec2(texCoord.s, texCoord.t);                         \n"
        "  LightFlag = vec2(texCoord.z, texCoord.x);                        \n"
        "}\n";

const char gYuvAFragmentShader[] = "#version 300 es                         \n"
        "precision mediump float;               \n"
        "in vec2 TexCoord;                      \n"
        "in vec2 LightFlag;                     \n"
        "uniform sampler2D tex_y;               \n"
        "uniform sampler2D tex_u;               \n"
        "uniform sampler2D tex_v;               \n"
        "uniform vec3 light;                    \n"
        "out vec4 color;                        \n"
        "void main() {                          \n"
        "  vec4 c = vec4((texture(tex_y, TexCoord).r - 16.0/255.0) * 1.164);\n"
        "  vec4 U = vec4(texture(tex_u, TexCoord).r - 128.0/255.0);\n"
        "  vec4 V = vec4(texture(tex_v, TexCoord).r - 128.0/255.0);\n"
        "  c += V * vec4(1.596, -0.813, 0.0, 0.0);  \n"
        "  c += U * vec4(0.0, -0.392, 2.017, 0.0);  \n"
        "  c.a = 1.0;                           \n"
        "  color = vec4(light, 1.0) * c;        \n"
        "}\n";

class PlayYuv : public GLRenderer {
public:
    PlayYuv(TransformBean *transformBean, SettingsBean *settingsBean);

    ~PlayYuv();

    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t pThreadForTexture;
    GLboolean bExitThread;
    AVFrame *pYuvFrame;

    void compose(AVFrame *frame);

    void prepareTexture();

protected:
    void loadShader();

    GLboolean prepareDraw(Bitmap *bmp, GLboolean updateFrameData);

    GLuint updateTextureAuto();

private:
    GLboolean bFirstFrame;
    GLboolean bFirstFrameForCompose;
    SettingsBean *mSettingsBean;
    void *pSO;

    AVFrame *(*funcGetFrame)(void);

    GLuint mTextureY, mTextureU, mTextureV;
    GLuint mComposeTextureY, mComposeTextureU, mComposeTextureV;
    GLint mTexHandleY, mTexHandleU, mTexHandleV;

    GLboolean useYUVDraw();

    void drawForYUV(GLBean *glBean);

    void prepareComposeTexture(AVFrame *frame);

    void initCompose(GLint w, GLint h);

    static void *thread_fun(void *arg);

    //初始化参数
    Mat imapx_roi0, imapy_roi0;     //imag_0 经纬展开 map
    Mat imapx_roi1, imapy_roi1;     //imag_1 经纬展开 map
    Mat mapx_roi0_2, mapy_roi0_2;
    Mat mapx_roi1_2, mapy_roi1_2;
    Mat im, m_uv;
    Mat out_y, out_u, out_v;
};

#endif //GL_PLAY_YUV_H
