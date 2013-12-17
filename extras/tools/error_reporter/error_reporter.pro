HEADERS   += smtp.h \
             ErrorReporter.h 
SOURCES   += smtp.cpp \
             ErrorReporter.cpp \
             main.cpp

#RESOURCES += securesocketclient.qrc

FORMS     += errorreporter.ui

QT        += network core gui

QMAKE_CXXFLAGS += -std=gnu++11

# install
target.path = ./build
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro *.png *.jpg images
sources.path = ./build
INSTALLS += target sources


