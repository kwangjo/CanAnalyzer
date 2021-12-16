#include "CanDeviceInfo.h"
#include "commoncan.h"
#include <QDebug>

CanDeviceInfo::CanDeviceInfo() {

}

CanDeviceInfo::CanDeviceInfo(const qint8 Channel, const qint8 FrameType, const qint8 CanMode, const qint8 CanBuad, const qint8 CanBuadData)
    : channel(Channel), frameType(FrameType), canMode(CanMode), canBuad(CanBuad), canBuadData(CanBuadData) { }


QByteArray CanDeviceInfo::getCanDevieData() {
    QByteArray arrayData;
    arrayData.append(channel);
    arrayData.append(canMode);
    arrayData.append(frameType);
    arrayData.append(canBuad);
    arrayData.append(canBuadData);

    return arrayData;
}

bool CanDeviceInfo::setChannel(const qint8 Channel) {
    channel = Channel;
    return true;
}

bool CanDeviceInfo::setFrameType(const qint8 FrameType) {
    frameType = FrameType;
    return true;
}

bool CanDeviceInfo::setCanMode(const qint8 CanMode) {
    canMode = CanMode;
    return true;
}

bool CanDeviceInfo::setCanBuad(const qint8 CanBuad) {
    canBuad = CanBuad;
    return true;
}

bool CanDeviceInfo::setCanBuadData(const qint8 CanBuadData) {
    canBuadData = CanBuadData;
    return true;
}

qint8 CanDeviceInfo::getChannel() const {
    return channel;
}

qint8 CanDeviceInfo::getFrameType() const {
    return frameType;
}

qint8 CanDeviceInfo::getCanMode() const {
    return canMode;
}

qint8 CanDeviceInfo::getCanBuad() const {
    return canBuad;
}

qint8 CanDeviceInfo::getCanBuadData() const {
    return canBuadData;
}

void CanDeviceInfo::printInfo() {
    qDebug() << "[CAN" << channel << getStringType() << getStringMode()
             << getStringBauad() << getStringBauadData();
}

QString CanDeviceInfo::getStringType() {
    QString string = " Type : ";
    switch (frameType) {
        case CANFrameType::CAN_CLASSIC:
            string += "CAN_CLASSIC";
            break;
        case CANFrameType::CAN_FD_NO_BRS:
            string += "CAN_FD_NO_BRS";
            break;
        case CANFrameType::CAN_FD_BRS:
            string += "CAN_FD_BRS";
            break;
        default:
            string += "NONE";
    }
    return string;
}

QString CanDeviceInfo::getStringMode() {
    QString string = " Mode : ";
    switch (canMode) {
        case CANMode::CAN_NORMAL:
            string += "CAN_NORMAL";
            break;
        case CANMode::CAN_MONITOR:
            string += "CAN_MONITOR";
            break;
        case CANMode::CAN_LOOPBACK:
            string += "CAN_LOOPBACK";
            break;
        default:
            string += "NONE";
    }
    return string;
}

QString CanDeviceInfo::getStringBauad() {
    QString string = " Bauad : ";
    switch (canBuad) {
        case CanBitRate::CAN_100K:
            string += "CAN_100K";
            break;
        case CanBitRate::CAN_125K:
            string += "CAN_125K";
            break;
        case CanBitRate::CAN_250K:
            string += "CAN_250K";
            break;
        case CanBitRate::CAN_500K:
            string += "CAN_500K";
            break;
        case CanBitRate::CAN_1M:
            string += "CAN_1M";
            break;
        case CanBitRate::CAN_2M:
            string += "CAN_2M";
            break;
        case CanBitRate::CAN_4M:
            string += "CAN_4M";
            break;
        case CanBitRate::CAN_5M:
            string += "CAN_5M";
            break;
        default:
            string += "NONE";
    }
    return string;
}

QString CanDeviceInfo::getStringBauadData() {
    QString string = " BauadData : ";
    switch (canBuadData) {
        case CanBitRate::CAN_100K:
            string += "CAN_100K";
            break;
        case CanBitRate::CAN_125K:
            string += "CAN_125K";
            break;
        case CanBitRate::CAN_250K:
            string += "CAN_250K";
            break;
        case CanBitRate::CAN_500K:
            string += "CAN_500K";
            break;
        case CanBitRate::CAN_1M:
            string += "CAN_1M";
            break;
        case CanBitRate::CAN_2M:
            string += "CAN_2M";
            break;
        case CanBitRate::CAN_4M:
            string += "CAN_4M";
            break;
        case CanBitRate::CAN_5M:
            string += "CAN_5M";
            break;
        default:
            string += "NONE";
    }
    string += "]";
    return string;
}
