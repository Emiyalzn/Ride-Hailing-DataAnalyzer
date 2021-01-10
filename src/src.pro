QT       += core gui charts network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chartview.cpp \
    dataloader.cpp \
    dataset.cpp \
    drawchart.cpp \
    main.cpp \
    mainwindow.cpp \
    mainwindowsettings.cpp \
    networkmanager.cpp \
    nn.cpp \
    selectgrid.cpp \
    thermaldiagram.cpp \
    timepredictor.cpp \
    wizard.cpp


HEADERS += \
    chartview.h \
    dataloader.h \
    dataset.h \
    drawchart.h \
    mainwindow.h \
    networkmanager.h \
    nn.h \
    selectgrid.h \
    thermaldiagram.h \
    timepredictor.h \
    wizard.h

FORMS += \
    mainwindow.ui \
    wizard.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Figures.qrc
