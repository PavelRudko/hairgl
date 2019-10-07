project(sample LANGUAGES CXX)

set (SAMPLE_SOURCE_FILES
	main.cpp
	Application.cpp
	CameraController.cpp
)

set (SAMPLE_HEADER_FILES
	Application.h
	CameraController.h
)

set (IMGUI_SOURCE_FILES 
	${IMGUI_DIR}/imgui.cpp
	${IMGUI_DIR}/imgui_draw.cpp
	${IMGUI_DIR}/imgui_demo.cpp
	${IMGUI_DIR}/imgui_widgets.cpp
	${IMGUI_DIR}/imgui_impl_opengl3.cpp
	${IMGUI_DIR}/imgui_impl_glfw.cpp
)

source_group(imgui FILES ${IMGUI_SOURCE_FILES})

add_executable(sample ${SAMPLE_SOURCE_FILES} ${SAMPLE_HEADER_FILES} ${IMGUI_SOURCE_FILES})
target_include_directories(sample PUBLIC ${OPENGL_INCLUDE_DIR} ${IMGUI_DIR})
target_link_libraries(sample PUBLIC glfw hairgl ${OPENGL_LIBRARIES})

set_target_properties(sample PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")

add_custom_target(
	sample_data
	COMMAND ${CMAKE_COMMAND}
	-E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/data" "${OUTPUT_DIRECTORY}/data"
	COMMENT "Copying sample data to output directory" VERBATIM
)

add_dependencies(sample sample_data)