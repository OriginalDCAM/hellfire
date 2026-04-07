set(APP_NAME HellfireEditor)
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
)


