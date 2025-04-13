cmake_minimum_required(VERSION 3.16)

project(Advanced_Notebook VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS "ui")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES Advanced_Notebook_zh_CN.ts)

add_subdirectory(config_system)
add_subdirectory(Advanced_Calculator)

set(PROJECT_SOURCES
        ${PROJECT_SOURCES}
        main.cpp
        ${TS_FILES}
        src/MainWindow.h src/MainWindow.cpp
        ui/MainWindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Advanced_Notebook
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        src/notebook_widget/NoteBookWidget.cpp
        src/notebook_widget/NoteBookWidget.h
        ui/NoteBookWidget.ui
        src/notebook_widget/RecordObject.h src/notebook_widget/RecordObject.cpp
        src/notebook_widget/AdvTextEdit.h src/notebook_widget/AdvTextEdit.cpp
        notebook_resources.qrc
        src/util/ProgramSettings.h
        src/util/AbstractFileEditor.h src/util/AbstractFileEditor.cpp
        src/backtrace_widget/BackTraceWidget.h src/backtrace_widget/BackTraceWidget.cpp
        src/util/NoteBookStaticProperties.h
        src/util/NoSinkingToolButton.h src/util/NoSinkingToolButton.cpp
        src/util/IconHelper.h src/util/IconHelper.cpp
        src/util/ColorPicker.h src/util/ColorPicker.cpp
        src/util/SettingsWidget.h src/util/SettingsWidget.cpp ui/SettingsWidget.ui
        src/util/ColorEditBox.h src/util/ColorEditBox.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Advanced_Notebook APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(Advanced_Notebook SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Advanced_Notebook
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(Advanced_Notebook PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Advanced_Calculator
    config_system)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Advanced_Notebook)
endif()
set_target_properties(Advanced_Notebook PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Advanced_Notebook
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Advanced_Notebook)
endif()

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
    "src" "src/notebook_widget" "src/backtrace_widget" "src/util"
)
