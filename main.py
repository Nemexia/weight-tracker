import core
import plotter


def main():
    """Main menu for the weight tracking program."""
    df = core.preprocess_data()
    while True:
        print("\n=== Weight Tracker Menu ===")
        print("1. Record new weight")
        print("2. Show all records")
        print("3. Plot records")
        print("0. Exit")

        choice = input("Enter your choice: ").strip()
        if choice == "1":
            try:
                value = float(input("Enter new weight: ").strip())
                core.add_new_weight(value)
                df = core.preprocess_data()
            except ValueError:
                print("Invalid input. Please enter a numeric value.")
        elif choice == "2":
            core.show_records(df)
        elif choice == "3":
            plotter.plot_records(df)
        elif choice == "0":
            print("Exiting. Stay healthy!")
            break
        else:
            print("Invalid choice. Please try again.")


if __name__ == "__main__":
    main()
