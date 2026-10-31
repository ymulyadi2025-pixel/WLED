# Katalog 55 animasi Welcoming Mizuma

Setiap animasi adalah **rangkaian efek asli WLED** (tahap demi tahap). Durasi tiap show dihitung dari cue terakhir + 1500 ms waktu penutup; tidak dikunci ke 7 detik.
Transisi antartahap memakai crossfade WLED yang disesuaikan dengan kategori dan jarak cue. Durasi berbeda untuk setiap animasi.
Palette yang dipilih pengguna di UI menjadi palette global saat show berjalan. Nilai palette di resep dipertahankan sebagai metadata/original look, bukan memaksa pilihan pengguna.
Warna `A1/A2/A3` menjadi aksen yang dipetakan ke palette aktif; hitam dan putih tetap menjadi latar/highlight.


## Signature Tetrix

### MZ01 — Mizuma Ignition
Tetrix lambat ke cepat (kanan ke kiri), Wipe mati kiri ke kanan, Sweep kanan ke kiri, Fireworks 1D, ditutup Fade menyala penuh.

**Durasi: 7.95 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Tetrix | L+R | lewati tunggu 2 dtk Tetrix, sx=160, ix=128, resep-pal=35, crossfade=370ms |
| 900 | Tetrix | L+R | sx=200, ix=128, resep-pal=35, crossfade=330ms |
| 1700 | Tetrix | L+R | sx=235, ix=128, resep-pal=35, crossfade=250ms |
| 2300 | Tetrix | L+R | sx=255, ix=128, resep-pal=35, crossfade=620ms |
| 3800 | Wipe | L+R | mulai di fasa 600 ms, sx=252, ix=128, resep-pal=0, crossfade=270ms |
| 4450 | Sweep | L+R | arah dibalik, sx=252, ix=128, resep-pal=0, crossfade=270ms |
| 5100 | Fireworks 1D | L+R | crossfade=560ms |
| 6450 | Fade | L+R | sx=200, resep-pal=0, crossfade=620ms |

### MZ02 — Mizuma Afterburn
Alur Ignition dibalik: Fade menyala, Fireworks 1D, Sweep, Wipe mati (arah dibalik), lalu Tetrix cepat ke lambat.

**Durasi: 7.30 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Fade | L+R | sx=215, resep-pal=0, crossfade=310ms |
| 750 | Fireworks 1D | L+R | crossfade=540ms |
| 2050 | Sweep | L+R | sx=252, ix=128, resep-pal=0, crossfade=270ms |
| 2700 | Wipe | L+R | arah dibalik, mulai di fasa 600 ms, sx=252, ix=128, resep-pal=0, crossfade=270ms |
| 3350 | Tetrix | L+R | arah dibalik, lewati tunggu 2 dtk Tetrix, sx=255, ix=128, resep-pal=35, crossfade=330ms |
| 4150 | Tetrix | L+R | arah dibalik, sx=240, ix=128, resep-pal=35, crossfade=330ms |
| 4950 | Tetrix | L+R | arah dibalik, sx=215, ix=128, resep-pal=35, crossfade=350ms |
| 5800 | Tetrix | L+R | arah dibalik, sx=185, ix=128, resep-pal=35, crossfade=620ms |

### MZ03 — Golden Dawn
Sunrise fajar emas, Fireworks 1D, Twinkle, ditutup Shimmer.

**Durasi: 7.00 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sunrise | L+R | sx=174, resep-pal=35, crossfade=620ms |
| 2350 | Fireworks 1D | L+R | resep-pal=35, crossfade=620ms |
| 4150 | Twinkle | L+R | sx=190, ix=200, resep-pal=0, crossfade=560ms |
| 5500 | Shimmer | L+R | resep-pal=35, crossfade=620ms |

### MZ04 — Prism Finale
Shimmer, Sinelon Rainbow, Chase Rainbow, Ripple Rainbow lalu penutup Fade amber.

**Durasi: 7.40 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Shimmer | L+R | resep-pal=11, crossfade=620ms |
| 1700 | Sinelon Rainbow | L+R | sx=200, ix=110, resep-pal=11, crossfade=620ms |
| 3400 | Chase Rainbow | L+R | sx=238, ix=100, resep-pal=11, crossfade=540ms |
| 4700 | Ripple Rainbow | L+R | sx=160, ix=120, crossfade=500ms |
| 5900 | Fade | L+R | sx=190, resep-pal=0, crossfade=620ms |

### MZ05 — Stack Royale
Tetrix campuran: pelangi cepat, ledakan PS Starburst, Tetrix satu warna arah balik, Colortwinkles, Fade.

