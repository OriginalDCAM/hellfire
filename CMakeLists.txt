cmake_minimum_required(VERSION 3.14...3.29)
project(Hellfire VERSION 1.1)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include FetchContent module
include(FetchContent)

# If we have multiple cores, use them for building
include(ProcessorCount)
ProcessorCount(N)
if (NOT N EQUAL 0)
    set(CMAKE_BUILD_PARALLEL_LEVEL ${N})
endif ()

# Use the cache for repeat builds
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# Set minimum policy version for dependencies to avoid CMake version errors
set(CMAKE_POLICY_DEFAULT_CMP0000 NEW)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# Fetch GLEW using a more reliable CMake-native approach
FetchContent_Declare(
        glew
        URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip
        URL_HASH SHA256=a9046a913774395a095edcc0b0ac2d81c3aacca61787b39839b941e9be14e0d4
)

FetchContent_GetProperties(glew)
if(NOT glew_POPULATED)
    FetchContent_MakeAvailable(glew)

    # Create GLEW library manually
    add_library(libglew_static STATIC
            ${glew_SOURCE_DIR}/src/glew.c
    )

    target_include_directories(libglew_static PUBLIC
            ${glew_SOURCE_DIR}/include
    )

    target_compile_definitions(libglew_static PUBLIC
            GLEW_STATIC
            GLEW_NO_GLU
    )

    if(WIN32)
        target_link_libraries(libglew_static PUBLIC opengl32)
    else()
        target_link_libraries(libglew_static PUBLIC GL)
    endif()

    # Set target properties
    set_target_properties(libglew_static PROPERTIES
            OUTPUT_NAME "glew32s"
            PREFIX ""
    )
endif()

# Fetch GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

# Configure GLFW build options
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

# Fetch Assimp
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG master

)

# Assimp build options
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_MINIZIP OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_IFC_IMPORTER OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_UNITY_BUILD OFF CACHE BOOL "" FORCE)

set(CMAKE_CXX_FLAGS_OLD ${CMAKE_CXX_FLAGS})
string(REPLACE "-Werror" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Enable PCH for faster build
set(ASSIMP_BUILD_PCH ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)



# Create GLM target from vendor directory
add_library(GLM::GLM INTERFACE IMPORTED)
set_target_properties(GLM::GLM PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm"
)

enable_testing()

# Set up Catch2
Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0)
FetchContent_MakeAvailable(Catch2)

# Include Catch2's CTest integration
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

# Add the vendor directory to the include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor)

# Add the stb_image library
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb)

# Add the json library
add_library(json INTERFACE)
target_include_directories(json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/nlohmann)

# Add the imgui library
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)
add_library(imgui STATIC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${glew_SOURCE_DIR}/include # GLEW headers
)

# Link against OpenGL, GLEW, and GLUT using the fetched libraries
target_link_libraries(imgui PUBLIC
        libglew_static  # GLEW static library from FetchContent
        glfw
        ${CMAKE_DL_LIBS}
)

if(WIN32)
    target_link_libraries(imgui PUBLIC opengl32)
elseif(APPLE)
    target_link_libraries(imgui PUBLIC "-framework OpenGL")
else()
    target_link_libraries(imgui PUBLIC GL)
endif()



add_subdirectory(Editor)
add_subdirectory(engine)
#add_subdirectory(sandbox)
add_subdirectory(tests)

find_package(Doxygen)

