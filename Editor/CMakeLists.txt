set(APP_NAME HellfireEditor)

add_compile_definitions(HELLFIRE_EDITOR_ENABLED)

file(GLOB_RECURSE APP_SOURCES "src/*.cpp")
file(GLOB_RECURSE APP_HEADERS "include/*.h" "include*.hpp")
add_executable(${APP_NAME} ${APP_SOURCES} ${APP_HEADERS})

target_include_directories(${APP_NAME}
PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${APP_NAME}
        PRIVATE
        HellfireEngine 
        imgui
        glfw
)

target_compile_definitions(${APP_NAME} PRIVATE HELLFIRE_EDITOR_ENABLED)

# Set working directory for debugging in various IDEs
set_target_properties(${APP_NAME} PROPERTIES
        # Visual Studio
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"

        # Xcode
        XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"

        # For other IDEs that support it
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

if (MSVC)
    # Try additional VS properties for working directory
    set_target_properties(${APP_NAME} PROPERTIES
            VS_DEBUGGER_COMMAND_ARGUMENTS ""
            VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%"
    )

    # Add source grouping
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${APP_SOURCES})
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${APP_HEADERS})
endif()