**Durasi: 7.55 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Tetrix | L+R | lewati tunggu 2 dtk Tetrix, sx=250, ix=128, resep-pal=11, crossfade=620ms |
| 1950 | PS Starburst | L+R | resep-pal=72, crossfade=390ms |
| 2900 | Tetrix | L+R | arah dibalik, lewati tunggu 2 dtk Tetrix, sx=250, ix=160, resep-pal=0, crossfade=620ms |
| 4900 | Colortwinkles | L+R | sx=170, ix=170, resep-pal=6, crossfade=480ms |
| 6050 | Fade | L+R | sx=210, resep-pal=0, crossfade=620ms |


## Kedipan Cepat

### MZ06 — Raijin Strobe
Dewa petir: burst Strobe Mega putih, Strobe amber, Lightning, ditutup menyala.

**Durasi: 7.70 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Mega | L+R | sx=236, ix=30, resep-pal=0, crossfade=140ms |
| 1700 | Strobe | L+R | sx=249, resep-pal=0, crossfade=140ms |
| 3400 | Lightning | L+R | sx=185, ix=180, crossfade=140ms |
| 5100 | Strobe Mega | L+R | sx=240, ix=20, resep-pal=0, crossfade=140ms |
| 6200 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ07 — Katana Slash
Tebasan beruntun Wipe/Sweep bergantian arah dan warna.

**Durasi: 7.30 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Wipe | L+R | sx=254, ix=128, resep-pal=0, crossfade=140ms |
| 500 | Sweep | L+R | arah dibalik, sx=254, ix=128, resep-pal=0, crossfade=140ms |
| 1000 | Wipe | L+R | sx=254, ix=128, resep-pal=0, crossfade=140ms |
| 1500 | Sweep | L+R | arah dibalik, sx=254, ix=128, resep-pal=0, crossfade=140ms |
| 2000 | Wipe | L+R | sx=254, ix=128, resep-pal=0, crossfade=140ms |
| 2500 | Chase Flash | L+R | sx=246, resep-pal=0, crossfade=140ms |
| 4300 | Strobe | L+R | sx=250, resep-pal=0, crossfade=140ms |
| 5800 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ08 — Shinobi Bergantian
Kiri dan kanan menyala bergantian makin cepat seperti ninja.

**Durasi: 7.50 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Solid | Kanan | resep-pal=0, crossfade=100ms |
| 0 | Solid | Kiri | resep-pal=0, crossfade=100ms |
| 240 | Solid | Kanan | resep-pal=0, crossfade=100ms |
| 240 | Solid | Kiri | resep-pal=0, crossfade=100ms |
| 480 | Solid | Kanan | resep-pal=0, crossfade=100ms |
| 480 | Solid | Kiri | resep-pal=0, crossfade=100ms |
| 720 | Solid | Kanan | resep-pal=0, crossfade=100ms |
| 720 | Solid | Kiri | resep-pal=0, crossfade=100ms |
| 960 | Solid | Kanan | resep-pal=0, crossfade=70ms |
| 960 | Solid | Kiri | resep-pal=0, crossfade=70ms |
| 1130 | Solid | Kanan | resep-pal=0, crossfade=70ms |
| 1130 | Solid | Kiri | resep-pal=0, crossfade=70ms |
| 1300 | Solid | Kanan | resep-pal=0, crossfade=70ms |
| 1300 | Solid | Kiri | resep-pal=0, crossfade=70ms |
| 1470 | Solid | Kanan | resep-pal=0, crossfade=70ms |
| 1470 | Solid | Kiri | resep-pal=0, crossfade=70ms |
| 1640 | Solid | Kanan | resep-pal=0, crossfade=70ms |
| 1640 | Solid | Kiri | resep-pal=0, crossfade=70ms |
| 1810 | Solid | Kanan | resep-pal=0, crossfade=70ms |
| 1810 | Solid | Kiri | resep-pal=0, crossfade=70ms |
| 1980 | Solid | Kanan | resep-pal=0, crossfade=50ms |
| 1980 | Solid | Kiri | resep-pal=0, crossfade=50ms |
| 2100 | Solid | Kanan | resep-pal=0, crossfade=50ms |
| 2100 | Solid | Kiri | resep-pal=0, crossfade=50ms |
| 2220 | Solid | Kanan | resep-pal=0, crossfade=50ms |
| 2220 | Solid | Kiri | resep-pal=0, crossfade=50ms |
| 2340 | Solid | Kanan | resep-pal=0, crossfade=50ms |
| 2340 | Solid | Kiri | resep-pal=0, crossfade=50ms |
| 2460 | Solid | Kanan | resep-pal=0, crossfade=50ms |
| 2460 | Solid | Kiri | resep-pal=0, crossfade=50ms |
| 2580 | Solid | Kanan | resep-pal=0, crossfade=50ms |
| 2580 | Solid | Kiri | resep-pal=0, crossfade=50ms |
| 2700 | Solid | Kanan | resep-pal=0, crossfade=50ms |
| 2700 | Solid | Kiri | resep-pal=0, crossfade=50ms |
| 2820 | Solid | Kanan | resep-pal=0, crossfade=70ms |
| 2820 | Solid | Kiri | resep-pal=0, crossfade=70ms |
| 3000 | Strobe | L+R | sx=249, resep-pal=0, crossfade=140ms |
| 4600 | Strobe Mega | L+R | sx=238, ix=40, resep-pal=0, crossfade=140ms |
| 6000 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ09 — Oni Heartbeat
Detak jantung oni merah-oranye yang makin liar.

