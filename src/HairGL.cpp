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

    void HairSystem::Simulate(HairInstance* instance, float timeStep) const
    {
        renderer->Simulate(instance, timeStep);
    }

    void HairSystem::Render(const HairInstance* instance, const Matrix4& viewMatrix, const Matrix4& projectionMatrix) const
    {
        renderer->Render(instance, viewMatrix, projectionMatrix);
    }

    void CalculateConstraints(const std::vector<Vector4>& vertices, int verticesPerStrand, std::vector<Vector4>& tangentsDistances)
    {
        tangentsDistances.resize(vertices.size());

        for (int guideIndex = 0; guideIndex < vertices.size() / verticesPerStrand; guideIndex++) {
            for (int i = 0; i < verticesPerStrand - 1; i++) {
                auto p0 = vertices[guideIndex * verticesPerStrand + i];
                auto p1 = vertices[guideIndex * verticesPerStrand + i + 1];

                tangentsDistances[guideIndex * verticesPerStrand + i].w = (p1.XYZ() - p0.XYZ()).Length();
            }
        }
    }

	void CalculateRotations(const std::vector<Vector4>& vertices, int verticesPerStrand, std::vector<Quaternion>& globalRotations, std::vector<Vector4>& refVectors)
	{
		std::vector<Quaternion> localRotations(vertices.size());
		globalRotations.resize(vertices.size());
		refVectors.resize(vertices.size());

		for (int guideIndex = 0; guideIndex < vertices.size() / verticesPerStrand; guideIndex++) {
			int rootVertexIndex = guideIndex * verticesPerStrand;

			auto position = vertices[rootVertexIndex].XYZ();
			auto positionNext = vertices[rootVertexIndex + 1].XYZ();
			auto tangent = positionNext - position;
			auto xAxis = tangent.Normalized();
			auto zAxis = Vector3::Cross(xAxis, Vector3(1.0f, 0, 0));

			if (zAxis.Length() < 0.0001f) {
				zAxis = Vector3::Cross(xAxis, Vector3(0, 1.0f, 0));
			}

			zAxis.Normalize();
			auto yAxis = Vector3::Cross(zAxis, xAxis);

			Matrix3 r;
			r.m[0][0] = xAxis[0];
			r.m[0][1] = yAxis[0];
			r.m[0][2] = zAxis[0];
			r.m[1][0] = xAxis[1];
			r.m[1][1] = yAxis[1];
			r.m[1][2] = zAxis[1];
			r.m[2][0] = xAxis[2];
			r.m[2][1] = yAxis[2];
			r.m[2][2] = zAxis[2];

			globalRotations[rootVertexIndex] = localRotations[rootVertexIndex] = Quaternion::FromMatrix(r);

			for (int i = 1; i < verticesPerStrand; i++) {
				auto positionPrev = vertices[rootVertexIndex + i - 1].XYZ();
				position = vertices[rootVertexIndex + i].XYZ();
				tangent = position - positionPrev;
				auto tangentLocal = globalRotations[rootVertexIndex + i - 1].Inversed() * tangent;
				auto test = globalRotations[rootVertexIndex + i - 1] * tangentLocal;

				xAxis = tangentLocal.Normalized();
				Vector3 x(1.0f, 0, 0);
				auto rotationAxis = Vector3::Cross(x, xAxis);
				float angle = acos(Vector3::Dot(x, xAxis));

				if (abs(angle) > 0.001 && rotationAxis.Length2() > 0.001) {
					rotationAxis.Normalize();
					localRotations[rootVertexIndex + i] = Quaternion(rotationAxis, angle);
				}
				else {
					localRotations[rootVertexIndex + i] = Quaternion();
				}

				globalRotations[rootVertexIndex + i] = globalRotations[rootVertexIndex + i - 1] * localRotations[rootVertexIndex + i];
				refVectors[rootVertexIndex + i] = Vector4(tangentLocal.x, tangentLocal.y, tangentLocal.z, 0.0f);
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

        int verticesPerStrand = segmentsCount + 1;
        std::vector<Vector4> vertices(guidesCount * verticesPerStrand);
        for (int i = 0; i < vertices.size(); i++) {
            if (feof(file)) {
                throw std::runtime_error(std::string("Invalid hair asset file ") + path);
            }

            fread(&vertices[i], sizeof(float), 3, file);
            vertices[i].w = i % verticesPerStrand == 0 ? 0 : 1;
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
        CalculateConstraints(vertices, verticesPerStrand, tangetsDistances);

		std::vector<Vector4> refVectors;
		std::vector<Quaternion> globalRotations;
		CalculateRotations(vertices, verticesPerStrand, globalRotations, refVectors);

        auto asset = new HairAsset();
        asset->guidesCount = guidesCount;
        asset->segmentsCount = segmentsCount;
        asset->trianglesCount = trianglesCount;

        glGenBuffers(1, &asset->restPositionsBufferID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, asset->restPositionsBufferID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, vertices.size() * sizeof(Vector4), vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &asset->hairIndicesBufferID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, asset->hairIndicesBufferID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, triangles.size() * sizeof(int), triangles.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &asset->tangentsDistancesBufferID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, asset->tangentsDistancesBufferID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, tangetsDistances.size() * sizeof(Vector4), tangetsDistances.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &asset->refVectorsBufferID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, asset->refVectorsBufferID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, refVectors.size() * sizeof(Vector4), refVectors.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &asset->globalRotationsBufferID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, asset->globalRotationsBufferID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, globalRotations.size() * sizeof(Quaternion), globalRotations.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &asset->debugBufferID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, asset->debugBufferID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, vertices.size() * sizeof(Vector4), nullptr, GL_STATIC_DRAW);

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