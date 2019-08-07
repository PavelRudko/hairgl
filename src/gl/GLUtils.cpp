#include "GLUtils.h"
#include <stdexcept>
#include <stdio.h>
#include <vector>

namespace HairGL
{
    constexpr uint32_t MaxLogSize = 1024;

    bool InitGL()
    {
        if (gl3wInit()) {
            return false;
        }
        if (!gl3wIsSupported(4, 0)) {
            return false;
        }
        return true;
    }

    uint32_t CompileShader(const std::string& version, const std::string& shaderSource, GLenum type, const std::string* includeSource)
    {
        std::vector<const char*> sources;
        std::vector<int> sizes;

        sources.push_back(version.data());
        sizes.push_back(version.length());

        if (includeSource) {
            sources.push_back(includeSource->data());
            sizes.push_back(includeSource->length());
        }

        sources.push_back(shaderSource.data());
        sizes.push_back(shaderSource.length());

        uint32_t shaderID = glCreateShader(type);

        glShaderSource(shaderID, sources.size(), sources.data(), sizes.data());
        glCompileShader(shaderID);

        int compileStatus;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);

        if (compileStatus == GL_FALSE) {
            static char log[MaxLogSize + 1];
            int logSize;
            glGetShaderInfoLog(shaderID, MaxLogSize, &logSize, log);
            log[logSize] = '\0';

            throw std::runtime_error(log);
        }

        return shaderID;
    }

    uint32_t LinkProgram(const uint32_t* shaderIDs, uint32_t stagesCount)
    {
        uint32_t programID = glCreateProgram();
        for (int i = 0; i < stagesCount; i++) {
            glAttachShader(programID, shaderIDs[i]);
        } 
        glLinkProgram(programID);

        int linkStatus;
        glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);

        if (linkStatus == GL_FALSE) {
            static char log[MaxLogSize + 1];
            int logSize;
            glGetProgramInfoLog(programID, MaxLogSize, &logSize, log);
            log[logSize] = '\0';

            throw std::runtime_error(std::string("Error in linking program:\n") + log);
        }

        return programID;
    }

    uint32_t LinkProgram(uint32_t vertexShaderID, uint32_t tessControlShaderID, uint32_t tessEvaluationShaderID, uint32_t geometryShaderID, uint32_t fragmentShaderID)
    {
        uint32_t shaderIDs[5] = { vertexShaderID, tessControlShaderID, tessEvaluationShaderID, geometryShaderID, fragmentShaderID };
        return LinkProgram(shaderIDs, 5);
    }

    uint32_t LinkProgram(uint32_t vertexShaderID, uint32_t fragmentShaderID)
    {
        uint32_t shaderIDs[2] = { vertexShaderID, fragmentShaderID };
        return LinkProgram(shaderIDs, 2);
    }

    uint32_t LinkProgram(uint32_t computeShaderID)
    {
        return LinkProgram(&computeShaderID, 1);
    }
}