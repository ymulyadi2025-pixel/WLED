# Katalog 55 animasi Welcoming Mizuma

Setiap animasi adalah **rangkaian efek asli WLED** (tahap demi tahap). Semua show dinormalisasi ke 7000 ms.
Stage penutup stabil dimulai pada 6250 ms dan ditahan sebelum perpindahan ke Riding. Transisi antartahap memakai crossfade WLED sesuai kategori dan jeda cue.
Palette yang dipilih pengguna di UI menjadi palette global saat show berjalan. Nilai palette di resep dipertahankan sebagai metadata/original look, bukan memaksa pilihan pengguna.
Warna `A1/A2/A3` menjadi aksen yang dipetakan ke palette aktif; hitam dan putih tetap menjadi latar/highlight.


## Signature Tetrix

### MZ01 — Mizuma Ignition
Tetrix lambat ke cepat (kanan ke kiri), Wipe mati kiri ke kanan, Sweep kanan ke kiri, Fireworks 1D, ditutup Fade menyala penuh.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Tetrix | L+R | lewati tunggu 2 dtk Tetrix, sx=160, ix=128, resep-pal=35, crossfade=270ms |
| 720 | Tetrix | L+R | sx=200, ix=128, resep-pal=35, crossfade=240ms |
| 1360 | Tetrix | L+R | sx=235, ix=128, resep-pal=35, crossfade=180ms |
| 1840 | Tetrix | L+R | sx=255, ix=128, resep-pal=35, crossfade=450ms |
| 3040 | Wipe | L+R | mulai di fasa 600 ms, sx=252, ix=128, resep-pal=0, crossfade=190ms |
| 3560 | Sweep | L+R | arah dibalik, sx=252, ix=128, resep-pal=0, crossfade=190ms |
| 4080 | Fireworks 1D | L+R | crossfade=410ms |
| 5160 | Fade | L+R | sx=200, resep-pal=0, crossfade=410ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ02 — Mizuma Afterburn
Alur Ignition dibalik: Fade menyala, Fireworks 1D, Sweep, Wipe mati (arah dibalik), lalu Tetrix cepat ke lambat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Fade | L+R | sx=215, resep-pal=0, crossfade=220ms |
| 600 | Fireworks 1D | L+R | crossfade=390ms |
| 1640 | Sweep | L+R | sx=252, ix=128, resep-pal=0, crossfade=190ms |
| 2160 | Wipe | L+R | arah dibalik, mulai di fasa 600 ms, sx=252, ix=128, resep-pal=0, crossfade=190ms |
| 2680 | Tetrix | L+R | arah dibalik, lewati tunggu 2 dtk Tetrix, sx=255, ix=128, resep-pal=35, crossfade=240ms |
| 3320 | Tetrix | L+R | arah dibalik, sx=240, ix=128, resep-pal=35, crossfade=240ms |
| 3960 | Tetrix | L+R | arah dibalik, sx=215, ix=128, resep-pal=35, crossfade=250ms |
| 4640 | Tetrix | L+R | arah dibalik, sx=185, ix=128, resep-pal=35, crossfade=480ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ03 — Golden Dawn
Sunrise fajar emas, Fireworks 1D, Twinkle, ditutup Shimmer.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sunrise | L+R | sx=174, resep-pal=35, crossfade=480ms |
| 1880 | Fireworks 1D | L+R | resep-pal=35, crossfade=480ms |
| 3320 | Twinkle | L+R | sx=190, ix=200, resep-pal=0, crossfade=410ms |
| 4400 | Shimmer | L+R | resep-pal=35, crossfade=480ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ04 — Prism Finale
Shimmer, Sinelon Rainbow, Chase Rainbow, Ripple Rainbow lalu penutup Fade amber.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Shimmer | L+R | resep-pal=11, crossfade=480ms |
| 1360 | Sinelon Rainbow | L+R | sx=200, ix=110, resep-pal=11, crossfade=480ms |
| 2720 | Chase Rainbow | L+R | sx=238, ix=100, resep-pal=11, crossfade=390ms |
| 3760 | Ripple Rainbow | L+R | sx=160, ix=120, crossfade=360ms |
| 4720 | Fade | L+R | sx=190, resep-pal=0, crossfade=480ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ05 — Stack Royale
Tetrix campuran: pelangi cepat, ledakan PS Starburst, Tetrix satu warna arah balik, Colortwinkles, Fade.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Tetrix | L+R | lewati tunggu 2 dtk Tetrix, sx=250, ix=128, resep-pal=11, crossfade=480ms |
| 1560 | PS Starburst | L+R | resep-pal=72, crossfade=280ms |
| 2320 | Tetrix | L+R | arah dibalik, lewati tunggu 2 dtk Tetrix, sx=250, ix=160, resep-pal=0, crossfade=480ms |
| 3920 | Colortwinkles | L+R | sx=170, ix=170, resep-pal=6, crossfade=340ms |
| 4840 | Fade | L+R | sx=210, resep-pal=0, crossfade=480ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |


