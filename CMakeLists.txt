#требуемая cmake версия
cmake_minimum_required(VERSION 3.10)

#доп флаги для cmake
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#требуемый стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(kpp)

set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")

# задаем директории исходников и библиотек
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Compiling for Windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Compiling for Linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Compiling for MacOS")
endif()

#собираем sqlite3
add_library(sqlite3 STATIC ${LIB_DIR}/sqlite3/sqlite3.c
    lib/sqlite3/sqlite3.h)

#собираем outputdb библиотеку
add_library(OutputDB ${SOURCE_DIR}/OutputDB/outputDB.cpp
    src/OutputDB/outputDB.h)
target_link_libraries(OutputDB sqlite3)

# собираем nsrlrepo библиотеку
add_library(nsrlRepository STATIC ${SOURCE_DIR}/nsrlRepository/nsrlRepository.cpp
    src/nsrlRepository/nsrlRepository.h)
target_link_libraries(nsrlRepository sqlite3)

#компиляция getfilefromdir библиотеки
add_library(getFileFromDir STATIC ${SOURCE_DIR}/getFileFromDir/getFileFromDir.cpp
    src/getFileFromDir/getFileFromDir.h)

# компиляция cryptopp
add_subdirectory(${LIB_DIR}/cryptopp-cmake)

#компиляция calculateshahash билиотеки
add_library(calculateShaHash STATIC ${SOURCE_DIR}/calculateShaHash/calculateShaHash.cpp
    src/calculateShaHash/calculateShaHash.h)
target_link_libraries(calculateShaHash PRIVATE cryptopp)

# ищем и задем путь для буст
find_package(Boost COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})

# Ошибка, если нет буста
if(NOT Boost_FOUND)
    message(FATAL_ERROR "NOT BOOST")
endif()

# собираем application библиотеку(cli версия)
add_library(application-cli STATIC ${SOURCE_DIR}/application/cli/application.cpp
                                   ${SOURCE_DIR}/application/cli/application.h)
target_link_libraries(application-cli calculateShaHash getFileFromDir nsrlRepository OutputDB Boost::program_options)

#собираем application библиотеку с модификациями для гуя
add_library(application-gui STATIC ${SOURCE_DIR}/application/gui/application.cpp
                                   ${SOURCE_DIR}/application/gui/application.h)
target_link_libraries(application-gui calculateShaHash getFileFromDir nsrlRepository OutputDB Boost::program_options)

#задаем директорию для уже собранных файлов
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CLI_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/build/bin/cli)
set(GUI_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/build/bin/gui)

#сборка всего приложения(cli версия)
add_executable(k-pp-cli ${SOURCE_DIR}/cli/сli_main.cpp)
target_link_libraries(k-pp-cli application-cli)
set_target_properties(k-pp-cli PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CLI_OUTPUT_DIR})

#ищем qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# задаем исходники для гуя
set(PROJECT_SOURCES
        ${SOURCE_DIR}/gui/main.cpp
        ${SOURCE_DIR}/gui/mainwindow.cpp
        ${SOURCE_DIR}/gui/mainwindow.h
        ${SOURCE_DIR}/gui/mainwindow.ui
)

# собираем гуй
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(k-pp-gui
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        src/gui/slave.h 
        src/gui/slave.cpp
    )
endif()
target_link_libraries(k-pp-gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets application-gui)
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(k-pp-gui)
endif()

#отправляем в нужную директорию
set_target_properties(k-pp-gui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${GUI_OUTPUT_DIR})


