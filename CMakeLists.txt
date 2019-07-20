cmake_minimum_required(VERSION 3.12.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# This project is strictly C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ensure CMAKE_BUILD_TYPE is set correctly.
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set up C++ project and executable
project(ygopen-gui CXX)
add_executable(ygopen-gui)

# Find all dependencies
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

option(YGOPEN_GUI_USE_SYSTEM_JSON "Use system's JSON for Modern C++" ON)
if(YGOPEN_GUI_USE_SYSTEM_JSON)
	find_package(nlohmann_json REQUIRED)
else()
	set(JSON_BuildTests OFF CACHE INTERNAL "")
	add_subdirectory(lib/json)
endif()

option(YGOPEN_GUI_USE_SYSTEM_FMT "Use system's {fmt} library" ON)
if(YGOPEN_GUI_USE_SYSTEM_FMT)
	find_package(fmt REQUIRED)
else()
	add_subdirectory(lib/fmt) # TODO: test needed
endif()

find_package(glm REQUIRED)

add_subdirectory(lib/ocgcore-proto)
add_subdirectory(lib/ygopen)

# Add include directories from dependencies to executable
target_include_directories(ygopen-gui PRIVATE
	${SDL2_INCLUDE_DIRS}
	${SDL2_IMAGE_INCLUDE_DIR}
	${SDL2_TTF_INCLUDE_DIR}
)

# Add libraries from dependencies to executable
if(WIN32)
	set(SDL2_LIBRARIES SDL2::SDL2 SDL2::SDL2main)
endif()
target_link_libraries(ygopen-gui PRIVATE
	${SDL2_LIBRARIES}
	${SDL2_IMAGE_LIBRARIES}
	${SDL2_TTF_LIBRARIES}
	glm
	nlohmann_json::nlohmann_json
	fmt::fmt
	ocgcore-proto
	ygopen
)

# Add compiler definitions to executable
target_compile_definitions(ygopen-gui PUBLIC
	GLM_FORCE_CXX17
	GLM_FORCE_INLINE
	GLM_FORCE_SIZE_T_LENGTH
	GLM_FORCE_INTRINSICS
	GLM_ENABLE_EXPERIMENTAL
)

# Add compiler flags to executable
set(CLANG_OR_GNU $<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>)
target_compile_options(ygopen-gui PRIVATE
	# Clang and GNU compilers
	$<${CLANG_OR_GNU}:-Wall -Wextra -pedantic>
	$<$<AND:$<CONFIG:Debug>,${CLANG_OR_GNU}>:-ggdb3 -Og -D_DEBUG>
	$<$<AND:$<CONFIG:Release>,${CLANG_OR_GNU}>:-O2>
	$<$<AND:$<CONFIG:RelWithDebInfo>,${CLANG_OR_GNU}>:-ggdb3 -O2 -D_DEBUG>
	# Microsoft compiler
	$<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Source files
add_subdirectory(src)
