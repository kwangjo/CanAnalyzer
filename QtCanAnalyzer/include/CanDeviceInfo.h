#ifndef CANDEVICEINFO_H
#define CANDEVICEINFO_H

#include <qglobal.h>
#include <QByteArray>

class CanDeviceInfo {

public:
    CanDeviceInfo();
    CanDeviceInfo(const qint8 Channel, const qint8 FrameType, const qint8 CanMode, const qint8 CanBuad, const qint8 CanBuadData);
    ~CanDeviceInfo() = default;

    QByteArray getCanDevieData();

    bool setChannel(const qint8 Channel);
    bool setFrameType(const qint8 FrameType);
    bool setCanMode(const qint8 CanMode);
    bool setCanBuad(const qint8 CanBuad);
    bool setCanBuadData(const qint8 CanBuadData);

    qint8 getChannel() const;
    qint8 getFrameType() const;
    qint8 getCanMode() const;
    qint8 getCanBuad() const;
    qint8 getCanBuadData() const;

    void printInfo();
private:
    QString getStringType();
    QString getStringMode();
    QString getStringBauad();
    QString getStringBauadData();
private:
    qint8 channel;
    qint8 frameType;
    qint8 canMode;
    qint8 canBuad;
    qint8 canBuadData;
};

#endif  //  CANDEVICEINFO_H
