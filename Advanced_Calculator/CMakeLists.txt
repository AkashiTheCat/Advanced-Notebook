cmake_minimum_required(VERSION 3.16)

project(Advanced_Calculator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS "ui")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES Advanced_Calculator_zh_CN.ts)

set(PROJECT_SOURCES

        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_library(Advanced_Calculator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources.qrc
        src/CalqlatrWidget.h
        src/CalqlatrWidget.cpp
        ui/CalqlatrWidget.ui
        src/scientific_calqlatr/CalculationObject.cpp
        src/scientific_calqlatr/CalculationObject.h
        src/scientific_calqlatr/SciCalqlatrWidget.cpp
        src/scientific_calqlatr/SciCalqlatrWidget.h
        ui/SciCalqlatrWidget.ui
        src/scientific_calqlatr/SciCalqlatrButton.cpp
        src/scientific_calqlatr/SciCalqlatrButton.h

        src/calqlatr_history/CalqlatrHistoryWidget.cpp
        src/calqlatr_history/CalqlatrHistoryWidget.h
        src/calqlatr_history/CalqlatrHistory.h src/calqlatr_history/CalqlatrHistory.cpp
        ui/CalqlatrHistoryWidget.ui

        src/util/ButtonState.h src/util/ButtonState.cpp
        src/util/MultiStatePushButton.h
        src/util/MultiStatePushButton.cpp
        src/util/MultiStateToolButton.h
        src/util/MultiStateToolButton.cpp
        src/util/RecordTransferSrcInterface.h
        src/util/CalqlatrElement.h
        src/util/CalqlatrRecord.cpp
        src/util/CalqlatrRecord.h
        src/util/ReadOnlyLineEdit.cpp
        src/util/ReadOnlyLineEdit.h
        src/scientific_calqlatr/SciCalqlatr.h src/scientific_calqlatr/SciCalqlatr.cpp
        src/util/CalqlatrStaticProperties.h
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Advanced_Calculator APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(Advanced_Calculator SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Advanced_Calculator
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(Advanced_Calculator PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_include_directories(Advanced_Calculator
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/util
        ${CMAKE_CURRENT_SOURCE_DIR}/src/calqlatr
        ${CMAKE_CURRENT_SOURCE_DIR}/src/calqlatr_history
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Advanced_Calculator)
endif()
set_target_properties(Advanced_Calculator PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Advanced_Calculator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Advanced_Calculator)
endif()

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
    "src" "src/scientific_calqlatr/" "src/calqlatr_history/" "src/util/"
)
