# Katalog 55 animasi Welcoming Mizuma

Setiap animasi adalah **rangkaian efek asli WLED** (tahap demi tahap). Waktu dalam milidetik dari awal show (total ±7000 ms).
Warna `A1/A2/A3` = tiga warna aksen yang Anda atur di UI (default: amber / putih / oranye-merah). `d` = versi redup.


## Signature Tetrix

### MZ01 — Mizuma Ignition
Tetrix lambat ke cepat (kanan ke kiri), Wipe mati kiri ke kanan, Sweep kanan ke kiri, Fireworks 1D, ditutup Fade menyala penuh.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Tetrix | L+R | lewati tunggu 2 dtk Tetrix, sx=160, ix=128, pal=35 |
| 900 | Tetrix | L+R | sx=200, ix=128, pal=35 |
| 1700 | Tetrix | L+R | sx=235, ix=128, pal=35 |
| 2300 | Tetrix | L+R | sx=255, ix=128, pal=35 |
| 3800 | Wipe | L+R | mulai di fasa 600 ms, sx=252, ix=128, pal=0 |
| 4450 | Sweep | L+R | arah dibalik, sx=252, ix=128, pal=0 |
| 5100 | Fireworks 1D | L+R |  |
| 6450 | Fade | L+R | sx=200, pal=0 |

### MZ02 — Mizuma Afterburn
Alur Ignition dibalik: Fade menyala, Fireworks 1D, Sweep, Wipe mati (arah dibalik), lalu Tetrix cepat ke lambat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Fade | L+R | sx=215, pal=0 |
| 750 | Fireworks 1D | L+R |  |
| 2050 | Sweep | L+R | sx=252, ix=128, pal=0 |
| 2700 | Wipe | L+R | arah dibalik, mulai di fasa 600 ms, sx=252, ix=128, pal=0 |
| 3350 | Tetrix | L+R | arah dibalik, lewati tunggu 2 dtk Tetrix, sx=255, ix=128, pal=35 |
| 4150 | Tetrix | L+R | arah dibalik, sx=240, ix=128, pal=35 |
| 4950 | Tetrix | L+R | arah dibalik, sx=215, ix=128, pal=35 |
| 5800 | Tetrix | L+R | arah dibalik, sx=185, ix=128, pal=35 |

### MZ03 — Golden Dawn
Sunrise fajar emas, Fireworks 1D, Twinkle, ditutup Shimmer.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sunrise | L+R | sx=174, pal=35 |
| 2350 | Fireworks 1D | L+R | pal=35 |
| 4150 | Twinkle | L+R | sx=190, ix=200, pal=0 |
| 5500 | Shimmer | L+R | pal=35 |

### MZ04 — Prism Finale
Shimmer, Sinelon Rainbow, Chase Rainbow, Ripple Rainbow lalu penutup Fade amber.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Shimmer | L+R | pal=11 |
| 1700 | Sinelon Rainbow | L+R | sx=200, ix=110, pal=11 |
| 3400 | Chase Rainbow | L+R | sx=238, ix=100, pal=11 |
| 4700 | Ripple Rainbow | L+R | sx=160, ix=120 |
| 5900 | Fade | L+R | sx=190, pal=0 |

### MZ05 — Stack Royale
Tetrix campuran: pelangi cepat, ledakan PS Starburst, Tetrix satu warna arah balik, Colortwinkles, Fade.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Tetrix | L+R | lewati tunggu 2 dtk Tetrix, sx=250, ix=128, pal=11 |
| 1950 | PS Starburst | L+R | pal=72 |
| 2900 | Tetrix | L+R | arah dibalik, lewati tunggu 2 dtk Tetrix, sx=250, ix=160, pal=0 |
| 4900 | Colortwinkles | L+R | sx=170, ix=170, pal=6 |
| 6050 | Fade | L+R | sx=210, pal=0 |


## Kedipan Cepat

