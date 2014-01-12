project(cwall)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 COMPONENTS QtSql REQUIRED )
include (${QT_USE_FILE})

set(SQL_INSTALL schema.mysql.sql)
set(SQL_PATH /share/cwall/sql)
set(cwall_UI cwall_manager.ui dialog_database.ui dialog_configure.ui dialog_db_create.ui dialog_hostname.ui dialog_rebuild.ui dialog_auth.ui form_library_editor.ui)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(cwall_SRCS formlibraryeditor.cpp competition.cpp dialogconfigure.cpp dialogdatabase.cpp cwallmanager.cpp main.cpp)
qt4_automoc(${cwall_SRCS})
qt4_wrap_ui(cwall_FORMS ${cwall_UI})
add_executable(cwall ${cwall_SRCS} ${cwall_FORMS})
target_link_libraries(cwall ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTSQL_LIBRARY})

install_files( ${SQL_PATH} ${SQL_INSTALL})