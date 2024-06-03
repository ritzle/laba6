use rand::{thread_rng, Rng};
use std::collections::VecDeque;
use std::io::{self, Write};

const SBOX: [u8; 256] = [
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    // ...
];

const INV_SBOX: [u8; 256] = [
    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
    0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
    // ...
];

const RCON: [u8; 11] = [
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
    0x08, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x00, 0x00, 0x1B, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00,
];

fn generate_master_key(master_key: &mut Vec<u8>) {
    let mut rng = thread_rng();
    for _ in 0..8 {
        master_key.push(rng.gen_range(b'a'..=b'z'));
        master_key.push(b'0' + rng.gen_range(0..=9));
    }
}

fn shift_rows(line: &mut Vec<u8>) {
    let mut v = line.clone();
    for i in 1..line.len() {
        line[i - 1] = v[i];
    }
    line[line.len() - 1] = v[0];
}

fn inv_shift_rows(line: &mut Vec<u8>) {
    let mut v = Vec::with_capacity(line.len());
    for i in 0..line.len() {
        v.push(line[(i + (i % 4) * (i % 4)) % line.len()]);
    }
    *line = v;
}

fn sub_bytes(line: &mut Vec<u8>) {
    for byte in line.iter_mut() {
        *byte = SBOX[*byte as usize];
    }
}

fn inv_sub_bytes(line: &mut Vec<u8>) {
    for byte in line.iter_mut() {
        *byte = INV_SBOX[*byte as usize];
    }
}

fn add_round_key(first: &[u8], second: &[u8]) -> Vec<u8> {
    first.iter().zip(second.iter()).map(|(a, b)| a ^ b).collect()
}

fn key_expansion(master_key: &[u8], round_keys: &mut Vec<Vec<u8>>) {
    let mut v = Vec::with_capacity(4);
    let nk = 4;
    let nb = 4;
    let nr = 10;

    round_keys.resize(nb * (nr + 1), vec![0; 4]);

    let mut i = 0;
    while i < nk {
        v = vec![master_key[4 * i], master_key[4 * i + 1], master_key[4 * i + 2], master_key[4 * i + 3]];
        round_keys[i] = v;
        i += 1;
    }

    i = nk;
    while i < nb * (nr + 1) {
        v = round_keys[i - 1].clone();
        if i % nk == 0 {
            shift_rows(&mut v);
            sub_bytes(&mut v);
            for byte in &mut v {
                *byte ^= RCON[i / nk];
            }
        } else if nk > 6 && i % nk == 4 {
            sub_bytes(&mut v);
        }
        for j in 0..v.len() {
            round_keys[i][j] = round_keys[i - nk][j] ^ v[j];
        }
        i += 1;
    }
}

fn galois_multiply(a: u8, b: u8) -> u8 {
    let mut result = 0;
    let mut carry;

    for _ in 0..8 {
        if b & 1 != 0 {
            result ^= a;
        }

        carry = a & 0x80;
        a <<= 1;
        if carry != 0 {
            a ^= 0x1B;
        }

        b >>= 1;
    }

    result
}

fn generate_mult_by_2() -> Vec<u8> {
    (0..256).map(|i| galois_multiply(i as u8, 2)).collect()
}

fn generate_mult_by_3() -> Vec<u8> {
    (0..256).map(|i| galois_multiply(i as u8, 3)).collect()
}

fn generate_mult_by_14() -> Vec<u8> {
    (0..256).map(|i| galois_multiply(i as u8, 14)).collect()
}

fn generate_mult_by_9() -> Vec<u8> {
    (0..256).map(|i| galois_multiply(i as u8, 9)).collect()
}

fn generate_mult_by_13() -> Vec<u8> {
    (0..256).map(|i| galois_multiply(i as u8, 13)).collect()
}

fn generate_mult_by_11() -> Vec<u8> {
    (0..256).map(|i| galois_multiply(i as u8, 11)).collect()
}


fn mix_column(line: &mut Vec<u8>) {
    let mult_by_2 = generate_mult_by_2();
    let mult_by_3 = generate_mult_by_3();

    let mut v = Vec::with_capacity(4);
    v.push(
        mult_by_2[line[0] as usize]
            ^ mult_by_3[line[1] as usize]
            ^ line[2]
            ^ line[3],
    );
    v.push(
        mult_by_2[line[1] as usize]
            ^ mult_by_3[line[2] as usize]
            ^ line[0]
            ^ line[3],
    );
    v.push(
        mult_by_2[line[2] as usize]
            ^ mult_by_3[line[3] as usize]
            ^ line[0]
            ^ line[1],
    );
    v.push(
        mult_by_2[line[3] as usize]
            ^ mult_by_3[line[0] as usize]
            ^ line[1]
            ^ line[2],
    );
    *line = v;
}

