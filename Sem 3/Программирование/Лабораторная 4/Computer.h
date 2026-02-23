
#ifndef COMPUTER_H
#define COMPUTER_H

#include <string>

class Computer {
public:
    Computer(const std::string& brand, double price);
    virtual ~Computer() {}

    virtual void display() const;
    virtual void updatePrice(double newPrice);

    double getPrice() const;

protected:
    std::string brand;
    double price;
};

#endif // COMPUTER_H
