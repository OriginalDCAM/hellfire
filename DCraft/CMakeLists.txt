set(LIBRARY_NAME DCraft)

# Collect source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

# Create the library
add_library(${LIBRARY_NAME} ${SOURCES} ${HEADERS})

# Check if the targets exist instead of using find_package
if (NOT TARGET GLEW::GLEW)
    message(FATAL_ERROR "GLEW target not found, check your root CMakeLists.txt")
endif ()

if (NOT TARGET GLUT::GLUT)
    message(FATAL_ERROR "GLUT target not found, check your root CMakeLists.txt")
endif ()

if (NOT TARGET GLM::GLM)
    message(FATAL_ERROR "GLM target not found, check your root CMakeLists.txt")
endif ()

# Set include directories
target_include_directories(${LIBRARY_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

find_package(OpenGL REQUIRED)

# Link dependencies
target_link_libraries(${LIBRARY_NAME}
        PUBLIC
        GLEW::GLEW
        GLUT::GLUT
        GLM::GLM
        OpenGL::GL
        imgui
        PRIVATE
        stb_image
)

# If on Windows, link against opengl32
if(WIN32)
    target_link_libraries(${LIBRARY_NAME}
            PUBLIC
            opengl32
    )
endif()

# Set Visual Studio folder structure
if (MSVC)
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SOURCES})
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADERS})
endif ()

install(TARGETS ${LIBRARY_NAME}
        EXPORT ${LIBRARY_NAME}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)

install(
        DIRECTORY include/
        DESTINATION include
)