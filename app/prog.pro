TEMPLATE = app
TARGET = prog
CONFIG += debug_and_release
CONFIG += c++20
QMAKE_CXXFLAGS += -fconcepts-ts -std=c++20
QT = core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

Release:DESTDIR = release
Release:OBJECTS_DIR = release/obj
Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/obj

SOURCES += gui.cpp database.cpp main.cpp
           #sqlhighlighter.cpp  \
           #opengl.cpp
HEADERS += gui.hpp database.hpp qthelpers.hpp
           #sqlhighlighter.hpp \
           #opengl.hpp
#LIBS += -lfmt
RESOURCES = images.qrc

# Uncomment this to add a cat prism screen to the application.
#DEFINES += _CATPRISM
