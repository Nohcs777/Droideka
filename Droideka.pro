#-------------------------------------------------
#
# Project created by QtCreator 2012-02-05T16:47:31
#
#-------------------------------------------------

QT       += core gui opengl network

TARGET = Droideka
TEMPLATE = app


SOURCES += main.cpp\
        LoginWindow.cpp \
    WindowTestOpenGL.cpp \
    WidgetTestOpenGL.cpp

HEADERS  += LoginWindow.h \
    WindowTestOpenGL.hpp \
    WidgetTestOpenGL.hpp \
    Matrix4x4.hpp

FORMS    += LoginWindow.ui