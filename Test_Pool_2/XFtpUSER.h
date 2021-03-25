#pragma once
#include "XFtpTask.h"
#include <iostream>
class XFtpUSER :
    public XFtpTask
{
    virtual void Parse(std::string type, std::string msg);
};

