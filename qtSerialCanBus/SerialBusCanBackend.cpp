#include "SerialBusCanBackend.h"

//  Log
#include <QDebug>
#include <QTimerEvent>

const unsigned long OUTGOING_QUEUE_TIMEOUT_MSEC = 50;

SerialBusCanBackend::SerialBusCanBackend() {
    qDebug() << __FUNCTION__;
}

bool SerialBusCanBackend::writeFrame(const QCanBusFrame &frame) {
    qDebug() << __FUNCTION__;
    enqueueOutgoingFrame(frame);
    return true;
}

QString SerialBusCanBackend::interpretErrorFrame(const QCanBusFrame &errorFrame) {
    qDebug() << __FUNCTION__;
    return "Error frame received";
}

bool SerialBusCanBackend::open() {
    qDebug() << __FUNCTION__;

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
    qDebug() << __FUNCTION__;

    killTimer(timerID);
    timerID = 0;
}

void SerialBusCanBackend::timerEvent(QTimerEvent *event) {
    qDebug() << __FUNCTION__;
    Q_ASSERT(event->timerId() == timerID);

    auto frame = dequeueOutgoingFrame();
}

void SerialBusCanBackend::handlePacket(const QByteArray &data) {
    qDebug() << __FUNCTION__;
}

void SerialBusCanBackend::dataAvailable() {
    qDebug() << __FUNCTION__;
//    handlePacket(data)
}