**Durasi: 7.40 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Heartbeat | L+R | sx=190, ix=170, resep-pal=0, crossfade=140ms |
| 2000 | Heartbeat | L+R | sx=220, ix=190, resep-pal=0, crossfade=140ms |
| 3400 | Blink | L+R | sx=244, ix=70, resep-pal=0, crossfade=140ms |
| 4600 | Strobe | L+R | sx=250, resep-pal=0, crossfade=140ms |
| 5900 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ10 — Kitsune Dash
Rubah berlari: Chase 2 cepat, Scan Dual, Chase 3 lalu menyala.

**Durasi: 7.40 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Chase 2 | L+R | sx=246, ix=80, resep-pal=0, crossfade=140ms |
| 1500 | Scan Dual | L+R | sx=252, ix=80, resep-pal=0, crossfade=140ms |
| 3000 | Chase 3 | L+R | sx=246, ix=70, resep-pal=0, crossfade=140ms |
| 4500 | Chase Flash | L+R | sx=247, resep-pal=0, crossfade=140ms |
| 5900 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ11 — Tengu Burst
Dissolve cepat, Glitter, Strobe Mega: ledakan acak berkilau.

**Durasi: 7.40 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Dissolve | L+R | sx=235, ix=235, resep-pal=0, crossfade=140ms |
| 1700 | Glitter | L+R | sx=220, ix=220, resep-pal=0, crossfade=140ms |
| 3300 | Strobe Mega | L+R | sx=238, ix=30, resep-pal=0, crossfade=140ms |
| 4700 | Dissolve | L+R | sx=245, ix=245, resep-pal=0, crossfade=140ms |
| 5900 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ12 — Hayate Rush
Angin kencang: Chase Flash, Running Dual, Two Dots berkejaran.

**Durasi: 7.30 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Chase Flash | L+R | sx=247, resep-pal=0, crossfade=140ms |
| 1500 | Running Dual | L+R | sx=238, ix=100, resep-pal=0, crossfade=140ms |
| 3000 | Two Dots | L+R | sx=238, ix=110, resep-pal=0, crossfade=140ms |
| 4500 | Chase Flash | L+R | sx=250, resep-pal=0, crossfade=140ms |
| 5800 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ13 — Ronin Spark
Percikan pedang ronin: Sparkle, Glitter, Twinkle rapat.

**Durasi: 7.20 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sparkle | L+R | sx=250, resep-pal=0, crossfade=140ms |
| 1500 | Glitter | L+R | sx=225, ix=225, resep-pal=0, crossfade=140ms |
| 3000 | Twinkle | L+R | sx=230, ix=230, resep-pal=0, crossfade=140ms |
| 4400 | Sparkle | L+R | sx=252, resep-pal=0, crossfade=140ms |
| 5700 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ14 — Kabuto Pulse
Pola Blink berirama seperti genderang di helm kabuto.

**Durasi: 7.40 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Blink | L+R | sx=236, ix=60, resep-pal=0, crossfade=140ms |
| 1100 | Blink | L+R | sx=242, ix=90, resep-pal=0, crossfade=140ms |
| 2200 | Blink | L+R | sx=247, ix=110, resep-pal=0, crossfade=140ms |
| 3300 | Blink Rainbow | L+R | sx=246, ix=120, crossfade=140ms |
| 4700 | Strobe | L+R | sx=250, resep-pal=0, crossfade=140ms |
| 5900 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ15 — Tsunami Flash
Riak cepat berwarna lalu kilat Strobe Rainbow.

**Durasi: 7.40 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Ripple | L+R | sx=210, ix=170, resep-pal=9, crossfade=140ms |
| 1700 | Strobe Rainbow | L+R | sx=247, crossfade=140ms |
| 3200 | Ripple | L+R | sx=220, ix=190, resep-pal=11, crossfade=140ms |
| 4600 | Strobe Mega | L+R | sx=238, ix=40, resep-pal=0, crossfade=140ms |
| 5900 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ16 — Yami Glitch
Efek glitch kegelapan: Glitter, Dissolve Rnd, Strobe patah-patah.

