set(APP_NAME Sandbox)

file(GLOB_RECURSE APP_SOURCES "src/*.cpp")
file(GLOB_RECURSE APP_HEADERS "include/*.h" "include/*.hpp")

add_executable(${APP_NAME} ${APP_SOURCES} ${APP_HEADERS})

target_include_directories(${APP_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link against DCraft Engine library
target_link_libraries(${APP_NAME}
        PRIVATE
        DCraft
)

if (WIN32)
    # Determine which DLLs to copy based on architecture
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)  # 64-bit
        set(GLEW_DLL_PATH "${GLEW_ROOT_DIR}/bin/Release/x64/glew32.dll")
        set(GLUT_DLL_PATH "${GLUT_ROOT_DIR}/bin/x64/freeglut.dll")
    else ()  # 32-bit
        set(GLEW_DLL_PATH "${GLEW_ROOT_DIR}/bin/Release/Win32/glew32.dll")
        set(GLUT_DLL_PATH "${GLUT_ROOT_DIR}/bin/freeglut.dll")
    endif ()

    # Copy the DLLs
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GLEW_DLL_PATH}"
            "${GLUT_DLL_PATH}"
            $<TARGET_FILE_DIR:${APP_NAME}>
    )
endif ()

# Copy the assets directory
add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying assets from ${CMAKE_CURRENT_SOURCE_DIR}/assets to $<TARGET_FILE_DIR:${APP_NAME}>/assets"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${APP_NAME}>/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${APP_NAME}>/assets"
)

if (MSVC)
    # Set this project as the startup project
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${APP_NAME})

    # Set the working directory for the debugger
    set_target_properties(${APP_NAME} PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${APP_NAME}>"
    )

    # Add source grouping
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SOURCES})
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADERS})

endif ()
