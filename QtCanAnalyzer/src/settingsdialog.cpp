/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "commoncan.h"

#include <QIntValidator>
#include <QLineEdit>
#include <QSerialPortInfo>

#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

#include <QDebug>


static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog),
    m_intValidator(new QIntValidator(0, 4000000, this))
{
    m_ui->setupUi(this);

    m_ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(m_ui->applyButton, &QPushButton::clicked,
            this, &SettingsDialog::apply);
    connect(m_ui->serialPortInfoListBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::showPortInfo);
    connect(m_ui->baudRateBox,  QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::checkCustomBaudRatePolicy);
    connect(m_ui->serialPortInfoListBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::checkCustomDevicePathPolicy);
    connect(m_ui->saveButton, &QPushButton::clicked,
            this, &SettingsDialog::saveSetting);
    fillPortsParameters();
    fillPortsInfo();

    updateSettings();
    loadJson();
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

SettingsDialog::Settings SettingsDialog::settings() const
{
    return m_currentSettings;
}

void SettingsDialog::showPortInfo(int idx)
{
    if (idx == -1)
        return;

    const QStringList list = m_ui->serialPortInfoListBox->itemData(idx).toStringList();
    m_ui->descriptionLabel->setText(tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    m_ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    m_ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
    m_ui->locationLabel->setText(tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
    m_ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
    m_ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}

void SettingsDialog::apply()
{
    updateSettings();
    hide();
}

void SettingsDialog::checkCustomBaudRatePolicy(int idx)
{
    const bool isCustomBaudRate = !m_ui->baudRateBox->itemData(idx).isValid();
    m_ui->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        m_ui->baudRateBox->clearEditText();
        QLineEdit *edit = m_ui->baudRateBox->lineEdit();
        edit->setValidator(m_intValidator);
    }
}

void SettingsDialog::checkCustomDevicePathPolicy(int idx)
{
    const bool isCustomPath = !m_ui->serialPortInfoListBox->itemData(idx).isValid();
    m_ui->serialPortInfoListBox->setEditable(isCustomPath);
    if (isCustomPath)
        m_ui->serialPortInfoListBox->clearEditText();
}

void SettingsDialog::saveSetting() {
    QJsonObject json;
    QJsonDocument document;

    json.insert("Can1Mode" , m_ui->can1ModeBox->currentIndex());
    json.insert("Can1Frame" , m_ui->can1FrameBox->currentIndex());
    json.insert("Can1Buad" , m_ui->can1BuadBox->currentIndex());
    json.insert("Can1BuadData" , m_ui->can1BaudDataBox->currentIndex());
    json.insert("Can2Mode" , m_ui->can2ModeBox->currentIndex());
    json.insert("Can2Frame" , m_ui->can2FrameBox->currentIndex());
    json.insert("Can2Buad" , m_ui->can2BuadBox->currentIndex());
    json.insert("Can2BuadData" , m_ui->can2BaudDataBox->currentIndex());
    json.insert("SerialPort" , m_ui->serialPortInfoListBox->currentText());

    if (0) {
        qDebug () << "1 Mode" << m_ui->can1ModeBox->currentIndex() << " Frame : " << m_ui->can1FrameBox->currentIndex()
                  << " Buad: " << m_ui->can1BuadBox->currentIndex() << " BuadData: " << m_ui->can1BaudDataBox->currentIndex();
        qDebug () << "2 Mode" << m_ui->can2ModeBox->currentIndex() << " Frame : " << m_ui->can2FrameBox->currentIndex()
                  << " Buad: " << m_ui->can2BuadBox->currentIndex() << " BuadData: " << m_ui->can2BaudDataBox->currentIndex();
        qDebug() << "Json:" << json;
    }

    document.setObject(json);
    QByteArray bytes = document.toJson(QJsonDocument::Indented);
    QFile jsonFile("Setting.json");
    if (jsonFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate )) {
        QTextStream iStream( &jsonFile );
        iStream.setCodec( "utf-8" );
        iStream << bytes;
        jsonFile.close();
    }
}

void SettingsDialog::fillPortsParameters()
{
    m_ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    m_ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    m_ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    m_ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    m_ui->baudRateBox->addItem(QStringLiteral("115201"), 115201);
    m_ui->baudRateBox->addItem(tr("Custom"));
    m_ui->baudRateBox->setCurrentIndex(3);
    m_ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    m_ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    m_ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    m_ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    m_ui->dataBitsBox->setCurrentIndex(3);

    m_ui->parityBox->addItem(tr("None"), QSerialPort::NoParity);
    m_ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    m_ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    m_ui->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    m_ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    m_ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    m_ui->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    m_ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    m_ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    m_ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    m_ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);

    m_ui->can1FrameBox->addItem(tr("CLASSIC"), CANFrameType::CAN_CLASSIC);
    m_ui->can1FrameBox->addItem(tr("FDNOBRS"), CANFrameType::CAN_FD_NO_BRS);
    m_ui->can1FrameBox->addItem(tr("FDBRS"), CANFrameType::CAN_FD_BRS);
    m_ui->can2FrameBox->addItem(tr("CLASSIC"), CANFrameType::CAN_CLASSIC);
    m_ui->can2FrameBox->addItem(tr("FDNOBRS"), CANFrameType::CAN_FD_NO_BRS);
    m_ui->can2FrameBox->addItem(tr("FDBRS"), CANFrameType::CAN_FD_BRS);

    m_ui->can1ModeBox->addItem(tr("NORMAL"), CANMode::CAN_NORMAL);
    m_ui->can1ModeBox->addItem(tr("MONITOR"), CANMode::CAN_NORMAL);
    m_ui->can1ModeBox->addItem(tr("LOOPBACK"), CANMode::CAN_NORMAL);
    m_ui->can2ModeBox->addItem(tr("NORMAL"), CANMode::CAN_NORMAL);
    m_ui->can2ModeBox->addItem(tr("MONITOR"), CANMode::CAN_NORMAL);
    m_ui->can2ModeBox->addItem(tr("LOOPBACK"), CANMode::CAN_NORMAL);

    m_ui->can1BuadBox->addItem(tr("100K"), CanBitRate::CAN_100K);
    m_ui->can1BuadBox->addItem(tr("125K"), CanBitRate::CAN_125K);
    m_ui->can1BuadBox->addItem(tr("250K"), CanBitRate::CAN_250K);
    m_ui->can1BuadBox->addItem(tr("500K"), CanBitRate::CAN_500K);
    m_ui->can1BuadBox->addItem(tr("1M"), CanBitRate::CAN_1M);
    m_ui->can1BuadBox->addItem(tr("2M"), CanBitRate::CAN_2M);
    m_ui->can1BuadBox->addItem(tr("4M"), CanBitRate::CAN_4M);
    m_ui->can1BuadBox->addItem(tr("5M"), CanBitRate::CAN_5M);
    m_ui->can2BuadBox->addItem(tr("100K"), CanBitRate::CAN_100K);
    m_ui->can2BuadBox->addItem(tr("125K"), CanBitRate::CAN_125K);
    m_ui->can2BuadBox->addItem(tr("250K"), CanBitRate::CAN_250K);
    m_ui->can2BuadBox->addItem(tr("500K"), CanBitRate::CAN_500K);
    m_ui->can2BuadBox->addItem(tr("1M"), CanBitRate::CAN_1M);
    m_ui->can2BuadBox->addItem(tr("2M"), CanBitRate::CAN_2M);
    m_ui->can2BuadBox->addItem(tr("4M"), CanBitRate::CAN_4M);
    m_ui->can2BuadBox->addItem(tr("5M"), CanBitRate::CAN_5M);

    m_ui->can1BaudDataBox->addItem(tr("100K"), CanBitRate::CAN_100K);
    m_ui->can1BaudDataBox->addItem(tr("125K"), CanBitRate::CAN_125K);
    m_ui->can1BaudDataBox->addItem(tr("250K"), CanBitRate::CAN_250K);
    m_ui->can1BaudDataBox->addItem(tr("500K"), CanBitRate::CAN_500K);
    m_ui->can1BaudDataBox->addItem(tr("1M"), CanBitRate::CAN_1M);
    m_ui->can1BaudDataBox->addItem(tr("2M"), CanBitRate::CAN_2M);
    m_ui->can1BaudDataBox->addItem(tr("4M"), CanBitRate::CAN_4M);
    m_ui->can1BaudDataBox->addItem(tr("5M"), CanBitRate::CAN_5M);
    m_ui->can2BaudDataBox->addItem(tr("100K"), CanBitRate::CAN_100K);
    m_ui->can2BaudDataBox->addItem(tr("125K"), CanBitRate::CAN_125K);
    m_ui->can2BaudDataBox->addItem(tr("250K"), CanBitRate::CAN_250K);
    m_ui->can2BaudDataBox->addItem(tr("500K"), CanBitRate::CAN_500K);
    m_ui->can2BaudDataBox->addItem(tr("1M"), CanBitRate::CAN_1M);
    m_ui->can2BaudDataBox->addItem(tr("2M"), CanBitRate::CAN_2M);
    m_ui->can2BaudDataBox->addItem(tr("4M"), CanBitRate::CAN_4M);
    m_ui->can2BaudDataBox->addItem(tr("5M"), CanBitRate::CAN_5M);
}

