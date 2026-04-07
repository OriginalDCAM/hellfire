set(APP_NAME HellfireEditor)
set(LIB_NAME HellfireEditorLib)

set(IMGUIZMO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imguizmo)
add_library(imguizmo STATIC
        ${IMGUIZMO_DIR}/ImGuizmo.h
        ${IMGUIZMO_DIR}/ImGuizmo.cpp
)

target_link_libraries(imguizmo PRIVATE imgui)

target_include_directories(imguizmo
        PUBLIC ${IMGUIZMO_DIR}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
)

file(GLOB_RECURSE LIB_SOURCES "src/*.cpp")
file(GLOB_RECURSE LIB_HEADERS "src/*.h" "src/*.hpp")

# Remove main.cpp from library sources 
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

add_library(${LIB_NAME} STATIC ${LIB_SOURCES} ${LIB_HEADERS})

target_include_directories(${LIB_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${LIB_NAME}
        PUBLIC
        HellfireEngine
        imgui
        imguizmo
        glfw
)

target_compile_definitions(${LIB_NAME} PUBLIC HELLFIRE_EDITOR_ENABLED)

add_executable(${APP_NAME} src/main.cpp)
target_link_libraries(${APP_NAME} PRIVATE ${LIB_NAME})

# Asset symlinks
include(${CMAKE_SOURCE_DIR}/cmake/AssetSymlink.cmake)
setup_asset_symlink(${APP_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/assets")

# IDE Settings
set_target_properties(${APP_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

if (MSVC)
    set_target_properties(${APP_NAME} PROPERTIES
            VS_DEBUGGER_COMMAND_ARGUMENTS ""
            VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%"
    )

    # Source grouping for library
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${LIB_SOURCES})
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Header Files" FILES ${LIB_HEADERS})
endif ()