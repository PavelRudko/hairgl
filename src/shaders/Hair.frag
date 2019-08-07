layout (std140, binding = HAIR_DATA_BINDING) uniform HairDataBlock {
    HairRenderData hairData;
};

layout (std140, binding = LIGHT_DATA_BINDING) uniform LightDataBlock {
    LightRenderData lightData;
};

layout (std140, binding = SCENE_DATA_BINDING) uniform SceneDataBlock {
    SceneRenderData sceneData;
};

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

out vec4 out_color;

void main() {
    Light light = lightData.lights[0];
	vec4 result = hairData.color * hairData.ambient;
	vec3 lightDir = normalize(light.position - in_pos);
	vec3 reflectedDir = reflect(lightDir, in_normal);
	vec3 eyeDir = normalize(in_pos - sceneData.eyePosition);
	result += clamp(dot(lightDir, in_normal), 0.0, 1.0) * hairData.diffuse * light.color * hairData.color;
	result += (clamp(pow(dot(eyeDir, reflectedDir), hairData.specularPower), 0.0, 1.0)) * hairData.specular * light.color * hairData.color;
	result.w = hairData.color.w;
	out_color = result;
}