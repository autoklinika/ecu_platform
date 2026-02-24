#include "virtual_cockpit/VirtualCockpit.h"
#include <iostream>
#include <thread>

int main()
{
    VirtualCockpit cockpit;

    cockpit.start();

    if(!cockpit.configureCAN("can0", 500000))
    {
        std::cout << "CAN configuration failed\n";
        return 1;
    }

    if(!cockpit.selectECU("SAC"))
    {
        std::cout << "ECU selection failed\n";
        return 1;
    }

    cockpit.connect();

    // Czekamy aż silnik przejdzie w Connected albo Error
    while(true)
    {
        auto state = cockpit.getState();

        if(state == VirtualCockpit::State::Connected)
        {
            std::cout << "Connected to ECU\n";
            break;
        }

        if(state == VirtualCockpit::State::Error)
        {
            std::cout << "Connection failed\n";
            cockpit.stop();
            return 1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    std::cout << "Press ENTER to exit\n";
    std::cin.get();

    cockpit.stop();

    return 0;
}