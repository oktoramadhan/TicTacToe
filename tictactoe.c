#include <limits.h>
#include <stdio.h>
#include <stdlib.h> // Untuk system("cls") atau clear screen
#include <time.h>

// Struktur untuk menyimpan data permainan
typedef struct {
  char grid[3][3];
  char manusia; // Player 1 (X)
  char ai;      // Player 2 atau AI (O)
} Game;

// --- FUNGSI UTILITAS ---

// Membersihkan layar terminal (kompatibel Windows/Linux)
void bersihkan_layar() {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

// Inisialisasi papan permainan menjadi kosong
void diDalamGame(Game *g) {
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      g->grid[i][j] = ' '; // Isi dengan spasi

  g->manusia = 'X';
  g->ai = 'O';
}

// Menampilkan papan permainan ke layar
void print(Game *g) {
  printf("\n");
  printf("  -------------\n");
  for (int i = 0; i < 3; i++) {
    printf("  |");
    for (int j = 0; j < 3; j++) {
      // Menampilkan X, O, atau spasi
      printf(" %c |", g->grid[i][j]);
    }
    printf("\n  -------------\n");
  }
  printf("\n");
}

// --- LOGIKA PENGECEKAN KEMENANGAN ---

// Cek apakah ada baris yang isinya sama semua
int cek_baris(Game *g, char pemain, int baris) {
  for (int kolom = 0; kolom < 3; kolom++)
    if (g->grid[baris][kolom] != pemain)
      return 0;
  return 1;
}

// Cek apakah ada kolom yang isinya sama semua
int cek_kolom(Game *g, char pemain, int kolom) {
  for (int baris = 0; baris < 3; baris++)
    if (g->grid[baris][kolom] != pemain)
      return 0;
  return 1;
}

// Cek diagonal (kiri atas ke kanan bawah)
int cek_diagonal(Game *g, char pemain) {
  for (int i = 0; i < 3; i++)
    if (g->grid[i][i] != pemain)
      return 0;
  return 1;
}

// Cek anti-diagonal (kanan atas ke kiri bawah)
int cek_anti_diagonal(Game *g, char pemain) {
  for (int i = 0; i < 3; i++)
    if (g->grid[i][2 - i] != pemain)
      return 0;
  return 1;
}

// Fungsi utama pengecekan kemenangan (menggabungkan semua cek di atas)
int cek_menang(Game *g, char pemain) {
  for (int i = 0; i < 3; i++) {
    if (cek_baris(g, pemain, i))
      return 1;
    if (cek_kolom(g, pemain, i))
      return 1;
  }
  if (cek_diagonal(g, pemain))
    return 1;
  if (cek_anti_diagonal(g, pemain))
    return 1;
  return 0;
}

// Cek apakah papan penuh (untuk kondisi Seri)
int apakah_grid_penuh(Game *g) {
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      if (g->grid[i][j] == ' ')
        return 0; // Masih ada tempat kosong
  return 1;       // Penuh
}

// --- ALGORITMA KECERDASAN BUATAN (MINIMAX) ---

/* Fungsi Rekursif Minimax:
   Fungsi ini mensimulasikan semua kemungkinan langkah ke depan.
   - Jika giliran AI (Maximizing), cari skor tertinggi (+10).
   - Jika giliran Manusia (Minimizing), cari skor terendah (-10).
*/
int langkah_terbaik_komputer(Game *g, char pemain) {
  // BASE CASE (Kondisi berhenti rekursi)
  if (cek_menang(g, g->manusia))
    return -10; // Manusia menang -> Buruk buat AI
  if (cek_menang(g, g->ai))
    return 10; // AI menang -> Bagus buat AI
  if (apakah_grid_penuh(g))
    return 0; // Seri -> Netral

  // REKURSI (Simulasi Langkah)
  if (pemain == g->ai) {
    // Giliran AI: Cari nilai MAX (Terbaik)
    int value_terbaik = INT_MIN;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        if (g->grid[i][j] == ' ') {
          g->grid[i][j] = g->ai; // Coba taruh 'O'

          // Panggil diri sendiri untuk giliran lawan
          int value = langkah_terbaik_komputer(g, g->manusia);

          g->grid[i][j] = ' '; // Undo (balikin jadi kosong)

          if (value > value_terbaik)
            value_terbaik = value;
        }
      }
    }
    return value_terbaik;
  } else {
    // Giliran Manusia: AI berasumsi Manusia main jago (Cari nilai MIN)
    int value_terbaik = INT_MAX;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        if (g->grid[i][j] == ' ') {
          g->grid[i][j] = g->manusia; // Coba taruh 'X'

          // Panggil diri sendiri untuk giliran AI
          int value = langkah_terbaik_komputer(g, g->ai);

          g->grid[i][j] = ' '; // Undo

          if (value < value_terbaik)
            value_terbaik = value;
        }
      }
    }
    return value_terbaik;
  }
}

// Fungsi pembungkus untuk memulai perhitungan Minimax
int cari_langkah_optimal_ai(Game *g) {
  int value_terbaik = INT_MIN;
  int langkah_terbaik = -1;

  // Loop semua kotak kosong di papan
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (g->grid[i][j] == ' ') {
        // Coba langkah ini
        g->grid[i][j] = g->ai;

        // Hitung skor langkah ini menggunakan Minimax
        int value = langkah_terbaik_komputer(g, g->manusia);

        // Batalkan langkah
        g->grid[i][j] = ' ';

        // Jika langkah ini punya skor lebih tinggi, simpan!
        if (value > value_terbaik) {
          value_terbaik = value;
          langkah_terbaik = i * 3 + j; // Konversi (baris,kolom) ke index 0-8
        }
      }
    }
  }
  return langkah_terbaik;
}