void SettingsDialog::fillPortsInfo()
{
    m_ui->serialPortInfoListBox->clear();
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
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        m_ui->serialPortInfoListBox->addItem(list.first(), list);
    }

    m_ui->serialPortInfoListBox->addItem(tr("Custom"));
}

void SettingsDialog::updateSettings()
{
    m_currentSettings.name = m_ui->serialPortInfoListBox->currentText();

    if (m_ui->baudRateBox->currentIndex() == 4) {
        m_currentSettings.baudRate = m_ui->baudRateBox->currentText().toInt();
    } else {
        m_currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                    m_ui->baudRateBox->itemData(m_ui->baudRateBox->currentIndex()).toInt());
    }

    m_currentSettings.stringBaudRate = QString::number(m_currentSettings.baudRate);

    m_currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
                m_ui->dataBitsBox->itemData(m_ui->dataBitsBox->currentIndex()).toInt());
    m_currentSettings.stringDataBits = m_ui->dataBitsBox->currentText();

    m_currentSettings.parity = static_cast<QSerialPort::Parity>(
                m_ui->parityBox->itemData(m_ui->parityBox->currentIndex()).toInt());
    m_currentSettings.stringParity = m_ui->parityBox->currentText();

    m_currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                m_ui->stopBitsBox->itemData(m_ui->stopBitsBox->currentIndex()).toInt());
    m_currentSettings.stringStopBits = m_ui->stopBitsBox->currentText();

    m_currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                m_ui->flowControlBox->itemData(m_ui->flowControlBox->currentIndex()).toInt());
    m_currentSettings.stringFlowControl = m_ui->flowControlBox->currentText();

    //    m_currentSettings.localEchoEnabled = m_ui->localEchoCheckBox->isChecked();
}

