cmake_minimum_required(VERSION 3.14)
project(FinalAssignment VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Configure paths to external libraries
set(GLEW_ROOT_DIR "C:/Libraries/glew-2.0.0-win32/glew-2.0.0" CACHE PATH "GLEW root directory")
set(GLUT_ROOT_DIR "C:/Libraries/freeglut-MSVC-3.0.0-2.mp/freeglut" CACHE PATH "GLUT root directory")
# For GLM, set to the parent directory of the first glm folder
set(GLM_ROOT_DIR "C:/Libraries/glm-0.9.6.3/glm" CACHE PATH "GLM root directory")

include(ProcessorCount)
ProcessorCount(N)
if (NOT N EQUAL 0)
    set(CMAKE_BUILD_PARALLEL_LEVEL ${N})
endif ()

# Add these paths to CMAKE_PREFIX_PATH to help find_package find them
list(APPEND CMAKE_PREFIX_PATH ${GLEW_ROOT_DIR} ${GLUT_ROOT_DIR} ${GLM_ROOT_DIR})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Create GLM target manually - note the root directory is one level up from where glm is
if (NOT TARGET GLM::GLM AND EXISTS "${GLM_ROOT_DIR}")
    add_library(GLM::GLM INTERFACE IMPORTED)
    set_target_properties(GLM::GLM PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${GLM_ROOT_DIR}"
    )
    set(GLM_FOUND TRUE)
endif ()

# Define GLEW target
if (NOT TARGET GLEW::GLEW AND EXISTS "${GLEW_ROOT_DIR}")
    set(GLEW_INCLUDE_DIR "${GLEW_ROOT_DIR}/include")
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)  # 64-bit
        set(GLEW_LIBRARY "${GLEW_ROOT_DIR}/lib/Release/x64/glew32.lib")
    else ()  # 32-bit
        set(GLEW_LIBRARY "${GLEW_ROOT_DIR}/lib/Release/Win32/glew32.lib")
    endif ()

    add_library(GLEW::GLEW UNKNOWN IMPORTED)
    set_target_properties(GLEW::GLEW PROPERTIES
            IMPORTED_LOCATION "${GLEW_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIR}"
    )
endif ()

# Define GLUT target
if (NOT TARGET GLUT::GLUT AND EXISTS "${GLUT_ROOT_DIR}")
    set(GLUT_INCLUDE_DIR "${GLUT_ROOT_DIR}/include")
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)  # 64-bit
        set(GLUT_LIBRARY "${GLUT_ROOT_DIR}/lib/x64/freeglut.lib") # Adjust this path as needed
    else ()  # 32-bit
        set(GLUT_LIBRARY "${GLUT_ROOT_DIR}/lib/freeglut.lib") # Adjust this path as needed
    endif ()

    add_library(GLUT::GLUT UNKNOWN IMPORTED)
    set_target_properties(GLUT::GLUT PROPERTIES
            IMPORTED_LOCATION "${GLUT_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${GLUT_INCLUDE_DIR}"
    )
endif ()

# Add the vendor directory to the include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor)

# Add the stb_image library
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb)

# Add the json library
add_library(json INTERFACE)
target_include_directories(json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/nlohmann)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)
add_library(imgui STATIC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glut.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${GLEW_INCLUDE_DIR}
        ${GLUT_INCLUDE_DIR}
)

target_link_libraries(imgui PUBLIC
        opengl32
        GLEW::GLEW
        GLUT::GLUT
)

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)  # Build zlib with Assimp
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)  # Only build needed importers

# Select only the importers you need to reduce build time
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_UNITY_BUILD ON CACHE BOOL "" FORCE)

# Use the cache for repeat builds
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

set(CMAKE_CXX_FLAGS_OLD ${CMAKE_CXX_FLAGS})
string(REPLACE "-Werror" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Enable PCH for faster build
set(ASSIMP_BUILD_PCH ON CACHE BOOL "" FORCE)

add_subdirectory(vendor/assimp)

add_subdirectory(DCraft)
add_subdirectory(Sandbox)