**Durasi: 7.60 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Glitter | L+R | sx=230, ix=230, resep-pal=0, crossfade=140ms |
| 1400 | Dissolve Rnd | L+R | sx=235, ix=235, crossfade=140ms |
| 2800 | Strobe | L+R | sx=249, resep-pal=0, crossfade=140ms |
| 3800 | Glitter | L+R | sx=245, ix=245, resep-pal=0, crossfade=140ms |
| 5000 | Strobe Mega | L+R | sx=236, ix=50, resep-pal=0, crossfade=140ms |
| 6100 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ17 — Hanabi Rapid
Kembang api beruntun: Fireworks 1D, Popcorn, Sparkle lalu menyala.

**Durasi: 7.40 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Fireworks 1D | L+R | crossfade=140ms |
| 1800 | Popcorn | L+R | sx=190, resep-pal=35, crossfade=140ms |
| 3300 | Sparkle | L+R | sx=248, resep-pal=0, crossfade=140ms |
| 4300 | Fireworks 1D | L+R | resep-pal=11, crossfade=140ms |
| 5900 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ18 — Taiko Beat
Dentuman taiko: kedip berirama dengan Bouncing Balls.

**Durasi: 7.40 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Mega | L+R | sx=232, ix=20, resep-pal=0, crossfade=140ms |
| 1000 | Strobe Mega | L+R | sx=236, ix=20, resep-pal=0, crossfade=140ms |
| 2000 | Bouncing Balls | L+R | resep-pal=6, crossfade=140ms |
| 3600 | Strobe Mega | L+R | sx=240, ix=30, resep-pal=0, crossfade=140ms |
| 4700 | Blink | L+R | sx=246, ix=120, resep-pal=0, crossfade=140ms |
| 5900 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ19 — Kaen Burst
Api membara: Chase Flash Rnd, Strobe oranye, Lightning hangat.

**Durasi: 7.60 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Chase Flash Rnd | L+R | sx=246, crossfade=140ms |
| 1500 | Strobe | L+R | sx=248, resep-pal=0, crossfade=140ms |
| 2800 | Lightning | L+R | sx=190, ix=190, crossfade=140ms |
| 4200 | Chase Flash | L+R | sx=248, resep-pal=0, crossfade=140ms |
| 5200 | Strobe Mega | L+R | sx=238, ix=30, resep-pal=0, crossfade=140ms |
| 6100 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ20 — Shuriken Spin
Bintang lempar: Scan, Scan Dual, Two Dots dan Sinelon Dual cepat.

**Durasi: 7.60 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Scan | L+R | sx=252, ix=90, resep-pal=0, crossfade=140ms |
| 1300 | Scan Dual | L+R | sx=254, ix=90, resep-pal=0, crossfade=140ms |
| 2600 | Two Dots | L+R | sx=245, ix=90, resep-pal=0, crossfade=140ms |
| 3900 | Sinelon Dual | L+R | sx=215, ix=70, resep-pal=0, crossfade=140ms |
| 5200 | Strobe | L+R | sx=250, resep-pal=0, crossfade=140ms |
| 6100 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ21 — Nitro Flicker
Kelip nitro: Colortwinkles cepat, Twinkle, Strobe pendek.

**Durasi: 7.50 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Colortwinkles | L+R | sx=220, ix=220, resep-pal=6, crossfade=140ms |
| 1500 | Twinkle | L+R | sx=240, ix=240, resep-pal=0, crossfade=140ms |
| 2800 | Strobe | L+R | sx=250, resep-pal=0, crossfade=140ms |
| 3500 | Colortwinkles | L+R | sx=230, ix=230, resep-pal=35, crossfade=140ms |
| 4800 | Strobe Mega | L+R | sx=240, ix=30, resep-pal=0, crossfade=140ms |
| 6000 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ22 — Dragon Roar
Raungan naga: ledakan PS Starburst berulang dan Strobe Mega.

**Durasi: 7.10 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | PS Starburst | L+R | resep-pal=72, crossfade=140ms |
| 1500 | Strobe Mega | L+R | sx=236, ix=40, resep-pal=0, crossfade=140ms |
| 2900 | PS Starburst | L+R | resep-pal=35, crossfade=140ms |
| 4400 | Strobe Mega | L+R | sx=240, ix=30, resep-pal=0, crossfade=140ms |
| 5600 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ23 — Samurai Triple
Tiga kedipan tegas per kelompok dengan tiga warna aksen.

