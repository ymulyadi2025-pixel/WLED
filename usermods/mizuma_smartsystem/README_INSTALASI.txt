MIZUMA WELCOMING SHOW ENGINE — INSTALASI
Target: WLED 16.0.1 / PlatformIO environment: mizuma

ISI PAKET
1) mizuma_smartsystem.cpp  : usermod Mizuma SmartSystem + show runner + UI.
2) mizuma_shows.h          : tabel 55 animasi (dibuat otomatis oleh generator).
3) gen_mizuma_shows.py     : sumber definisi 55 show; jangan edit header generated secara manual.
4) katalog_55_animasi.md  : dokumentasi urutan stage dan resep animasi.

PEMASANGAN KE PROYEK
1. Buat salinan/backup file mizuma_smartsystem.cpp lama.
2. Di proyek WLED, cari folder usermod/library yang saat ini berisi
   mizuma_smartsystem.cpp. GANTI isi file lama dengan mizuma_smartsystem.cpp
   dari paket ini. Jangan menambahkan salinan .cpp kedua di folder compile.
3. Letakkan mizuma_shows.h di FOLDER YANG SAMA dengan mizuma_smartsystem.cpp,
   karena .cpp menggunakan #include "mizuma_shows.h".
4. gen_mizuma_shows.py dan katalog_55_animasi.md boleh disimpan bersama dua
   file runtime tersebut sebagai sumber dan dokumentasi. File .py/.md tidak
   ikut dikompilasi ke firmware.
5. Pastikan proyek memakai WLED 16.0.1 dan environment PlatformIO bernama
   "mizuma" (nama ini mengikuti log build proyek yang diberikan).
6. Jalankan clean build, lalu build dan upload:

      pio run -e mizuma -t clean
      pio run -e mizuma
      pio run -e mizuma -t upload

   Di VS Code, padanannya adalah PlatformIO: Clean, Build, lalu Upload.
7. Setelah firmware selesai di-flash, buka ulang aplikasi Mizuma. Lakukan
   hard refresh browser (Ctrl+F5) agar halaman UI terbaru dimuat.

HASIL YANG DIHARAPKAN
- Tab Welcoming menampilkan 55 show MZ01–MZ55 lalu 15 native FX pilihan
  pada posisi 56–70. Jika efek native tertentu tidak ikut dikompilasi oleh
  build WLED, posisinya tetap tampil abu-abu sebagai "tidak tersedia".
- Tab Riding, Sein, Rem, dan Hazard tetap memakai penyaringan/daftar lama.
- Kontrol sisi ditata terpisah: Pilih sisi lampu (Kiri/Semua/Kanan), Posisi
  output (Tukar kiri-kanan), dan Arah gerak (Arah asli/Arah dibalik).
- Tukar kiri-kanan tetap tersedia, tersimpan di konfigurasi Mizuma, dan
  runner yang sedang berjalan dihentikan sebelum pemetaan diubah.
- Tombol Arah asli/Arah dibalik tersedia pada semua tab: Welcoming, Riding,
  Sein, Rem, dan Hazard. Pengaturan arah berlaku global pada kedua segmen,
  tersimpan di konfigurasi ESP32, dan tidak mengubah urutan stage show Mizuma.
- Show Mizuma menggabungkan pilihan arah global dengan pembalikan khusus resep
  setiap stage. Efek WLED biasa menerima properti segmen `rev` melalui JSON API.
- Show memakai nama efek WLED yang di-resolve saat boot, bukan nomor FX
  hard-coded, sehingga lebih tahan terhadap perubahan indeks efek WLED.
- Tampilan menu warna/palette yang sudah ada tetap dipakai; tidak ada menu warna baru.
  Pada tab Welcoming, palette nonzero menjadi
  palette global bagi semua stage; warna aksen RGB di stage dipetakan ke
  tiga posisi palette. Memilih palette "Default" (ID 0) mengembalikan resep
  warna asli per-stage Mizuma.
- Setiap perpindahan stage menggunakan crossfade WLED yang lebih lembut untuk
  Signature, Beauty, dan Hybrid, sementara Fast Blink tetap cepat.
- Durasi tiap animasi dihitung sendiri: timestamp stage terakhir + 1500 ms
  waktu penutup. Tidak ada pemampatan cue, pemotongan stage, atau batas 7 detik.
- Durasi 55 resep saat ini bervariasi dari 6,10 sampai 7,95 detik. Animasi
  terlama adalah MZ01 Mizuma Ignition (7,95 detik); animasi terpendek adalah
  MZ50 Katana Dawn (6,10 detik).
- Saat boot menggunakan show Mizuma, perpindahan ke Riding menunggu semua sisi
  yang sedang menjalankan show sampai durasi masing-masing selesai. Jika sisi
  kanan dan kiri memakai show berbeda, sistem menunggu sisi yang lebih lama.
- Preset boot native WLED yang bukan show Mizuma tetap memakai fallback 7 detik,
  karena preset native tersebut tidak membawa metadata durasi dari generator.

PENTING: PARTICLE SYSTEM 1D
Untuk memperoleh 15 native FX secara lengkap, jangan mendefinisikan
WLED_DISABLE_PARTICLESYSTEM1D pada konfigurasi build. WLED 16.0.1 hanya
mendaftarkan PS Fireworks 1D, PS Spray 1D, PS 1D Balance, dan PS Starburst
ketika Particle System 1D diaktifkan. Jika macro itu didefinisikan, efek yang
hilang akan ditandai "tidak tersedia" di UI; tahap show yang memerlukan efek
yang tidak ada akan dilewati dengan aman.

DIAGNOSTIK SETELAH BOOT
- Serial log yang diharapkan jika semua 52 jenis efek show ditemukan:
    [Mizuma] 52/52 efek WLED siap untuk 55 show
- Jika jumlahnya kurang dari 52, periksa log berikutnya yang menyebut nama
  efek yang tidak ditemukan. Efek tersebut mungkin tidak disertakan dalam
  konfigurasi build WLED.
- Endpoint diagnostik usermod: /mizuma/debug (lihat showFxResolved).

MENGUBAH RESEP ANIMASI
1. Edit definisi show pada gen_mizuma_shows.py, bukan mizuma_shows.h.
2. Jalankan dari folder yang berisi generator:
      python gen_mizuma_shows.py
3. Generator akan menulis ulang mizuma_shows.h dan katalog_55_animasi.md.
4. Build dan flash ulang firmware.

CATATAN STATUS PENGUJIAN
- Python generator: py_compile berhasil; menghasilkan 55 show, 52 nama FX,
  dan 311 stage resep asli. Durasi dihitung dinamis dari cue terakhir + 1500 ms.
- Header C++: kompilasi mandiri C++11 dengan -Wall -Wextra -Werror berhasil.
- Audit jadwal: seluruh cue asli dipertahankan. Rentang durasi hasil generator
  saat ini adalah 6100–7950 ms; perpindahan boot menunggu runner selesai.
- JavaScript inline lolos pemeriksaan sintaks Node.js.
- Endpoint durasi per-show dan tombol pembalik arah global untuk semua tab
  sudah ditambahkan dan diperiksa secara struktural; perilaku pada perangkat
  belum diuji.
- Build firmware penuh ESP32-C3 belum diverifikasi dalam lingkungan ini,
  karena executable PlatformIO (pio/platformio) tidak tersedia. Lakukan
  clean build dan upload pada proyek lokal sebelum dipakai di motor.
