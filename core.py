import storage
from datetime import datetime
import pandas as pd
import config

def preprocess_data():
    """
    Load, interpolate, and calculate required columns:
    EMA, daily deltas, and days since first record.
    """
    df_original = storage.load_data()

    if df_original.empty:
        print("No data found. Please record your first weight.")
        return pd.DataFrame()

    # Interpolate missing dates
    df = df_original.set_index("Date")
    all_days = pd.date_range(df.index.min(), df.index.max())
    df = df.reindex(all_days)

    # Interpolate missing weights linearly
    df["Weight"] = df["Weight"].interpolate(method="linear")

    # Calculate daily change and EMAs
    df["DailyDelta"] = df["Weight"].diff()
    df["EMA7"] = df["Weight"].ewm(span=7, adjust=False).mean()
    df["EMA30"] = df["Weight"].ewm(span=30, adjust=False).mean()

    # Reset index and filter original dates only
    df = df.reset_index().rename(columns={"index": "Date"})
    df_final = df[df["Date"].isin(df_original["Date"])].copy()

    # Calculate days since first entry & weight delta
    df_final["Days"] = (df_final["Date"] - df_final["Date"].iloc[0]).dt.days
    df_final["Delta"] = df_final["Weight"].diff()

    # Calculate EMAs without interpolation
    df_final["EMA7-NP"] = df_final["Weight"].ewm(span=7, adjust=False).mean()
    df_final["EMA30-NP"] = df_final["Weight"].ewm(span=30, adjust=False).mean()

    return df_final[
        [
            "Date",
            "Days",
            "Weight",
            "Delta",
            "DailyDelta",
            "EMA7",
            "EMA30",
            "EMA7-NP",
            "EMA30-NP",
        ]
    ]


def add_new_weight(value):
    """Store a new weight measurement."""
    date = datetime.today().strftime("%Y-%m-%d")
    storage.save_weight(date, value)


def show_records(df):
    """Display all processed weight records in a clean table."""

    with pd.option_context(
        "display.max_rows",
        None,
        "display.max_columns",
        None,
        "display.width",
        700,
        "display.float_format",
        "{:.2f}".format,
    ):
        print(df)
