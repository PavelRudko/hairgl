layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

layout (std140, binding = SCENE_DATA_BINDING) uniform SceneDataBlock {
    SceneRenderData sceneData;
};

layout(location = 0) in vec3 in_pos[];
layout(location = 1) in vec3 in_tangent[];
layout(location = 2) in float in_width[];

layout(location = 0) out vec3 out_pos;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv;

void emitProjected(vec3 position, vec3 offset)
{
    vec3 worldPosition = position + offset;
	out_pos = worldPosition;
	out_uv = vec2(0.0, 0.0);
	out_normal = normalize(offset);
    gl_Position = sceneData.viewProjectionMatrix * vec4(worldPosition, 1.0);
	EmitVertex();
}

void main() {
    vec3 eyeVecBottom = normalize(sceneData.eyePosition - in_pos[0]);
	vec3 eyeVecTop = normalize(sceneData.eyePosition - in_pos[1]);
	vec3 sideVecBottom = normalize(cross(eyeVecBottom, in_tangent[0])) * in_width[0] / 2.0;
	vec3 sideVecTop = normalize(cross(eyeVecTop, in_tangent[1])) * in_width[1] / 2.0;

	emitProjected(in_pos[0], sideVecBottom);
	emitProjected(in_pos[1], sideVecTop);
	emitProjected(in_pos[0], -sideVecBottom);
	emitProjected(in_pos[1], -sideVecTop);
}