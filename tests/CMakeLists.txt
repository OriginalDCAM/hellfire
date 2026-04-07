set(APP_NAME Tests)
add_compile_definitions(HELLFIRE_TESTING_ENABLED)

file(GLOB_RECURSE APP_SOURCES "*.cpp")
file(GLOB_RECURSE APP_HEADERS "*.h" "*.hpp")
add_executable(${APP_NAME} ${APP_SOURCES} ${APP_HEADERS})

target_link_libraries(${APP_NAME}
        PRIVATE
        HellfireEngine
        Catch2::Catch2WithMain
)

catch_discover_tests(${APP_NAME})