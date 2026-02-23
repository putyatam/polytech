#ifndef DESKTOPCOMPUTER_H
#define DESKTOPCOMPUTER_H

#include "Computer.h"

class DesktopComputer : public Computer {
public:
    DesktopComputer(const std::string& brand, double price, bool hasMonitor);
    void display() const override;
    void updatePrice(double newPrice) override;

private:
    bool hasMonitor;
};

#endif // DESKTOPCOMPUTER_H