## Kedipan Cepat

### MZ06 — Raijin Strobe
Dewa petir: burst Strobe Mega putih, Strobe amber, Lightning, ditutup menyala.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Mega | L+R | sx=236, ix=30, resep-pal=0, crossfade=140ms |
| 1360 | Strobe | L+R | sx=249, resep-pal=0, crossfade=140ms |
| 2720 | Lightning | L+R | sx=185, ix=180, crossfade=140ms |
| 4080 | Strobe Mega | L+R | sx=240, ix=20, resep-pal=0, crossfade=140ms |
| 4960 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ07 — Katana Slash
Tebasan beruntun Wipe/Sweep bergantian arah dan warna.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Wipe | L+R | sx=254, ix=128, resep-pal=0, crossfade=140ms |
| 400 | Sweep | L+R | arah dibalik, sx=254, ix=128, resep-pal=0, crossfade=140ms |
| 800 | Wipe | L+R | sx=254, ix=128, resep-pal=0, crossfade=140ms |
| 1200 | Sweep | L+R | arah dibalik, sx=254, ix=128, resep-pal=0, crossfade=140ms |
| 1600 | Wipe | L+R | sx=254, ix=128, resep-pal=0, crossfade=140ms |
| 2000 | Chase Flash | L+R | sx=246, resep-pal=0, crossfade=140ms |
| 3440 | Strobe | L+R | sx=250, resep-pal=0, crossfade=140ms |
| 4640 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ08 — Shinobi Bergantian
Kiri dan kanan menyala bergantian makin cepat seperti ninja.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Solid | Kanan | resep-pal=0, crossfade=70ms |
| 0 | Solid | Kiri | resep-pal=0, crossfade=70ms |
| 190 | Solid | Kanan | resep-pal=0, crossfade=70ms |
| 190 | Solid | Kiri | resep-pal=0, crossfade=70ms |
| 380 | Solid | Kanan | resep-pal=0, crossfade=70ms |
| 380 | Solid | Kiri | resep-pal=0, crossfade=70ms |
| 580 | Solid | Kanan | resep-pal=0, crossfade=70ms |
| 580 | Solid | Kiri | resep-pal=0, crossfade=70ms |
| 770 | Solid | Kanan | resep-pal=0, crossfade=40ms |
| 770 | Solid | Kiri | resep-pal=0, crossfade=40ms |
| 900 | Solid | Kanan | resep-pal=0, crossfade=50ms |
| 900 | Solid | Kiri | resep-pal=0, crossfade=50ms |
| 1040 | Solid | Kanan | resep-pal=0, crossfade=50ms |
| 1040 | Solid | Kiri | resep-pal=0, crossfade=50ms |
| 1180 | Solid | Kanan | resep-pal=0, crossfade=40ms |
| 1180 | Solid | Kiri | resep-pal=0, crossfade=40ms |
| 1310 | Solid | Kanan | resep-pal=0, crossfade=50ms |
| 1310 | Solid | Kiri | resep-pal=0, crossfade=50ms |
| 1450 | Solid | Kanan | resep-pal=0, crossfade=40ms |
| 1450 | Solid | Kiri | resep-pal=0, crossfade=40ms |
| 1580 | Solid | Kanan | resep-pal=0, crossfade=30ms |
| 1580 | Solid | Kiri | resep-pal=0, crossfade=30ms |
| 1680 | Solid | Kanan | resep-pal=0, crossfade=30ms |
| 1680 | Solid | Kiri | resep-pal=0, crossfade=30ms |
| 1780 | Solid | Kanan | resep-pal=0 |
| 1780 | Solid | Kiri | resep-pal=0 |
| 1870 | Solid | Kanan | resep-pal=0, crossfade=30ms |
| 1870 | Solid | Kiri | resep-pal=0, crossfade=30ms |
| 1970 | Solid | Kanan | resep-pal=0 |
| 1970 | Solid | Kiri | resep-pal=0 |
| 2060 | Solid | Kanan | resep-pal=0, crossfade=30ms |
| 2060 | Solid | Kiri | resep-pal=0, crossfade=30ms |
| 2160 | Solid | Kanan | resep-pal=0, crossfade=30ms |
| 2160 | Solid | Kiri | resep-pal=0, crossfade=30ms |
| 2260 | Solid | Kanan | resep-pal=0, crossfade=50ms |
| 2260 | Solid | Kiri | resep-pal=0, crossfade=50ms |
| 2400 | Strobe | L+R | sx=249, resep-pal=0, crossfade=140ms |
| 3680 | Strobe Mega | L+R | sx=238, ix=40, resep-pal=0, crossfade=140ms |
| 4800 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ09 — Oni Heartbeat
Detak jantung oni merah-oranye yang makin liar.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Heartbeat | L+R | sx=190, ix=170, resep-pal=0, crossfade=140ms |
| 1600 | Heartbeat | L+R | sx=220, ix=190, resep-pal=0, crossfade=140ms |
| 2720 | Blink | L+R | sx=244, ix=70, resep-pal=0, crossfade=140ms |
| 3680 | Strobe | L+R | sx=250, resep-pal=0, crossfade=140ms |
| 4720 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ10 — Kitsune Dash
Rubah berlari: Chase 2 cepat, Scan Dual, Chase 3 lalu menyala.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Chase 2 | L+R | sx=246, ix=80, resep-pal=0, crossfade=140ms |
| 1200 | Scan Dual | L+R | sx=252, ix=80, resep-pal=0, crossfade=140ms |
| 2400 | Chase 3 | L+R | sx=246, ix=70, resep-pal=0, crossfade=140ms |
| 3600 | Chase Flash | L+R | sx=247, resep-pal=0, crossfade=140ms |
| 4720 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ11 — Tengu Burst
Dissolve cepat, Glitter, Strobe Mega: ledakan acak berkilau.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Dissolve | L+R | sx=235, ix=235, resep-pal=0, crossfade=140ms |
| 1360 | Glitter | L+R | sx=220, ix=220, resep-pal=0, crossfade=140ms |
| 2640 | Strobe Mega | L+R | sx=238, ix=30, resep-pal=0, crossfade=140ms |
| 3760 | Dissolve | L+R | sx=245, ix=245, resep-pal=0, crossfade=140ms |
| 4720 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ12 — Hayate Rush
Angin kencang: Chase Flash, Running Dual, Two Dots berkejaran.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Chase Flash | L+R | sx=247, resep-pal=0, crossfade=140ms |
| 1200 | Running Dual | L+R | sx=238, ix=100, resep-pal=0, crossfade=140ms |
| 2400 | Two Dots | L+R | sx=238, ix=110, resep-pal=0, crossfade=140ms |
| 3600 | Chase Flash | L+R | sx=250, resep-pal=0, crossfade=140ms |
| 4640 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ13 — Ronin Spark
Percikan pedang ronin: Sparkle, Glitter, Twinkle rapat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sparkle | L+R | sx=250, resep-pal=0, crossfade=140ms |
| 1200 | Glitter | L+R | sx=225, ix=225, resep-pal=0, crossfade=140ms |
| 2400 | Twinkle | L+R | sx=230, ix=230, resep-pal=0, crossfade=140ms |
| 3520 | Sparkle | L+R | sx=252, resep-pal=0, crossfade=140ms |
| 4560 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ14 — Kabuto Pulse
Pola Blink berirama seperti genderang di helm kabuto.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Blink | L+R | sx=236, ix=60, resep-pal=0, crossfade=140ms |
| 880 | Blink | L+R | sx=242, ix=90, resep-pal=0, crossfade=140ms |
| 1760 | Blink | L+R | sx=247, ix=110, resep-pal=0, crossfade=140ms |
| 2640 | Blink Rainbow | L+R | sx=246, ix=120, crossfade=140ms |
| 3760 | Strobe | L+R | sx=250, resep-pal=0, crossfade=140ms |
| 4720 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ15 — Tsunami Flash
Riak cepat berwarna lalu kilat Strobe Rainbow.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Ripple | L+R | sx=210, ix=170, resep-pal=9, crossfade=140ms |
| 1360 | Strobe Rainbow | L+R | sx=247, crossfade=140ms |
| 2560 | Ripple | L+R | sx=220, ix=190, resep-pal=11, crossfade=140ms |
| 3680 | Strobe Mega | L+R | sx=238, ix=40, resep-pal=0, crossfade=140ms |
| 4720 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ16 — Yami Glitch
Efek glitch kegelapan: Glitter, Dissolve Rnd, Strobe patah-patah.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Glitter | L+R | sx=230, ix=230, resep-pal=0, crossfade=140ms |
| 1120 | Dissolve Rnd | L+R | sx=235, ix=235, crossfade=140ms |
| 2240 | Strobe | L+R | sx=249, resep-pal=0, crossfade=140ms |
| 3040 | Glitter | L+R | sx=245, ix=245, resep-pal=0, crossfade=140ms |
| 4000 | Strobe Mega | L+R | sx=236, ix=50, resep-pal=0, crossfade=140ms |
| 4880 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ17 — Hanabi Rapid
Kembang api beruntun: Fireworks 1D, Popcorn, Sparkle lalu menyala.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Fireworks 1D | L+R | crossfade=140ms |
| 1440 | Popcorn | L+R | sx=190, resep-pal=35, crossfade=140ms |
| 2640 | Sparkle | L+R | sx=248, resep-pal=0, crossfade=140ms |
| 3440 | Fireworks 1D | L+R | resep-pal=11, crossfade=140ms |
| 4720 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ18 — Taiko Beat
Dentuman taiko: kedip berirama dengan Bouncing Balls.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Mega | L+R | sx=232, ix=20, resep-pal=0, crossfade=140ms |
| 800 | Strobe Mega | L+R | sx=236, ix=20, resep-pal=0, crossfade=140ms |
| 1600 | Bouncing Balls | L+R | resep-pal=6, crossfade=140ms |
| 2880 | Strobe Mega | L+R | sx=240, ix=30, resep-pal=0, crossfade=140ms |
| 3760 | Blink | L+R | sx=246, ix=120, resep-pal=0, crossfade=140ms |
| 4720 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ19 — Kaen Burst
Api membara: Chase Flash Rnd, Strobe oranye, Lightning hangat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Chase Flash Rnd | L+R | sx=246, crossfade=140ms |
| 1200 | Strobe | L+R | sx=248, resep-pal=0, crossfade=140ms |
| 2240 | Lightning | L+R | sx=190, ix=190, crossfade=140ms |
| 3360 | Chase Flash | L+R | sx=248, resep-pal=0, crossfade=140ms |
| 4160 | Strobe Mega | L+R | sx=238, ix=30, resep-pal=0, crossfade=140ms |
| 4880 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ20 — Shuriken Spin
Bintang lempar: Scan, Scan Dual, Two Dots dan Sinelon Dual cepat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Scan | L+R | sx=252, ix=90, resep-pal=0, crossfade=140ms |
| 1040 | Scan Dual | L+R | sx=254, ix=90, resep-pal=0, crossfade=140ms |
| 2080 | Two Dots | L+R | sx=245, ix=90, resep-pal=0, crossfade=140ms |
| 3120 | Sinelon Dual | L+R | sx=215, ix=70, resep-pal=0, crossfade=140ms |
| 4160 | Strobe | L+R | sx=250, resep-pal=0, crossfade=140ms |
| 4880 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ21 — Nitro Flicker
Kelip nitro: Colortwinkles cepat, Twinkle, Strobe pendek.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Colortwinkles | L+R | sx=220, ix=220, resep-pal=6, crossfade=140ms |
| 1200 | Twinkle | L+R | sx=240, ix=240, resep-pal=0, crossfade=140ms |
| 2240 | Strobe | L+R | sx=250, resep-pal=0, crossfade=140ms |
| 2800 | Colortwinkles | L+R | sx=230, ix=230, resep-pal=35, crossfade=140ms |
| 3840 | Strobe Mega | L+R | sx=240, ix=30, resep-pal=0, crossfade=140ms |
| 4800 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ22 — Dragon Roar
Raungan naga: ledakan PS Starburst berulang dan Strobe Mega.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | PS Starburst | L+R | resep-pal=72, crossfade=140ms |
| 1200 | Strobe Mega | L+R | sx=236, ix=40, resep-pal=0, crossfade=140ms |
| 2320 | PS Starburst | L+R | resep-pal=35, crossfade=140ms |
| 3520 | Strobe Mega | L+R | sx=240, ix=30, resep-pal=0, crossfade=140ms |
| 4480 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ23 — Samurai Triple
Tiga kedipan tegas per kelompok dengan tiga warna aksen.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Mega | L+R | sx=234, ix=20, resep-pal=0, crossfade=140ms |
| 800 | Strobe Mega | L+R | sx=234, ix=20, resep-pal=0, crossfade=140ms |
| 1600 | Strobe Mega | L+R | sx=234, ix=20, resep-pal=0, crossfade=140ms |
| 2400 | Strobe Mega | L+R | sx=240, ix=20, resep-pal=0, crossfade=140ms |
| 3120 | Strobe Mega | L+R | sx=240, ix=20, resep-pal=0, crossfade=140ms |
| 3840 | Strobe Mega | L+R | sx=240, ix=20, resep-pal=0, crossfade=140ms |
| 4640 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ24 — Thunder Chase
Kilat dan kejaran pelangi cepat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Lightning | L+R | sx=180, ix=190, crossfade=140ms |
| 1200 | Chase Rainbow | L+R | sx=244, ix=90, resep-pal=11, crossfade=140ms |
| 2320 | Strobe | L+R | sx=249, resep-pal=0, crossfade=140ms |
| 2960 | Chase Rainbow | L+R | sx=250, ix=100, resep-pal=6, crossfade=140ms |
| 4000 | Lightning | L+R | sx=200, ix=200, crossfade=140ms |
| 4880 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |

