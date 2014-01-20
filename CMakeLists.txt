project(cwall)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 COMPONENTS QtSql REQUIRED )
include (${QT_USE_FILE})

set(SQL_FILES schema.mysql.sql)
set(SQL_PATH /share/cwall/sql)
set(PDF_FILES rules2012.pdf)
set(PDF_PATH /share/cwall/pdf)
set(IMG_FILES splash.jpg)
set(IMG_PATH /share/cwall/img)

set(cwall_UI cwall_manager.ui dialog_database.ui dialog_configure.ui dialog_db_create.ui dialog_hostname.ui dialog_rebuild.ui dialog_auth.ui form_library_editor.ui dialog_user_password.ui form_rules_viewer.ui)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(cwall_SRCS rules.cpp rulesviewer.cpp user.cpp formlibraryeditor.cpp competition.cpp dialogconfigure.cpp dialogdatabase.cpp cwallmanager.cpp main.cpp)
qt4_automoc(${cwall_SRCS})
qt4_wrap_ui(cwall_FORMS ${cwall_UI})
add_executable(cwall ${cwall_SRCS} ${cwall_FORMS})
target_link_libraries(cwall ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTSQL_LIBRARY} -lpoppler-qt4)


set (kgen_SRCS kgen_main.cpp kgen.cpp)
set (kgen_UI kgen.ui)
qt4_wrap_ui(kgen_FORM ${kgen_UI})
qt4_automoc(${kgen_SRCS})
add_executable(kgen ${kgen_SRCS} ${kgen_FORM})
target_link_libraries(kgen ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

install_files(${SQL_PATH} ${SQL_FILES})
install_files(${PDF_PATH} ${PDF_FILES})
install_files(${IMG_PATH} ${IMG_FILES})
