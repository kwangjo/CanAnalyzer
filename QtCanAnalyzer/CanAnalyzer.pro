QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

INCLUDEPATH += include

HEADERS += \
    include/commoncan.h \
    include/mainwindow.h \
    include/settingsdialog.h \
    include/canframe.cpp

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/settingsdialog.cpp \
    src/canframe.cpp

RC_ICONS = images/CanIcon.ico

FORMS += \
    graphic/mainwindow.ui \
    graphic/settingsdialog.ui

RESOURCES += \
    CanAnalyzer.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    CanAnalyzer.qrc