### MZ25 — Zenkai Flash
Puncak kedipan: gabungan Wipe, Strobe, Glitter, Chase Flash lalu ledakan terang.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Wipe | L+R | sx=254, ix=128, resep-pal=0, crossfade=140ms |
| 400 | Strobe | L+R | sx=250, resep-pal=0, crossfade=140ms |
| 960 | Glitter | L+R | sx=235, ix=235, resep-pal=0, crossfade=140ms |
| 1760 | Chase Flash | L+R | sx=248, resep-pal=0, crossfade=140ms |
| 2560 | Strobe Mega | L+R | sx=238, ix=40, resep-pal=0, crossfade=140ms |
| 3440 | Dissolve | L+R | sx=245, ix=245, resep-pal=0, crossfade=140ms |
| 4160 | Solid | Kanan | resep-pal=0, crossfade=30ms |
| 4160 | Solid | Kiri | resep-pal=0, crossfade=30ms |
| 4260 | Solid | Kanan | resep-pal=0 |
| 4260 | Solid | Kiri | resep-pal=0 |
| 4350 | Solid | Kanan | resep-pal=0, crossfade=30ms |
| 4350 | Solid | Kiri | resep-pal=0, crossfade=30ms |
| 4450 | Solid | Kanan | resep-pal=0, crossfade=100ms |
| 4450 | Solid | Kiri | resep-pal=0, crossfade=100ms |
| 4720 | Fade | L+R | sx=200, resep-pal=0, crossfade=140ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=140ms |


