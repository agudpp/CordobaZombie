HEADERS   += smtp.h 
SOURCES   += smtp.cpp \
             main.cpp

#RESOURCES += securesocketclient.qrc

#FORMS     += certificateinfo.ui \
#             sslclient.ui \
#             sslerrors.ui
#

QT        += network core gui

# install
target.path = ./build
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro *.png *.jpg images
sources.path = ./build
INSTALLS += target sources


