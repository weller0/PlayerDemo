#ifndef GL_VIDEO_H
#define GL_VIDEO_H

#include <GLES3/gl3.h>
#include <GLES/glext.h>
#include <stdlib.h>

#include "gl/gl_renderer.h"
#include "bean/bean_base.h"
#include "file/file.h"
#include "log.h"

const char gVideoVertexShader[] =
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

const char gVideoFragmentShader[] =
        "#version 300 es                        \n"
        "#extension GL_OES_EGL_image_external : require\n"
        "precision mediump float;               \n"
        "in vec2 TexCoord;                      \n"
        "uniform samplerExternalOES tTexture;   \n"
        "uniform vec3 light;                    \n"
        "out vec4 color;                        \n"
        "void main() {                          \n"
        "  color = vec4(light, 1.0) * texture(tTexture, TexCoord); \n"
        "}\n";


const GLfloat videoVertex[][12] = {
        {
                -2,   -0.5, -1,
                -2,   0.5, -1,
                -1,  -0.5, -1,
                -1,  0.5, -1,
        },
        {
                -0.5, -0.5, -1,
                -0.5, 0.5, -1,
                0.5, -0.5, -1,
                0.5, 0.5, -1,
        },
        {
                1,    -0.5, -1,
                1,    0.5, -1,
                2,   -0.5, -1,
                2,   0.5, -1,
        },
};
const GLfloat videoTexture[][8] = {
        {
                0.0, 0.0,
                0.0, 1.0,
                1.0, 0.0,
                1.0, 1.0,
        },
        {
                0.0, 0.0,
                0.0, 1.0,
                1.0, 0.0,
                1.0, 1.0,
        },
        {
                0.0, 0.0,
                0.0, 1.0,
                1.0, 0.0,
                1.0, 1.0,
        },
};

class Video : public GLRenderer {
public:
    Video(TransformBean *transformBean, SettingsBean *settingsBean);

    ~Video();

protected:
    void loadShader();

    void prepareProcessBuffer();

    SettingsBean *mSettingsBean;
};

#endif //GL_VIDEO_H
