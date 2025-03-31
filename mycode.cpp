#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<ctime>
#include<chrono>
#include<iomanip>

using namespace std;

struct Record{
    int date{};
    double value{};
};

using RV = vector<Record>;

int timestamp_to_date(const string& timestamp)
{
    tm timestruct{};
    istringstream ss(timestamp);
    ss>>get_time(&timestruct, "%Y-%m-%d");
    
    time_t tt{mktime(&timestruct)};
    return tt/ (24 * 60 * 60);
}

string get_current_date() {
    time_t now = time(nullptr);
    tm* local = localtime(&now);
    ostringstream oss;
    oss << put_time(local, "%Y-%m-%d");
    return oss.str();
}

RV read_file(const string &filename)
{
    RV records;
    ifstream file(filename);
    
    if(!file.is_open()) return records;
    
    string line{};
    getline(file, line);
    while(getline(file, line))
    {
        stringstream ss(line);
        string date{};
        string value{};
        getline(ss, date, ',');
        getline(ss, value, ',');
        Record record{timestamp_to_date(date), stod(value)};
        records.push_back(record);
    }
    
    for(int i{1}; i<records.size();++i)
    {
        records[i].date -= records[0].date;
    }
    records[0].date=0;
    return records;
}

void add_record(const double value, const string &filename)
{
    ofstream file(filename, ios::app);
    if (file.is_open())
    {
        file << get_current_date() << "," << value << "\n";
        file.close();
        cout << "Value recorded successfully!\n";
    }
    else
    {
        cout << "Error opening file!\n";
    }
}


void print_records(const string& filename)
{
    RV records{read_file(filename)};
    
    if(records.empty())
    {
        cout << "No records found!\n";
        return;
    }
    
}

int main()
{
    const string filename{"data.csv"};
    while (true)
    {
        cout << "\nWeight Tracker Menu:\n";
        cout << "1. Record new weight\n";
        cout << "2. Show all records\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        int choice{};
        cin >> choice;
        
        switch (choice)
        {
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