## Keindahan

### MZ26 — Sakura Drift
Kelopak sakura melayang: Colortwinkles, Meteor lembut, Shimmer muda.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Colortwinkles | L+R | sx=90, ix=110, resep-pal=49, crossfade=560ms |
| 1520 | Meteor | L+R | sx=150, ix=160, resep-pal=49, crossfade=510ms |
| 2880 | Shimmer | L+R | resep-pal=49, crossfade=540ms |
| 4320 | Breathe | L+R | sx=120, resep-pal=0, crossfade=560ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ27 — Fuji Sunrise
Fajar di Gunung Fuji: Sunrise, Colorwaves, Fade hangat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sunrise | L+R | sx=174, resep-pal=35, crossfade=560ms |
| 1880 | Colorwaves | L+R | resep-pal=13, crossfade=560ms |
| 4000 | Fade | L+R | sx=120, resep-pal=0, crossfade=560ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ28 — Mizu Ripple
Riak air (mizu): Ripple, Pacifica, Lake.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Ripple | L+R | sx=110, ix=120, resep-pal=9, crossfade=560ms |
| 1680 | Pacifica | L+R | resep-pal=51, crossfade=560ms |
| 3360 | Lake | L+R | resep-pal=9, crossfade=540ms |
| 4800 | Fade | L+R | sx=200, resep-pal=0, crossfade=550ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ29 — Hotaru Fireflies
Kunang-kunang malam: Twinkle kuning, Fairy, Breathe hangat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Twinkle | L+R | sx=80, ix=110, resep-pal=0, crossfade=560ms |
| 1600 | Fairy | L+R | sx=110, ix=110, resep-pal=0, crossfade=540ms |
| 3040 | Twinkle | L+R | sx=70, ix=160, resep-pal=0, crossfade=420ms |
| 4160 | Breathe | L+R | sx=110, resep-pal=0, crossfade=560ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ30 — Tsuki Moonlight
Cahaya bulan: Breathe putih kebiruan, Shimmer es, Fade.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Breathe | L+R | sx=90, resep-pal=0, crossfade=560ms |
| 1840 | Shimmer | L+R | resep-pal=7, crossfade=560ms |
| 3680 | Fade | L+R | sx=110, resep-pal=0, crossfade=300ms |
| 4480 | Fade | L+R | sx=110, resep-pal=0, crossfade=560ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ31 — Ginga Galaxy
Galaksi: Sinelon Dual, Twinklefox, Shimmer ungu.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sinelon Dual | L+R | sx=120, ix=120, resep-pal=0, crossfade=560ms |
| 1680 | Twinklefox | L+R | sx=90, ix=130, resep-pal=45, crossfade=560ms |
| 3200 | Shimmer | L+R | resep-pal=46, crossfade=540ms |
| 4640 | Fade | L+R | sx=120, resep-pal=0, crossfade=560ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ32 — Nami Wave
Ombak berlapis: Wavesins, Pacifica, Colorwaves laut.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Wavesins | L+R | sx=90, resep-pal=9, crossfade=560ms |
| 1760 | Pacifica | L+R | resep-pal=51, crossfade=560ms |
| 3360 | Colorwaves | L+R | resep-pal=22, crossfade=540ms |
| 4800 | Fade | L+R | sx=200, resep-pal=0, crossfade=550ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ33 — Kasumi Mist
Kabut pagi: Noise Pal pastel, Lake, Breathe.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Noise Pal | L+R | sx=70, ix=100, resep-pal=20, crossfade=560ms |
| 1840 | Lake | L+R | resep-pal=20, crossfade=560ms |
| 3440 | Breathe | L+R | sx=100, resep-pal=0, crossfade=450ms |
| 4640 | Fade | L+R | sx=110, resep-pal=0, crossfade=560ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ34 — Akari Bloom
Cahaya lampion mekar: Wipe lambat, Fade, Shimmer hangat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Wipe | L+R | sx=160, ix=128, resep-pal=0, crossfade=560ms |
| 1840 | Fade | L+R | sx=150, resep-pal=0, crossfade=510ms |
| 3200 | Shimmer | L+R | resep-pal=39, crossfade=560ms |
| 4720 | Fade | L+R | sx=130, resep-pal=0, crossfade=560ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ35 — Yuki Shimmer
Salju berkilau: Shimmer es, Twinkle putih, Sparkle.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Shimmer | L+R | resep-pal=36, crossfade=560ms |
| 1760 | Twinkle | L+R | sx=90, ix=160, resep-pal=0, crossfade=540ms |
| 3200 | Sparkle | L+R | sx=200, resep-pal=0, crossfade=480ms |
| 4480 | Fade | L+R | sx=110, resep-pal=0, crossfade=210ms |
| 5040 | Fade | L+R | sx=110, resep-pal=0, crossfade=450ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ36 — Koi Pond
Ikan koi berenang: Sinelon Dual oranye-putih, Ripple, Pacifica.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sinelon Dual | L+R | sx=130, ix=130, resep-pal=0, crossfade=560ms |
| 1760 | Ripple | L+R | sx=120, ix=120, resep-pal=44, crossfade=560ms |
| 3360 | Pacifica | L+R | resep-pal=44, crossfade=540ms |
| 4800 | Fade | L+R | sx=200, resep-pal=0, crossfade=550ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ37 — Hanabi Garden
Taman kembang api: Fireworks 1D, PS Starburst, Colortwinkles.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Fireworks 1D | L+R | resep-pal=72, crossfade=560ms |
| 1600 | PS Starburst | L+R | resep-pal=72, crossfade=540ms |
| 3040 | Colortwinkles | L+R | sx=130, ix=150, resep-pal=6, crossfade=540ms |
| 4480 | Fade | L+R | sx=170, resep-pal=0, crossfade=560ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ38 — Tanabata Stars
Festival bintang: Twinklefox, Colortwinkles, Shimmer.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Twinklefox | L+R | sx=90, ix=130, resep-pal=46, crossfade=560ms |
| 1600 | Colortwinkles | L+R | sx=90, ix=130, resep-pal=45, crossfade=560ms |
| 3120 | Shimmer | L+R | resep-pal=45, crossfade=540ms |
| 4560 | Fade | L+R | sx=120, resep-pal=0, crossfade=560ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ39 — Aurora Kaze
Aurora tertiup angin: Aurora, Colorwaves aurora, Fade.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Aurora | L+R | resep-pal=50, crossfade=560ms |
| 2240 | Colorwaves | L+R | resep-pal=55, crossfade=510ms |
| 3600 | Shimmer | L+R | resep-pal=50, crossfade=450ms |
| 4800 | Fade | L+R | sx=200, resep-pal=0, crossfade=550ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ40 — Ame Neon
Hujan neon kota: Drip, Bouncing Balls, Ripple Rainbow.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Drip | L+R | resep-pal=6, crossfade=560ms |
| 1680 | Bouncing Balls | L+R | resep-pal=6, crossfade=540ms |
| 3120 | Ripple Rainbow | L+R | sx=120, ix=130, crossfade=560ms |
| 4640 | Fade | L+R | sx=200, resep-pal=0, crossfade=560ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ41 — Kaze Whisper
Bisikan angin: Sinelon lembut, Wavesins, Fade.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sinelon | L+R | sx=110, ix=130, resep-pal=0, crossfade=560ms |
| 1680 | Wavesins | L+R | sx=70, resep-pal=20, crossfade=560ms |
| 3280 | Breathe | L+R | sx=100, resep-pal=0, crossfade=450ms |
| 4480 | Fade | L+R | sx=120, resep-pal=0, crossfade=560ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ42 — Hoshi Cascade
Hujan bintang: Meteor, PS Spray 1D, Twinkle.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Meteor | L+R | sx=140, ix=170, resep-pal=0, crossfade=560ms |
| 1680 | PS Spray 1D | L+R | resep-pal=72, crossfade=560ms |
| 3200 | Twinkle | L+R | sx=110, ix=170, resep-pal=0, crossfade=510ms |
| 4560 | Fade | L+R | sx=200, resep-pal=0, crossfade=560ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ43 — Asagiri Dawn
Fajar berkabut: Sunrise, Pacifica lembut, Fade.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sunrise | L+R | sx=163, resep-pal=21, crossfade=560ms |
| 2160 | Pacifica | L+R | resep-pal=21, crossfade=560ms |
| 4160 | Fade | L+R | sx=120, resep-pal=0, crossfade=560ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ44 — Kyoto Lantern
Lentera Kyoto: Candle, Fire 2012, Breathe hangat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Candle | L+R | resep-pal=0, crossfade=560ms |
| 1680 | Fire 2012 | L+R | resep-pal=35, crossfade=560ms |
| 3440 | Breathe | L+R | sx=100, resep-pal=0, crossfade=420ms |
| 4560 | Fade | L+R | sx=110, resep-pal=0, crossfade=560ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ45 — Mizuma Eternal
Tanda tangan Mizuma yang abadi: Colorwaves emas, Shimmer, Fade.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Colorwaves | L+R | resep-pal=35, crossfade=560ms |
| 1840 | Shimmer | L+R | resep-pal=35, crossfade=560ms |
| 3440 | Meteor | L+R | sx=170, ix=150, resep-pal=0, crossfade=390ms |
| 4480 | Fade | L+R | sx=130, resep-pal=0, crossfade=560ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |


