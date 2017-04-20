#include <bean/bean_base.h>
#include "gl/gl_base.h"

GLBase::GLBase() {
}

GLBase::~GLBase() {
}

GLuint64 GLBase::getCurrentTimeUs() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

GLuint64 GLBase::getCurrentTimeMs() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

GLboolean GLBase::checkGLError(const char *op) {
    GLint error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("[GLBase:checkGLError]%s, (0x%x)", op, error);
        return GL_FALSE;
    }
    return GL_TRUE;
}

GLuint GLBase::_loadShader(GLenum shaderType, const char *pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char *buf = (char *) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("[GLBase:loadShader]Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint GLBase::createProgram(const char *pVertexSource, const char *pFragmentSource) {
    GLuint vertexShader = _loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        LOGE("[GLBase:createProgram]vertex shader fail");
        return 0;
    }

    GLuint pixelShader = _loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        LOGE("[GLBase:createProgram]fragment shader fail");
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        glAttachShader(program, pixelShader);
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char *buf = (char *) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("[GLBase:createProgram]Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}