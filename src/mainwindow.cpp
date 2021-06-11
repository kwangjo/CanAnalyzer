/****************************************************************************
**
** This Code Base Qt example CAN Bus example, Terminal Example
**
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "commoncan.h"
#include "canframe.h"

#include <QLabel>
#include <QMessageBox>
#include <QDesktopServices>


#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_status(new QLabel),
    m_settings(new SettingsDialog),
    m_serial(new QSerialPort(this)),
    mStatus(0)
{
    m_ui->setupUi(this);
    QWidget::setWindowTitle("CanAnalyzer v0.1  ");
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionQuit->setEnabled(true);
    m_ui->actionConfigure->setEnabled(true);
    m_ui->statusBar->addWidget(m_status);

    initActionsConnections();

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
}

MainWindow::~MainWindow()
{
    delete m_settings;
    delete m_ui;
}

void MainWindow::openSerialPort()
{
    const SettingsDialog::Settings p = m_settings->settings();
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
        m_ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());

        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen())
        m_serial->close();
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("안녕하세요."),
                       tr("전자공작 까페에서 Can FD Analyzer 관련 개인취미생활을 진행하려고 만들게 되었습니다."
                          "반갑습니다. Qt 예제와 오픈소스를 참고로 진행 예정입니다."
                          "<td><a href=\"https://cafe.naver.com/circuitsmanual\">전자공작</a></td>"
                          "<a href=\"https://www.notion.so/CAN-FD-Analyzer-0ea49aa3d1664fd88a1eb4d8518fdae4\">스터디</a>"
                          ));
}

void MainWindow::writeData(const QByteArray &data)
{
    m_serial->write(data);
}

void MainWindow::readData()
{
    const QByteArray data = m_serial->readAll();
    mRecvData.append(data);
// ToDO 패킷 연결
    if (data.size() >= 3) {
        if (data[0] == static_cast<char>(0xF0)) {
            qDebug() << "startBit";
            if (data[1] == CanPacketDirection::CAN_READ || data[1] == CanPacketDirection::CAN_WRITE) {
                qDebug() << "cmd OK";
                auto length = data[2];
                qDebug() << "check Length :" << length + 3 << "  dataSize: " << data.size();
                if (length + 3 == data.size()) {
                    qDebug() << "data is OK";
                } else {
                    qDebug() << "copy Data";
                }
            } else {
                // Not running
            }
        } else {
            qDebug() << "size Error";
        }
    }
    qDebug() << "readData" << data.size();
    qDebug() << "HEX: " << data.toHex();

}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::initActionsConnections()
{
    connect(m_ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(m_ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(m_ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(m_ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

char MainWindow::makeCRC(const QByteArray &data)
{
    char checkSum = 0;
    for (auto cData : data) {
        checkSum ^= cData;
    }
    return checkSum;
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}

void MainWindow::on_btn_send_clicked() {
    int address = 0x44332211;

    QString text = m_ui->input_text->text();
    QByteArray data = QByteArray::fromHex(text.toLatin1()); // RawData

    CanFrame frame;
    frame.GenerateWriteData(address, data);
    auto length = m_serial->write(data);
    m_ui->text_recv->append(QString(data.toHex()).toUpper());
}
