#ifndef CANFRAME_H
#define CANFRAME_H



#include <QtGlobal>
#include <QByteArray>

class CanFrame {
public:
    CanFrame();

    void GenerateWriteData(const uint32_t &address, QByteArray& data);
    QByteArray DecodeData(const QByteArray& data);
};

#endif // CANFRAME_H
