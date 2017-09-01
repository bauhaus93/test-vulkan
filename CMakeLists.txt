cmake_minimum_required(VERSION 3.7.2)
project(test-vulkan CXX)

macro(add_sources)
	file (RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

	foreach(_src ${ARGN})
		if (${_relPath} MATCHES "/shaders$")
			set_property(TARGET shaders APPEND PROPERTY SHADER_SRC_LIST "${_relPath}/${_src}")
		elseif(_relPath)
			set_property(TARGET test-vulkan APPEND PROPERTY SRC_LIST "${_relPath}/${_src}")
		else()
			set_property(TARGET test-vulkan APPEND PROPERTY SRC_LIST "${_src}")
		endif()

	endforeach()
endmacro()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(COMPILE_FLAGS "-std=c++14 -Wall -fmax-errors=1 -ggdb")
	set(LINK_FLAGS "-std=c++14 -Wall -fmax-errors=1 -ggdb")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(COMPILE_FLAGS "-std=c++14 -Wall -ferror-limit=1")
		set(LINK_FLAGS "--std=c++14 -Wall -ferror-limit=1")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(COMPILE_FLAGS "/W4 /std:c++latest /MP")
	set(LINK_FLAGS "/NODEFAULTLIB:msvcrtd.lib")
endif()

#set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINK_FLAGS}")

add_executable(test-vulkan "")
add_custom_target(shaders)
add_dependencies(test-vulkan shaders)

add_subdirectory(src)

get_property(SRCS TARGET test-vulkan PROPERTY SRC_LIST)
target_sources(test-vulkan PRIVATE ${SRCS})

get_property(SHADER_SRCS TARGET shaders PROPERTY SHADER_SRC_LIST)

foreach(_shader ${SHADER_SRCS})
	string(REGEX MATCH "[a-z]+$" _filename ${_shader})
	add_custom_command(TARGET shaders POST_BUILD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND glslangValidator -V ${_shader} -o "${CMAKE_BINARY_DIR}/${_filename}.spv")
endforeach()

include_directories(src)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(test-vulkan glfw)
target_link_libraries(test-vulkan Vulkan::Vulkan)

if (UNIX)
    include_directories(/usr/include)
endif (UNIX)
