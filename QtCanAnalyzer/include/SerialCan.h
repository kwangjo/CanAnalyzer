#ifndef SERIALCAN_H
#define SERIALCAN_H

//class QCanBusFrame;

#include <memory>
#include <QSerialPort>

#include <QMutex>

class SerialCan : public QSerialPort {
    Q_OBJECT
public:
    static SerialCan &getInstance();
    static void destroyInstance();

    ~SerialCan();

    bool serialOpen(QString &name, qint32 &baudRate);
    void serialClose();
    QList<QSerialPortInfo> getPortsInfo();

    qint64 writePacket(const QByteArray &data);

    void clearBuffer();
private:
    SerialCan();
//    ~SerialCan() = default;


    bool calPacket(QByteArray &data);
private slots:
    void readData();

private:
    QByteArray mRecvData;
    QMutex mMutex;
//    static std::unique_ptr<SerialCan> instance;
};

#endif  // SERIALCAN_H
