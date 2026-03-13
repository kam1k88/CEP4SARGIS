add_library(gaussian_cdf_plugin MODULE
    gaussian_cdf_plugin.cpp
)

target_include_directories(gaussian_cdf_plugin PRIVATE
    ${CMAKE_SOURCE_DIR}/core/include
)

target_link_libraries(gaussian_cdf_plugin PRIVATE
    SargisLab_core
)

set_target_properties(gaussian_cdf_plugin PROPERTIES
    PREFIX ""
    OUTPUT_NAME "gaussian_cdf_plugin"
)

if(WIN32)
    set_target_properties(gaussian_cdf_plugin PROPERTIES SUFFIX ".dll")
elseif(APPLE)
    set_target_properties(gaussian_cdf_plugin PROPERTIES SUFFIX ".dylib")
else()
    set_target_properties(gaussian_cdf_plugin PROPERTIES SUFFIX ".so")
endif()
