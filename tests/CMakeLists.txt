# Unit tests
set(UNIT_TEST_NAME UnitTests)
file(GLOB_RECURSE UNIT_TEST_SOURCES "unit/*.cpp")
file(GLOB_RECURSE UNIT_TEST_HEADERS "unit/*.h" "unit/*.hpp")

add_executable(${UNIT_TEST_NAME} ${UNIT_TEST_SOURCES} ${UNIT_TEST_HEADERS})
target_compile_definitions(${UNIT_TEST_NAME} PRIVATE HELLFIRE_TESTING_ENABLED)
target_link_libraries(${UNIT_TEST_NAME}
        PRIVATE
        HellfireEngine
        Catch2::Catch2WithMain
)
catch_discover_tests(${UNIT_TEST_NAME})

# UI tests
option(HELLFIRE_UI_TESTS "Build UI tests with ImGui Test Engine" ON)

if(HELLFIRE_UI_TESTS)
    set(UI_TEST_NAME UITests)

    include(FetchContent)
    include(${CMAKE_SOURCE_DIR}/cmake/AssetSymlink.cmake)

    # Fetch ImGui Test Engine
    FetchContent_Declare(
            imgui_test_engine
            GIT_REPOSITORY https://github.com/ocornut/imgui_test_engine.git
            GIT_TAG v1.92.4
    )
    FetchContent_MakeAvailable(imgui_test_engine)

    set(IMGUI_TEST_ENGINE_DIR "${imgui_test_engine_SOURCE_DIR}/imgui_test_engine")
    set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/vendor/imgui")
    set(IMGUIZMO_DIR "${CMAKE_SOURCE_DIR}/editor/vendor/imguizmo")

    # ============================================
    # ImGui with Test Engine
    # ============================================
    add_library(imgui_with_test_engine STATIC
            ${IMGUI_DIR}/imgui.cpp
            ${IMGUI_DIR}/imgui_demo.cpp
            ${IMGUI_DIR}/imgui_draw.cpp
            ${IMGUI_DIR}/imgui_tables.cpp
            ${IMGUI_DIR}/imgui_widgets.cpp
            ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
            ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
            ${IMGUI_TEST_ENGINE_DIR}/imgui_te_context.cpp
            ${IMGUI_TEST_ENGINE_DIR}/imgui_te_coroutine.cpp
            ${IMGUI_TEST_ENGINE_DIR}/imgui_te_engine.cpp
            ${IMGUI_TEST_ENGINE_DIR}/imgui_te_exporters.cpp
            ${IMGUI_TEST_ENGINE_DIR}/imgui_te_perftool.cpp
            ${IMGUI_TEST_ENGINE_DIR}/imgui_te_ui.cpp
            ${IMGUI_TEST_ENGINE_DIR}/imgui_te_utils.cpp
            ${IMGUI_TEST_ENGINE_DIR}/imgui_capture_tool.cpp
    )

    target_include_directories(imgui_with_test_engine PUBLIC
            ${CMAKE_SOURCE_DIR}/vendor
            ${IMGUI_DIR}
            ${IMGUI_DIR}/backends
            ${IMGUI_TEST_ENGINE_DIR}
            ${glew_SOURCE_DIR}/include
    )

    target_compile_definitions(imgui_with_test_engine PUBLIC
            IMGUI_ENABLE_TEST_ENGINE
            IMGUI_TEST_ENGINE_ENABLE_COROUTINE_STDTHREAD_IMPL=1
    )

    target_link_libraries(imgui_with_test_engine PUBLIC
            libglew_static
            glfw
            ${CMAKE_DL_LIBS}
    )

    if (WIN32)
        target_link_libraries(imgui_with_test_engine PUBLIC opengl32)
    elseif (APPLE)
        target_link_libraries(imgui_with_test_engine PUBLIC "-framework OpenGL")
    else ()
        target_link_libraries(imgui_with_test_engine PUBLIC GL)
    endif ()

    # ============================================
    # ImGuizmo with Test Engine
    # ============================================
    add_library(imguizmo_test STATIC
            ${IMGUIZMO_DIR}/ImGuizmo.h
            ${IMGUIZMO_DIR}/ImGuizmo.cpp
    )

    target_link_libraries(imguizmo_test PRIVATE imgui_with_test_engine)
    target_include_directories(imguizmo_test PUBLIC ${IMGUIZMO_DIR})

    # ============================================
    # Engine Library for Testing (links against test-enabled imgui)
    # ============================================
    file(GLOB_RECURSE ENGINE_LIB_SOURCES "${CMAKE_SOURCE_DIR}/engine/src/*.cpp")
    file(GLOB_RECURSE ENGINE_LIB_HEADERS "${CMAKE_SOURCE_DIR}/engine/src/*.h" "${CMAKE_SOURCE_DIR}/engine/src/*.hpp")

    add_library(HellfireEngine_Test STATIC ${ENGINE_LIB_SOURCES} ${ENGINE_LIB_HEADERS})

    target_include_directories(HellfireEngine_Test PUBLIC
            ${CMAKE_SOURCE_DIR}/engine/src
            ${CMAKE_SOURCE_DIR}/engine/include
    )

    find_package(OpenGL REQUIRED)

    # Copy the same link libraries as HellfireEngine, but swap imgui for imgui_with_test_engine
    target_link_libraries(HellfireEngine_Test
            PUBLIC
            libglew_static
            glfw
            GLM::GLM
            OpenGL::GL
            imgui_with_test_engine  # Use test-enabled imgui
            assimp
            PRIVATE
            stb_image
            json
    )

    # ============================================
    # Editor Library for Testing
    # ============================================
    file(GLOB_RECURSE EDITOR_LIB_SOURCES "${CMAKE_SOURCE_DIR}/editor/src/*.cpp")
    file(GLOB_RECURSE EDITOR_LIB_HEADERS "${CMAKE_SOURCE_DIR}/editor/src/*.h" "${CMAKE_SOURCE_DIR}/editor/src/*.hpp")
    list(FILTER EDITOR_LIB_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

    add_library(HellfireEditorLib_Test STATIC ${EDITOR_LIB_SOURCES} ${EDITOR_LIB_HEADERS})

    target_include_directories(HellfireEditorLib_Test PUBLIC
            ${CMAKE_SOURCE_DIR}/editor/src
    )

    target_compile_definitions(HellfireEditorLib_Test PUBLIC
            HELLFIRE_EDITOR_ENABLED
            HELLFIRE_UI_TESTS_BUILD
    )

    # Link against test-enabled engine and imgui
    target_link_libraries(HellfireEditorLib_Test PUBLIC
            HellfireEngine_Test  # Use test-enabled engine
            imgui_with_test_engine
            imguizmo_test
            glfw
    )

    # ============================================
    # UI Test Executable
    # ============================================
    file(GLOB_RECURSE UI_TEST_SOURCES "ui/*.cpp")
    file(GLOB_RECURSE UI_TEST_HEADERS "ui/*.h" "ui/*.hpp")

    add_executable(${UI_TEST_NAME} ${UI_TEST_SOURCES} ${UI_TEST_HEADERS})

    target_compile_definitions(${UI_TEST_NAME} PRIVATE
            HELLFIRE_TESTING_ENABLED
            HELLFIRE_UI_TESTS_BUILD
    )

    target_link_libraries(${UI_TEST_NAME}
            PRIVATE
            HellfireEditorLib_Test
    )

    setup_asset_symlink(${UI_TEST_NAME} "${CMAKE_SOURCE_DIR}/editor/assets")

    add_test(
            NAME UITests
            COMMAND ${UI_TEST_NAME} --exit-on-completion
            WORKING_DIRECTORY $<TARGET_FILE_DIR:${UI_TEST_NAME}>
    )
endif()