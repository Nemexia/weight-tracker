#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


using namespace std;

struct Record {
  string timestamp{};
  int date{};
  double value{};
  double change{};
  double weekly_rate{};
  double ema_7{};
  double ema_30{};
};

using RV = vector<Record>;

int timestamp_to_date(const string &timestamp) {
  tm timestruct{};
  istringstream ss(timestamp);
  ss >> get_time(&timestruct, "%Y-%m-%d");

  time_t tt{mktime(&timestruct)};
  return static_cast<int>(tt) / (24 * 60 * 60);
}

string get_current_date() {
  time_t now = time(nullptr);
  tm *local = localtime(&now);
  ostringstream oss;
  oss << put_time(local, "%Y-%m-%d");
  return oss.str();
}

double get_ema(const int N, const double prev, const double now,
               const int interval, double ema) {
  double factor{2.0 / (1.0 + N)};
  for (int i{1}; i < interval + 1; ++i) {
    double val{((now - prev) / interval * i) + prev};
    ema = val * factor + ema * (1 - factor);
  }
  return ema;
}

RV read_file(const string &filename) {
  RV records;
  ifstream file(filename);

  if (!file.is_open())
    return records;

  string line{};
  getline(file, line);
  while (getline(file, line)) {
    stringstream ss(line);
    string timestamp{};
    string value{};
    getline(ss, timestamp, ',');
    getline(ss, value, ',');
    Record record{timestamp, timestamp_to_date(timestamp), stod(value)};
    records.push_back(record);
  }

  int initial_date{records[0].date};
  records[0].date = 0;
  records[0].ema_7 = records[0].value;
  records[0].ema_30 = records[0].value;
  for (std::vector<Record>::size_type i{1}; i < records.size(); ++i) {
    records[i].date -= initial_date;
    records[i].change = records[i].value - records[i - 1].value;
    const int interval{records[i].date - records[i - 1].date};
    records[i].weekly_rate = records[i].change / interval * 7;
    records[i].ema_7 = get_ema(7, records[i - 1].value, records[i].value,
                               interval, records[i - 1].ema_7);
    records[i].ema_30 = get_ema(30, records[i - 1].value, records[i].value,
                                interval, records[i - 1].ema_30);
  }
  return records;
}

void add_record(const double value, const string &filename) {
  ofstream file(filename, ios::app);
  if (file.is_open()) {
    file << get_current_date() << "," << value << "\n";
    file.close();
    cout << "Value recorded successfully!\n";
  } else {
    cout << "Error opening file!\n";
  }
}

void print_records(const string &filename) {
  RV records{read_file(filename)};

  if (records.empty()) {
    cout << "No records found!\n";
    return;
  }

  cout << "Records:\n";
  cout << "--------------------------------------------------------\n";
  cout << "timestamp\tdate\tvalue\tchange\trate_7\tema_7\tema_30\n";
  for (auto r : records) {
    cout << r.timestamp << "\t" << r.date << fixed << setprecision(2) << "\t"
         << r.value << "\t" << r.change << "\t" << r.weekly_rate << "\t"
         << r.ema_7 << "\t" << r.ema_30 << endl;
  }
  cout << "--------------------------------------------------------\n";
}

int main() {
  const string filename{"data.csv"};
  while (true) {
    cout << "\nWeight Tracker Menu:\n";
    cout << "1. Record new weight\n";
    cout << "2. Show all records\n";
    cout << "0. Exit\n";
    cout << "Enter your choice: ";
    int choice{};
    cin >> choice;

    switch (choice) {
    case 1:
      cout << "Enter your weight (kg): ";
      double weight;
      cin >> weight;
      add_record(weight, filename);
      break;

    case 2:
      print_records(filename);
      break;

    case 0:
      cout << "Goodbye!\n";
      return 0;

    default:
      cout << "Invalid choice! Please try again.\n";
    }
  }
}
