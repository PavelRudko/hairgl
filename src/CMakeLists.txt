set(HAIRGL_SOURCE_FILES
	HairGL.cpp
	Math.cpp
	Renderer.cpp
	Common.cpp
	gl/gl3w.cpp 
	gl/GLUtils.cpp
)

set(HAIRGL_HEADER_FILES
	${HAIRGL_INCLUDE_DIR}/hairgl/HairGL.h
	${HAIRGL_INCLUDE_DIR}/hairgl/Math.h
	${HAIRGL_INCLUDE_DIR}/hairgl/HairTypes.h
	Renderer.h
	Common.h
	gl/GLUtils.h
	shaders/ShaderTypes.h
)

set(HAIRGL_SHADER_FILES
	shaders/GuidesVisualization.vert
	shaders/GrowthMeshVisualization.vert
	shaders/SimpleColor.frag
	shaders/Simulation.comp
	shaders/Hair.vert
	shaders/Hair.tesc
	shaders/Hair.tese
	shaders/Hair.geom
	shaders/Hair.frag
)

add_definitions(-DSHADER_CPP_INCLUDE)
add_library(hairgl STATIC ${HAIRGL_SOURCE_FILES} ${HAIRGL_HEADER_FILES})
target_include_directories(hairgl PUBLIC ${HAIRGL_INCLUDE_DIR})

add_custom_target(
	shaders
	COMMAND ${CMAKE_COMMAND}
	-E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "${OUTPUT_DIRECTORY}/hairglshaders"
	COMMENT "Copying shaders to output directory" VERBATIM
	SOURCES ${HAIRGL_SHADER_FILES} shaders/ShaderTypes.h
)

add_dependencies(hairgl shaders)