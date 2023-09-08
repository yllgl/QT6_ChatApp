#Reference: 
# https://doc.qt.io/qt-6/cmake-get-started.html

cmake_minimum_required(VERSION 3.20)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
project(main VERSION 1.0.0 LANGUAGES CXX)
FILE (GLOB ALL_SOURCES "*.cpp" "*.c" "./src/*.cpp" )
FILE (GLOB ALL_INCLUDES "*.hpp" "*.h" "./src/*.hpp"  "./src/*.h" )

set(ENV{QT_FATAL_WARNINGS} ON)
SET (ALL_SRCS 
	 ${ALL_SOURCES}
	 ${ALL_INCLUDES}
)
add_executable(${PROJECT_NAME} ${ALL_SRCS})
set(CMAKE_TOOLCHAIN_FILE qt6-mingw-x64.cmake)

project(main VERSION 1.0.0 LANGUAGES CXX)

# 查找 Qt6 的模块
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()
# add_executable(${PROJECT_NAME} chatWindow.h chatWindow.cpp chatWidgetItem.cpp chatWidgetItem.h mainwindow.cpp mainwindow.h usercontroller.h userChatItem.h messagecontroller.h message.h abstractChatItem.h BaseController.h MainController.h MainController.cpp loginWidget.h loginWidget.cpp View.h loginWidget.h loginWidget.cpp main.cpp)
target_link_libraries(${PROJECT_NAME} 
    PRIVATE Qt6::Widgets
)
target_link_libraries(${PROJECT_NAME}  PRIVATE Qt6::Core)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Network)
# 发布程序时不创建控制台窗口
if(CMAKE_BUILD_TYPE STREQUAL Release)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON)
endif()

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${WINDEPLOYQT_EXECUTABLE}
    ARGS --no-translations $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "Running windeployqt..."
)