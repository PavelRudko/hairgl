#define POSITIONS_BUFFER_BINDING 3

layout(std430, binding = POSITIONS_BUFFER_BINDING) buffer Positions
{
    vec4 data[];
} positions;

uniform mat4 viewProjectionMatrix;
uniform int doubleSegments;
uniform int verticesPerStrand;

void main()
{
    int guideIndex = gl_VertexID / doubleSegments;
	int lineVertexIndex = gl_VertexID % doubleSegments;
	int vertexIndex = lineVertexIndex / 2 + lineVertexIndex % 2;

	vec4 position = positions.data[guideIndex * verticesPerStrand + vertexIndex];

	gl_Position = viewProjectionMatrix * vec4(position.xyz, 1.0);
}