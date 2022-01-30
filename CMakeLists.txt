cmake_minimum_required(VERSION 3.14)

project(runtestcases VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package(...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()


set(QT_ROOT)

set (QT_STATIC OFF)
if(QT_STATIC)
    set(QT_ROOT "D:/Qt/qtinstall_static_5.15.2")
    if(NOT QT_ROOT IN_LIST CMAKE_PREFIX_PATH)
        list(APPEND CMAKE_PREFIX_PATH ${QT_ROOT})
    endif()
else()
    set(QT_ROOT "D:/Qt/Qt5.9.7/5.9.7/msvc2017_64")
    if(NOT QT_ROOT IN_LIST CMAKE_PREFIX_PATH)
        list(APPEND CMAKE_PREFIX_PATH ${QT_ROOT})
    endif()
endif()


find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)

set(BOOST_ROOT "$ENV{BOOST_ROOT}")
set(BOOST_INCLUDEDIR ${BOOST_ROOT})
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/msvc142")
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_NO_WARN_NEW_VERSIONS   ON)

find_package(Boost REQUIRED COMPONENTS
    log
    filesystem
    container
)

set(PROJECT_SOURCES
        main.cpp
        qml.qrc
        log.h
        logtext.cpp
        logtext.h  
        caseinfomodel.h
        caseinfomodel.cpp
        caseinfoitem.cpp
        caseinfoitem.h
        caseinfo.h
        commonhdr.h
        configinfo.h
        jsonmgr.h
        refinecase.h
        iocontextwrapper.h
        monitor.h
        monitor.cpp
        cmdprocess.h
        casecmd.h
        datamanager.cpp
        datamanager.h
        monitorthread.cpp
        monitorthread.h
        manager.cpp
        manager.h
)

if(QT_STATIC)
    add_definitions(-DQT_STATIC)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(runtestcases
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(runtestcases SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(runtestcases
          ${PROJECT_SOURCES}
        )
    endif()
endif()

target_compile_definitions(runtestcases
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_compile_features(${PROJECT_NAME} PRIVATE
    cxx_std_17
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${Boost_INCLUDE_DIRS}
)

set(Extra_Static_Lib)

if(QT_STATIC)
    set (Extra_Static_Lib
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Qml
        Qt${QT_VERSION_MAJOR}::Gui
        ${QT_ROOT}/qml/QtQuick.2/qtquick2plugind.lib
        ${QT_ROOT}/qml/QtQuick/Controls/qtquickcontrolsplugind.lib
        ${QT_ROOT}/qml/QtQuick/Layouts/qquicklayoutsplugind.lib
        ${QT_ROOT}/qml/QtQuick/Window.2/windowplugind.lib
        ${QT_ROOT}/qml/QtQml/Models.2/modelsplugind.lib
        ${QT_ROOT}/qml/QtQml/qmlplugind.lib
        ${QT_ROOT}/lib/Qt5QmlWorkerScriptd.lib
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
    ${Boost_LIBRARIES}
    ${Extra_Static_Lib}
)

set_target_properties(runtestcases PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(runtestcases)
    qt_finalize_executable(runtestcases)
endif()
