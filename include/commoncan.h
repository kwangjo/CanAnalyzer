#ifndef COMMONCAN_H
#define COMMONCAN_H

constexpr char P_STX = 0xF0;
constexpr char P_EOF = 0xFE;

enum CanPacketType : char {
    CAN = 0,
    EVENT,
    COMMAND,
    LOG
};

enum CanChannel : char {
    CH1 = 0,
    CH2,
    MAX
};

enum CanPacketDirection : char {
    CAN_WRITE = 0,  // PC -> MCU
    CAN_READ,       // MCU -> PC
    CAN_ALL,        // ALL
    ERROR,
};

enum CanCommand : char {
    CAN_INIT = 0,       // Set CAN Initialize
    CAN_ID,             // Set CAN ID
    CAN_BITRATE,        // Set CAN Bitrate
    CAN_RS232_BITRATE,  // Set RS232 Bitrate
};

enum CanBitRate : char {
    CAN_100K,

};

enum CanSerialBitRate : char {
    UART_9600 = 0,
    UART_19200,
    UART_38400,
    UART_57600,
    UART_115200,
    UART_230400,
    UART_460800
};

#endif // COMMONCAN_H
