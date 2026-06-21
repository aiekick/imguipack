#pragma once

#include <string>

class IApp {
public:
    virtual void closeApp() = 0;
    virtual bool isAppClosingNeeded() = 0;
    virtual void appClosingNeeded(const bool& aFlag) = 0;
    virtual void setAppTitle(const std::string& aTitle) = 0;
};
