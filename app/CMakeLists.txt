if(WIN32)
    set(APP_ENTRY win/main_win.cpp)
elseif(APPLE)
    set(APP_ENTRY macos/main_mac.mm)
else()
    set(APP_ENTRY win/main_win.cpp)
endif()

add_executable(SargisLab ${APP_ENTRY})

target_link_libraries(SargisLab PRIVATE
    sargislab_ui
)

# Windows: добавить иконку и ресурсный файл
if (WIN32)
    target_sources(SargisLab PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/app.rc
        ${CMAKE_SOURCE_DIR}/resources/app.ico
    )
    set_target_properties(SargisLab PROPERTIES OUTPUT_NAME SargisLabApp)
endif()

# macOS bundle
if(APPLE)
    set_target_properties(SargisLab PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
        MACOSX_RPATH TRUE
    )
    set_target_properties(SargisLab PROPERTIES OUTPUT_NAME SargisLabApp)
    set(MACOSX_BUNDLE_BUNDLE_NAME "SargisLabApp")
endif()
