set(CORE_SOURCES
    src/regression/gaussian_cdf_model.cpp
    src/regression/polynomial_model.cpp
    src/regression/exponential_model.cpp
    src/regression/logistic_model.cpp
    src/regression/regression_engine.cpp
    src/analysis/residuals_analyzer.cpp
    src/analysis/symmetry_analyzer.cpp
    src/analysis/forecast_error.cpp
    src/montecarlo/monte_carlo_engine.cpp
    src/io/csv_io.cpp
    src/io/json_io.cpp
    src/plugins/plugin_loader.cpp
)

if(CURVEFIT_EXCEL_AVAILABLE)
    list(APPEND CORE_SOURCES src/io/excel_io.cpp)
endif()

add_library(curvefit_core STATIC ${CORE_SOURCES})

target_include_directories(curvefit_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(curvefit_core PUBLIC
    curvefit_platform
    Ceres::ceres
    Qt6::Core
)

if(CURVEFIT_EXCEL_AVAILABLE AND TARGET OpenXLSX)
    target_link_libraries(curvefit_core PUBLIC OpenXLSX)
endif()

target_compile_definitions(curvefit_core PRIVATE
    $<IF:$<BOOL:${CURVEFIT_EXCEL_AVAILABLE}>,CURVEFIT_EXCEL_AVAILABLE=1,CURVEFIT_EXCEL_AVAILABLE=0>
)
