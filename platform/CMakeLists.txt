set(PLATFORM_SOURCES)
if(WIN32)
    list(APPEND PLATFORM_SOURCES src/os_utils_win.cpp)
else()
    list(APPEND PLATFORM_SOURCES src/os_utils_macos.cpp)
endif()

add_library(curvefit_platform STATIC ${PLATFORM_SOURCES})

target_include_directories(curvefit_platform PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(curvefit_platform PUBLIC
    Qt6::Core
)
