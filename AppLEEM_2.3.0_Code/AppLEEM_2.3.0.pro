QT       += widgets core gui quick qml 3dcore 3drender 3dinput 3dlogic 3dextras 3danimation serialport uitools quickwidgets printsupport positioning datavisualization #core5compat

CONFIG += c++17

LIBS += -lOpenGL32

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RESOURCES = qml.qrc

SOURCES += \
    bench.cpp \
    benchgraphs.cpp \
    dataprocessor.cpp \
    extradisplay.cpp \
    helpdialog.cpp \
    ignition.cpp \
    ignitors.cpp \
    main.cpp \
    mainwindow.cpp \
    metadata.cpp \
    powermodule.cpp \
    qcustomplot.cpp \
    rocketcontroller.cpp \
    rocketgraphics.cpp \
    serialdevice.cpp \
    rocketmain.cpp \
    serialconnector.cpp \
    switch.cpp \  
    terminal.cpp

HEADERS += \
    bench.h \
    benchgraphs.h \
    dataprocessor.h \
    extradisplay.h \
    helpdialog.h \
    ignition.h \
    ignitors.h \
    mainwindow.h \
    metadata.h \
    myconfig.h \
    powermodule.h \
    qcustomplot.h \
    rocketcontroller.h \
    rocketgraphics.h \
    serialdevice.h \
    rocketmain.h \
    serialconnector.h \
    styleButton.h \
    switch.h \
    terminal.h

FORMS += \
    bench.ui \
    benchgraphs.ui \
    extradisplay.ui \
    helpdialog.ui \
    ignition.ui \
    ignitors.ui \
    mainwindow.ui \
    metadata.ui \
    powermodule.ui \
    serialconnector.ui \
    rocketcontroller.ui \
    rocketgraphics.ui \   
    rocketmain.ui \
    terminal.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

TARGET = AppLEEM
RC_ICONS = LogoLEEM.ico
