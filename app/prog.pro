TEMPLATE = app
TARGET = prog
CONFIG += debug_and_release
CONFIG += c++20
QMAKE_CXXFLAGS += -fconcepts-ts
QT = core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

Release:DESTDIR = release
Release:OBJECTS_DIR = release/obj
Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/obj

SOURCES += gui.cpp sqlhighlighter.cpp database.cpp \
           main.cpp \
           opengl.cpp
HEADERS += gui.hpp sqlhighlighter.hpp database.hpp \
           qthelpers.hpp \
           opengl.hpp
LIBS += -lfmt
RESOURCES = images.qrc

# Uncomment this to add a cat prism screen to the application.
#DEFINES += _CATPRISM