### MZ06 — Raijin Strobe
Dewa petir: burst Strobe Mega putih, Strobe amber, Lightning, ditutup menyala.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Mega | L+R | sx=236, ix=30, pal=0 |
| 1700 | Strobe | L+R | sx=249, pal=0 |
| 3400 | Lightning | L+R | sx=185, ix=180 |
| 5100 | Strobe Mega | L+R | sx=240, ix=20, pal=0 |
| 6200 | Fade | L+R | sx=200, pal=0 |

### MZ07 — Katana Slash
Tebasan beruntun Wipe/Sweep bergantian arah dan warna.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Wipe | L+R | sx=254, ix=128, pal=0 |
| 500 | Sweep | L+R | arah dibalik, sx=254, ix=128, pal=0 |
| 1000 | Wipe | L+R | sx=254, ix=128, pal=0 |
| 1500 | Sweep | L+R | arah dibalik, sx=254, ix=128, pal=0 |
| 2000 | Wipe | L+R | sx=254, ix=128, pal=0 |
| 2500 | Chase Flash | L+R | sx=246, pal=0 |
| 4300 | Strobe | L+R | sx=250, pal=0 |
| 5800 | Fade | L+R | sx=200, pal=0 |

### MZ08 — Shinobi Bergantian
Kiri dan kanan menyala bergantian makin cepat seperti ninja.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Solid | Kanan | pal=0 |
| 0 | Solid | Kiri | pal=0 |
| 240 | Solid | Kanan | pal=0 |
| 240 | Solid | Kiri | pal=0 |
| 480 | Solid | Kanan | pal=0 |
| 480 | Solid | Kiri | pal=0 |
| 720 | Solid | Kanan | pal=0 |
| 720 | Solid | Kiri | pal=0 |
| 960 | Solid | Kanan | pal=0 |
| 960 | Solid | Kiri | pal=0 |
| 1130 | Solid | Kanan | pal=0 |
| 1130 | Solid | Kiri | pal=0 |
| 1300 | Solid | Kanan | pal=0 |
| 1300 | Solid | Kiri | pal=0 |
| 1470 | Solid | Kanan | pal=0 |
| 1470 | Solid | Kiri | pal=0 |
| 1640 | Solid | Kanan | pal=0 |
| 1640 | Solid | Kiri | pal=0 |
| 1810 | Solid | Kanan | pal=0 |
| 1810 | Solid | Kiri | pal=0 |
| 1980 | Solid | Kanan | pal=0 |
| 1980 | Solid | Kiri | pal=0 |
| 2100 | Solid | Kanan | pal=0 |
| 2100 | Solid | Kiri | pal=0 |
| 2220 | Solid | Kanan | pal=0 |
| 2220 | Solid | Kiri | pal=0 |
| 2340 | Solid | Kanan | pal=0 |
| 2340 | Solid | Kiri | pal=0 |
| 2460 | Solid | Kanan | pal=0 |
| 2460 | Solid | Kiri | pal=0 |
| 2580 | Solid | Kanan | pal=0 |
| 2580 | Solid | Kiri | pal=0 |
| 2700 | Solid | Kanan | pal=0 |
| 2700 | Solid | Kiri | pal=0 |
| 2820 | Solid | Kanan | pal=0 |
| 2820 | Solid | Kiri | pal=0 |
| 3000 | Strobe | L+R | sx=249, pal=0 |
| 4600 | Strobe Mega | L+R | sx=238, ix=40, pal=0 |
| 6000 | Fade | L+R | sx=200, pal=0 |

### MZ09 — Oni Heartbeat
Detak jantung oni merah-oranye yang makin liar.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Heartbeat | L+R | sx=190, ix=170, pal=0 |
| 2000 | Heartbeat | L+R | sx=220, ix=190, pal=0 |
| 3400 | Blink | L+R | sx=244, ix=70, pal=0 |
| 4600 | Strobe | L+R | sx=250, pal=0 |
| 5900 | Fade | L+R | sx=200, pal=0 |

### MZ10 — Kitsune Dash
Rubah berlari: Chase 2 cepat, Scan Dual, Chase 3 lalu menyala.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Chase 2 | L+R | sx=246, ix=80, pal=0 |
| 1500 | Scan Dual | L+R | sx=252, ix=80, pal=0 |
| 3000 | Chase 3 | L+R | sx=246, ix=70, pal=0 |
| 4500 | Chase Flash | L+R | sx=247, pal=0 |
| 5900 | Fade | L+R | sx=200, pal=0 |

