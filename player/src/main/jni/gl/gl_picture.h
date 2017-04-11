#ifndef GL_PICTURE_H
#define GL_PICTURE_H

#include <GLES3/gl3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gl/gl_renderer.h"
#include "gl/gl_bitmap.h"
#include "bean/bean_base.h"
#include "log.h"

const char gPicVertexShader[] =
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

const char gPicFragmentShader[] =
        "#version 300 es                        \n"
                "precision mediump float;               \n"
                "in vec2 TexCoord;                      \n"
                "uniform sampler2D tTexture;            \n"
                "uniform vec3 light;                    \n"
                "out vec4 color;                        \n"
                "void main() {                          \n"
                "  color = vec4(light, 1.0) * texture(tTexture, TexCoord); \n"
                "}\n";

const Point3 A = {-0.5, 0.5, -1};
const Point3 B = {-0.5, -0.5, -1};
const Point3 C = {0.5, -0.5, -1};
const Point3 D = {0.5, 0.5, -1};
const Point3 E = {-0.5, 0.5, -2};
const Point3 F = {-0.5, -0.5, -2};
const Point3 G = {0.5, -0.5, -2};
const Point3 H = {0.5, 0.5, -2};

const GLfloat picVertex[][12] = {
        {// 左
                F.x, F.y, F.z,
                E.x, E.y, E.z,
                B.x, B.y, B.z,
                A.x, A.y, A.z,
        },
        {// 前
                B.x, B.y, B.z,
                A.x, A.y, A.z,
                C.x, C.y, C.z,
                D.x, D.y, D.z,
        },
        {// 上
                A.x, A.y, A.z,
                E.x, E.y, E.z,
                D.x, D.y, D.z,
                H.x, H.y, H.z,
        },
        {// 下
                F.x, F.y, F.z,
                B.x, B.y, B.z,
                G.x, G.y, G.z,
                C.x, C.y, C.z,
        },
        {//右
                C.x, C.y, C.z,
                D.x, D.y, D.z,
                G.x, G.y, G.z,
                H.x, H.y, H.z,
        },
        {// 后
                G.x, G.y, G.z,
                H.x, H.y, H.z,
                F.x, F.y, F.z,
                E.x, E.y, E.z,
        },
};
const GLfloat picTexture[][8] = {
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

class Picture : public GLRenderer {
public:
    Picture(TransformBean *transformBean, SettingsBean *settingsBean);

    ~Picture();

protected:
    void loadShader();

    void prepareProcessBuffer();

    void prepareDraw(Bitmap *bmp);

private:
    GLboolean bFirstFrame;
};

#endif //GL_PICTURE_H
