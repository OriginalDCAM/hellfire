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

    file(GLOB_RECURSE UI_TEST_SOURCES "ui/*.cpp")
    file(GLOB_RECURSE UI_TEST_HEADERS "ui/*.h" "ui/*.hpp")

    add_executable(${UI_TEST_NAME} ${UI_TEST_SOURCES} ${UI_TEST_HEADERS})

    target_compile_definitions(${UI_TEST_NAME} PRIVATE HELLFIRE_TESTING_ENABLED)

    target_sources(${UI_TEST_NAME} PRIVATE
            ${IMGUI_TEST_ENGINE_DIR}/imgui_te_context.cpp
            ${IMGUI_TEST_ENGINE_DIR}/imgui_te_coroutine.cpp
            ${IMGUI_TEST_ENGINE_DIR}/imgui_te_engine.cpp
            ${IMGUI_TEST_ENGINE_DIR}/imgui_te_exporters.cpp
            ${IMGUI_TEST_ENGINE_DIR}/imgui_te_perftool.cpp
            ${IMGUI_TEST_ENGINE_DIR}/imgui_te_ui.cpp
            ${IMGUI_TEST_ENGINE_DIR}/imgui_te_utils.cpp
            ${IMGUI_TEST_ENGINE_DIR}/imgui_capture_tool.cpp
    )

    target_include_directories(${UI_TEST_NAME} PRIVATE ${IMGUI_TEST_ENGINE_DIR})

    target_include_directories(${UI_TEST_NAME} PRIVATE ${IMGUI_DIR})

    target_link_libraries(${UI_TEST_NAME}
            PRIVATE
            HellfireEditorLib  # Link the static library
    )

    setup_asset_symlink(${UI_TEST_NAME} "${CMAKE_SOURCE_DIR}/editor/assets")

    add_test(
            NAME UITests
            COMMAND ${UI_TEST_NAME} --exit-on-completion
            WORKING_DIRECTORY $<TARGET_FILE_DIR:${UI_TEST_NAME}>
    )
endif()