### MZ11 — Tengu Burst
Dissolve cepat, Glitter, Strobe Mega: ledakan acak berkilau.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Dissolve | L+R | sx=235, ix=235, pal=0 |
| 1700 | Glitter | L+R | sx=220, ix=220, pal=0 |
| 3300 | Strobe Mega | L+R | sx=238, ix=30, pal=0 |
| 4700 | Dissolve | L+R | sx=245, ix=245, pal=0 |
| 5900 | Fade | L+R | sx=200, pal=0 |

### MZ12 — Hayate Rush
Angin kencang: Chase Flash, Running Dual, Two Dots berkejaran.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Chase Flash | L+R | sx=247, pal=0 |
| 1500 | Running Dual | L+R | sx=238, ix=100, pal=0 |
| 3000 | Two Dots | L+R | sx=238, ix=110, pal=0 |
| 4500 | Chase Flash | L+R | sx=250, pal=0 |
| 5800 | Fade | L+R | sx=200, pal=0 |

### MZ13 — Ronin Spark
Percikan pedang ronin: Sparkle, Glitter, Twinkle rapat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sparkle | L+R | sx=250, pal=0 |
| 1500 | Glitter | L+R | sx=225, ix=225, pal=0 |
| 3000 | Twinkle | L+R | sx=230, ix=230, pal=0 |
| 4400 | Sparkle | L+R | sx=252, pal=0 |
| 5700 | Fade | L+R | sx=200, pal=0 |

### MZ14 — Kabuto Pulse
Pola Blink berirama seperti genderang di helm kabuto.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Blink | L+R | sx=236, ix=60, pal=0 |
| 1100 | Blink | L+R | sx=242, ix=90, pal=0 |
| 2200 | Blink | L+R | sx=247, ix=110, pal=0 |
| 3300 | Blink Rainbow | L+R | sx=246, ix=120 |
| 4700 | Strobe | L+R | sx=250, pal=0 |
| 5900 | Fade | L+R | sx=200, pal=0 |

### MZ15 — Tsunami Flash
Riak cepat berwarna lalu kilat Strobe Rainbow.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Ripple | L+R | sx=210, ix=170, pal=9 |
| 1700 | Strobe Rainbow | L+R | sx=247 |
| 3200 | Ripple | L+R | sx=220, ix=190, pal=11 |
| 4600 | Strobe Mega | L+R | sx=238, ix=40, pal=0 |
| 5900 | Fade | L+R | sx=200, pal=0 |

### MZ16 — Yami Glitch
Efek glitch kegelapan: Glitter, Dissolve Rnd, Strobe patah-patah.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Glitter | L+R | sx=230, ix=230, pal=0 |
| 1400 | Dissolve Rnd | L+R | sx=235, ix=235 |
| 2800 | Strobe | L+R | sx=249, pal=0 |
| 3800 | Glitter | L+R | sx=245, ix=245, pal=0 |
| 5000 | Strobe Mega | L+R | sx=236, ix=50, pal=0 |
| 6100 | Fade | L+R | sx=200, pal=0 |

### MZ17 — Hanabi Rapid
Kembang api beruntun: Fireworks 1D, Popcorn, Sparkle lalu menyala.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Fireworks 1D | L+R |  |
| 1800 | Popcorn | L+R | sx=190, pal=35 |
| 3300 | Sparkle | L+R | sx=248, pal=0 |
| 4300 | Fireworks 1D | L+R | pal=11 |
| 5900 | Fade | L+R | sx=200, pal=0 |

### MZ18 — Taiko Beat
Dentuman taiko: kedip berirama dengan Bouncing Balls.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Mega | L+R | sx=232, ix=20, pal=0 |
| 1000 | Strobe Mega | L+R | sx=236, ix=20, pal=0 |
| 2000 | Bouncing Balls | L+R | pal=6 |
| 3600 | Strobe Mega | L+R | sx=240, ix=30, pal=0 |
| 4700 | Blink | L+R | sx=246, ix=120, pal=0 |
| 5900 | Fade | L+R | sx=200, pal=0 |

