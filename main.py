import csv
import datetime
import os
from typing import List


class Record:
    def __init__(self, timestamp: str, value: float):
        self.timestamp = timestamp
        self.date = timestamp_to_date(timestamp)
        self.value = value
        self.change = 0.0
        self.weekly_rate = 0.0
        self.ema_7 = 0.0
        self.ema_30 = 0.0


def timestamp_to_date(timestamp: str) -> int:
    dt = datetime.datetime.strptime(timestamp, "%Y-%m-%d")
    return int(dt.timestamp()) // (24 * 60 * 60)


def get_current_date() -> str:
    return datetime.datetime.now().strftime("%Y-%m-%d")


def get_ema(N: int, prev: float, now: float, interval: int, ema: float) -> float:
    factor = 2.0 / (1.0 + N)
    for i in range(1, interval + 1):
        val = ((now - prev) / interval * i) + prev
        ema = val * factor + ema * (1 - factor)
    return ema


def read_file(filename: str) -> List[Record]:
    records: List[Record] = []
    if not os.path.exists(filename):
        return records

    with open(filename, "r") as file:
        reader = csv.reader(file)
        next(reader)  # skip header
        for row in reader:
            timestamp, value = row
            records.append(Record(timestamp, float(value)))

    if not records:
        return records

    r0 = records[0]
    initial_date = r0.date
    r0.date = 0
    r0.ema_7 = r0.value
    r0.ema_30 = r0.value

    for i in range(1, len(records)):
        r = records[i]
        prev = records[i - 1]
        r.date -= initial_date
        interval = r.date - prev.date
        r.change = r.value - prev.value
        r.weekly_rate = r.change / interval * 7 if interval != 0 else 0.0
        r.ema_7 = get_ema(7, prev.value, r.value, interval, prev.ema_7)
        r.ema_30 = get_ema(30, prev.value, r.value, interval, prev.ema_30)

    return records


def add_record(value: float, filename: str):
    file_exists = os.path.isfile(filename)
    with open(filename, "a", newline="") as file:
        writer = csv.writer(file)
        if not file_exists:
            writer.writerow(["timestamp", "value"])
        writer.writerow([get_current_date(), value])
        print("Value recorded successfully!")


def print_records(filename: str):
    records = read_file(filename)

    if not records:
        print("No records found!")
        return

    print("Records:")
    print("--------------------------------------------------------")
    print("timestamp\tdate\tvalue\tchange\trate_7\tema_7\tema_30")
    for r in records:
        print(
            f"{r.timestamp}\t{r.date}\t{r.value:.2f}\t{r.change:.2f}\t"
            f"{r.weekly_rate:.2f}\t{r.ema_7:.2f}\t{r.ema_30:.2f}"
        )
    print("--------------------------------------------------------")


def main():
    filename = "data.csv"
    while True:
        print("\nWeight Tracker Menu:")
        print("1. Record new weight")
        print("2. Show all records")
        print("0. Exit")
        choice = input("Enter your choice: ")

        if choice == "1":
            try:
                weight = float(input("Enter your weight (kg): "))
                add_record(weight, filename)
            except ValueError:
                print("Invalid input. Please enter a number.")
        elif choice == "2":
            print_records(filename)
        elif choice == "0":
            print("Goodbye!")
            break
        else:
            print("Invalid choice! Please try again.")


if __name__ == "__main__":
    main()
