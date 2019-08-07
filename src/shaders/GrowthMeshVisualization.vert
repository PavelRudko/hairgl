#define POSITIONS_BUFFER_BINDING 3
#define HAIR_INDICES_BUFFER_BINDING 4

layout(std430, binding = POSITIONS_BUFFER_BINDING) buffer Positions
{
    vec4 data[];
} positions;

layout(std430, binding = HAIR_INDICES_BUFFER_BINDING) buffer HairIndices {
    ivec4 data[];
} hairIndices;

uniform mat4 viewProjectionMatrix;
uniform int verticesPerStrand;

const int TRIANGLE_BREAKDOWN[6] = int[6](0, 1, 1, 2, 2, 0);

layout(location = 0) out vec4 out_uv;

void main()
{
    int triangleIndex = gl_VertexID / 6;
	int vertexIndex = TRIANGLE_BREAKDOWN[gl_VertexID % 6];
	
	int hairIndex = hairIndices.data[triangleIndex][vertexIndex];
	vec4 position = positions.data[hairIndex * verticesPerStrand];

	gl_Position = viewProjectionMatrix * vec4(position.xyz, 1.0);

	out_uv = vec4(hairIndices.data[triangleIndex].xyz, float(hairIndex));
}