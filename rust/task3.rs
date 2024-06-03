use std::f64::EPSILON;

fn find_pivot(matrix: &Vec<Vec<f64>>, col: usize, start_row: usize) -> usize {
    let mut max_val = 0.0;
    let mut pivot_row = start_row;

    for i in start_row..matrix.len() {
        if (matrix[i][col]).abs() > max_val {
            max_val = matrix[i][col].abs();
            pivot_row = i;
        }
    }
    pivot_row + 1
}

fn solve_linear_system(matrix: &Vec<Vec<f64>>) -> Vec<f64> {
    let n = matrix.len();
    let m = matrix[0].len() - 1;
    let mut working_matrix = matrix.to_vec();

    println!("Прямой ход:");
    for col in 0..m {
        let pivot_row = find_pivot(&working_matrix, col, col);
        let pivot_value = working_matrix[pivot_row - 1][col];
        println!("Выбран главный элемент {} в строке {}", pivot_value, pivot_row);

        if pivot_row != col + 1 {
            working_matrix.swap(col, pivot_row - 1);
            println!("Меняем местами строки {} и {}", col + 1, pivot_row);
        }

        for i in col + 1..n {
            let factor = working_matrix[i][col] / working_matrix[col][col];
            println!("Вычитаем из строки {} {}", i + 1, factor);

            for j in col..m + 1 {
                working_matrix[i][j] -= factor * working_matrix[col][j];
            }
        }

        println!("Промежуточная матрица после {}:го столбца:", col + 1);
        for row in &working_matrix {
            for elem in row {
                if (*elem).abs() < 3e-17 {
                    print!("0 ");
                } else {
                    print!("{} ", elem);
                }
            }
            println!();
        }
        println!();
    }

    println!("Обратный ход:");
    let mut solution = vec![0.0; m];
    for i in (0..n).rev() {
        let mut sum = 0.0;
        for j in i + 1..m {
            sum += working_matrix[i][j] * solution[j];
        }
        solution[i] = (working_matrix[i][m] - sum) / working_matrix[i][i];
        println!("x{} = {}", i + 1, solution[i]);
    }

    solution
}

fn check_spectral_radius(c: &Vec<Vec<f64>>) {
    let mut spectral_radius = 0.0;
    for i in 0..4 {
        let mut sum = 0.0;
        for j in 0..4 {
            sum += c[i][j].abs();
        }
        if sum > spectral_radius {
            spectral_radius = sum;
        }
    }

    println!("\nСпектральный радиус матрицы C: {}", spectral_radius);

    if spectral_radius >= 1.0 {
        println!("Условие сходимости метода простых итераций не выполняется.");
    } else {
        println!("Условие сходимости метода простых итераций выполняется.");
    }
}

fn form_canonical_system(
    a: &Vec<Vec<f64>>,
    b: &Vec<f64>,
    c: &mut Vec<Vec<f64>>,
    f: &mut Vec<f64>,
    n: usize,
) {
    for i in 0..n {
        f[i] = b[i] / a[i][i];
        for j in 0..n {
            if j != i {
                c[i][j] = -a[i][j] / a[i][i];
            } else {
                c[i][j] = 0.0;
            }
        }
    }
}

fn simple_iteration(
    c: &Vec<Vec<f64>>,
    f: &Vec<f64>,
    x: &mut Vec<f64>,
    n: usize,
    epsilon: f64,
) -> bool {
    let mut x_new = vec![0.0; n];
    let mut k = 0;
    let mut max_diff = 0.0;
    let max_iteration = 10;

    for i in 0..n {
        x[i] = 0.0;
    }

    println!("N\tx1\tx2\tx3\tx4\tεn");

    loop {
        for i in 0..n {
            let mut sum = f[i];
            for j in 0..n {
                sum += c[i][j] * x[j];
            }
            x_new[i] = sum;
        }

        max_diff = 0.0;
        for i in 0..n {
            if (x_new[i] - x[i]).abs() > max_diff {
                max_diff = (x_new[i] - x[i]).abs();
            }
            x[i] = x_new[i];
        }
        k += 1;

        println!(
            "{}\t{:.3}\t{:.3}\t{:.3}\t{:.3}\t{:.3}",
            k, x[0], x[1], x[2], x[3], max_diff
        );

        if max_diff <= epsilon {
            println!("Число итераций: {}", k);
            println!("Метод сходится.");
            return false;
        }

        if k >= max_iteration {
            println!("Число итераций: {} (максимум)", k);
            println!("Метод расходится.");
            return true;
        }
    }
}

fn main() {
    let matrix = vec![
        vec![-1.13, -0.04, 0.21, -18.0, -1.24],
        vec![0.25, -1.23, 0.14, -0.09, 0.87],
        vec![-0.21, 0.14, 0.80, -0.13, 2.56],
        vec![0.15, -1.31, 0.06, 0.87, -1.13],
    ];

    println!("\tРешение СЛАУ с выбором главного элемента:\n");
    let solution = solve_linear_system(&matrix);
    println!("Матрица решений СЛАУ:");
    for x in &solution {
        print!("{} ", x);
    }
    println!();

    let a = vec![
        vec![-1.13, -0.04, 0.21, -18.0],
        vec![0.25, -1.23, 0.14, -0.09],
        vec![-0.21, 0.14, 0.80, -0.13],
        vec![0.15, -1.31, 0.06, 0.87],
    ];
    let b = vec![-1.24, 0.87, 2.56, -1.13];
    let mut x = vec![0.0; 4];
    let mut c = vec![vec![0.0; 4]; 4];
    let mut f = vec![0.0; 4];

    form_canonical_system(&a, &b, &mut c, &mut f, 4);
    check_spectral_radius(&c);

    println!("\nРешение методом простых итераций:");
    if simple_iteration(&c, &f, &mut x, 4, 0.001) {
        println!("x1 = {:.3}", x[0]);
        println!("x2 = {:.3}", x[1]);
        println!("x3 = {:.3}", x[2]);
        println!("x4 = {:.3}", x[3]);
    }
}

