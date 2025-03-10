#pragma once

#ifndef MAX_POINT_LIGHTS
	#define  MAX_POINT_LIGHTS 2
#endif

#ifndef MAX_SPOT_LIGHTS
	#define  MAX_SPOT_LIGHTS 2
#endif


void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
        //abort();
    }
}

#define GL_CHECK(stmt) do { \
        stmt; \
        CheckOpenGLError(#stmt, __FILE__, __LINE__); \
    } while (0)