**Durasi: 7.30 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Mega | L+R | sx=234, ix=20, resep-pal=0, crossfade=140ms |
| 1000 | Strobe Mega | L+R | sx=234, ix=20, resep-pal=0, crossfade=140ms |
| 2000 | Strobe Mega | L+R | sx=234, ix=20, resep-pal=0, crossfade=140ms |
| 3000 | Strobe Mega | L+R | sx=240, ix=20, resep-pal=0, crossfade=140ms |
| 3900 | Strobe Mega | L+R | sx=240, ix=20, resep-pal=0, crossfade=140ms |
| 4800 | Strobe Mega | L+R | sx=240, ix=20, resep-pal=0, crossfade=140ms |
| 5800 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ24 — Thunder Chase
Kilat dan kejaran pelangi cepat.

**Durasi: 7.60 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Lightning | L+R | sx=180, ix=190, crossfade=140ms |
| 1500 | Chase Rainbow | L+R | sx=244, ix=90, resep-pal=11, crossfade=140ms |
| 2900 | Strobe | L+R | sx=249, resep-pal=0, crossfade=140ms |
| 3700 | Chase Rainbow | L+R | sx=250, ix=100, resep-pal=6, crossfade=140ms |
| 5000 | Lightning | L+R | sx=200, ix=200, crossfade=140ms |
| 6100 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |

### MZ25 — Zenkai Flash
Puncak kedipan: gabungan Wipe, Strobe, Glitter, Chase Flash lalu ledakan terang.

**Durasi: 7.40 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Wipe | L+R | sx=254, ix=128, resep-pal=0, crossfade=140ms |
| 500 | Strobe | L+R | sx=250, resep-pal=0, crossfade=140ms |
| 1200 | Glitter | L+R | sx=235, ix=235, resep-pal=0, crossfade=140ms |
| 2200 | Chase Flash | L+R | sx=248, resep-pal=0, crossfade=140ms |
| 3200 | Strobe Mega | L+R | sx=238, ix=40, resep-pal=0, crossfade=140ms |
| 4300 | Dissolve | L+R | sx=245, ix=245, resep-pal=0, crossfade=140ms |
| 5200 | Solid | Kanan | resep-pal=0, crossfade=50ms |
| 5200 | Solid | Kiri | resep-pal=0, crossfade=50ms |
| 5320 | Solid | Kanan | resep-pal=0, crossfade=50ms |
| 5320 | Solid | Kiri | resep-pal=0, crossfade=50ms |
| 5440 | Solid | Kanan | resep-pal=0, crossfade=50ms |
| 5440 | Solid | Kiri | resep-pal=0, crossfade=50ms |
| 5560 | Solid | Kanan | resep-pal=0, crossfade=140ms |
| 5560 | Solid | Kiri | resep-pal=0, crossfade=140ms |
| 5900 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |


## Keindahan

### MZ26 — Sakura Drift
Kelopak sakura melayang: Colortwinkles, Meteor lembut, Shimmer muda.

**Durasi: 6.90 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Colortwinkles | L+R | sx=90, ix=110, resep-pal=49, crossfade=700ms |
| 1900 | Meteor | L+R | sx=150, ix=160, resep-pal=49, crossfade=700ms |
| 3600 | Shimmer | L+R | resep-pal=49, crossfade=700ms |
| 5400 | Breathe | L+R | sx=120, resep-pal=0, crossfade=630ms |

### MZ27 — Fuji Sunrise
Fajar di Gunung Fuji: Sunrise, Colorwaves, Fade hangat.

**Durasi: 6.50 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sunrise | L+R | sx=174, resep-pal=35, crossfade=700ms |
| 2350 | Colorwaves | L+R | resep-pal=13, crossfade=700ms |
| 5000 | Fade | L+R | sx=120, resep-pal=0, crossfade=630ms |

### MZ28 — Mizu Ripple
Riak air (mizu): Ripple, Pacifica, Lake.

**Durasi: 7.50 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Ripple | L+R | sx=110, ix=120, resep-pal=9, crossfade=700ms |
| 2100 | Pacifica | L+R | resep-pal=51, crossfade=700ms |
| 4200 | Lake | L+R | resep-pal=9, crossfade=700ms |
| 6000 | Fade | L+R | sx=200, resep-pal=0, crossfade=630ms |

### MZ29 — Hotaru Fireflies
Kunang-kunang malam: Twinkle kuning, Fairy, Breathe hangat.

**Durasi: 6.70 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Twinkle | L+R | sx=80, ix=110, resep-pal=0, crossfade=700ms |
| 2000 | Fairy | L+R | sx=110, ix=110, resep-pal=0, crossfade=700ms |
| 3800 | Twinkle | L+R | sx=70, ix=160, resep-pal=0, crossfade=580ms |
| 5200 | Breathe | L+R | sx=110, resep-pal=0, crossfade=630ms |