### MZ19 — Kaen Burst
Api membara: Chase Flash Rnd, Strobe oranye, Lightning hangat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Chase Flash Rnd | L+R | sx=246 |
| 1500 | Strobe | L+R | sx=248, pal=0 |
| 2800 | Lightning | L+R | sx=190, ix=190 |
| 4200 | Chase Flash | L+R | sx=248, pal=0 |
| 5200 | Strobe Mega | L+R | sx=238, ix=30, pal=0 |
| 6100 | Fade | L+R | sx=200, pal=0 |

### MZ20 — Shuriken Spin
Bintang lempar: Scan, Scan Dual, Two Dots dan Sinelon Dual cepat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Scan | L+R | sx=252, ix=90, pal=0 |
| 1300 | Scan Dual | L+R | sx=254, ix=90, pal=0 |
| 2600 | Two Dots | L+R | sx=245, ix=90, pal=0 |
| 3900 | Sinelon Dual | L+R | sx=215, ix=70, pal=0 |
| 5200 | Strobe | L+R | sx=250, pal=0 |
| 6100 | Fade | L+R | sx=200, pal=0 |

### MZ21 — Nitro Flicker
Kelip nitro: Colortwinkles cepat, Twinkle, Strobe pendek.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Colortwinkles | L+R | sx=220, ix=220, pal=6 |
| 1500 | Twinkle | L+R | sx=240, ix=240, pal=0 |
| 2800 | Strobe | L+R | sx=250, pal=0 |
| 3500 | Colortwinkles | L+R | sx=230, ix=230, pal=35 |
| 4800 | Strobe Mega | L+R | sx=240, ix=30, pal=0 |
| 6000 | Fade | L+R | sx=200, pal=0 |

### MZ22 — Dragon Roar
Raungan naga: ledakan PS Starburst berulang dan Strobe Mega.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | PS Starburst | L+R | pal=72 |
| 1500 | Strobe Mega | L+R | sx=236, ix=40, pal=0 |
| 2900 | PS Starburst | L+R | pal=35 |
| 4400 | Strobe Mega | L+R | sx=240, ix=30, pal=0 |
| 5600 | Fade | L+R | sx=200, pal=0 |

### MZ23 — Samurai Triple
Tiga kedipan tegas per kelompok dengan tiga warna aksen.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Mega | L+R | sx=234, ix=20, pal=0 |
| 1000 | Strobe Mega | L+R | sx=234, ix=20, pal=0 |
| 2000 | Strobe Mega | L+R | sx=234, ix=20, pal=0 |
| 3000 | Strobe Mega | L+R | sx=240, ix=20, pal=0 |
| 3900 | Strobe Mega | L+R | sx=240, ix=20, pal=0 |
| 4800 | Strobe Mega | L+R | sx=240, ix=20, pal=0 |
| 5800 | Fade | L+R | sx=200, pal=0 |

### MZ24 — Thunder Chase
Kilat dan kejaran pelangi cepat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Lightning | L+R | sx=180, ix=190 |
| 1500 | Chase Rainbow | L+R | sx=244, ix=90, pal=11 |
| 2900 | Strobe | L+R | sx=249, pal=0 |
| 3700 | Chase Rainbow | L+R | sx=250, ix=100, pal=6 |
| 5000 | Lightning | L+R | sx=200, ix=200 |
| 6100 | Fade | L+R | sx=200, pal=0 |

### MZ25 — Zenkai Flash
Puncak kedipan: gabungan Wipe, Strobe, Glitter, Chase Flash lalu ledakan terang.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Wipe | L+R | sx=254, ix=128, pal=0 |
| 500 | Strobe | L+R | sx=250, pal=0 |
| 1200 | Glitter | L+R | sx=235, ix=235, pal=0 |
| 2200 | Chase Flash | L+R | sx=248, pal=0 |
| 3200 | Strobe Mega | L+R | sx=238, ix=40, pal=0 |
| 4300 | Dissolve | L+R | sx=245, ix=245, pal=0 |
| 5200 | Solid | Kanan | pal=0 |
| 5200 | Solid | Kiri | pal=0 |
| 5320 | Solid | Kanan | pal=0 |
| 5320 | Solid | Kiri | pal=0 |
| 5440 | Solid | Kanan | pal=0 |
| 5440 | Solid | Kiri | pal=0 |
| 5560 | Solid | Kanan | pal=0 |
| 5560 | Solid | Kiri | pal=0 |
| 5900 | Fade | L+R | sx=200, pal=0 |


