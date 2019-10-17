#include "Renderer.h"
#include "gl/GLUtils.h"
#include <vector>
#include <algorithm>
#include <hairgl/Math.h>
#include "shaders/ShaderTypes.h"

namespace HairGL
{
    const std::string GLSLVersion = "#version 430 core\n";

    Renderer::Renderer() :
        guidesVisualizationProgramID(0),
        growthMeshVisualizationProgramID(0),
        simulationProgramID(0),
        hairRenderingProgramID(0),
        emptyVertexArrayID(0)
    {
        glGenVertexArrays(1, &emptyVertexArrayID);

        glGenBuffers(1, &hairDataBufferID);
        glBindBuffer(GL_UNIFORM_BUFFER, hairDataBufferID);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(HairRenderData), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glGenBuffers(1, &sceneDataBufferID);
        glBindBuffer(GL_UNIFORM_BUFFER, sceneDataBufferID);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(SceneRenderData), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glGenBuffers(1, &lightDataBufferID);
        glBindBuffer(GL_UNIFORM_BUFFER, lightDataBufferID);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(LightRenderData), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        shaderIncludeSrc = LoadFile("hairglshaders/ShaderTypes.h");

        guidesVisualizationProgramID = CreateGuidesVisualizationProgram();
        growthMeshVisualizationProgramID = CreateGrowthMeshVisualizationProgram();
        simulationProgramID = CreateSimulationProgram();
        hairRenderingProgramID = CreateHairRenderingProgram();
    }