### MZ30 — Tsuki Moonlight
Cahaya bulan: Breathe putih kebiruan, Shimmer es, Fade.

**Durasi: 7.10 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Breathe | L+R | sx=90, resep-pal=0, crossfade=700ms |
| 2300 | Shimmer | L+R | resep-pal=7, crossfade=700ms |
| 4600 | Fade | L+R | sx=110, resep-pal=0, crossfade=420ms |
| 5600 | Fade | L+R | sx=110, resep-pal=0, crossfade=630ms |

### MZ31 — Ginga Galaxy
Galaksi: Sinelon Dual, Twinklefox, Shimmer ungu.

**Durasi: 7.30 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sinelon Dual | L+R | sx=120, ix=120, resep-pal=0, crossfade=700ms |
| 2100 | Twinklefox | L+R | sx=90, ix=130, resep-pal=45, crossfade=700ms |
| 4000 | Shimmer | L+R | resep-pal=46, crossfade=700ms |
| 5800 | Fade | L+R | sx=120, resep-pal=0, crossfade=630ms |

### MZ32 — Nami Wave
Ombak berlapis: Wavesins, Pacifica, Colorwaves laut.

**Durasi: 7.50 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Wavesins | L+R | sx=90, resep-pal=9, crossfade=700ms |
| 2200 | Pacifica | L+R | resep-pal=51, crossfade=700ms |
| 4200 | Colorwaves | L+R | resep-pal=22, crossfade=700ms |
| 6000 | Fade | L+R | sx=200, resep-pal=0, crossfade=630ms |

### MZ33 — Kasumi Mist
Kabut pagi: Noise Pal pastel, Lake, Breathe.

**Durasi: 7.30 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Noise Pal | L+R | sx=70, ix=100, resep-pal=20, crossfade=700ms |
| 2300 | Lake | L+R | resep-pal=20, crossfade=700ms |
| 4300 | Breathe | L+R | sx=100, resep-pal=0, crossfade=630ms |
| 5800 | Fade | L+R | sx=110, resep-pal=0, crossfade=630ms |

### MZ34 — Akari Bloom
Cahaya lampion mekar: Wipe lambat, Fade, Shimmer hangat.

**Durasi: 7.40 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Wipe | L+R | sx=160, ix=128, resep-pal=0, crossfade=700ms |
| 2300 | Fade | L+R | sx=150, resep-pal=0, crossfade=700ms |
| 4000 | Shimmer | L+R | resep-pal=39, crossfade=700ms |
| 5900 | Fade | L+R | sx=130, resep-pal=0, crossfade=630ms |

### MZ35 — Yuki Shimmer
Salju berkilau: Shimmer es, Twinkle putih, Sparkle.

**Durasi: 7.80 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Shimmer | L+R | resep-pal=36, crossfade=700ms |
| 2200 | Twinkle | L+R | sx=90, ix=160, resep-pal=0, crossfade=700ms |
| 4000 | Sparkle | L+R | sx=200, resep-pal=0, crossfade=670ms |
| 5600 | Fade | L+R | sx=110, resep-pal=0, crossfade=290ms |
| 6300 | Fade | L+R | sx=110, resep-pal=0, crossfade=630ms |

### MZ36 — Koi Pond
Ikan koi berenang: Sinelon Dual oranye-putih, Ripple, Pacifica.

**Durasi: 7.50 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sinelon Dual | L+R | sx=130, ix=130, resep-pal=0, crossfade=700ms |
| 2200 | Ripple | L+R | sx=120, ix=120, resep-pal=44, crossfade=700ms |
| 4200 | Pacifica | L+R | resep-pal=44, crossfade=700ms |
| 6000 | Fade | L+R | sx=200, resep-pal=0, crossfade=630ms |

### MZ37 — Hanabi Garden
Taman kembang api: Fireworks 1D, PS Starburst, Colortwinkles.

**Durasi: 7.10 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Fireworks 1D | L+R | resep-pal=72, crossfade=700ms |
| 2000 | PS Starburst | L+R | resep-pal=72, crossfade=700ms |
| 3800 | Colortwinkles | L+R | sx=130, ix=150, resep-pal=6, crossfade=700ms |
| 5600 | Fade | L+R | sx=170, resep-pal=0, crossfade=630ms |

### MZ38 — Tanabata Stars
Festival bintang: Twinklefox, Colortwinkles, Shimmer.

**Durasi: 7.20 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Twinklefox | L+R | sx=90, ix=130, resep-pal=46, crossfade=700ms |
| 2000 | Colortwinkles | L+R | sx=90, ix=130, resep-pal=45, crossfade=700ms |
| 3900 | Shimmer | L+R | resep-pal=45, crossfade=700ms |
| 5700 | Fade | L+R | sx=120, resep-pal=0, crossfade=630ms |