## Keindahan

### MZ26 — Sakura Drift
Kelopak sakura melayang: Colortwinkles, Meteor lembut, Shimmer muda.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Colortwinkles | L+R | sx=90, ix=110, pal=49 |
| 1900 | Meteor | L+R | sx=150, ix=160, pal=49 |
| 3600 | Shimmer | L+R | pal=49 |
| 5400 | Breathe | L+R | sx=120, pal=0 |

### MZ27 — Fuji Sunrise
Fajar di Gunung Fuji: Sunrise, Colorwaves, Fade hangat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sunrise | L+R | sx=174, pal=35 |
| 2350 | Colorwaves | L+R | pal=13 |
| 5000 | Fade | L+R | sx=120, pal=0 |

### MZ28 — Mizu Ripple
Riak air (mizu): Ripple, Pacifica, Lake.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Ripple | L+R | sx=110, ix=120, pal=9 |
| 2100 | Pacifica | L+R | pal=51 |
| 4200 | Lake | L+R | pal=9 |
| 6000 | Fade | L+R | sx=200, pal=0 |

### MZ29 — Hotaru Fireflies
Kunang-kunang malam: Twinkle kuning, Fairy, Breathe hangat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Twinkle | L+R | sx=80, ix=110, pal=0 |
| 2000 | Fairy | L+R | sx=110, ix=110, pal=0 |
| 3800 | Twinkle | L+R | sx=70, ix=160, pal=0 |
| 5200 | Breathe | L+R | sx=110, pal=0 |

### MZ30 — Tsuki Moonlight
Cahaya bulan: Breathe putih kebiruan, Shimmer es, Fade.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Breathe | L+R | sx=90, pal=0 |
| 2300 | Shimmer | L+R | pal=7 |
| 4600 | Fade | L+R | sx=110, pal=0 |
| 5600 | Fade | L+R | sx=110, pal=0 |

### MZ31 — Ginga Galaxy
Galaksi: Sinelon Dual, Twinklefox, Shimmer ungu.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sinelon Dual | L+R | sx=120, ix=120, pal=0 |
| 2100 | Twinklefox | L+R | sx=90, ix=130, pal=45 |
| 4000 | Shimmer | L+R | pal=46 |
| 5800 | Fade | L+R | sx=120, pal=0 |

### MZ32 — Nami Wave
Ombak berlapis: Wavesins, Pacifica, Colorwaves laut.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Wavesins | L+R | sx=90, pal=9 |
| 2200 | Pacifica | L+R | pal=51 |
| 4200 | Colorwaves | L+R | pal=22 |
| 6000 | Fade | L+R | sx=200, pal=0 |

### MZ33 — Kasumi Mist
Kabut pagi: Noise Pal pastel, Lake, Breathe.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Noise Pal | L+R | sx=70, ix=100, pal=20 |
| 2300 | Lake | L+R | pal=20 |
| 4300 | Breathe | L+R | sx=100, pal=0 |
| 5800 | Fade | L+R | sx=110, pal=0 |

### MZ34 — Akari Bloom
Cahaya lampion mekar: Wipe lambat, Fade, Shimmer hangat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Wipe | L+R | sx=160, ix=128, pal=0 |
| 2300 | Fade | L+R | sx=150, pal=0 |
| 4000 | Shimmer | L+R | pal=39 |
| 5900 | Fade | L+R | sx=130, pal=0 |

### MZ35 — Yuki Shimmer
Salju berkilau: Shimmer es, Twinkle putih, Sparkle.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Shimmer | L+R | pal=36 |
| 2200 | Twinkle | L+R | sx=90, ix=160, pal=0 |
| 4000 | Sparkle | L+R | sx=200, pal=0 |
| 5600 | Fade | L+R | sx=110, pal=0 |
| 6300 | Fade | L+R | sx=110, pal=0 |

