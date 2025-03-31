# FindGLM.cmake - Custom find module for GLM
#
# This module defines:
#  GLM_INCLUDE_DIR - where to find glm/glm.hpp
#  GLM_FOUND - if the library was successfully found
#  GLM::GLM - imported target to use

# Try to find GLM
find_path(GLM_INCLUDE_DIR
    NAMES glm/glm.hpp
    PATHS
        ${GLM_ROOT_DIR}
        ENV GLM_ROOT_DIR
        /usr/include
        /usr/local/include
        /sw/include
        /opt/local/include
    DOC "GLM include directory"
)

# Handle REQUIRED and QUIET arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLM
    REQUIRED_VARS GLM_INCLUDE_DIR
)

# Create imported target
if(GLM_FOUND AND NOT TARGET GLM::GLM)
    add_library(GLM::GLM INTERFACE IMPORTED)
    set_target_properties(GLM::GLM PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${GLM_INCLUDE_DIR}"
    )
endif()

# Hide these variables in the GUI
mark_as_advanced(GLM_INCLUDE_DIR)