### MZ39 — Aurora Kaze
Aurora tertiup angin: Aurora, Colorwaves aurora, Fade.

**Durasi: 7.50 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Aurora | L+R | resep-pal=50, crossfade=700ms |
| 2800 | Colorwaves | L+R | resep-pal=55, crossfade=700ms |
| 4500 | Shimmer | L+R | resep-pal=50, crossfade=630ms |
| 6000 | Fade | L+R | sx=200, resep-pal=0, crossfade=630ms |

### MZ40 — Ame Neon
Hujan neon kota: Drip, Bouncing Balls, Ripple Rainbow.

**Durasi: 7.30 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Drip | L+R | resep-pal=6, crossfade=700ms |
| 2100 | Bouncing Balls | L+R | resep-pal=6, crossfade=700ms |
| 3900 | Ripple Rainbow | L+R | sx=120, ix=130, crossfade=700ms |
| 5800 | Fade | L+R | sx=200, resep-pal=0, crossfade=630ms |

### MZ41 — Kaze Whisper
Bisikan angin: Sinelon lembut, Wavesins, Fade.

**Durasi: 7.10 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sinelon | L+R | sx=110, ix=130, resep-pal=0, crossfade=700ms |
| 2100 | Wavesins | L+R | sx=70, resep-pal=20, crossfade=700ms |
| 4100 | Breathe | L+R | sx=100, resep-pal=0, crossfade=630ms |
| 5600 | Fade | L+R | sx=120, resep-pal=0, crossfade=630ms |

### MZ42 — Hoshi Cascade
Hujan bintang: Meteor, PS Spray 1D, Twinkle.

**Durasi: 7.20 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Meteor | L+R | sx=140, ix=170, resep-pal=0, crossfade=700ms |
| 2100 | PS Spray 1D | L+R | resep-pal=72, crossfade=700ms |
| 4000 | Twinkle | L+R | sx=110, ix=170, resep-pal=0, crossfade=700ms |
| 5700 | Fade | L+R | sx=200, resep-pal=0, crossfade=630ms |

### MZ43 — Asagiri Dawn
Fajar berkabut: Sunrise, Pacifica lembut, Fade.

**Durasi: 6.70 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sunrise | L+R | sx=163, resep-pal=21, crossfade=700ms |
| 2700 | Pacifica | L+R | resep-pal=21, crossfade=700ms |
| 5200 | Fade | L+R | sx=120, resep-pal=0, crossfade=630ms |

### MZ44 — Kyoto Lantern
Lentera Kyoto: Candle, Fire 2012, Breathe hangat.

**Durasi: 7.20 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Candle | L+R | resep-pal=0, crossfade=700ms |
| 2100 | Fire 2012 | L+R | resep-pal=35, crossfade=700ms |
| 4300 | Breathe | L+R | sx=100, resep-pal=0, crossfade=580ms |
| 5700 | Fade | L+R | sx=110, resep-pal=0, crossfade=630ms |

### MZ45 — Mizuma Eternal
Tanda tangan Mizuma yang abadi: Colorwaves emas, Shimmer, Fade.

**Durasi: 7.10 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Colorwaves | L+R | resep-pal=35, crossfade=700ms |
| 2300 | Shimmer | L+R | resep-pal=35, crossfade=700ms |
| 4300 | Meteor | L+R | sx=170, ix=150, resep-pal=0, crossfade=540ms |
| 5600 | Fade | L+R | sx=130, resep-pal=0, crossfade=630ms |


## Campuran

### MZ46 — Raijin Bloom
Kilat Strobe yang mekar menjadi Shimmer sakura.

**Durasi: 6.40 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Mega | L+R | sx=236, ix=30, resep-pal=0, crossfade=480ms |
| 1300 | Strobe | L+R | sx=250, resep-pal=0, crossfade=480ms |
| 2600 | Shimmer | L+R | resep-pal=49, crossfade=480ms |
| 4900 | Breathe | L+R | sx=120, resep-pal=0, crossfade=480ms |

### MZ47 — Hanabi Rush
Kembang api cepat yang berakhir lembut.

**Durasi: 6.50 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Fireworks 1D | L+R | crossfade=480ms |
| 1700 | Strobe Mega | L+R | sx=238, ix=30, resep-pal=0, crossfade=480ms |
| 2900 | Colortwinkles | L+R | sx=180, ix=190, resep-pal=72, crossfade=480ms |
| 5000 | Fade | L+R | sx=160, resep-pal=0, crossfade=480ms |

### MZ48 — Oni Prism
Strobe Rainbow menuju Sinelon Rainbow yang indah.

