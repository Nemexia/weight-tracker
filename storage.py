from pathlib import Path
import pandas as pd
import config

def load_data():
    """Load weight data from CSV."""
    path = Path(config.FILE_PATH)

    if not path.exists():
        return pd.DataFrame(columns=["Date", "Weight"])

    df = pd.read_csv(path)
    df["Date"] = pd.to_datetime(df["Date"])
    return df

def save_weight(date, weight):
    """Append a weight measurment to the CSV."""
    path = Path(config.FILE_PATH)

    new_entery = pd.DataFrame(
        {
            "Date": [date],
            "Weight": [weight]
        }
    )

    new_entery.to_csv(path, mode="a", header=not path.exists(), index=False)