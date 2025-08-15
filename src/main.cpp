#include "core.hpp"
#include <iostream>
#include <sstream>
#include <string>

int main() {
  const std::string filename{"data.csv"};
  while (true) {
    std::cout << "Weight Tracker Menu:\n";
    std::cout << "1. Record new weight\n";
    std::cout << "2. Show all records\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter your choice: ";
    int choice;
    std::string input;
    std::getline(std::cin, input);
    std::stringstream ss(input);
    ss >> choice;
    if (!(!ss.fail() && ss.eof())) {
      choice = -1;
    }
    switch (choice) {
    case 1:
      record_new_weight(filename);
    case 2:
      print_records(filename);
      break;
    case 0:
      std::cout << "Goodbye!\n";
      return 0;
    default:
      std::cout << "Invalid choice!\n\n";
    }
  }
}
