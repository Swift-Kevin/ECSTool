cmake_minimum_required(VERSION 3.25)

set(PROJECT_NAME ECSTool)
project(${PROJECT_NAME})

# Collect PreCompiled Header files
set(PRE_COMPILED
	./Source/precompiled.h
	./ExternalAPI/gateware-main/Gateware.h
)


# Create list of source files.
# GLOB APIs
file(GLOB APIS CONFIGURE_DEPENDS 
	./ExternalAPI/entt-3.13.1/single_include/entt/entt.hpp
	./ExternalAPI/inifile-cpp-master/include/inicpp.h
	./ExternalAPI/TinyGLTF/*.*
	./ExternalAPI/imgui/*.*
)
# GLOB source files
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS ./Source/*.h)
file(GLOB_RECURSE CPPS CONFIGURE_DEPENDS ./Source/*.cpp)
file(GLOB_RECURSE HPPS CONFIGURE_DEPENDS ./Source/*.hpp)
file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS ./Shaders/*.hlsl)
file(GLOB_RECURSE INIS CONFIGURE_DEPENDS ./SaveFiles/*.ini)

# force unicode in some libraries on win32
ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

if (WIN32)
	# Set Startup as our project, not ALL_BUILD (the default) 
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
  	add_executable(${PROJECT_NAME}
        ${CPPS}
        ${HPPS}
        ${HEADERS}
        ${APIS}
    	${SHADER_FILES}
		${INIS}
	)	

	# Tell Visual Studio to put files in area
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    source_group(TREE ${CMAKE_SOURCE_DIR} FILES 
		${APIS} 
		${HEADERS} 
		${CPPS} 
		${HPPS} 
		${SHADER_FILES}
		${INIS}
	)

	target_include_directories(${PROJECT_NAME} PUBLIC $ENV{VULKAN_SDK}/Include/)
	target_link_directories(${PROJECT_NAME} PUBLIC $ENV{VULKAN_SDK}/Lib/)

	# shaderc_combined.lib in Vulkan requires this for debug & release (runtime shader compiling)
	target_compile_options(${PROJECT_NAME} PRIVATE "/MD")
endif(WIN32)

# EnTT requires C++17 support
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Define DEBUG only for Debug build type
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG>)

# Reduce Compile times with PCH
target_precompile_headers(${PROJECT_NAME} PRIVATE ${PRE_COMPILED})

# Disable all shaders from compiling since they may have Vulkan specifics
set_source_files_properties( ${SHADER_FILES} PROPERTIES
	VS_SHADER_TYPE Vertex # not all are vertex shaders, but that's ok here
    VS_SHADER_MODEL 5.1
    VS_SHADER_ENTRYPOINT main
    VS_TOOL_OVERRIDE "None" # stop VS from compiling, we will do it
)
