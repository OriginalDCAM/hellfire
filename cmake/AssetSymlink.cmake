function(setup_asset_symlink TARGET_NAME ASSETS_SOURCE_DIR)
    if (WIN32)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove -f "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets"
            COMMAND cmd /c "mklink /J \"$<TARGET_FILE_DIR:${TARGET_NAME}>\\assets\" \"${ASSETS_SOURCE_DIR}\""
            COMMENT "Creating directory junction to assets directory for ${TARGET_NAME}"
        )
    else ()
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove -f "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets"
            COMMAND ${CMAKE_COMMAND} -E create_symlink
                "${ASSETS_SOURCE_DIR}"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets"
            COMMENT "Creating symlink to assets directory for ${TARGET_NAME}"
        )
    endif ()
endfunction()