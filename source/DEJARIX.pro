#-------------------------------------------------
#
# Project created by QtCreator 2013-06-19T18:03:56
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DEJARIX
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    MainWidget.cpp \
    CardSpecifications.cpp \
    CardBuilder.cpp \
    CardBuffer.cpp \
    MainProgram.cpp

HEADERS  += MainWindow.hpp \
    MainWidget.hpp \
    CardSpecifications.hpp \
    CardBuilder.hpp \
    CardBuffer.hpp \
    MainProgram.hpp
