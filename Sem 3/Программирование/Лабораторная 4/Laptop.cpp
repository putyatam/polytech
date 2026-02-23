#include "Laptop.h"
#include <iostream>

Laptop::Laptop(const std::string& brand, double price, double weight)
    : Computer(brand, price), weight(weight) {}

void Laptop::display() const {
    Computer::display();
    std::cout << "Weight: " << weight << " kg" << std::endl;
}

void Laptop::updatePrice(double newPrice) {
    price = newPrice;
}