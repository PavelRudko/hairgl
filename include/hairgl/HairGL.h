#ifndef HAIRGL_H
#define HAIRGL_H

#include "Math.h"
#include "HairTypes.h"
#include <stdint.h>

namespace HairGL
{
    class Renderer;
    class HairAsset;
    class HairInstance;

    class HairSystem
    {
    public:
        HairSystem();
        HairSystem(const HairSystem&) = delete;
        void Simulate(const HairInstance* instance, float timeStep = 1.0f / 60.0f) const;
        void Render(const HairInstance* instance, const Matrix4& viewMatrix, const Matrix4& projectionMatrix) const;
        HairAsset* LoadAsset(const char* path) const;
        void DestroyAsset(HairAsset* asset) const;
        HairInstance* CreateInstance(const HairAsset* asset) const;
        void UpdateInstanceSettings(HairInstance* instance, const HairInstanceSettings& settings) const;
        void DestroyInstance(HairInstance* instance) const;
        ~HairSystem();

    private:
        Renderer* renderer;
    };
}

#endif