// --- PROGRAM UTAMA ---

int main() {
  Game game;
  diDalamGame(&game);

  int mode;
  bersihkan_layar();
  printf("\n=== GAME TIC-TAC-TOE CANGGIH ===\n");
  printf("1. Player (X) vs AI (O)\n");
  printf("2. Player 1 (X) vs Player 2 (O)\n");
  printf("3. Keluar\n");
  printf("================================\n");
  printf("Pilih mode (1-3): ");

  // Validasi input menu
  while (scanf("%d", &mode) != 1 || mode < 1 || mode > 3) {
    printf("Input salah! Masukkan angka 1-3: ");
    while (getchar() != '\n')
      ; // Bersihkan buffer
  }

  if (mode == 3) {
    printf("Sampai jumpa!\n");
    return 0;
  }

  char pemain_sekarang = 'X';

  // Statistik
  double waktu_langkah[9]; // Maksimal 9 langkah dalam tictactoe
  int index_waktu = 0;
  int jumlah_langkah[2] = {0, 0}; // [0]=X, [1]=O
  int kode_pemenang = 0;          // 0=Belum, 1=P1/Manusia, 2=AI/P2, 3=Seri

  printf("\nPermainan Dimulai!\n");

  while (1) {
    print(&game);

    // --- GILIRAN PEMAIN MANUSIA (ATAU PLAYER 1 DI PVP) ---
    // Logika ini jalan jika mode PvP ATAU (Mode PvAI dan giliran manusia)
    if (mode == 2 || (mode == 1 && pemain_sekarang == 'X')) {
      int pos;
      clock_t start = clock(); // Mulai timer

      printf("Giliran %c. Pilih posisi (1-9): ", pemain_sekarang);

      // Validasi Input: Pastikan angka
      if (scanf("%d", &pos) != 1) {
        printf(">> Input harus angka! Coba lagi.\n");
        while (getchar() != '\n')
          ; // Hapus input sampah
        continue;
      }

      clock_t end = clock(); // Stop timer

      // Simpan statistik waktu
      double durasi = (double)(end - start) / CLOCKS_PER_SEC;
      waktu_langkah[index_waktu++] = durasi;

      // Validasi Logika: Range 1-9
      if (pos < 1 || pos > 9) {
        printf(">> Posisi tidak valid! (1-9)\n");
        continue;
      }

      // Validasi Logika: Kotak penuh
      int baris = (pos - 1) / 3;
      int kolom = (pos - 1) % 3;
      if (game.grid[baris][kolom] != ' ') {
        printf(">> Spot sudah terisi! Pilih yang lain.\n");
        continue;
      }

      // Eksekusi Langkah
      game.grid[baris][kolom] = pemain_sekarang;
      if (pemain_sekarang == 'X')
        jumlah_langkah[0]++;
      else
        jumlah_langkah[1]++; // Untuk Player 2 di mode PvP

      // Cek Menang/Seri
      if (cek_menang(&game, pemain_sekarang)) {
        kode_pemenang = (pemain_sekarang == 'X') ? 1 : 2;
        break;
      }
      if (apakah_grid_penuh(&game)) {
        kode_pemenang = 3;
        break;
      }

      // Ganti Giliran (Khusus PvP)
      if (mode == 2) {
        pemain_sekarang = (pemain_sekarang == 'X') ? 'O' : 'X';
        bersihkan_layar(); // membersihkan layar biar rapi
        continue;          // Lanjut ke loop berikutnya
      }
    }

    // --- GILIRAN AI (Hanya jalan di mode 1) ---
    if (mode == 1) {
      printf("AI sedang berpikir...\n");

      clock_t start_ai = clock();
      int langkah_ai = cari_langkah_optimal_ai(&game);
      clock_t end_ai = clock();

      double durasi_ai = (double)(end_ai - start_ai) / CLOCKS_PER_SEC;
      waktu_langkah[index_waktu++] = durasi_ai;

      int baris = langkah_ai / 3;
      int kolom = langkah_ai % 3;

      game.grid[baris][kolom] = game.ai;
      jumlah_langkah[1]++;

      bersihkan_layar(); // Bersihkan layar setelah AI jalan
      printf(">> AI memilih posisi %d (%.4f detik)\n", langkah_ai + 1,
             durasi_ai);

      if (cek_menang(&game, game.ai)) {
        kode_pemenang = 2;
        break;
      }
      if (apakah_grid_penuh(&game)) {
        kode_pemenang = 3;
        break;
      }
    }
  }

  // --- HASIL AKHIR ---
  print(&game);
  printf("=============================\n");
  if (kode_pemenang == 3)
    printf("     PERTANDINGAN SERI!     \n");
  else if (kode_pemenang == 1)
    printf("     SELAMAT! PEMAIN X MENANG! \n");
  else
    printf("     PEMAIN O MENANG!    \n");
  printf("=============================\n\n");

  // --- LAPORAN STATISTIK ---
  printf("--- Statistik Permainan ---\n");
  printf("Total Langkah X : %d\n", jumlah_langkah[0]);
  printf("Total Langkah O : %d\n", jumlah_langkah[1]);
  printf("\nRiwayat Waktu (Berpikir/Kalkulasi):\n");

  for (int i = 0; i < index_waktu; i++) {
    // Menentukan siapa yang melangkah berdasarkan urutan (Ganjil=X, Genap=O)
    char siapa = (i % 2 == 0) ? 'X' : 'O';
    printf("- Langkah %d (%c): %.4f detik\n", i + 1, siapa, waktu_langkah[i]);
  }

  return 0;
}
