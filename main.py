import core


def main():
    """Main menu for the weight tracking program."""
    while True:
        print("\n=== Weight Tracker Menu ===")
        print("1. Record new weight")
        print("2. Show all records")
        print("3. Plot records")
        print("0. Exit")

        choice = input("Enter your choice: ").strip()
        if choice == "1":
            core.add_new_weight()
        elif choice == "2":
            core.show_records()
        elif choice == "3":
            core.plot_records()
        elif choice == "0":
            print("Exiting. Stay healthy!")
            break
        else:
            print("Invalid choice. Please try again.")


if __name__ == "__main__":
    main()
