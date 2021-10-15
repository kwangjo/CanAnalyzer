//#include "CannelloniCanBackend.h"
#include "SerialBusCanBackend.h"


//#include <QCanBusFactory>
#include <QtSerialBus/qcanbus.h>
#include <QtSerialBus/qcanbusdevice.h>
#include <QtSerialBus/qcanbusfactory.h>
#include <QObject>
#include <QStringList>
#include <limits>

#include <QtCore/qloggingcategory.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(QT_CANBUS_PLUGINS_SERIALCAN, "qt.canbus.plugins.serialcan")

class QtSerialCanBusPlugin : public QObject, public QCanBusFactoryV2 {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QCanBusFactory" FILE "plugin.json")
    Q_INTERFACES(QCanBusFactoryV2)

public:
    QList<QCanBusDeviceInfo> availableDevices(QString *errorMessage) const override {
//        if (errorMessage != nullptr)
//            errorMessage->clear();
        qDebug() << "availableDevices";
        QList<QCanBusDeviceInfo> info;
        return info;
    }

    QCanBusDevice* createDevice(const QString& interfaceName,
                                QString* errorMessage) const override {
        qDebug() << "Create Device";
        /*
        auto tokens = interfaceName.split(QChar(','));
        if (tokens.size() != 3) {
            *errorMessage = "Invalid interface name format";
            return nullptr;
        }
        bool ok;
        const auto quint16max = std::numeric_limits<quint16>::max();
        auto localPort = tokens[0].toUInt(&ok);
        if (!ok || localPort > quint16max) {
            *errorMessage = "Invalid local port format";
            return nullptr;
        }
        QHostAddress remoteAddr(tokens[1]);
        if (remoteAddr.isNull()) {
            *errorMessage = "Invalid remote address format";
            return nullptr;
        }
        auto remotePort = tokens[2].toUInt(&ok);
        if (!ok || remotePort > quint16max) {
            *errorMessage = "Invalid remote port format";
            return nullptr;
        }

        return new CannelloniCanBackend(localPort, remoteAddr, remotePort);
        */
        auto device = new SerialBusCanBackend("test");
//        return new SerialBusCanBackend("test");
//        return new SerialBusCanBackend();
        return device;
    }
};

QT_END_NAMESPACE


#include "main.moc"
