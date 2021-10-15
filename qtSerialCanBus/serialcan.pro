TARGET = qtserialcanbus
TEMPLATE = lib

QT = core network serialbus

CONFIG += plugin
CONFIG += c++14

HEADERS += \
    SerialBusCanBackend.h

SOURCES += \
    QtSerialCanBusPlugin.cpp \
    SerialBusCanBackend.cpp

DISTFILES = plugin.json

PLUGIN_TYPE = canbus
PLUGIN_CLASS_NAME = SerialCanBusPlugin
