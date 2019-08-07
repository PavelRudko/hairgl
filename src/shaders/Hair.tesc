layout (vertices = 2) out;

layout (std140, binding = HAIR_DATA_BINDING) uniform HairDataBlock {
    HairRenderData hairData;
};

patch out int triangleIndex;
patch out int segmentIndex;

void main()
{
	if(gl_InvocationID == 0) {
        gl_TessLevelOuter[0] = hairData.density;
        gl_TessLevelOuter[1] = hairData.tesselationFactor;
		triangleIndex = gl_PrimitiveID / hairData.segmentsCount;
	    segmentIndex = gl_PrimitiveID % hairData.segmentsCount;
    }
}