### MZ36 — Koi Pond
Ikan koi berenang: Sinelon Dual oranye-putih, Ripple, Pacifica.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sinelon Dual | L+R | sx=130, ix=130, pal=0 |
| 2200 | Ripple | L+R | sx=120, ix=120, pal=44 |
| 4200 | Pacifica | L+R | pal=44 |
| 6000 | Fade | L+R | sx=200, pal=0 |

### MZ37 — Hanabi Garden
Taman kembang api: Fireworks 1D, PS Starburst, Colortwinkles.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Fireworks 1D | L+R | pal=72 |
| 2000 | PS Starburst | L+R | pal=72 |
| 3800 | Colortwinkles | L+R | sx=130, ix=150, pal=6 |
| 5600 | Fade | L+R | sx=170, pal=0 |

### MZ38 — Tanabata Stars
Festival bintang: Twinklefox, Colortwinkles, Shimmer.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Twinklefox | L+R | sx=90, ix=130, pal=46 |
| 2000 | Colortwinkles | L+R | sx=90, ix=130, pal=45 |
| 3900 | Shimmer | L+R | pal=45 |
| 5700 | Fade | L+R | sx=120, pal=0 |

### MZ39 — Aurora Kaze
Aurora tertiup angin: Aurora, Colorwaves aurora, Fade.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Aurora | L+R | pal=50 |
| 2800 | Colorwaves | L+R | pal=55 |
| 4500 | Shimmer | L+R | pal=50 |
| 6000 | Fade | L+R | sx=200, pal=0 |

### MZ40 — Ame Neon
Hujan neon kota: Drip, Bouncing Balls, Ripple Rainbow.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Drip | L+R | pal=6 |
| 2100 | Bouncing Balls | L+R | pal=6 |
| 3900 | Ripple Rainbow | L+R | sx=120, ix=130 |
| 5800 | Fade | L+R | sx=200, pal=0 |

### MZ41 — Kaze Whisper
Bisikan angin: Sinelon lembut, Wavesins, Fade.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sinelon | L+R | sx=110, ix=130, pal=0 |
| 2100 | Wavesins | L+R | sx=70, pal=20 |
| 4100 | Breathe | L+R | sx=100, pal=0 |
| 5600 | Fade | L+R | sx=120, pal=0 |

### MZ42 — Hoshi Cascade
Hujan bintang: Meteor, PS Spray 1D, Twinkle.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Meteor | L+R | sx=140, ix=170, pal=0 |
| 2100 | PS Spray 1D | L+R | pal=72 |
| 4000 | Twinkle | L+R | sx=110, ix=170, pal=0 |
| 5700 | Fade | L+R | sx=200, pal=0 |

### MZ43 — Asagiri Dawn
Fajar berkabut: Sunrise, Pacifica lembut, Fade.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Sunrise | L+R | sx=163, pal=21 |
| 2700 | Pacifica | L+R | pal=21 |
| 5200 | Fade | L+R | sx=120, pal=0 |

### MZ44 — Kyoto Lantern
Lentera Kyoto: Candle, Fire 2012, Breathe hangat.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Candle | L+R | pal=0 |
| 2100 | Fire 2012 | L+R | pal=35 |
| 4300 | Breathe | L+R | sx=100, pal=0 |
| 5700 | Fade | L+R | sx=110, pal=0 |

### MZ45 — Mizuma Eternal
Tanda tangan Mizuma yang abadi: Colorwaves emas, Shimmer, Fade.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Colorwaves | L+R | pal=35 |
| 2300 | Shimmer | L+R | pal=35 |
| 4300 | Meteor | L+R | sx=170, ix=150, pal=0 |
| 5600 | Fade | L+R | sx=130, pal=0 |


## Campuran

### MZ46 — Raijin Bloom
Kilat Strobe yang mekar menjadi Shimmer sakura.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Mega | L+R | sx=236, ix=30, pal=0 |
| 1300 | Strobe | L+R | sx=250, pal=0 |
| 2600 | Shimmer | L+R | pal=49 |
| 4900 | Breathe | L+R | sx=120, pal=0 |

