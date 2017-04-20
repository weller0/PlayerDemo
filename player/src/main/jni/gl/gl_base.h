#ifndef GL_BASE_H
#define GL_BASE_H

#include <GLES3/gl3.h>
#include <stdlib.h>
#include <sys/time.h>
#include "log.h"

#define STR(s) #s
#define STRV(s) STR(s)
#define SHADER_IN_POSITION          0
#define SHADER_IN_TEX_COORDS        1

class GLBase {
public:
    GLBase();

    virtual ~GLBase();

protected:
    GLuint createProgram(const char *pVertexSource, const char *pFragmentSource);

    GLboolean checkGLError(const char *op);

    GLuint64 getCurrentTimeUs();

    GLuint64 getCurrentTimeMs();

private :
    GLuint _loadShader(GLenum shaderType, const char *pSource);
};

#endif //GL_BASE_H
