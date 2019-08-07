#ifndef HAIRGL_SHADER_TYPES_H
#define HAIRGL_SHADER_TYPES_H

#ifdef SHADER_CPP_INCLUDE
#include <hairgl/Math.h>
#define mat4 HairGL::Matrix4
#define vec4 HairGL::Vector4
#define vec3 HairGL::Vector3
#endif

#define MAX_LIGHTS 16
#define HAIR_DATA_BINDING 0
#define SCENE_DATA_BINDING 1
#define LIGHT_DATA_BINDING 2
#define POSITIONS_BUFFER_BINDING 3
#define HAIR_INDICES_BUFFER_BINDING 4
#define PREVIOUS_POSITIONS_BUFFER_BINDING 5
#define REST_POSITIONS_BUFFER_BINDING 6
#define TANGENTS_DISTANCES_BINDING 7

struct HairRenderData
{
    //GENERAL
    int segmentsCount;
    float tesselationFactor;
    float density;
    float _padding0;

    //SHAPE
    float rootWidth;
    float tipWidth;
    float thinningStart;
    float _padding1;

    //MATERIAL
    float specular;
    float diffuse;
    float ambient;
    float specularPower;
    vec4 color;
};

struct SceneRenderData
{
    mat4 viewProjectionMatrix;
    vec3 eyePosition;
    float _padding0;
};

struct Light
{
    vec4 color;
    vec3 position;
    float _padding;
};

struct LightRenderData
{
    Light lights[MAX_LIGHTS];
    int lightsCount;
    int _padding0;
    int _padding1;
    int _padding2;
};

#endif
