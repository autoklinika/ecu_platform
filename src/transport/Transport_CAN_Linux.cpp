#include "Transport_CAN_Linux.h"

#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

Transport_CAN_Linux::Transport_CAN_Linux(const char* interface)
{
    socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if (socket_fd < 0)
    {
        perror("Socket");
        return;
    }

    struct ifreq ifr {};
    std::strncpy(ifr.ifr_name, interface, IFNAMSIZ);

    ioctl(socket_fd, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr {};
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("Bind");
    }
}

Transport_CAN_Linux::~Transport_CAN_Linux()
{
    close(socket_fd);
}

bool Transport_CAN_Linux::receiveFrame(uint32_t& id,
                                       uint8_t* data,
                                       uint8_t& len)
{
    struct can_frame frame;

    int nbytes = read(socket_fd, &frame, sizeof(frame));
    if (nbytes <= 0)
        return false;

    id = frame.can_id & CAN_EFF_MASK;
    len = frame.can_dlc;
    std::memcpy(data, frame.data, len);

    return true;
}