layout(isolines) in;

layout (std140, binding = HAIR_DATA_BINDING) uniform HairDataBlock {
    HairRenderData hairData;
};

layout(std430, binding = POSITIONS_BUFFER_BINDING) buffer Positions {
    vec4 data[];
} positions;

layout(std430, binding = HAIR_INDICES_BUFFER_BINDING) buffer HairIndices {
    ivec4 data[];
} hairIndices;

patch in int triangleIndex;
patch in int segmentIndex;

layout(location = 0) out vec3 out_pos;
layout(location = 1) out vec3 out_tangent;
layout(location = 2) out float out_width;

vec3 getVertexPosition(int hairIndex, int vertexIndex)
{
    int index = hairIndex * (hairData.segmentsCount + 1) + clamp(vertexIndex, 0, hairData.segmentsCount);
	return positions.data[index].xyz;
}

ivec3 getHairIndices(int triangleIndex)
{
	return hairIndices.data[triangleIndex].xyz;
}

vec3 getControlPoint(ivec3 hairIndices, int vertexIndex, vec3 weights)
{
    vec3 position = vec3(0, 0, 0);
	position += getVertexPosition(hairIndices[0], vertexIndex) * weights[0];
	position += getVertexPosition(hairIndices[1], vertexIndex) * weights[1];
	position += getVertexPosition(hairIndices[2], vertexIndex) * weights[2];
	return position;
}

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 getBarycentricCoordinates()
{
    float u = rand(vec2(gl_TessCoord.y, 0.3));
	float v = rand(vec2(gl_TessCoord.y, -0.7));
	if(u + v > 1.0)
    {
        u = 1.0 - u;
        v = 1.0 - v;
    }
	return vec3(u, v, 1.0 - u - v);
}

float getHairCoordinate()
{
    return (segmentIndex + gl_TessCoord.x) / hairData.segmentsCount;
}

void main()
{
	ivec3 hairIndices = getHairIndices(triangleIndex);
	vec3 weights = getBarycentricCoordinates();

    vec3 p0 = getControlPoint(hairIndices, segmentIndex - 1, weights);
	vec3 p1 = getControlPoint(hairIndices, segmentIndex, weights);
	vec3 p2 = getControlPoint(hairIndices, segmentIndex + 1, weights);
	vec3 p3 = getControlPoint(hairIndices, segmentIndex + 2, weights);

	float u = gl_TessCoord.x;
	float u2 = u * u;
	float u3 = u2 * u;
	vec4 uVec = vec4(u3, u2, u, 1) / 6.0;
	mat4 coeffMatrix;
	coeffMatrix[0] = vec4(-1, 3, -3, 1);
	coeffMatrix[1] = vec4(3, -6, 0, 4);
	coeffMatrix[2] = vec4(-3, 3, 3, 1);
	coeffMatrix[3] = vec4(1, 0, 0, 0);
	vec4 bVec = uVec * coeffMatrix;

	out_pos = p0 * bVec[0] + p1 * bVec[1] + p2 * bVec[2] + p3 * bVec[3];

	float t = getHairCoordinate() - hairData.thinningStart;
	t = clamp(t, 0.0, 1.0);
	out_width = mix(hairData.rootWidth, hairData.tipWidth, t);

	vec3 tangentBottom = normalize(p2 - p1);
	vec3 tangentTop = p3 - p2;
	if(length(tangentTop) == 0)
	{
	    out_tangent = tangentBottom;
	}
	else 
	{
	    tangentTop = normalize(tangentTop);
	    out_tangent = mix(tangentBottom, tangentTop, u);
	}
}