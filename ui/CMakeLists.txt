set(UI_SOURCES
    src/main_window.cpp
    src/plot_view.cpp
    src/data_manager.cpp
    src/residuals_window.cpp
    src/symmetry_window.cpp
    src/forecast_error_window.cpp
)

add_library(SargisLab_ui STATIC ${UI_SOURCES})

target_include_directories(SargisLab_ui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SargisLab_ui PUBLIC
    SargisLab_core
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

if(SargisLab_QCUSTOMPLOT_AVAILABLE)
    target_sources(SargisLab_ui PRIVATE ${QCUSTOMPLOT_DIR}/qcustomplot.cpp)
    target_include_directories(SargisLab_ui PRIVATE ${QCUSTOMPLOT_DIR})
    target_compile_definitions(SargisLab_ui PRIVATE SargisLab_QCUSTOMPLOT=1)
elseif(SargisLab_QT_CHARTS_AVAILABLE)
    target_link_libraries(SargisLab_ui PUBLIC Qt6::Charts)
    target_compile_definitions(SargisLab_ui PRIVATE SargisLab_QT_CHARTS=1)
else()
    target_compile_definitions(SargisLab_ui PRIVATE SargisLab_QT_CHARTS=1)
    # Try to link Charts anyway for basic build
    find_package(Qt6 QUIET COMPONENTS Charts)
    if(Qt6Charts_FOUND)
        target_link_libraries(SargisLab_ui PUBLIC Qt6::Charts)
    endif()
endif()
