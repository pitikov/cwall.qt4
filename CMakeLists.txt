project(cwall)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(cwall_SRCS cwall.cpp main.cpp)
qt4_automoc(${cwall_SRCS})
add_executable(cwall ${cwall_SRCS})
target_link_libraries(cwall ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
