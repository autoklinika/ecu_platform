#include "Transport_CAN_Linux.h"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

Transport_CAN_Linux::Transport_CAN_Linux()
{
}

Transport_CAN_Linux::~Transport_CAN_Linux()
{
    close();
}

bool Transport_CAN_Linux::open(const std::string& iface, int /*bitrate*/)
{
    socketFd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(socketFd < 0)
        return false;

    struct ifreq ifr {};
    std::strncpy(ifr.ifr_name, iface.c_str(), IFNAMSIZ - 1);

    if(ioctl(socketFd, SIOCGIFINDEX, &ifr) < 0)
        return false;

    struct sockaddr_can addr {};
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if(bind(socketFd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        return false;

    return true;
}

void Transport_CAN_Linux::close()
{
    if(socketFd >= 0)
    {
        ::close(socketFd);
        socketFd = -1;
    }
}


bool Transport_CAN_Linux::sendFrame(uint32_t id,
                                    const uint8_t* data,
                                    uint8_t len)
{
    if(socketFd < 0)
        return false;

    struct can_frame frame {};
    frame.can_id  = id | CAN_EFF_FLAG;
    frame.can_dlc = len;
    std::memcpy(frame.data, data, len);

    return write(socketFd, &frame, sizeof(frame)) == sizeof(frame);
}

bool Transport_CAN_Linux::receiveFrame(uint32_t& id,
                                       uint8_t* data,
                                       uint8_t& len)
{
    if(socketFd < 0)
        return false;

    struct can_frame frame {};

    int n = read(socketFd, &frame, sizeof(frame));
    if(n <= 0)
        return false;

    id  = frame.can_id & CAN_EFF_MASK;
    len = frame.can_dlc;
    std::memcpy(data, frame.data, len);

    return true;
}


bool Transport_CAN_Linux::isValid() const
{
    return socketFd >= 0;
}