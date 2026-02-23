#include <iostream>
#include <vector>
#include <limits>
#include "Computer.h"
#include "DesktopComputer.h"
#include "Laptop.h"

void displayMenu();
void viewComputers(const std::vector<Computer*>& computers);
void updateComputerPrice(std::vector<Computer*>& computers);
void findCheapestComputer(const std::vector<Computer*>& computers);

void clearInputBuffer() {
    std::cin.clear(); // Сбросим флаг ошибок
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Игнорируем оставшиеся символы до конца строки
}

int main() {
    std::vector<Computer*> computers;
    computers.push_back(new DesktopComputer("Dell", 800, true));
    computers.push_back(new Laptop("HP", 1200, 2.5));
    computers.push_back(new DesktopComputer("Lenovo", 600, false));
    computers.push_back(new Laptop("Apple", 1500, 1.8));

    int choice;
    do {
        displayMenu();
        
        std::cin >> choice;

        if (std::cin.fail() || choice < 0 || choice > 3) {
            std::cin.clear(); // Сброс состояния cin
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Игнорирование до конца строки
            std::cout << "Invalid choice. Please try again." << std::endl;
            continue;
        }

        switch (choice) {
            case 1:
                viewComputers(computers);
                break;
            case 2:
                updateComputerPrice(computers);
                break;
            case 3:
                findCheapestComputer(computers);
                break;
            case 0:
                std::cout << "Exiting program." << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    } while (choice != 0);

    for (auto comp : computers) {
        delete comp;
    }

    return 0;
}

void displayMenu() {
    std::cout << "\nMenu:\n";
    std::cout << "1. View Computers\n";
    std::cout << "2. Update Computer Price\n";
    std::cout << "3. Find Cheapest Computer\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter your choice: ";
}

void viewComputers(const std::vector<Computer*>& computers) {
    for (const auto& comp : computers) {
        comp->display();
    }
}

void updateComputerPrice(std::vector<Computer*>& computers) {
    int index;
    double newPrice;
    std::cout << "Enter the index of the computer to update (0 to " << computers.size() - 1 << "): ";
    std::cin >> index;
    if (index >= 0 && index < computers.size()) {
        std::cout << "Enter the new price: ";
        std::cin >> newPrice;
        computers[index]->updatePrice(newPrice);
        std::cout << "Price updated successfully." << std::endl;
    } else {
        std::cout << "Invalid index." << std::endl;
    }
}

void findCheapestComputer(const std::vector<Computer*>& computers) {
    double minPrice = std::numeric_limits<double>::max();
    Computer* cheapest = nullptr;
    for (const auto& comp : computers) {
        if (comp->getPrice() < minPrice) {
            minPrice = comp->getPrice();
            cheapest = comp;
        }
    }
    if (cheapest) {
        std::cout << "The cheapest computer is: " << std::endl;
        cheapest->display();
    } else {
        std::cout << "No computers available." << std::endl;
    }
}