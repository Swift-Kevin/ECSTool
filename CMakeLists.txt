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
file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS ./Shaders/*.hlsl)

# force unicode in some libraries on win32
ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

if (WIN32)
	# Set Startup as our project, not ALL_BUILD (the default) 
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
  	add_executable(${PROJECT_NAME}
        ${CPPS}
        ${HEADERS}
        ${APIS}
    	${SHADER_FILES}
	)	

	# Tell Visual Studio to put files in area
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    source_group(TREE ${CMAKE_SOURCE_DIR} FILES 
		${APIS} 
		${HEADERS} 
		${CPPS} 
		${SHADER_FILES}
	)

	target_include_directories(${PROJECT_NAME} PUBLIC $ENV{VULKAN_SDK}/Include/)
	target_link_directories(${PROJECT_NAME} PUBLIC $ENV{VULKAN_SDK}/Lib/)

	# shaderc_combined.lib in Vulkan requires this for debug & release (runtime shader compiling)
	target_compile_options(${PROJECT_NAME} PRIVATE "/MD")

endif(WIN32)

# WHEN I TEST APPLE / UNIX THEN I WILL COME BACK TO THIS
# LETS HOPE I DONT FORGET!!

# if(UNIX AND NOT APPLE)
# 	# libshaderc_combined.a is required for runtime shader compiling
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lX11 -lshaderc_combined")
#     find_package(X11)
# 	find_package(Vulkan REQUIRED)
#     link_libraries(${X11_LIBRARIES})
#     include_directories(${X11_INCLUDE_DIR})
#     include_directories(${Vulkan_INCLUDE_DIR}) 
# 	#link_directories(${Vulkan_LIBRARY}) this is currently not working
# 	link_libraries(${Vulkan_LIBRARIES})
# 	# the path is (properly)hardcoded because "${Vulkan_LIBRARY}" currently does not 
# 	# return a proper path on MacOS (it has the .dynlib appended)
#     link_libraries(/usr/lib/x86_64-linux-gnu/libshaderc_combined.a)
#     add_executable (${PROJECT_NAME} ${SOURCE_FILES})
#     source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SOURCE_FILES})
# endif(UNIX AND NOT APPLE)

# if(APPLE)
# 	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fmodules -fcxx-modules")
# 	set(Architecture ${CMAKE_OSX_ARCHITECTURES})
# 	find_package(Vulkan REQUIRED)
# 	include_directories(${Vulkan_INCLUDE_DIR}) 
# 	#link_directories(${Vulkan_LIBRARY}) this is currently not working
# 	link_libraries(${Vulkan_LIBRARIES})
# 	# libshaderc_combined.a is required for runtime shader compiling
# 	# the path is (properly)hardcoded because "${Vulkan_LIBRARY}" currently does not 
# 	# return a proper path on MacOS (it has the .dynlib appended)
# 	link_libraries(/usr/local/lib/libshaderc_combined.a)
# 	add_executable (${PROJECT_NAME} source/main.mm)
# endif(APPLE)

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
