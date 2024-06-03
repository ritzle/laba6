use std::env;
use std::fs::File;
use std::io::{self, BufRead, BufReader};
use std::time::Duration;
use std::{thread, time};

fn generate_playing_field(lines: usize, columns: usize) -> Vec<Vec<bool>> {
    let mut rng = rand::thread_rng();
    (0..lines)
        .map(|_| (0..columns).map(|_| rng.gen_bool(0.5)).collect())
        .collect()
}

fn read_playing_field(file_path: &str) -> (usize, usize, Vec<Vec<bool>>) {
    let file = File::open(file_path).unwrap_or_else(|_| {
        eprintln!("Failed to open file: {}", file_path);
        std::process::exit(1);
    });
    let reader = BufReader::new(file);

    let mut lines = 0;
    let mut max_columns = 0;
    let mut playing_field = Vec::new();

    for line in reader.lines() {
        let row: Vec<bool> = line
            .unwrap()
            .chars()
            .map(|c| c == '1')
            .collect();
        playing_field.push(row);
        lines += 1;
        max_columns = max_columns.max(row.len());
    }

    (lines, max_columns, playing_field)
}

fn play_life(playing_field: &mut Vec<Vec<bool>>) {
    let lines = playing_field.len();
    let columns = playing_field[0].len();
    let mut next_field = playing_field.clone();

    for i in 0..lines {
        for j in 0..columns {
            let neighbors = count_neighbors(playing_field, i, j, lines, columns);
            next_field[i][j] = match (playing_field[i][j], neighbors) {
                (true, 2) | (true, 3) => true,
                (false, 3) => true,
                _ => false,
            };
        }
    }

    *playing_field = next_field;
}

fn count_neighbors(
    playing_field: &Vec<Vec<bool>>,
    i: usize,
    j: usize,
    lines: usize,
    columns: usize,
) -> usize {
    let mut count = 0;
    for x in i.saturating_sub(1)..=i.min(lines - 1) {
        for y in j.saturating_sub(1)..=j.min(columns - 1) {
            if !(x == i && y == j) && playing_field[x][y] {
                count += 1;
            }
        }
    }
    count
}

fn display_playing_field(playing_field: &Vec<Vec<bool>>) {
    let lines = playing_field.len();
    let columns = playing_field[0].len();

    for i in 0..lines {
        for j in 0..columns {
            if playing_field[i][j] {
                print!("@ ");
            } else {
                print!("- ");
            }
        }
        println!();
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        eprintln!("Usage: {} <file_path>", args[0]);
        return;
    }

    let (lines, columns, mut playing_field) = read_playing_field(&args[1]);

    loop {
        system("clear");
        display_playing_field(&playing_field);
        play_life(&mut playing_field);
        thread::sleep(Duration::from_millis(200));
    }
}
