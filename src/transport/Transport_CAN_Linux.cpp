#include "Transport_CAN_Linux.h"

#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>

Transport_CAN_Linux::Transport_CAN_Linux(const char* interface)
    : socket_fd(-1)
{
    socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_fd < 0)
    {
        perror("CAN socket creation failed");
        return;
    }

    struct ifreq ifr {};
    std::strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);

    if (ioctl(socket_fd, SIOCGIFINDEX, &ifr) < 0)
    {
        perror("ioctl failed");
        close(socket_fd);
        socket_fd = -1;
        return;
    }

    struct sockaddr_can addr {};
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(socket_fd,
             reinterpret_cast<struct sockaddr*>(&addr),
             sizeof(addr)) < 0)
    {
        perror("bind failed");
        close(socket_fd);
        socket_fd = -1;
        return;
    }

    fcntl(socket_fd, F_SETFL, O_NONBLOCK);
}

Transport_CAN_Linux::~Transport_CAN_Linux()
{
    if (socket_fd >= 0)
        close(socket_fd);
}

bool Transport_CAN_Linux::isValid() const
{
    return socket_fd >= 0;
}

bool Transport_CAN_Linux::receiveFrame(uint32_t& id,
                                       uint8_t* data,
                                       uint8_t& len)
{
    if (socket_fd < 0)
        return false;

    struct can_frame frame {};

    int nbytes = read(socket_fd, &frame, sizeof(frame));
    if (nbytes <= 0)
        return false;

    if (frame.can_id & CAN_EFF_FLAG)
        id = frame.can_id & CAN_EFF_MASK;
    else
        id = frame.can_id & CAN_SFF_MASK;

    len = frame.can_dlc;
    if (len > 8) len = 8;

    std::memcpy(data, frame.data, len);

    return true;
}

bool Transport_CAN_Linux::sendFrame(uint32_t id,
                                    const uint8_t* data,
                                    uint8_t len,
                                    bool extended)
{
    if (socket_fd < 0)
        return false;

    if (len > 8) len = 8;

    struct can_frame frame {};
    frame.can_dlc = len;

    if (extended)
        frame.can_id = (id & CAN_EFF_MASK) | CAN_EFF_FLAG;
    else
        frame.can_id = (id & CAN_SFF_MASK);

    std::memcpy(frame.data, data, len);

    return write(socket_fd, &frame, sizeof(frame)) > 0;
}