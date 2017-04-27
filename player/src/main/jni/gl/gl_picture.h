#ifndef GL_PICTURE_H
#define GL_PICTURE_H

#include <GLES3/gl3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gl/gl_renderer.h"
#include "gl/gl_bitmap.h"
#include "bean/bean_base.h"
#include "file/file.h"
#include "log.h"
const char gPicVertexShader[] =
        "#version 300 es                                                    \n"
        "layout (location = "STRV(SHADER_IN_POSITION)") in vec3 position;   \n"
        "layout (location = "STRV(SHADER_IN_TEX_COORDS)") in vec2 texCoord; \n"
        "out vec2 TexCoord;                                                 \n"
        "void main() {                                                      \n"
        "  gl_Position = vec4(position, 1.0);                               \n"
        "  TexCoord = vec2(texCoord.s, texCoord.t);                         \n"
        "}\n";

const char gPicFragmentShader[] =
        "#version 300 es                        \n"
        "precision mediump float;               \n"
        "in vec2 TexCoord;                      \n"
        "uniform sampler2D tTexture;            \n"
        "out vec4 color;                        \n"
        "void main() {                          \n"
        "  color = texture(tTexture, TexCoord); \n"
        "}\n";

class Picture : public GLRenderer {
public:
    Picture(TransformBean *transformBean, SettingsBean *settingsBean);

    ~Picture();

protected:
    void loadShader();

    void prepareDraw(Bitmap *bmp);

private:
    GLboolean bFirstFrame;
    SettingsBean *mSettingsBean;
};

#endif //GL_PICTURE_H
