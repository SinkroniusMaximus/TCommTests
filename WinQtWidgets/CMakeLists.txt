cmake_minimum_required(VERSION 3.5)

project(WinQtWidgets VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

file(GLOB_RECURSE Headers ./*.h)
file(GLOB_RECURSE Source ./*.cpp)
file(GLOB_RECURSE Ui ./*.cpp)
#file(GLOB_RECURSE Library ../Lib/*.h)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(WinQtWidgets
        MANUAL_FINALIZATION
        ${Source} ${Headers} ${Ui}
    )
endif()

target_link_libraries(WinQtWidgets PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(WinQtWidgets)
endif()
