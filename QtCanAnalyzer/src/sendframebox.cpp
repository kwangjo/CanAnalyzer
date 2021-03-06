/****************************************************************************
**
** Copyright (C) 2017 Andre Hartmann <aha_1980@gmx.de>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the QtSerialBus module.
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

#include "sendframebox.h"
#include "ui_sendframebox.h"
#include "canframe.h"
#include "SerialCan.h"
#include <QDebug>

constexpr char THREE_DIGITS[] = "[[:xdigit:]]{3}";

enum {
    MaxStandardId = 0x7FF,
    MaxExtendedId = 0x10000000
};

enum {
    MaxPayload = 8,
    MaxPayloadFd = 64
};

bool isEvenHex(QString input)
{
    const QChar space = QLatin1Char(' ');
    input.remove(space);

    if (input.size() % 2)
        return false;

    return true;
}

// Formats a string of hex characters with a space between every byte
// Example: "012345" -> "01 23 45"
static QString formatHexData(const QString &input)
{
    const QChar space = QLatin1Char(' ');
    QString out = input;

    const QRegularExpression threeDigits(THREE_DIGITS);
    const QRegularExpression oneDigitAndSpace(QStringLiteral("((\\s+)|^)([[:xdigit:]]{1})(\\s+)"));

    while (oneDigitAndSpace.match(out).hasMatch() || threeDigits.match(out).hasMatch()) {
        if (threeDigits.match(out).hasMatch()) {
            const QRegularExpressionMatch match = threeDigits.match(out);
            out.insert(match.capturedEnd() - 1, space);
        } else if (oneDigitAndSpace.match(out).hasMatch()) {
            const QRegularExpressionMatch match = oneDigitAndSpace.match(out);
            if (out.at(match.capturedEnd() - 1) == space)
                out.remove(match.capturedEnd() - 1, 1);
        }
    }

    return out.simplified().toUpper();
}

HexIntegerValidator::HexIntegerValidator(QObject *parent) :
    QValidator(parent),
    m_maximum(MaxExtendedId)
{
}

QValidator::State HexIntegerValidator::validate(QString &input, int &) const
{
    bool ok;
    uint value = input.toUInt(&ok, 16);

    if (input.isEmpty())
        return Intermediate;

    if (!ok || value > m_maximum)
        return Invalid;

    return Acceptable;
}

void HexIntegerValidator::setMaximum(uint maximum)
{
    m_maximum = maximum;
}

HexStringValidator::HexStringValidator(QObject *parent) :
    QValidator(parent),
    m_maxLength(MaxPayloadFd)
{
}

QValidator::State HexStringValidator::validate(QString &input, int &pos) const
{
    const int maxSize = 2 * m_maxLength;
    const QChar space = QLatin1Char(' ');
    QString data = input;
    data.remove(space);

    if (data.isEmpty())
        return Intermediate;

    // limit maximum size and forbid trailing spaces
    if ((data.size() > maxSize) || (data.size() == maxSize && input.endsWith(space)))
        return Invalid;

    // check if all input is valid
    const QRegularExpression re(QStringLiteral("^[[:xdigit:]]*$"));
    if (!re.match(data).hasMatch())
        return Invalid;

    // insert a space after every two hex nibbles
    const QRegularExpression threeDigits(THREE_DIGITS);

    while (threeDigits.match(input).hasMatch()) {
        const QRegularExpressionMatch match = threeDigits.match(input);
        input.insert(match.capturedEnd() - 1, space);
        pos = match.capturedEnd() + 1;
    }

    return Acceptable;
}

void HexStringValidator::setMaxLength(int maxLength)
{
    m_maxLength = maxLength;
}

SendFrameBox::SendFrameBox(QWidget *parent) :
    QGroupBox(parent),
    m_ui(new Ui::SendFrameBox)
{
    m_ui->setupUi(this);

    m_hexIntegerValidator = new HexIntegerValidator(this);
    m_ui->frameIdEdit->setValidator(m_hexIntegerValidator);
    m_hexStringValidator = new HexStringValidator(this);
    m_ui->payloadEdit->setValidator(m_hexStringValidator);

    connect(m_ui->dataFrame, &QRadioButton::toggled, this, [this](bool set) {
        if (set)
            m_ui->flexibleDataRateBox->setEnabled(true);
    });

    connect(m_ui->remoteFrame, &QRadioButton::toggled, this, [this](bool set) {
        if (set) {
            m_ui->flexibleDataRateBox->setEnabled(false);
            m_ui->flexibleDataRateBox->setChecked(false);
        }
    });

    connect(m_ui->errorFrame, &QRadioButton::toggled, this, [this](bool set) {
        if (set) {
            m_ui->flexibleDataRateBox->setEnabled(false);
            m_ui->flexibleDataRateBox->setChecked(false);
        }
    });

    connect(m_ui->extendedFormatBox, &QCheckBox::toggled, this, [this](bool set) {
        m_hexIntegerValidator->setMaximum(set ? MaxExtendedId : MaxExtendedId);
    });

    connect(m_ui->flexibleDataRateBox, &QCheckBox::toggled, this, [this](bool set) {
        m_hexStringValidator->setMaxLength(set ? MaxPayloadFd : MaxPayloadFd);
        m_ui->bitrateSwitchBox->setEnabled(set);
        if (!set)
            m_ui->bitrateSwitchBox->setChecked(false);
    });

    auto frameIdOrPayloadChanged = [this]() {
        const bool hasFrameId = !m_ui->frameIdEdit->text().isEmpty();
        m_ui->sendButton->setEnabled(hasFrameId);
        m_ui->sendButton->setToolTip(hasFrameId
                                     ? QString() : tr("Cannot send because no Frame ID was given."));
        if (hasFrameId) {
            const bool isEven = isEvenHex(m_ui->payloadEdit->text());
            m_ui->sendButton->setEnabled(isEven);
            m_ui->sendButton->setToolTip(isEven
                                         ? QString() : tr("Cannot send because Payload hex string is invalid."));
        }
    };
    connect(m_ui->frameIdEdit, &QLineEdit::textChanged, frameIdOrPayloadChanged);
    connect(m_ui->payloadEdit, &QLineEdit::textChanged, frameIdOrPayloadChanged);
    frameIdOrPayloadChanged();

    connect(m_ui->sendButton, &QPushButton::clicked, [this]() {
        const uint frameId = m_ui->frameIdEdit->text().toUInt(nullptr, 16); // 8 -> 64
        QString data = m_ui->payloadEdit->text();
        m_ui->payloadEdit->setText(formatHexData(data));
        const QByteArray payload = QByteArray::fromHex(data.remove(QLatin1Char(' ')).toLatin1());

//        QByteArray data = QByteArray::fromHex(text.toLatin1()); // RawData
        QByteArray frameData = payload;
        QCanBusFrame frame = QCanBusFrame(frameId, payload);
        frame.setExtendedFrameFormat(m_ui->extendedFormatBox->isChecked());
        frame.setFlexibleDataRateFormat(m_ui->flexibleDataRateBox->isChecked());
        frame.setBitrateSwitch(m_ui->bitrateSwitchBox->isChecked());

        if (m_ui->errorFrame->isChecked())
            frame.setFrameType(QCanBusFrame::ErrorFrame);
        else if (m_ui->remoteFrame->isChecked())
            frame.setFrameType(QCanBusFrame::RemoteRequestFrame);

        int8_t canDeviceNumber = 0;
        if (m_ui->canDeviceBox->currentText() == "CAN1") {
            canDeviceNumber = 0;
        } else if (m_ui->canDeviceBox->currentText() == "CAN2") {
            canDeviceNumber = 1;
        }
        // Send to Serial Packet
        CanFrame canframe;
        canframe.GenerateWriteData(canDeviceNumber, frameId, frameData);
//        canframe.GenerateWriteData(frameId, frameData); // fix to channel
//        canframe.GenerateWriteData(canDeviceNumber, frame, frameData); // fix to channel
        SerialCan::getInstance().writePacket(frameData);

        emit sendFrame(frame);
    });
}

SendFrameBox::~SendFrameBox()
{
    delete m_ui;
}