if(Doxygen_FOUND)
    # Fetch doxygen-awesome-css
    include(FetchContent)
    FetchContent_Declare(
            doxygen-awesome-css
            URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
    )
    FetchContent_MakeAvailable(doxygen-awesome-css)
    FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

    # Doxygen config
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_MAN NO)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs)

    # Basic settings
    set(DOXYGEN_RECURSIVE YES)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_EXTRACT_PRIVATE YES)
    set(DOXYGEN_EXTRACT_STATIC YES)
    set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)

    # Doxygen Awesome CSS Configuration
    set(DOXYGEN_GENERATE_TREEVIEW YES)  # Required for sidebar
    set(DOXYGEN_DISABLE_INDEX NO)
    set(DOXYGEN_FULL_SIDEBAR NO)
    set(DOXYGEN_HTML_COLORSTYLE "LIGHT")
    set(DOXYGEN_HTML_EXTRA_STYLESHEET
            "${AWESOME_CSS_DIR}/doxygen-awesome.css"
            "${AWESOME_CSS_DIR}/doxygen-awesome-sidebar-only.css"
            "${AWESOME_CSS_DIR}/doxygen-awesome-sidebar-only-darkmode-toggle.css"
            "${CMAKE_CURRENT_SOURCE_DIR}/doc/custom.css"
    )
    set(DOXYGEN_HTML_EXTRA_FILES
            "${AWESOME_CSS_DIR}/doxygen-awesome-darkmode-toggle.js"
            "${AWESOME_CSS_DIR}/doxygen-awesome-fragment-copy-button.js"
            "${AWESOME_CSS_DIR}/doxygen-awesome-paragraph-link.js"
            "${AWESOME_CSS_DIR}/doxygen-awesome-interactive-toc.js"
    )

    set(DOXYGEN_IMAGE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/images)
  
    # Enhanced HTML output settings for better appearance
    set(DOXYGEN_HTML_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/doc/header.html" CACHE STRING "" FORCE)
    set(DOXYGEN_PROJECT_NAME "🔥 Hellfire Game Engine")
    set(DOXYGEN_PROJECT_BRIEF "A modern C++ game engine built with OpenGL")
#    set(DOXYGEN_PROJECT_LOGO "") # Add path to your logo if you have one

    # Better code highlighting
    set(DOXYGEN_SOURCE_BROWSER YES)
    set(DOXYGEN_VERBATIM_HEADERS YES)
    set(DOXYGEN_CLANG_ASSISTED_PARSING YES)
    set(DOXYGEN_CLANG_DATABASE_PATH "${CMAKE_BINARY_DIR}")

    # Class diagrams and graphs
    set(DOXYGEN_HAVE_DOT YES)
    set(DOXYGEN_CLASS_GRAPH YES)
    set(DOXYGEN_COLLABORATION_GRAPH YES)
    set(DOXYGEN_UML_LOOK YES)
    set(DOXYGEN_TEMPLATE_RELATIONS YES)
    set(DOXYGEN_DOT_IMAGE_FORMAT svg)
    set(DOXYGEN_INTERACTIVE_SVG YES)

    # Modern navigation
    set(DOXYGEN_SEARCHENGINE YES)
    set(DOXYGEN_SERVER_BASED_SEARCH NO)

    doxygen_add_docs(
            project_docs
            ${CMAKE_CURRENT_SOURCE_DIR}/README.md
            ${CMAKE_CURRENT_SOURCE_DIR}/Editor
            ${CMAKE_CURRENT_SOURCE_DIR}/engine
            ${CMAKE_CURRENT_SOURCE_DIR}/tests
            COMMENT "Generating documentation with Doxygen"
    )

    message(STATUS "Doxygen found: Documentation target 'project_docs' available")
    message(STATUS "Doxygen Awesome CSS: ${AWESOME_CSS_DIR}")
else()
    message(STATUS "Doxygen not found: Documentation will not be built")
endif()

# Configure release build optimizations after target creation
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Apply IPO to main targets
    set_property(TARGET imgui PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)

    # Create directories and dummy exe at configure time
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Sandbox/Debug")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Sandbox/Release")

    # Create dummy exe to prevent VS errors 
    set(APP_NAME "CustomEngine")
    set(DUMMY_EXE "${CMAKE_BINARY_DIR}/Sandbox/Debug/${APP_NAME}.exe")
    if(NOT EXISTS ${DUMMY_EXE})
        file(WRITE "${DUMMY_EXE}.temp" "")
        file(RENAME "${DUMMY_EXE}.temp" "${DUMMY_EXE}")
    endif()
endif()