#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

struct Record {
    std::string date; // yyyy-mm-dd
    float weight;
};

// Parse yyyy-mm-dd to days since epoch for interpolation
long dateToDays(const std::string& date) {
    std::tm t = {};
    std::istringstream ss(date);
    ss >> std::get_time(&t, "%Y-%m-%d");
    return std::mktime(&t) / (24 * 3600); // Days since epoch
}

// Add days to a date string
std::string addDays(const std::string& date, int days) {
    std::tm t = {};
    std::istringstream ss(date);
    ss >> std::get_time(&t, "%Y-%m-%d");
    t.tm_mday += days;
    std::mktime(&t);
    std::ostringstream oss;
    oss << std::put_time(&t, "%Y-%m-%d");
    return oss.str();
}

// Get current date as yyyy-mm-dd
std::string getCurrentDate() {
    std::time_t now = std::time(nullptr);
    std::tm* local = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(local, "%Y-%m-%d");
    return oss.str();
}

// Load records from CSV
std::vector<Record> loadRecords(const std::string& filename) {
    std::vector<Record> records;
    std::ifstream file(filename);
    if (!file.is_open()) return records;

    std::string line;
    std::getline(file, line); // Skip header
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string date, weightStr;
        std::getline(ss, date, ',');
        std::getline(ss, weightStr, ',');
        records.push_back({date, std::stof(weightStr)});
    }
    file.close();
    return records;
}

// Save records to CSV
void saveRecords(const std::string& filename, const std::vector<Record>& records) {
    std::ofstream file(filename);
    file << "Date,Weight\n";
    for (const auto& r : records) {
        file << r.date << "," << r.weight << "\n";
    }
    file.close();
}

// Add a new record with current date
void addRecord(std::vector<Record>& records) {
    float weight;
    std::cout << "Enter weight: ";
    std::cin >> weight;
    std::string date = getCurrentDate();
    records.push_back({date, weight});
    std::sort(records.begin(), records.end(), [](const Record& a, const Record& b) {
        return a.date < b.date; // Sort by date
    });
    std::cout << "Record added for " << date << "\n";
}

// Interpolate daily weights
std::vector<std::pair<std::string, float>> interpolateDaily(const std::vector<Record>& records) {
    std::vector<std::pair<std::string, float>> daily;
    for (size_t i = 0; i < records.size() - 1; ++i) {
        long days1 = dateToDays(records[i].date);
        long days2 = dateToDays(records[i + 1].date);
        float w1 = records[i].weight;
        float w2 = records[i + 1].weight;
        int gap = days2 - days1;

        for (int d = 0; d <= gap; ++d) {
            float interpolatedWeight = w1 + (w2 - w1) * d / static_cast<float>(gap);
            std::string date = addDays(records[i].date, d);
            daily.push_back({date, interpolatedWeight});
        }
    }
    return daily;
}

// Calculate EMA
std::vector<float> calculateEMA(const std::vector<std::pair<std::string, float>>& daily, int period) {
    std::vector<float> ema(daily.size());
    float alpha = 2.0f / (period + 1);
    ema[0] = daily[0].second; // First EMA = first value
    for (size_t i = 1; i < daily.size(); ++i) {
        ema[i] = alpha * daily[i].second + (1 - alpha) * ema[i - 1];
    }
    return ema;
}

// Show records with calculations
void showRecords(const std::vector<Record>& records) {
    if (records.empty()) {
        std::cout << "No records found.\n";
        return;
    }

    // Interpolate daily for EMA
    auto daily = interpolateDaily(records);
    auto ema7 = calculateEMA(daily, 7);
    auto ema30 = calculateEMA(daily, 30);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Date       | Weight | Change | Weekly Rate | 7-day EMA | 30-day EMA\n";
    std::cout << "-------------------------------------------------------------\n";

    for (size_t i = 0; i < records.size(); ++i) {
        std::cout << records[i].date << " | " << records[i].weight;

        // Weight change
        float change = (i > 0) ? records[i].weight - records[i - 1].weight : 0;
        std::cout << " | " << (i > 0 ? (change >= 0 ? "+" : "") : " ") << change;

        // Weekly rate of change
        if (i > 0) {
            long daysDiff = dateToDays(records[i].date) - dateToDays(records[i - 1].date);
            float weeklyRate = (daysDiff > 0) ? change / daysDiff * 7 : 0;
            std::cout << " | " << (weeklyRate >= 0 ? "+" : "") << weeklyRate;
        } else {
            std::cout << " | -";
        }

        // Find corresponding EMA values
        auto it = std::find_if(daily.begin(), daily.end(), [&](const auto& d) {
            return d.first == records[i].date;
        });
        if (it != daily.end()) {
            size_t idx = std::distance(daily.begin(), it);
            std::cout << " | " << ema7[idx] << " | " << ema30[idx];
        } else {
            std::cout << " | - | -";
        }
        std::cout << "\n";
    }
}

int main() {
    std::string filename = "weights.csv";
    std::vector<Record> records = loadRecords(filename);
    int choice;

    while (true) {
        std::cout << "\n1. Add new record\n2. Show all records\n3. Exit\nChoose: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                addRecord(records);
                saveRecords(filename, records);
                break;
            case 2:
                showRecords(records);
                break;
            case 3:
                std::cout << "Exiting...\n";
                return 0;
            default:
                std::cout << "Invalid choice.\n";
        }
    }
}
