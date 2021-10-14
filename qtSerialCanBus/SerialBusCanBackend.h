#ifndef SERIALBUSCANBACKEND_H
#define SERIALBUSCANBACKEND_H

#include <QCanBusDevice>

class SerialBusCanBackend : public QCanBusDevice {
public:
    SerialBusCanBackend();
    ~SerialBusCanBackend() = default;

    bool writeFrame(const QCanBusFrame& frame) override;
    QString interpretErrorFrame(const QCanBusFrame& errorFrame) override;

protected:
    bool open() override;
    void close() override;
    void timerEvent(QTimerEvent*) override;

private:
    void handlePacket(const QByteArray& data);

private slots:
    void dataAvailable();

private:
    int timerID;
    quint8 serialPortNum;

};

#endif  // SERIALBUSCANBACKEND_H