### MZ47 — Hanabi Rush
Kembang api cepat yang berakhir lembut.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Fireworks 1D | L+R |  |
| 1700 | Strobe Mega | L+R | sx=238, ix=30, pal=0 |
| 2900 | Colortwinkles | L+R | sx=180, ix=190, pal=72 |
| 5000 | Fade | L+R | sx=160, pal=0 |

### MZ48 — Oni Prism
Strobe Rainbow menuju Sinelon Rainbow yang indah.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Rainbow | L+R | sx=247 |
| 1300 | Blink Rainbow | L+R | sx=244, ix=120 |
| 2500 | Sinelon Rainbow | L+R | sx=190, ix=120, pal=11 |
| 4400 | Ripple Rainbow | L+R | sx=150, ix=120 |
| 5900 | Fade | L+R | sx=200, pal=0 |

### MZ49 — Kitsune Aurora
Kejaran rubah lalu aurora tenang.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Chase 2 | L+R | sx=246, ix=80, pal=0 |
| 1400 | Scan Dual | L+R | sx=252, ix=80, pal=0 |
| 2800 | Aurora | L+R | pal=50 |
| 5800 | Fade | L+R | sx=200, pal=0 |

### MZ50 — Katana Dawn
Tebasan cepat yang berakhir dalam fajar.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Wipe | L+R | sx=254, ix=128, pal=0 |
| 480 | Sweep | L+R | arah dibalik, sx=254, ix=128, pal=0 |
| 960 | Wipe | L+R | sx=254, ix=128, pal=0 |
| 1440 | Sweep | L+R | arah dibalik, sx=254, ix=128, pal=0 |
| 2000 | Sunrise | L+R | sx=174, pal=35 |
| 4600 | Fade | L+R | sx=150, pal=0 |

### MZ51 — Tsunami Spark
Riak dan percikan yang bermuara ke Pacifica.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Ripple | L+R | sx=200, ix=160, pal=9 |
| 1500 | Sparkle | L+R | sx=245, pal=0 |
| 2800 | Pacifica | L+R | pal=51 |
| 5600 | Fade | L+R | sx=200, pal=0 |

### MZ52 — Yokai Glow
Kilatan yokai yang berubah menjadi cahaya bernapas.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Glitter | L+R | sx=235, ix=235, pal=0 |
| 1400 | Dissolve | L+R | sx=240, ix=240, pal=0 |
| 2800 | Breathe | L+R | sx=120, pal=0 |
| 4300 | Shimmer | L+R | pal=69 |
| 5800 | Fade | L+R | sx=200, pal=0 |

### MZ53 — Shogun Pulse
Denyut shogun, ledakan PS Starburst lalu Colorwaves megah.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Heartbeat | L+R | sx=200, ix=180, pal=0 |
| 1800 | PS Starburst | L+R | pal=35 |
| 3300 | Colorwaves | L+R | pal=35 |
| 5100 | Shimmer | L+R | pal=35 |

### MZ54 — Sakura Storm
Badai sakura: Dissolve, Glitter, Colortwinkles, Shimmer.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Dissolve | L+R | sx=238, ix=235, pal=0 |
| 1400 | Glitter | L+R | sx=225, ix=225, pal=0 |
| 2800 | Colortwinkles | L+R | sx=150, ix=170, pal=49 |
| 4400 | Shimmer | L+R | pal=49 |
| 5900 | Fade | L+R | sx=200, pal=0 |

### MZ55 — Mizuma Grand Finale
Final megah: Strobe, Fireworks 1D, Sinelon Rainbow, Shimmer, Fade.

| t (ms) | Efek | Sisi | Catatan |
|---:|---|:-:|---|
| 0 | Strobe Mega | L+R | sx=236, ix=30, pal=0 |
| 1300 | Fireworks 1D | L+R |  |
| 2800 | Sinelon Rainbow | L+R | sx=195, ix=120, pal=11 |
| 4400 | Shimmer | L+R | pal=11 |
| 5800 | Fade | L+R | sx=200, pal=0 |
