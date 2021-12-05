#ifndef COMMONCAN_H
#define COMMONCAN_H

constexpr char P_SOF = 0xF0;
constexpr char P_EOF = 0xFE;

constexpr char P_MIN_SIZE = 5;
constexpr char BIT_SOF        = 0;
constexpr char BIT_TYPE       = 1;
constexpr char BIT_CMD        = 2;
constexpr char BIT_LENGTH_LSB = 3;
constexpr char BIT_LENGTH_MSB = 4;
constexpr char BIT_ADDRESS_START = 5;

enum CanPacketType : char {
    CAN = 0x00,
    EVENT,
    COMMAND,
    LOG
};

enum CanPacketCMD : char {
    CAN_RECV_CAN0 = 0x00,
    CAN_RECV_CAN1 = 0x01,
    CAN_SEND_CAN0 = 0x02,
    CAN_SEND_CAN1 = 0x03
};

enum CanChannel : char {
    CH1 = 0x00,
    CH2,
    MAX
};

enum CanPacketDirection : char {
    CAN_WRITE = 0x00,  // PC -> MCU
    CAN_READ,          // MCU -> PC
    CAN_ALL,           // ALL
    ERROR,
};

enum CanCommand : char {
    CAN_INIT = 0x00,       // Set CAN Initialize
    CAN_ID,                // Set CAN ID
    CAN_BITRATE,           // Set CAN Bitrate
    CAN_RS232_BITRATE,     // Set RS232 Bitrate
};

enum CanBitRate : char {
    CAN_100K = 0x00,

};

enum CanSerialBitRate : char {
    UART_9600 = 0x00,
    UART_19200,
    UART_38400,
    UART_57600,
    UART_115200,
    UART_230400,
    UART_460800
};

#endif // COMMONCAN_H
