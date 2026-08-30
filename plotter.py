import matplotlib.pyplot as plt
import config
import pandas as pd


def plot_records(df, show=True):
    """Generate and save a weight trend plot:
    - Top: last 2 months (zoomed-in)
    - Bottom: full timeline (overview)
    """

    # Ensure data sorted
    df = df.sort_values("Date").reset_index(drop=True)

    # Define time window for the last 60 days
    last_60_days = df["Date"].max() - pd.Timedelta(days=60)
    df_recent = df[df["Date"] >= last_60_days].copy()

    # Setup figure with two subplots
    fig, (ax_recent, ax_full) = plt.subplots(
        2,
        1,
        figsize=(12, 8),
        dpi=300,
        sharey=True,
        gridspec_kw={"height_ratios": [2, 1]},
    )

    make_recent_trend_plot(df_recent, ax_recent)

    make_full_trend_plot(df, ax_full)

    plt.tight_layout()
    plt.savefig(config.PLOT_PATH)
    plt.close()

    print(f"Plot saved as {config.PLOT_PATH}")


def make_full_trend_plot(df, ax_full):
    # --- Full trend (bottom, smaller) ---
    ax_full.plot(
        df["Date"], df["Weight"], marker=".", color="steelblue", label="Weight"
    )
    ax_full.plot(df["Date"], df["EMA7"], linestyle="--", color="green", label="EMA 7")
    ax_full.plot(
        df["Date"], df["EMA30"], linestyle="--", color="orange", label="EMA 30"
    )
    ax_full.plot(
        df["Date"],
        df["EMA7-NP"],
        linestyle="--",
        color="red",
        label="EMA 7 (NP)",
    )
    ax_full.plot(
        df["Date"],
        df["EMA30-NP"],
        linestyle="--",
        color="purple",
        label="EMA 30 (NP)",
    )
    ax_full.set_title("Full Weight History", fontsize=12)
    ax_full.set_xlabel("Date")
    ax_full.set_ylabel("Weight (kg)")
    ax_full.grid(True)
    ax_full.legend()


def make_recent_trend_plot(df_recent, ax_recent):
    # --- Recent trend (top, bigger) ---
    ax_recent.plot(
        df_recent["Date"],
        df_recent["Weight"],
        marker="o",
        color="royalblue",
        label="Weight",
    )
    ax_recent.plot(
        df_recent["Date"],
        df_recent["EMA7"],
        linestyle="--",
        color="green",
        label="EMA 7",
    )
    ax_recent.plot(
        df_recent["Date"],
        df_recent["EMA30"],
        linestyle="--",
        color="orange",
        label="EMA 30",
    )
    ax_recent.plot(
        df_recent["Date"],
        df_recent["EMA7-NP"],
        linestyle="--",
        color="red",
        label="EMA 7 (NP)",
    )
    ax_recent.plot(
        df_recent["Date"],
        df_recent["EMA30-NP"],
        linestyle="--",
        color="purple",
        label="EMA 30 (NP)",
    )
    ax_recent.set_title("Recent Trend (Last 2 Months)", fontsize=14, fontweight="bold")
    ax_recent.set_ylabel("Weight (kg)")
    ax_recent.grid(True)
    ax_recent.legend()
