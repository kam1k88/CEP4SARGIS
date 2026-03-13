# CurveFit Dependencies
# Supports: vcpkg, system packages, FetchContent fallback

# Qt6 (required)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt6 QUIET COMPONENTS Charts)
if(Qt6Charts_FOUND)
    set(CURVEFIT_QT_CHARTS_AVAILABLE 1)
endif()

# Eigen3
find_package(Eigen3 3.4 QUIET)
if(NOT Eigen3_FOUND)
    message(STATUS "Eigen3 not found, using FetchContent")
    include(FetchContent)
    FetchContent_Declare(
        Eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4.0
    )
    FetchContent_MakeAvailable(Eigen)
    set(Eigen3_FOUND TRUE)
    set(Eigen3_INCLUDE_DIR ${eigen_SOURCE_DIR})
endif()

# Ceres Solver
find_package(Ceres QUIET)
if(NOT Ceres_FOUND)
    message(FATAL_ERROR "Ceres Solver not found. Install via vcpkg: vcpkg install ceres:x64-windows")
endif()

# OpenXLSX for Excel support (optional)
set(CURVEFIT_EXCEL_AVAILABLE 0)
find_path(OpenXLSX_INCLUDE_DIR NAMES OpenXLSX.hpp PATHS
    ${CMAKE_SOURCE_DIR}/external/OpenXLSX
    /usr/include
    /usr/local/include
)
if(OpenXLSX_INCLUDE_DIR)
    add_library(OpenXLSX INTERFACE)
    target_include_directories(OpenXLSX INTERFACE ${OpenXLSX_INCLUDE_DIR})
    set(CURVEFIT_EXCEL_AVAILABLE 1)
endif()

# QCustomPlot (optional) - use Qt Charts if QCustomPlot not available
set(CURVEFIT_QCUSTOMPLOT_AVAILABLE 0)
if(CURVEFIT_USE_QCUSTOMPLOT AND EXISTS "${CMAKE_SOURCE_DIR}/external/qcustomplot/qcustomplot.cpp")
    file(READ "${CMAKE_SOURCE_DIR}/external/qcustomplot/qcustomplot.cpp" _qcp_content)
    if(NOT _qcp_content MATCHES "Placeholder")
        set(CURVEFIT_QCUSTOMPLOT_AVAILABLE 1)
        set(QCUSTOMPLOT_DIR "${CMAKE_SOURCE_DIR}/external/qcustomplot")
    endif()
endif()
