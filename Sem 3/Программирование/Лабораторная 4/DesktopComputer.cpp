#include "DesktopComputer.h"
#include <iostream>

DesktopComputer::DesktopComputer(const std::string& brand, double price, bool hasMonitor)
    : Computer(brand, price), hasMonitor(hasMonitor) {}

void DesktopComputer::display() const {
    Computer::display();
    std::cout << "Has Monitor: " << (hasMonitor ? "Yes" : "No") << std::endl;
}

void DesktopComputer::updatePrice(double newPrice) {
    price = newPrice;
}