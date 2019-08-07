#ifndef HAIRGL_GL_H
#define HAIRGL_GL_H

#include "gl3w.h"
#include <stdint.h>
#include <string>

namespace HairGL
{
    bool InitGL();
    uint32_t CompileShader(const std::string& version, const std::string& shaderSource, GLenum type, const std::string* includeSource = nullptr);
    uint32_t LinkProgram(uint32_t vertexShaderID, uint32_t tessControlShaderID, uint32_t tessEvaluationShaderID, uint32_t geometryShaderID, uint32_t fragmentShaderID);
    uint32_t LinkProgram(uint32_t vertexShaderID, uint32_t fragmentShaderID);
    uint32_t LinkProgram(uint32_t computeShaderID);
}

#endif
