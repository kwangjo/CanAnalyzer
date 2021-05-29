#include "canframe.h"
#include "commoncan.h"
#include <QDebug>

CanFrame::CanFrame() {

}

void CanFrame::GenerateWriteData(const uint32_t& address, QByteArray &data) {
    qDebug() << "length is : " << data.length() << " size address" << sizeof (address) << " total :" << data.length() + sizeof (address);
    data.prepend(static_cast<uint8_t>(address) & 0xFF);
    data.prepend(static_cast<uint8_t>(address >> 0x8 & 0xFF));
    data.prepend(static_cast<uint8_t>(address >> 0x10 & 0xFF));
    data.prepend(static_cast<uint8_t>(address >> 0x18 & 0xFF));
    uint16_t length = static_cast<uint16_t>(data.size());
    qDebug() << "length:" << length;
    data.prepend(length >> 8);
    data.prepend(length & 0xFF);
    data.push_front(CanPacketType::CAN);
    data.push_front(CanPacketDirection::CAN_ALL);
    data.push_front(P_STX);
    char crc = 0x00;
    data.append(crc); // crc
    data.append(P_EOF);

    qDebug() << data.toHex();
}

QByteArray CanFrame::DecodeData(const QByteArray &data) {
    QByteArray ret;
    int dataSize = data.size();
    if (dataSize < 2) {
        return ret;
    }
    if (data[0] == P_STX) {
        if (data[1] == CanPacketDirection::CAN_ALL || data[1] == CanPacketDirection::CAN_WRITE) {

            int length = data[2];
            qDebug () << "dataSize :" << dataSize << "  length" << length;
            if (dataSize -3 == length) {
                qDebug() << "[CMD::WRITE] data length :" << length;
            }
        } else {

        }
    }
    return ret;
}