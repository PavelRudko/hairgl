#include <hairgl/HairGL.h>
#include <stdexcept>
#include <vector>
#include "gl/GLUtils.h"
#include "Renderer.h"

namespace HairGL
{
    HairSystem::HairSystem() :
        renderer(nullptr)
    {
        if (!InitGL()) {
            throw std::runtime_error("Cannot intitialize OpenGL resources.");
        }

        renderer = new Renderer();
    }

    void HairSystem::Simulate(const HairInstance* instance, float timeStep) const
    {
        renderer->Simulate(instance, timeStep);
    }

    void HairSystem::Render(const HairInstance* instance, const Matrix4& viewMatrix, const Matrix4& projectionMatrix) const
    {
        renderer->Render(instance, viewMatrix, projectionMatrix);
    }

    void CalculateConstraints(const std::vector<Vector4>& vertices, int numVerticesInSegment, std::vector<Vector4>& tangentsDistances)
    {
        tangentsDistances.resize(vertices.size());

        for (int guideIndex = 0; guideIndex < vertices.size() / numVerticesInSegment; guideIndex++) {
            for (int i = 0; i < numVerticesInSegment - 1; i++) {
                auto p0 = vertices[guideIndex * numVerticesInSegment + i];
                auto p1 = vertices[guideIndex * numVerticesInSegment + i + 1];

                tangentsDistances[guideIndex * numVerticesInSegment + i].w = (p1.XYZ() - p0.XYZ()).Length();
            }
        }
    }

    HairAsset* HairSystem::LoadAsset(const char* path) const
    {
        auto file = fopen(path, "rb");
        if (file == nullptr) {
            throw std::runtime_error(std::string("Cannot open file ") + path);
        }

        fseek(file, 0, SEEK_END);
        int size = ftell(file);
        fseek(file, 0, SEEK_SET);

        int guidesCount = 0;
        int segmentsCount = 0;
        int trianglesCount = 0;

        fread(&guidesCount, sizeof(guidesCount), 1, file);
        fread(&segmentsCount, sizeof(segmentsCount), 1, file);
        fread(&trianglesCount, sizeof(trianglesCount), 1, file);

        int numVerticesInSegment = segmentsCount + 1;
        std::vector<Vector4> vertices(guidesCount * numVerticesInSegment);
        for (int i = 0; i < vertices.size(); i++) {
            if (feof(file)) {
                throw std::runtime_error(std::string("Invalid hair asset file ") + path);
            }

            fread(&vertices[i], sizeof(float), 3, file);
            vertices[i].w = i % numVerticesInSegment == 0 ? 0 : 1;
        }

        std::vector<int> triangles(trianglesCount * 4, 0);
        for (int i = 0; i < trianglesCount; i++) {
            if (feof(file)) {
                throw std::runtime_error(std::string("Invalid hair asset file ") + path);
            }

            fread(&triangles[i * 4], sizeof(int), 3, file);
        }

        fclose(file);

        std::vector<Vector4> tangetsDistances;
        CalculateConstraints(vertices, numVerticesInSegment, tangetsDistances);

        auto asset = new HairAsset();
        asset->guidesCount = guidesCount;
        asset->segmentsCount = segmentsCount;
        asset->trianglesCount = trianglesCount;

        glGenBuffers(1, &asset->restPositionsBufferID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, asset->restPositionsBufferID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Vector4) * asset->guidesCount * (asset->segmentsCount + 1), vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &asset->hairIndicesBufferID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, asset->hairIndicesBufferID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, triangles.size() * sizeof(int), triangles.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &asset->tangentsDistancesBufferID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, asset->tangentsDistancesBufferID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, tangetsDistances.size() * sizeof(Vector4), tangetsDistances.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        return asset;
    }

    void HairSystem::DestroyAsset(HairAsset* asset) const
    {
        glDeleteBuffers(1, &asset->restPositionsBufferID);
        delete asset;
    }

    void CopyBuffer(uint32_t src, uint32_t dst, int size)
    {
        glBindBuffer(GL_COPY_READ_BUFFER, src);
        glBindBuffer(GL_COPY_WRITE_BUFFER, dst);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);
    }

    HairInstance* HairSystem::CreateInstance(const HairAsset* asset) const
    {
        auto instance = new HairInstance();
        instance->asset = asset;

        size_t positionsSize = sizeof(Vector4) * asset->guidesCount * (asset->segmentsCount + 1);

        glGenBuffers(1, &instance->positionsBufferID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, instance->positionsBufferID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, positionsSize, nullptr, GL_DYNAMIC_DRAW);

        glGenBuffers(1, &instance->previousPositionsBufferID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, instance->previousPositionsBufferID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, positionsSize, nullptr, GL_DYNAMIC_DRAW);

        CopyBuffer(asset->restPositionsBufferID, instance->positionsBufferID, positionsSize);
        CopyBuffer(asset->restPositionsBufferID, instance->previousPositionsBufferID, positionsSize);

        return instance;
    }

    void HairSystem::UpdateInstanceSettings(HairInstance* instance, const HairInstanceSettings& settings) const
    {
        instance->settings = settings;
    }

    void HairSystem::DestroyInstance(HairInstance* instance) const
    {
        glDeleteBuffers(1, &instance->positionsBufferID);
        delete instance;
    }

    HairSystem::~HairSystem()
    {
        delete renderer;
    }
}