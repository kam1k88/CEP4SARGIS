add_library(SargisLab_core
    src/regression/gaussian_cdf_model.cpp
    src/regression/polynomial_model.cpp
    src/regression/exponential_model.cpp
    src/regression/logistic_model.cpp
    src/regression/regression_engine.cpp
    src/regression/residuals_analyzer.cpp
    src/regression/symmetry_analyzer.cpp
    src/regression/forecast_error.cpp
    src/monte_carlo/monte_carlo_engine.cpp
    src/io/csv_io.cpp
    src/io/json_io.cpp
    src/plugins/plugin_loader.cpp
)

find_package(Eigen3 CONFIG REQUIRED)
find_package(Ceres CONFIG REQUIRED)

target_link_libraries(SargisLab_core
    PUBLIC
        Eigen3::Eigen
        Ceres::ceres
)