**Durasi: 7.40 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Rainbow | L+R | sx=247, crossfade=480ms |
| 1300 | Blink Rainbow | L+R | sx=244, ix=120, crossfade=480ms |
| 2500 | Sinelon Rainbow | L+R | sx=190, ix=120, resep-pal=11, crossfade=480ms |
| 4400 | Ripple Rainbow | L+R | sx=150, ix=120, crossfade=480ms |
| 5900 | Fade | L+R | sx=200, resep-pal=0, crossfade=480ms |

### MZ49 — Kitsune Aurora
Kejaran rubah lalu aurora tenang.

**Durasi: 7.30 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Chase 2 | L+R | sx=246, ix=80, resep-pal=0, crossfade=480ms |
| 1400 | Scan Dual | L+R | sx=252, ix=80, resep-pal=0, crossfade=480ms |
| 2800 | Aurora | L+R | resep-pal=50, crossfade=480ms |
| 5800 | Fade | L+R | sx=200, resep-pal=0, crossfade=480ms |

### MZ50 — Katana Dawn
Tebasan cepat yang berakhir dalam fajar.

**Durasi: 6.10 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Wipe | L+R | sx=254, ix=128, resep-pal=0, crossfade=200ms |
| 480 | Sweep | L+R | arah dibalik, sx=254, ix=128, resep-pal=0, crossfade=200ms |
| 960 | Wipe | L+R | sx=254, ix=128, resep-pal=0, crossfade=200ms |
| 1440 | Sweep | L+R | arah dibalik, sx=254, ix=128, resep-pal=0, crossfade=230ms |
| 2000 | Sunrise | L+R | sx=174, resep-pal=35, crossfade=480ms |
| 4600 | Fade | L+R | sx=150, resep-pal=0, crossfade=480ms |

### MZ51 — Tsunami Spark
Riak dan percikan yang bermuara ke Pacifica.

**Durasi: 7.10 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Ripple | L+R | sx=200, ix=160, resep-pal=9, crossfade=480ms |
| 1500 | Sparkle | L+R | sx=245, resep-pal=0, crossfade=480ms |
| 2800 | Pacifica | L+R | resep-pal=51, crossfade=480ms |
| 5600 | Fade | L+R | sx=200, resep-pal=0, crossfade=480ms |

### MZ52 — Yokai Glow
Kilatan yokai yang berubah menjadi cahaya bernapas.

**Durasi: 7.30 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Glitter | L+R | sx=235, ix=235, resep-pal=0, crossfade=480ms |
| 1400 | Dissolve | L+R | sx=240, ix=240, resep-pal=0, crossfade=480ms |
| 2800 | Breathe | L+R | sx=120, resep-pal=0, crossfade=480ms |
| 4300 | Shimmer | L+R | resep-pal=69, crossfade=480ms |
| 5800 | Fade | L+R | sx=200, resep-pal=0, crossfade=480ms |

### MZ53 — Shogun Pulse
Denyut shogun, ledakan PS Starburst lalu Colorwaves megah.

**Durasi: 6.60 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Heartbeat | L+R | sx=200, ix=180, resep-pal=0, crossfade=480ms |
| 1800 | PS Starburst | L+R | resep-pal=35, crossfade=480ms |
| 3300 | Colorwaves | L+R | resep-pal=35, crossfade=480ms |
| 5100 | Shimmer | L+R | resep-pal=35, crossfade=480ms |

### MZ54 — Sakura Storm
Badai sakura: Dissolve, Glitter, Colortwinkles, Shimmer.

**Durasi: 7.40 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Dissolve | L+R | sx=238, ix=235, resep-pal=0, crossfade=480ms |
| 1400 | Glitter | L+R | sx=225, ix=225, resep-pal=0, crossfade=480ms |
| 2800 | Colortwinkles | L+R | sx=150, ix=170, resep-pal=49, crossfade=480ms |
| 4400 | Shimmer | L+R | resep-pal=49, crossfade=480ms |
| 5900 | Fade | L+R | sx=200, resep-pal=0, crossfade=480ms |

### MZ55 — Mizuma Grand Finale
Final megah: Strobe, Fireworks 1D, Sinelon Rainbow, Shimmer, Fade.

**Durasi: 7.30 detik**

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Mega | L+R | sx=236, ix=30, resep-pal=0, crossfade=480ms |
| 1300 | Fireworks 1D | L+R | crossfade=480ms |
| 2800 | Sinelon Rainbow | L+R | sx=195, ix=120, resep-pal=11, crossfade=480ms |
| 4400 | Shimmer | L+R | resep-pal=11, crossfade=480ms |
| 5800 | Fade | L+R | sx=200, resep-pal=0, crossfade=480ms |
