use std::io::{self, Write};

fn get_weight() -> f64 {
    print!("Enter your weight: ");
    io::stdout().flush().unwrap();
    let mut weight = String::new();
    io::stdin()
        .read_line(&mut weight)
        .expect("Failed to read line");
    let weight: f64 = match weight.trim().parse() {
        Ok(num) => num,
        Err(_) => {
            println!("Invalid input. Please enter a valid number.");
            0.0
        }
    };
    weight
}

fn record_new_weight() {
    let weight = get_weight();
}

fn show_all_records() {}

fn main() {
    loop {
        println!("Weight Tracker Menu:");
        println!("1. Record new weight");
        println!("2. Show all records");
        println!("0. Exit");
        print!("Enter your choice: ");
        io::stdout().flush().unwrap();

        let mut choice = String::new();
        io::stdin()
            .read_line(&mut choice)
            .expect("Failed to read line");
        match choice.as_ref() {
            "1" => record_new_weight(),
            "2" => show_all_records(),
            _ => {
                break;
            }
        }
    }
}
