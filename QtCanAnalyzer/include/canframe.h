#ifndef CANFRAME_H
#define CANFRAME_H



#include <QtGlobal>
#include <QByteArray>

//#include <QCanBusFrame>

class CanFrame {
public:
    CanFrame();

//    void GenerateWriteData(const int8_t &channel, const QCanBusFrame &frame, QByteArray &data);
    void GenerateWriteData(const int8_t &channel, const uint32_t &address, QByteArray& data);
    void GenerateWriteData(const uint32_t &address, QByteArray& data);
    QByteArray DecodeData(const QByteArray& data);

private:
    char makeCRC(const QByteArray &data);
};

#endif // CANFRAME_H
