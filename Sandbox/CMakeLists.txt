set(APP_NAME Sandbox)

file(GLOB_RECURSE APP_SOURCES "src/*.cpp")
file(GLOB_RECURSE APP_HEADERS "include/*.h" "include/*.hpp")

add_executable(${APP_NAME} ${APP_SOURCES} ${APP_HEADERS})

target_include_directories(${APP_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link against your DCraft library
target_link_libraries(${APP_NAME}
    PRIVATE
        DCraft
)

if(WIN32)
    # Determine which DLLs to copy based on architecture
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)  # 64-bit
        set(GLEW_DLL_PATH "${GLEW_ROOT_DIR}/bin/Release/x64/glew32.dll")
        set(GLUT_DLL_PATH "${GLUT_ROOT_DIR}/bin/x64/freeglut.dll")
    else()  # 32-bit
        set(GLEW_DLL_PATH "${GLEW_ROOT_DIR}/bin/Release/Win32/glew32.dll")
        set(GLUT_DLL_PATH "${GLUT_ROOT_DIR}/bin/freeglut.dll")
    endif()

    # Copy the DLLs
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GLEW_DLL_PATH}"
            "${GLUT_DLL_PATH}"
            $<TARGET_FILE_DIR:${APP_NAME}>
    )
endif()

# Post-build command to copy assets folder
add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${APP_NAME}>/assets
)

# Set Visual Studio specific properties
if(MSVC)
    set_property(TARGET ${APP_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${APP_SOURCES})
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${APP_HEADERS})
endif()


