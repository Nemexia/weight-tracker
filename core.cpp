#include "core.hpp"
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Record {
  double value{};
  double change{};
  double weekly_rate{};
  double ema_7{};
  double ema_30{};
  int date{};
  std::string timestamp{};
};

using RV = std::vector<Record>;

static int timestamp_to_date(const std::string &timestamp) {
  tm timestruct{};
  std::istringstream ss(timestamp);
  ss >> std::get_time(&timestruct, "%Y-%m-%d");

  time_t tt{mktime(&timestruct)};
  return static_cast<int>(tt) / (24 * 60 * 60);
}

static std::string get_current_date() {
  time_t now = time(nullptr);
  tm *local_tm{localtime(&now)};
  std::ostringstream oss;
  oss << std::put_time(local_tm, "%Y-%m-%d");
  return oss.str();
}

static double get_ema(const int N, const double prev, const double now,
                      const int interval, double ema) {
  double factor{2.0 / (1.0 + N)};
  for (int i{1}; i < interval + 1; ++i) {
    double val{((now - prev) / interval * i) + prev};
    ema = val * factor + ema * (1 - factor);
  }
  return ema;
}

static RV read_file(const std::string &filename) {
  RV records;
  std::ifstream file(filename);

  if (!file.is_open())
    return records;

  std::string line{};
  getline(file, line);
  while (getline(file, line)) {
    std::stringstream ss(line);
    std::string timestamp{};
    std::string value{};
    getline(ss, timestamp, ',');
    getline(ss, value, ',');
    Record record{.value = stod(value),
                  .date = timestamp_to_date(timestamp),
                  .timestamp = timestamp};
    records.push_back(record);
  }

  Record &r0{records[0]};
  int initial_date{r0.date};
  r0.date = 0;
  r0.ema_7 = r0.value;
  r0.ema_30 = r0.value;
  for (std::vector<Record>::size_type i{1}; i < records.size(); ++i) {
    Record &r{records[i]};
    Record &prev{records[i - 1]};
    r.date -= initial_date;
    r.change = r.value - prev.value;
    const int interval{r.date - prev.date};
    r.weekly_rate = r.change / interval * 7;
    r.ema_7 = get_ema(7, prev.value, r.value, interval, prev.ema_7);
    r.ema_30 = get_ema(30, prev.value, r.value, interval, prev.ema_30);
  }
  return records;
}

static void add_record(const double value, const std::string &filename) {
  std::ofstream file(filename, std::ios::app);
  if (file.is_open()) {
    file << get_current_date() << "," << value << "\n";
    file.close();
    std::cout << "Value recorded successfully!\n";
  } else {
    std::cout << "Error opening file!\n";
  }
}

void record_new_weight(const std::string &filename) {
  double weight;
  while (true) {
    std::cout << "Enter your weight (kg): ";
    std::string input;
    std::getline(std::cin, input);
    std::stringstream ss(input);
    ss >> weight;
    if (!ss.fail() && ss.eof()) {
      break;
    }
    std::cout << "Invalid input. Please enter a valid number: ";
  }
  add_record(weight, filename);
}

void print_records(const std::string &filename) {
  RV records{read_file(filename)};

  if (records.empty()) {
    std::cout << "No records found!\n";
    return;
  }

  std::cout << "Records:\n";
  std::cout
      << "--------------------------------------------------------------\n";
  std::cout << "timestamp\tdate\tvalue\tchange\trate_7\tema_7\tema_30\n";
  for (auto &r : records) {
    std::cout << r.timestamp << "\t" << r.date << std::fixed
              << std::setprecision(2) << "\t" << r.value << "\t" << r.change
              << "\t" << r.weekly_rate << "\t" << r.ema_7 << "\t" << r.ema_30
              << std::endl;
  }
  std::cout
      << "--------------------------------------------------------------\n\n";
}
