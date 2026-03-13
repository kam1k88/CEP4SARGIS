if(WIN32)
    set(APP_ENTRY win/main_win.cpp)
elseif(APPLE)
    set(APP_ENTRY macos/main_mac.mm)
else()
    set(APP_ENTRY win/main_win.cpp)
endif()

add_executable(CurveFit ${APP_ENTRY})

target_link_libraries(CurveFit PRIVATE
    curvefit_ui
)

# Windows: добавить иконку и ресурсный файл
if (WIN32)
    target_sources(CurveFit PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/app.rc
        ${CMAKE_SOURCE_DIR}/resources/app.ico
    )
endif()

# macOS bundle
if(APPLE)
    set_target_properties(CurveFit PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
        MACOSX_RPATH TRUE
    )
    set_target_properties(CurveFit PROPERTIES OUTPUT_NAME CurveFitScientificApp)
    set(MACOSX_BUNDLE_BUNDLE_NAME "CurveFitScientificApp")
endif()
