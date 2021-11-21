QT       += core gui serialport widgets serialbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += include

HEADERS += \
    include/commoncan.h \
    include/mainwindow.h \
    include/settingsdialog.h \
    include/canframe.cpp \
    include/SerialCan.h \
    include/sendframebox.h

SOURCES += \
    src/SerialCan.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/settingsdialog.cpp \
    src/canframe.cpp \
    src/sendframebox.cpp

RC_ICONS = images/CanIcon.ico

FORMS += \
    graphic/mainwindow.ui \
    graphic/settingsdialog.ui \
    graphic/sendframebox.ui

RESOURCES += \
    CanAnalyzer.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    CanAnalyzer.qrc