QVariantMap SettingsDialog::loadJson() {
    qDebug() << "loadJson Complete";
    QFile jsonFile("Setting.json");
    QVariantMap map;
    if (jsonFile.open(QFile::ReadOnly)) {
        QJsonDocument document = QJsonDocument().fromJson(jsonFile.readAll());
        jsonFile.close();
        map = document.object().toVariantMap();
        QVariant value = map.value("Can1Mode");
        m_ui->can1ModeBox->setCurrentIndex(value.toInt());

        value = map.value("Can1Frame");
        m_ui->can1FrameBox->setCurrentIndex(value.toInt());

        value = map.value("Can1Buad");
        m_ui->can1BuadBox->setCurrentIndex(value.toInt());

        value = map.value("Can1BuadData");
        m_ui->can1BaudDataBox->setCurrentIndex(value.toInt());

        value = map.value("Can2Mode");
        m_ui->can2ModeBox->setCurrentIndex(value.toInt());

        value = map.value("Can2Frame");
        m_ui->can2FrameBox->setCurrentIndex(value.toInt());

        value = map.value("Can2Buad");
        m_ui->can2BuadBox->setCurrentIndex(value.toInt());

        value = map.value("Can2BuadData");
        m_ui->can2BaudDataBox->setCurrentIndex(value.toInt());

        value = map.value("SerialPort");
        m_ui->serialPortInfoListBox->setCurrentText(value.toString());
        m_currentSettings.name = value.toString();
    }
    return map;
}