    void Renderer::Simulate(HairInstance* instance, float timeStep) const
    {
        auto asset = instance->asset;

        glUseProgram(simulationProgramID);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, REST_POSITIONS_BUFFER_BINDING, asset->restPositionsBufferID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, POSITIONS_BUFFER_BINDING, instance->positionsBufferID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, PREVIOUS_POSITIONS_BUFFER_BINDING, instance->previousPositionsBufferID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, TANGENTS_DISTANCES_BINDING, asset->tangentsDistancesBufferID);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, REF_VECTORS_BINDING, asset->refVectorsBufferID);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GLOBAL_ROTATIONS_BINDING, asset->globalRotationsBufferID);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, DEBUG_BUFFER_BINDING, asset->debugBufferID);

        int verticesPerStrand = asset->segmentsCount + 1;
		auto windPyramid = CreateWindPyramid(instance->settings.wind, instance->simulationFrame);
        glUniformMatrix4fv(glGetUniformLocation(simulationProgramID, "modelMatrix"), 1, false, (float*)instance->settings.modelMatrix.m);
        glUniform1i(glGetUniformLocation(simulationProgramID, "verticesPerStrand"), verticesPerStrand);
        glUniform1f(glGetUniformLocation(simulationProgramID, "timeStep"), timeStep);
        glUniform1f(glGetUniformLocation(simulationProgramID, "globalStiffness"), instance->settings.globalStiffness);
		glUniform1f(glGetUniformLocation(simulationProgramID, "localStiffness"), (std::min)(instance->settings.localStiffness, 0.95f) * 0.5f);
        glUniform1f(glGetUniformLocation(simulationProgramID, "damping"), instance->settings.damping);
        glUniform3f(glGetUniformLocation(simulationProgramID, "gravity"), 0.0f, -9.8f, 0.0f);
        glUniform1i(glGetUniformLocation(simulationProgramID, "lengthConstraintIterations"), 5);
		glUniform1i(glGetUniformLocation(simulationProgramID, "localShapeIterations"), 10);
		glUniformMatrix4fv(glGetUniformLocation(simulationProgramID, "windPyramid"), 1, false, (float*)windPyramid.m);

        glDispatchCompute(instance->asset->guidesCount, 1, 1);
        glUseProgram(0);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		instance->simulationFrame++;
    }

    void Renderer::Render(const HairInstance* instance, const Matrix4& viewMatrix, const Matrix4& projectionMatrix) const
    {
        auto asset = instance->asset;
        auto settings = instance->settings;
        auto viewProjectionMatrix = projectionMatrix * viewMatrix;
        int verticesPerStrand = asset->segmentsCount + 1;

        glEnable(GL_DEPTH_TEST);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, REST_POSITIONS_BUFFER_BINDING, asset->restPositionsBufferID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, POSITIONS_BUFFER_BINDING, instance->positionsBufferID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, PREVIOUS_POSITIONS_BUFFER_BINDING, instance->previousPositionsBufferID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, HAIR_INDICES_BUFFER_BINDING, asset->hairIndicesBufferID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, TANGENTS_DISTANCES_BINDING, asset->tangentsDistancesBufferID);

        if (settings.visualizeGuides) {
            glUseProgram(guidesVisualizationProgramID);

            glUniformMatrix4fv(glGetUniformLocation(guidesVisualizationProgramID, "viewProjectionMatrix"), 1, false, (float*)viewProjectionMatrix.m);
            glUniform1i(glGetUniformLocation(guidesVisualizationProgramID, "doubleSegments"), asset->segmentsCount * 2);
            glUniform1i(glGetUniformLocation(guidesVisualizationProgramID, "verticesPerStrand"), verticesPerStrand);
            glUniform4f(glGetUniformLocation(guidesVisualizationProgramID, "color"), 1, 0, 0, 1);

            glBindVertexArray(emptyVertexArrayID);
            glDrawArrays(GL_LINES, 0, asset->guidesCount * asset->segmentsCount * 2);
            glUseProgram(0);
        }

        if (instance->settings.visualizeGrowthMesh) {
            glUseProgram(growthMeshVisualizationProgramID);

            glUniformMatrix4fv(glGetUniformLocation(growthMeshVisualizationProgramID, "viewProjectionMatrix"), 1, false, (float*)viewProjectionMatrix.m);
            glUniform1i(glGetUniformLocation(growthMeshVisualizationProgramID, "verticesPerStrand"), verticesPerStrand);
            glUniform4f(glGetUniformLocation(growthMeshVisualizationProgramID, "color"), 1, 1, 0, 1);

            glBindVertexArray(emptyVertexArrayID);
            glDrawArrays(GL_LINES, 0, asset->trianglesCount * 6);
            glUseProgram(0);
        }

        if (instance->settings.renderHair) {
            auto inversedViewMatrix = viewMatrix.EuclidianInversed();

            HairRenderData hairRenderData = {};
            hairRenderData.tesselationFactor = settings.tesselationFactor;
            hairRenderData.segmentsCount = instance->asset->segmentsCount;
            hairRenderData.rootWidth = settings.rootWidth;
            hairRenderData.tipWidth = settings.tipWidth;
            hairRenderData.density = settings.density;
            hairRenderData.color = settings.color;
            hairRenderData.ambient = settings.ambient;
            hairRenderData.diffuse = settings.diffuse;
            hairRenderData.specular = settings.specular;
            hairRenderData.specularPower = settings.specularPower;
            hairRenderData.thinningStart = settings.thinningStart;

            SceneRenderData sceneRenderData = {};
            sceneRenderData.viewProjectionMatrix = viewProjectionMatrix;
            sceneRenderData.eyePosition = inversedViewMatrix.m[3].XYZ();

            LightRenderData lightData = {};
            lightData.lightsCount = 1;
            lightData.lights[0].position = { 5, 5, 5 };
            lightData.lights[0].color = { 1, 1, 1, 1 };

            glBindBuffer(GL_UNIFORM_BUFFER, hairDataBufferID);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(HairRenderData), &hairRenderData, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            glBindBuffer(GL_UNIFORM_BUFFER, sceneDataBufferID);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(SceneRenderData), &sceneRenderData, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            glBindBuffer(GL_UNIFORM_BUFFER, lightDataBufferID);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(LightRenderData), &lightData, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, POSITIONS_BUFFER_BINDING, instance->positionsBufferID);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, HAIR_INDICES_BUFFER_BINDING, asset->hairIndicesBufferID);

            glUseProgram(hairRenderingProgramID);

            glBindBufferRange(GL_UNIFORM_BUFFER, HAIR_DATA_BINDING, hairDataBufferID, 0, sizeof(HairRenderData));
            glBindBufferRange(GL_UNIFORM_BUFFER, SCENE_DATA_BINDING, sceneDataBufferID, 0, sizeof(SceneRenderData));
            glBindBufferRange(GL_UNIFORM_BUFFER, LIGHT_DATA_BINDING, lightDataBufferID, 0, sizeof(LightRenderData));

            glPatchParameteri(GL_PATCH_VERTICES, 1);
            glDrawArrays(GL_PATCHES, 0, asset->trianglesCount * asset->segmentsCount);
            glUseProgram(0);
        }
    }

    uint32_t Renderer::CreateGuidesVisualizationProgram()
    {
        auto guidesVisualizationVertexShaderSource = LoadFile("hairglshaders/GuidesVisualization.vert");
        auto guidesVisualizationFragmentShaderSource = LoadFile("hairglshaders/SimpleColor.frag");
        uint32_t guidesVisualizationVertexShaderID = CompileShader(GLSLVersion, guidesVisualizationVertexShaderSource, GL_VERTEX_SHADER);
        uint32_t guidesVisualizationFragmentShaderID = CompileShader(GLSLVersion, guidesVisualizationFragmentShaderSource, GL_FRAGMENT_SHADER);
        return LinkProgram(guidesVisualizationVertexShaderID, guidesVisualizationFragmentShaderID);
    }

    uint32_t Renderer::CreateGrowthMeshVisualizationProgram()
    {
        auto growthMeshVisualizationVertexShaderSource = LoadFile("hairglshaders/GrowthMeshVisualization.vert");
        auto growthMeshVisualizationFragmentShaderSource = LoadFile("hairglshaders/SimpleColor.frag");
        uint32_t growthMeshVisualizationVertexShaderID = CompileShader(GLSLVersion, growthMeshVisualizationVertexShaderSource, GL_VERTEX_SHADER);
        uint32_t growthMeshVisualizationFragmentShaderID = CompileShader(GLSLVersion, growthMeshVisualizationFragmentShaderSource, GL_FRAGMENT_SHADER);
        return LinkProgram(growthMeshVisualizationVertexShaderID, growthMeshVisualizationFragmentShaderID);
    }

    uint32_t Renderer::CreateSimulationProgram()
    {
        auto simulationShaderSource = LoadFile("hairglshaders/Simulation.comp");
        uint32_t simulationShaderID = CompileShader(GLSLVersion, simulationShaderSource, GL_COMPUTE_SHADER, &shaderIncludeSrc);
        return LinkProgram(simulationShaderID);
    }

    uint32_t Renderer::CreateHairRenderingProgram()
    {
        auto hairVertexShaderSource = LoadFile("hairglshaders/Hair.vert");
        auto hairTessControlShaderSource = LoadFile("hairglshaders/Hair.tesc");
        auto hairTessEvaluationShaderSource = LoadFile("hairglshaders/Hair.tese");
        auto hairGeometrylShaderSource = LoadFile("hairglshaders/Hair.geom");
        auto hairFragmentShaderSource = LoadFile("hairglshaders/Hair.frag");

        uint32_t hairVertexShaderID = CompileShader(GLSLVersion, hairVertexShaderSource, GL_VERTEX_SHADER, &shaderIncludeSrc);
        uint32_t hairTessControlShaderID = CompileShader(GLSLVersion, hairTessControlShaderSource, GL_TESS_CONTROL_SHADER, &shaderIncludeSrc);
        uint32_t hairTessEvaluationShaderID = CompileShader(GLSLVersion, hairTessEvaluationShaderSource, GL_TESS_EVALUATION_SHADER, &shaderIncludeSrc);
        uint32_t hairGeometryShaderID = CompileShader(GLSLVersion, hairGeometrylShaderSource, GL_GEOMETRY_SHADER, &shaderIncludeSrc);
        uint32_t hairFragmentShaderID = CompileShader(GLSLVersion, hairFragmentShaderSource, GL_FRAGMENT_SHADER, &shaderIncludeSrc);

        uint32_t programID = LinkProgram(hairVertexShaderID, hairTessControlShaderID, hairTessEvaluationShaderID, hairGeometryShaderID, hairFragmentShaderID);

        glDeleteShader(hairVertexShaderID);
        glDeleteShader(hairTessControlShaderID);
        glDeleteShader(hairTessEvaluationShaderID);
        glDeleteShader(hairGeometryShaderID);
        glDeleteShader(hairFragmentShaderID);

        return programID;
    }

	Vector4 GetPyramidWindCorner(const Quaternion& rotationFromXToWind, const Vector3& axis, float angle, float magnitude)
	{
		Vector3 xAxis(1.0f, 0.0f, 0.0f);
		Quaternion rotation(axis, angle);
		auto side = rotationFromXToWind * rotation * xAxis * magnitude;
		return Vector4(side.x, side.y, side.z, 0.0f);
	}

	Matrix4 Renderer::CreateWindPyramid(const Vector3& wind, int frame) const
	{
		float magnitude = wind.Length();
		auto dir = wind / magnitude;
		//magnitude *= (pow(sinf(frame * 0.00001f), 2.0f) + 0.5f);

		Vector3 xAxis(1.0f, 0.0f, 0.0f);
		auto rotationAxis = Vector3::Cross(xAxis, dir);
		float angle = asin(rotationAxis.Length());

		Quaternion rotationFromXToWind;
		if (angle > 0.001)
		{
			rotationFromXToWind = Quaternion(rotationAxis.Normalized(), angle);
		}

		float coneAngle = 20.0f * DegToRad;

		Matrix4 pyramid;
		pyramid.m[0] = GetPyramidWindCorner(rotationFromXToWind, Vector3(0, 1, 0), coneAngle, magnitude);
		pyramid.m[1] = GetPyramidWindCorner(rotationFromXToWind, Vector3(0, -1, 0), coneAngle, magnitude);
		pyramid.m[2] = GetPyramidWindCorner(rotationFromXToWind, Vector3(0, 0, 1), coneAngle, magnitude);
		pyramid.m[3] = GetPyramidWindCorner(rotationFromXToWind, Vector3(0, 0, -1), coneAngle, magnitude);

		return pyramid;
	}

    Renderer::~Renderer()
    {
        glFinish();

        glDeleteProgram(guidesVisualizationProgramID);
        glDeleteProgram(hairRenderingProgramID);
        glDeleteProgram(simulationProgramID);
        glDeleteVertexArrays(1, &emptyVertexArrayID);
    }
}