#include "parser.h"

#include <arpa/inet.h>
#include <string.h>
#include <stdexcept>
#include <iostream>

void parseFrames(uint16_t len, const uint8_t* buffer, std::function<canfd_frame*()> frameAllocator,
        std::function<void(canfd_frame*, bool)> frameReceiver)
{
    std::cout << "parseFrames " << std::endl;
}

uint8_t* buildPacket(uint16_t len, uint8_t* packetBuffer,
        std::list<canfd_frame*>& frames, uint8_t seqNo,
        std::function<void(std::list<canfd_frame*>&, std::list<canfd_frame*>::iterator)> handleOverflow)
{
    uint16_t frameCount = 0;
    uint8_t *data = nullptr;
    std::cout << "buildPacket " << std::endl;
//    uint8_t* data = packetBuffer + CANNELLONI_DATA_PACKET_BASE_SIZE;

    return data;
}
