#pragma once

#include <string>

class IECU_Module
{
public:
    virtual ~IECU_Module() = default;

    virtual void startIdentification() = 0;
    virtual void update() = 0;

    virtual bool isReady() const = 0;
    virtual bool hasError() const = 0;

    virtual std::string getVIN() const = 0;
    virtual std::string getSW()  const = 0;
    virtual std::string getHW()  const = 0;
};