fn inv_mix_columns(line: &mut Vec<u8>) {
    let mult_by_14 = generate_mult_by_14();
    let mult_by_9 = generate_mult_by_9();
    let mult_by_13 = generate_mult_by_13();
    let mult_by_11 = generate_mult_by_11();

    let mut v = Vec::with_capacity(4);
    v.push(
        mult_by_14[line[0] as usize]
            ^ mult_by_9[line[1] as usize]
            ^ mult_by_13[line[2] as usize]
            ^ mult_by_11[line[3] as usize],
    );
    v.push(
        mult_by_14[line[1] as usize]
            ^ mult_by_9[line[2] as usize]
            ^ mult_by_13[line[3] as usize]
            ^ mult_by_11[line[0] as usize],
    );
    v.push(
        mult_by_14[line[2] as usize]
            ^ mult_by_9[line[3] as usize]
            ^ mult_by_13[line[0] as usize]
            ^ mult_by_11[line[1] as usize],
    );
    v.push(
        mult_by_14[line[3] as usize]
            ^ mult_by_9[line[0] as usize]
            ^ mult_by_13[line[1] as usize]
            ^ mult_by_11[line[2] as usize],
    );
    *line = v;
}

fn block_generate(text: &str, block: &mut Vec<Vec<Vec<u8>>>) {
    let mut text = text.to_string();
    while text.len() % 16 != 0 {
        text.push(' ');
    }

    let mut v = Vec::new();
    let mut sixteen = vec![vec![0; 4]; 4];

    for i in 0..text.len() {
        let a = (i % 16) % 4;
        let b = (i % 16) / 4;
        sixteen[a][b] = text.as_bytes()[i];

        if (i + 1) % 16 == 0 {
            v.push(sixteen.clone());
            sixteen = vec![vec![0; 4]; 4];
        }
    }

    *block = v;
}

fn cipher(block: &[Vec<u8>], round_keys: &[Vec<u8>], master_key: &[u8]) -> Vec<Vec<u8>> {
    let mut v = vec![vec![0; 4]; 4];
    for i in 0..4 {
        v[i] = add_round_key(&block[i], &round_keys[i]);
    }

    for i in 1..=9 {
        for j in 0..4 {
            sub_bytes(&mut v[j]);
            shift_rows(&mut v[j]);
            mix_column(&mut v[j]);
            v[j] = add_round_key(&v[j], &round_keys[i * 4 + j]);
        }
    }

    for j in 0..4 {
        sub_bytes(&mut v[j]);
        shift_rows(&mut v[j]);
        v[j] = add_round_key(&v[j], &round_keys[10 * 4 + j]);
    }

    v
}

fn decipher(block: &[Vec<u8>], round_keys: &[Vec<u8>], master_key: &[u8]) -> Vec<Vec<u8>> {
    let mut v = vec![vec![0; 4]; 4];
    for j in 0..4 {
        v[j] = add_round_key(&block[j], &round_keys[10 * 4 + j]);
    }

    for i in (1..=9).rev() {
        for j in 0..4 {
            inv_sub_bytes(&mut v[j]);
            inv_shift_rows(&mut v[j]);
            v[j] = add_round_key(&v[j], &round_keys[i * 4 + j]);
            inv_mix_columns(&mut v[j]);
        }
    }

    for j in 0..4 {
        inv_sub_bytes(&mut v[j]);
        inv_shift_rows(&mut v[j]);
        v[j] = add_round_key(&v[j], &round_keys[j]);
    }

    v
}

fn main() {
    let mut master_key = Vec::new();
    generate_master_key(&mut master_key);
    println!("128-bit master key: {}", master_key.iter().map(|&b| format!("{:02X}", b)).collect::<String>());

    let mut round_keys = Vec::new();
    key_expansion(&master_key, &mut round_keys);

    let mut block = Vec::new();
    block_generate("Hello, World!", &mut block);

    let encrypted_block = cipher(&block, &round_keys, &master_key);
    let decrypted_block = decipher(&encrypted_block, &round_keys, &master_key);

    let decrypted_text: String = decrypted_block
        .into_iter()
        .flat_map(|row| row.iter().map(|&b| b as char))
        .collect();
    println!("Decrypted message: {}", decrypted_text);
}
