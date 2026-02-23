#ifndef LAPTOP_H
#define LAPTOP_H

#include "Computer.h"

class Laptop : public Computer {
public:
    Laptop(const std::string& brand, double price, double weight);
    void display() const override;
    void updatePrice(double newPrice) override;

private:
    double weight;
};

#endif // LAPTOP_H