#include "Transport_CAN_Linux.h"

#include <cstring>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>

Transport_CAN_Linux::Transport_CAN_Linux(const std::string& ifname)
{
    socket_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(socket_ < 0)
        return;

    struct ifreq ifr {};
    std::strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ - 1);

    if(ioctl(socket_, SIOCGIFINDEX, &ifr) < 0)
        return;

    struct sockaddr_can addr {};
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if(bind(socket_, reinterpret_cast<struct sockaddr*>(&addr),
            sizeof(addr)) < 0)
        return;
}

Transport_CAN_Linux::~Transport_CAN_Linux()
{
    closeSocket();
}

void Transport_CAN_Linux::closeSocket()
{
    if(socket_ >= 0)
    {
        close(socket_);
        socket_ = -1;
    }
}

bool Transport_CAN_Linux::isValid() const
{
    return socket_ >= 0;
}

bool Transport_CAN_Linux::sendFrame(uint32_t id,
                                    const uint8_t* data,
                                    uint8_t len)
{
    if(socket_ < 0)
        return false;

    struct can_frame frame {};
    frame.can_id  = id;
    frame.can_dlc = len;

    std::memcpy(frame.data, data, len);

    return write(socket_, &frame, sizeof(frame)) == sizeof(frame);
}

bool Transport_CAN_Linux::receiveFrame(uint32_t& id,
                                       uint8_t* data,
                                       uint8_t& len)
{
    struct can_frame frame {};
    int nbytes = read(socket_, &frame, sizeof(frame));

    if(nbytes <= 0)
        return false;

    id  = frame.can_id;
    len = frame.can_dlc;
    std::memcpy(data, frame.data, len);

    return true;
}