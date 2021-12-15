#include "SerialCan.h"
#include "commoncan.h"

#include <QCanBusFrame>
#include <QByteArray>
#include <QSerialPortInfo>

#include <QDebug>


constexpr bool debug = false;

static std::unique_ptr<SerialCan> instance = nullptr;

SerialCan &SerialCan::getInstance() {
    if (instance == nullptr) {
        qDebug() << "1. CreateInstance";
        instance.reset(new SerialCan);
    }
    return *instance;
}

SerialCan::SerialCan() {
    qDebug() << "2. Create SerialCan";
    getPortsInfo();
}

// https://forum.kde.org/viewtopic.php?f=204&t=164584
bool SerialCan::calPacket(QByteArray &data) {
    int dataSize = data.size();

    // Serial RECV Check Data
    if (data[BIT_SOF] == static_cast<char>(P_SOF)) {
        if (data[BIT_TYPE] == static_cast<char>(CanPacketType::CAN)) {
            if (data[BIT_CMD] == static_cast<char>(CanPacketCMD::CAN_RECV_CAN0) ||
                    data[BIT_CMD] == static_cast<char>(CanPacketCMD::CAN_RECV_CAN1)) {
                int16_t dataLength =  (data[BIT_LENGTH_MSB] << 8) | data[BIT_LENGTH_LSB];
                int endPacket = P_MIN_SIZE +static_cast<int>(dataLength) + 1; // + CRC + EOF (2)
                if (dataSize >= endPacket) {
                    if (debug) qDebug() << "calPacket Lenth is" << static_cast<int>(dataLength) << " endPacketSize:" << endPacket;
                    if (data[endPacket] == static_cast<char>(P_EOF)) {
//                        if (debug) qDebug() << "EOF index[" << endPacket << "]";
                        // 1.Data Check CRC
                        // 2.MICOM -> PC
                        // 3 QCanBusFrame frame = QCanBusFrame(id, frameData);
                        quint32 id = data.mid(BIT_ADDRESS_START, 4).toHex().toUInt(nullptr, 16);
                        QByteArray frameData = data.mid(BIT_ADDRESS_START + 4, dataLength - 4);
                        QCanBusFrame frame = QCanBusFrame(id, frameData);
                        emit recvCanFrame(data[BIT_CMD] ,frame);
                        data.remove(0, endPacket + 1);
                        return true;
                    }
                } else {
                    qDebug() << "this packet is PC2";
                    return false;
                }
            } else {
                // This Pakcet is Only PC
                // USB to Serial [TX<->RX]
                qDebug() << "this packet is PC";
                if (data[BIT_CMD] == static_cast<char>(CanPacketCMD::CAN_SEND_CAN0) ||
                        data[BIT_CMD] == static_cast<char>(CanPacketCMD::CAN_SEND_CAN1)) {
                    int16_t dataLength =  (data[BIT_LENGTH_MSB] << 8) | data[BIT_LENGTH_LSB];
                    int endPacket = P_MIN_SIZE +static_cast<int>(dataLength) + 1; // + CRC + EOF (2)
                    if (dataSize >= endPacket) {
                        if (data[endPacket] == static_cast<char>(P_EOF)) {
                            quint32 id = data.mid(BIT_ADDRESS_START, 4).toHex().toUInt(nullptr, 16);
                            QByteArray frameData = data.mid(BIT_ADDRESS_START + 4, dataLength - 4);
                            QCanBusFrame frame = QCanBusFrame(id, frameData);
                            emit recvCanFrame(data[BIT_CMD] ,frame);
                            data.remove(0, endPacket + 1);
                            return true;
                        }
                    }
                }
            }
        } else {
            qDebug() << "remove !Packet[CanPacketType::CAN] dataSize" << data.size();
//            data.clear();
        }
    } else {
//        data.clear();
        return false;
    }
    return false;
}

void SerialCan::readData() {
    int count = 0;
    QMutexLocker lock(&mMutex);
    const QByteArray data = readAll();
    mRecvData.append(data);
    qDebug() << "mRecvData : " << mRecvData.size() << "read Data" << data.size();
    if (mRecvData.size() > P_MIN_SIZE) {
        while (calPacket(mRecvData)) {
            ++count;
        }
        if (debug) qDebug() << "recv Packet Data:" << count;
    }
    qDebug() << "Current mRecvData Size : " << mRecvData.size();
}

void SerialCan::destroyInstance() {
    if (instance != nullptr) {
        qDebug() << "3. DestroyInstance";
        instance.reset(nullptr);
    }
}

SerialCan::~SerialCan() {
    qDebug() << "4. Delete " << __FUNCTION__;
    serialClose();
    if (instance == nullptr) {
        qDebug() << "delete complete";
    }
}

bool SerialCan::serialOpen(QString &name, qint32 &baudRate) {
    setPortName(name);
    setBaudRate(baudRate);
    setDataBits(DataBits::Data8);
    setParity(Parity::NoParity);
    setStopBits(StopBits::UnknownStopBits);
    if (open(QIODevice::ReadWrite)) {
        setDataTerminalReady(true);  // true then the DTR signal is set to high
        setRequestToSend(true);      // true then the RTS signal is set to high
        qDebug() << "connected";
        connect(this, &QIODevice::readyRead, this, &SerialCan::readData);
    } else {
        qDebug() << "error Connected";
        return false;
    }

    return true;
}

void SerialCan::serialClose() {
    if (isOpen()) {
        qDebug() << "close SerialPort";
        close();
    }
}

QList<QSerialPortInfo> SerialCan::getPortsInfo() {
    QString description;
    QString manufacturer;
    QString serialNumber;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : "N/A")
             << (!manufacturer.isEmpty() ? manufacturer : "N/A")
             << (!serialNumber.isEmpty() ? serialNumber : "N/A")
             << info.systemLocation();
        qDebug() << list;
    }
    return infos;
}

qint64 SerialCan::writePacket(const QByteArray &data) {
    qint64 size = write(data);
    if (size != data.size()) {
        qDebug() << "wirte Packet Size origin:" << data.size() << " send: " << size;
    }
    return size;
}

void SerialCan::clearBuffer() {
    mRecvData.clear();
}
