# FindCeres.cmake - Fallback find script for Ceres Solver
# Use vcpkg or system install when possible

find_path(Ceres_INCLUDE_DIR ceres/ceres.h
    PATHS /usr/include /usr/local/include
    "$ENV{ProgramFiles}/Ceres/include"
)

find_library(Ceres_LIBRARY
    NAMES ceres
    PATHS /usr/lib /usr/local/lib
    "$ENV{ProgramFiles}/Ceres/lib"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Ceres
    REQUIRED_VARS Ceres_LIBRARY Ceres_INCLUDE_DIR
)

if(Ceres_FOUND AND NOT TARGET Ceres::ceres)
    add_library(Ceres::ceres UNKNOWN IMPORTED)
    set_target_properties(Ceres::ceres PROPERTIES
        IMPORTED_LOCATION "${Ceres_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${Ceres_INCLUDE_DIR}"
    )
endif()
