#ifndef HAIRGL_RENDERER_H
#define HAIRGL_RENDERER_H

#include <stdint.h>
#include <hairgl/Math.h>
#include "Common.h"

namespace HairGL
{
    class Renderer
    {
    public:
        Renderer();
        Renderer(const Renderer&) = delete;
        void Simulate(HairInstance* instance, float timeStep) const;
        void Render(const HairInstance* instance, const Matrix4& viewMatrix, const Matrix4& projectionMatrix) const;
        ~Renderer();

    private:
        uint32_t emptyVertexArrayID;

        uint32_t guidesVisualizationProgramID;
        uint32_t growthMeshVisualizationProgramID;
        uint32_t simulationProgramID;
        uint32_t hairRenderingProgramID;

        uint32_t hairDataBufferID;
        uint32_t sceneDataBufferID;
        uint32_t lightDataBufferID;

        uint32_t CreateGuidesVisualizationProgram();
        uint32_t CreateGrowthMeshVisualizationProgram();
        uint32_t CreateSimulationProgram();
        uint32_t CreateHairRenderingProgram();
		Matrix4 CreateWindPyramid(const Vector3& wind, int frame) const;

        std::string shaderIncludeSrc;
    };
}

#endif