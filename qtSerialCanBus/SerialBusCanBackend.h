#ifndef SERIALBUSCANBACKEND_H
#define SERIALBUSCANBACKEND_H

#include <QCanBusDevice>

class SerialBusCanBackend : public QCanBusDevice {
    Q_OBJECT
public:
//    explicit SerialBusCanBackend();
    explicit SerialBusCanBackend(const QString &interface, QObject *parent = nullptr);
    ~SerialBusCanBackend() = default;

    bool writeFrame(const QCanBusFrame& frame) override;
    QString interpretErrorFrame(const QCanBusFrame& errorFrame) override;

//    static QList<QCanBusDeviceInfo> interfaces();
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
