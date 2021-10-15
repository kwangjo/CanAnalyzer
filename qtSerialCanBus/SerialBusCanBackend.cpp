#include "SerialBusCanBackend.h"

//  Log
#include <QDebug>
#include <QTimerEvent>
#include <iostream>

const unsigned long OUTGOING_QUEUE_TIMEOUT_MSEC = 50;

//SerialBusCanBackend::SerialBusCanBackend() {
//    qDebug() << __FUNCTION__;
//}

SerialBusCanBackend::SerialBusCanBackend(const QString &interface, QObject *parent) : QCanBusDevice(parent) {
    qDebug() << Q_FUNC_INFO;
}

bool SerialBusCanBackend::writeFrame(const QCanBusFrame &frame) {
    qDebug() << Q_FUNC_INFO;
    enqueueOutgoingFrame(frame);
    return true;
}

QString SerialBusCanBackend::interpretErrorFrame(const QCanBusFrame &errorFrame) {
    qDebug() << Q_FUNC_INFO;
    return "Error frame received";
}

//QList<QCanBusDeviceInfo> SerialBusCanBackend::interfaces() {
//    QList<QCanBusDeviceInfo> result;
//    result.append(std::move(createDeviceInfo(
//                                QStringLiteral("can%1").arg(channel), QString(),
//                                QStringLiteral("Qt Virtual CAN bus"), channel,
//                                true, true)));
//    return result;
//}

bool SerialBusCanBackend::open() {
    qDebug() << Q_FUNC_INFO;
    std::cout << "SerialBusCanBackend::open()" << std::endl;
    // Not Connected
    if (0) {
        setState(QCanBusDevice::UnconnectedState);
        return false;
    }

    // Connect readRead -> dataAvailable
    setState(QCanBusDevice::ConnectedState);
    timerID = startTimer(OUTGOING_QUEUE_TIMEOUT_MSEC);
    return true;
}

void SerialBusCanBackend::close() {
    qDebug() << Q_FUNC_INFO;
    std::cout << "SerialBusCanBackend::close()" << std::endl;
    killTimer(timerID);
    timerID = 0;
}

void SerialBusCanBackend::timerEvent(QTimerEvent *event) {
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(event->timerId() == timerID);
    std::cout << "timerEvent" << std::endl;
    auto frame = dequeueOutgoingFrame();

    // test write Frame
    if (frame.isValid()) {
        std::cout << "frame id: " << frame.frameId() << std::endl;
        QVector<QCanBusFrame> newFrames;
        newFrames.push_back(frame);
        enqueueReceivedFrames(newFrames);
    }

}

void SerialBusCanBackend::handlePacket(const QByteArray &data) {
    qDebug() << Q_FUNC_INFO;
}

void SerialBusCanBackend::dataAvailable() {
    qDebug() << Q_FUNC_INFO;
//    handlePacket(data)
}
