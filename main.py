import csv
from datetime import datetime, timedelta
import pandas as pd
import os

CSV_FILE = "weights.csv"

# Initialize CSV if it doesn't exist
def init_csv():
    if not os.path.exists(CSV_FILE):
        with open(CSV_FILE, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(["Date", "Weight"])

# Load records from CSV
def load_records():
    records = []
    try:
        with open(CSV_FILE, 'r') as f:
            reader = csv.DictReader(f)
            for row in reader:
                records.append({"date": row["Date"], "weight": float(row["Weight"])})
    except FileNotFoundError:
        init_csv()
    return records

# Save records to CSV
def save_records(records):
    with open(CSV_FILE, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["Date", "Weight"])
        for r in records:
            writer.writerow([r["date"], r["weight"]])

# Add a new record with current date
def add_record(records):
    try:
        weight = float(input("Enter weight: "))
        date = datetime.now().strftime("%Y-%m-%d")
        records.append({"date": date, "weight": weight})
        records.sort(key=lambda x: x["date"])  # Sort by date
        save_records(records)
        print(f"Record added for {date}")
    except ValueError:
        print("Invalid weight. Please enter a number.")

# Convert date string to days since epoch
def date_to_days(date_str):
    return (datetime.strptime(date_str, "%Y-%m-%d") - datetime(1970, 1, 1)).days

# Interpolate daily weights and calculate EMA
def calculate_metrics(records):
    if len(records) < 1:
        return [], [], []

    # Create daily interpolated series
    dates = [r["date"] for r in records]
    weights = [r["weight"] for r in records]
    days = [date_to_days(d) for d in dates]
    daily_df = pd.DataFrame({"days": days, "weight": weights})
    
    # Interpolate to daily
    min_day, max_day = min(days), max(days)
    all_days = range(min_day, max_day + 1)
    daily_df = daily_df.set_index("days").reindex(all_days).interpolate(method="linear").reset_index()
    daily_df["date"] = [datetime(1970, 1, 1) + timedelta(days=int(d)) for d in daily_df["days"]]
    daily_df["date"] = daily_df["date"].dt.strftime("%Y-%m-%d")

    # Calculate EMA
    ema7 = daily_df["weight"].ewm(span=7, adjust=False).mean()
    ema30 = daily_df["weight"].ewm(span=30, adjust=False).mean()

    return daily_df, ema7, ema30

# Show records with calculations
def show_records(records):
    if not records:
        print("No records found.")
        return

    daily_df, ema7, ema30 = calculate_metrics(records)
    print(f"{'Date':<10} | {'Weight':>6} | {'Change':>7} | {'Weekly Rate':>11} | {'7-day EMA':>9} | {'30-day EMA':>10}")
    print("-" * 65)

    for i, r in enumerate(records):
        weight = r["weight"]
        date = r["date"]
        change = weight - records[i - 1]["weight"] if i > 0 else 0
        weekly_rate = "-"
        if i > 0:
            days_diff = date_to_days(date) - date_to_days(records[i - 1]["date"])
            weekly_rate = change / days_diff * 7 if days_diff > 0 else 0

        # Find EMA for this date
        daily_row = daily_df[daily_df["date"] == date].index[0]
        ema7_val = ema7[daily_row]
        ema30_val = ema30[daily_row]

        print(f"{date:<10} | {weight:>6.2f} | {change:+7.2f} | {weekly_rate:>11.2f} if isinstance(weekly_rate, float) else weekly_rate:>11} | {ema7_val:>9.2f} | {ema30_val:>10.2f}")

# Main CLI loop
def main():
    init_csv()
    records = load_records()

    while True:
        print("\n1. Add new record")
        print("2. Show all records")
        print("3. Exit")
        choice = input("Choose: ")

        if choice == "1":
            add_record(records)
        elif choice == "2":
            show_records(records)
        elif choice == "3":
            print("Exiting...")
            break
        else:
            print("Invalid choice.")

if __name__ == "__main__":
    main()
