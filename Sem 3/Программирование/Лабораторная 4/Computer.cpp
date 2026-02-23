#include "Computer.h"
#include <iostream>

Computer::Computer(const std::string& brand, double price) : brand(brand), price(price) {}

void Computer::display() const {
    std::cout << "Brand: " << brand << ", Price: $" << price << std::endl;
}

void Computer::updatePrice(double newPrice) {
    price = newPrice;
}

double Computer::getPrice() const {
    return price;
}