## Campuran

### MZ46 — Raijin Bloom
Kilat Strobe yang mekar menjadi Shimmer sakura.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Mega | L+R | sx=236, ix=30, resep-pal=0, crossfade=360ms |
| 1040 | Strobe | L+R | sx=250, resep-pal=0, crossfade=360ms |
| 2080 | Shimmer | L+R | resep-pal=49, crossfade=360ms |
| 3920 | Breathe | L+R | sx=120, resep-pal=0, crossfade=360ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ47 — Hanabi Rush
Kembang api cepat yang berakhir lembut.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Fireworks 1D | L+R | crossfade=360ms |
| 1360 | Strobe Mega | L+R | sx=238, ix=30, resep-pal=0, crossfade=360ms |
| 2320 | Colortwinkles | L+R | sx=180, ix=190, resep-pal=72, crossfade=360ms |
| 4000 | Fade | L+R | sx=160, resep-pal=0, crossfade=360ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ48 — Oni Prism
Strobe Rainbow menuju Sinelon Rainbow yang indah.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Rainbow | L+R | sx=247, crossfade=360ms |
| 1040 | Blink Rainbow | L+R | sx=244, ix=120, crossfade=360ms |
| 2000 | Sinelon Rainbow | L+R | sx=190, ix=120, resep-pal=11, crossfade=360ms |
| 3520 | Ripple Rainbow | L+R | sx=150, ix=120, crossfade=360ms |
| 4720 | Fade | L+R | sx=200, resep-pal=0, crossfade=360ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ49 — Kitsune Aurora
Kejaran rubah lalu aurora tenang.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Chase 2 | L+R | sx=246, ix=80, resep-pal=0, crossfade=360ms |
| 1120 | Scan Dual | L+R | sx=252, ix=80, resep-pal=0, crossfade=360ms |
| 2240 | Aurora | L+R | resep-pal=50, crossfade=360ms |
| 4640 | Fade | L+R | sx=200, resep-pal=0, crossfade=360ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ50 — Katana Dawn
Tebasan cepat yang berakhir dalam fajar.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Wipe | L+R | sx=254, ix=128, resep-pal=0, crossfade=140ms |
| 380 | Sweep | L+R | arah dibalik, sx=254, ix=128, resep-pal=0, crossfade=140ms |
| 770 | Wipe | L+R | sx=254, ix=128, resep-pal=0, crossfade=140ms |
| 1150 | Sweep | L+R | arah dibalik, sx=254, ix=128, resep-pal=0, crossfade=170ms |
| 1600 | Sunrise | L+R | sx=174, resep-pal=35, crossfade=360ms |
| 3680 | Fade | L+R | sx=150, resep-pal=0, crossfade=360ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ51 — Tsunami Spark
Riak dan percikan yang bermuara ke Pacifica.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Ripple | L+R | sx=200, ix=160, resep-pal=9, crossfade=360ms |
| 1200 | Sparkle | L+R | sx=245, resep-pal=0, crossfade=360ms |
| 2240 | Pacifica | L+R | resep-pal=51, crossfade=360ms |
| 4480 | Fade | L+R | sx=200, resep-pal=0, crossfade=360ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ52 — Yokai Glow
Kilatan yokai yang berubah menjadi cahaya bernapas.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Glitter | L+R | sx=235, ix=235, resep-pal=0, crossfade=360ms |
| 1120 | Dissolve | L+R | sx=240, ix=240, resep-pal=0, crossfade=360ms |
| 2240 | Breathe | L+R | sx=120, resep-pal=0, crossfade=360ms |
| 3440 | Shimmer | L+R | resep-pal=69, crossfade=360ms |
| 4640 | Fade | L+R | sx=200, resep-pal=0, crossfade=360ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ53 — Shogun Pulse
Denyut shogun, ledakan PS Starburst lalu Colorwaves megah.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Heartbeat | L+R | sx=200, ix=180, resep-pal=0, crossfade=360ms |
| 1440 | PS Starburst | L+R | resep-pal=35, crossfade=360ms |
| 2640 | Colorwaves | L+R | resep-pal=35, crossfade=360ms |
| 4080 | Shimmer | L+R | resep-pal=35, crossfade=360ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ54 — Sakura Storm
Badai sakura: Dissolve, Glitter, Colortwinkles, Shimmer.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Dissolve | L+R | sx=238, ix=235, resep-pal=0, crossfade=360ms |
| 1120 | Glitter | L+R | sx=225, ix=225, resep-pal=0, crossfade=360ms |
| 2240 | Colortwinkles | L+R | sx=150, ix=170, resep-pal=49, crossfade=360ms |
| 3520 | Shimmer | L+R | resep-pal=49, crossfade=360ms |
| 4720 | Fade | L+R | sx=200, resep-pal=0, crossfade=360ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |

### MZ55 — Mizuma Grand Finale
Final megah: Strobe, Fireworks 1D, Sinelon Rainbow, Shimmer, Fade.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Mega | L+R | sx=236, ix=30, resep-pal=0, crossfade=360ms |
| 1040 | Fireworks 1D | L+R | crossfade=360ms |
| 2240 | Sinelon Rainbow | L+R | sx=195, ix=120, resep-pal=11, crossfade=360ms |
| 3520 | Shimmer | L+R | resep-pal=11, crossfade=360ms |
| 4640 | Fade | L+R | sx=200, resep-pal=0, crossfade=360ms |
| 6250 | Solid | L+R | resep-pal=0, crossfade=280ms |
