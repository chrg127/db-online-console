TEMPLATE = app
TARGET = prog
CONFIG += debug_and_release
CONFIG += c++20
QT = core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

Release:DESTDIR = release
Release:OBJECTS_DIR = release/obj
Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/obj

SOURCES += main.cpp window.cpp sqlhighlighter.cpp screens.cpp opengl.cpp
HEADERS += window.hpp debug.hpp sqlhighlighter.hpp screens.hpp opengl.hpp
LIBS += -lfmt

# Uncomment this to add a cat prism screen to the application.
#DEFINES += _CATPRISM
