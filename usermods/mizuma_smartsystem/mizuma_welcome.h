// =====================================================================================================
// MIZUMA WELCOMING ENGINE — 45 mode (W01..W45)
// -----------------------------------------------------------------------------------------------------
// - Murni integer (Q12), tanpa float: aman & cepat untuk ESP32-C3 (RISC-V tanpa FPU).
// - Satu fungsi piksel dipakai 45 mode -> hemat flash dibanding 45 fungsi efek terpisah.
// - Warna TIDAK di-hardcode: mesin hanya menghasilkan (v = kecerahan, pos = posisi palette).
//   Warna akhir = palette segmen (palette "Default" => warna Fx/primary), latar = warna Bg (slot 2).
// - Konvensi arah: indeks 0 = UJUNG LUAR alis, indeks terakhir = UJUNG DALAM.
//   Jika kabel Kiri/Kanan terbalik, cukup aktifkan "Reverse" pada segmen terkait (UI WLED biasa).
// - Slider: Speed = durasi satu siklus (sx=128 ~ 6,9 dtk, cocok dg welcomeDur 7 dtk), Lebar = ekor/lebar/kepadatan.
// - Siklus selalu mulai dari t=0 saat efek dimulai (SEGENV.call==0) -> kedua sisi sinkron.
// =====================================================================================================
#pragma once
#include <stdint.h>

#define MZW_COUNT 45
#define MZW_U     4096                       // 1.0 dalam Q12
#define MZW_F(x)  ((int)((x) * 4096.0 + 0.5)) // konstanta waktu/posisi (dievaluasi compile-time)

struct MzwPx  { uint8_t v; uint8_t pos; };
struct MzwCtx { int mode; int n; int u; int wd; int soft; int env; };

// ---------- helper integer ----------
static inline int mzw_cl(int x) { return x < 0 ? 0 : (x > 255 ? 255 : x); }
static inline int mzw_ramp(int x, int a, int b) {              // x di [a,b] -> 0..255
  if (b <= a) return x >= a ? 255 : 0;
  return mzw_cl(((x - a) * 255) / (b - a));
}
static inline int mzw_tri(int x) {                              // fasa Q12 -> 0..255..0
  x &= 4095; return x < 2048 ? (x >> 3) : ((4095 - x) >> 3);
}
static inline int mzw_smooth(int s) { return (s * s * (765 - 2 * s)) / 65025; }
static inline int mzw_abs(int x) { return x < 0 ? -x : x; }
static inline int mzw_mx(int a, int b) { return a > b ? a : b; }
static inline uint8_t mzw_h(uint32_t x) {
  x ^= x >> 15; x *= 0x2c1b3c6dU; x ^= x >> 12; x *= 0x297a2d39U; x ^= x >> 15; return (uint8_t)x;
}
// komet linear: kepala di 'head', ekor sepanjang 'tail' di belakang (nilai x lebih kecil)
static inline int mzw_comet(int head, int x, int tail) {
  int d = head - x; if (d < 0 || d > tail) return 0;
  int v = 255 - (d * 255) / (tail ? tail : 1); return (v * v) / 255;
}
// komet melingkar (wrap)
static inline int mzw_cometRing(int head, int x, int tail) {
  int d = (head - x) & 4095; if (d > tail) return 0;
  int v = 255 - (d * 255) / (tail ? tail : 1); return (v * v) / 255;
}
// isi bertahap: 'front' bergerak dari -soft .. U+soft saat u melewati [a,b]
static inline int mzw_fillV(int x, int u, int a, int b, int soft) {
  int uu = u < a ? a : (u > b ? b : u);
  int front = -soft + (((uu - a) * (MZW_U + 2 * soft)) / (b > a ? (b - a) : 1));
  return mzw_ramp(front - x, 0, soft);
}
static inline int mzw_pulse(int b, int a, int c) {             // denyut halus pada jendela [a,c]
  if (b < a || b > c) return 0;
  return mzw_smooth(mzw_tri(((b - a) * 4095) / (c - a)));
}

// ---------- konteks per-frame ----------
static inline uint32_t mzwPeriod(uint8_t speed) {               // ms per siklus
  int T = 12000 - (int)speed * 40; return (uint32_t)(T < 1500 ? 1500 : T);
}
static inline void mzwInit(MzwCtx &c, int mode, int n, uint32_t t, uint32_t T, uint8_t intensity) {
  c.mode = mode; c.n = n;
  c.u    = (int)(((uint64_t)t * 4095ULL) / (T ? T : 1)); if (c.u > 4095) c.u = 4095;
  c.wd   = 400 + ((int)intensity * 1600) / 255;                 // 0.10 .. 0.49
  c.soft = 120 + c.wd / 8;
  int fin = mzw_ramp(c.u, 0, MZW_F(0.03)), fout = 255 - mzw_ramp(c.u, MZW_F(0.94), MZW_U);
  c.env  = (fin * fout) / 255;                                  // fade-in 3% & fade-out 6%
}

// ---------- 1 piksel ----------
static inline MzwPx mzwPixel(const MzwCtx &c, int i) {
  const int u = c.u, wd = c.wd, soft = c.soft;
  const int x  = c.n > 1 ? (int)(((uint32_t)i * 4096U) / (uint32_t)(c.n - 1)) : 0;   // 0 luar .. 4096 dalam
  const int xr = MZW_U - x;                                                           // 0 dalam .. 4096 luar
  const int q  = mzw_abs(2 * x - MZW_U);                                              // 0 tengah .. 4096 ujung
  const int qr = MZW_U - q;
  int v = 0, pos = x >> 4;
  const int tail = wd < 500 ? 500 : wd;

  switch (c.mode) {
    // ===== A. ISI / HAPUS (1-9) =====
    case 1:  v = mzw_fillV(x,  u, 0, MZW_F(0.55), soft); break;
    case 2:  v = mzw_fillV(xr, u, 0, MZW_F(0.55), soft); break;
    case 3:  v = mzw_fillV(q,  u, 0, MZW_F(0.55), soft); break;
    case 4:  v = mzw_fillV(qr, u, 0, MZW_F(0.55), soft); break;
    case 5:  v = 255 - mzw_fillV(x,  u, MZW_F(0.15), MZW_F(0.70), soft); break;
    case 6:  v = 255 - mzw_fillV(xr, u, MZW_F(0.15), MZW_F(0.70), soft); break;
    case 7:  v = (u < MZW_F(0.5)) ? mzw_fillV(x, u, 0, MZW_F(0.45), soft)
                                  : 255 - mzw_fillV(x, u, MZW_F(0.55), MZW_F(0.95), soft); break;
    case 8: case 9: {
      int B = (c.mode == 8) ? 6 : 12, blk = (x * B) >> 12; if (blk >= B) blk = B - 1;
      int stage = (u * B) / MZW_F(0.60);                                           // 0..B saat u=0.60
      // blok terbaru terang penuh, blok lama redup; setelah lengkap semuanya menyala penuh (membedakan dari W01)
      v = (blk >= stage) ? 0 : (blk == stage - 1 && u < MZW_F(0.60) ? 255 : 150 + (mzw_ramp(u, MZW_F(0.60), MZW_F(0.72)) * 105) / 255);
      break; }

    // ===== B. KOMET / PELARI (10-20) =====
    case 10: { int h = ((u * 2) & 4095) * (MZW_U + tail) / MZW_U; v = mzw_comet(h, x,  tail); } break;
    case 11: { int h = ((u * 2) & 4095) * (MZW_U + tail) / MZW_U; v = mzw_comet(h, xr, tail); } break;
    case 12: {                                                   // Knight Rider
      int s = (u * 3) & 4095, p = s < 2048 ? s * 2 : (4095 - s) * 2;
      int d = (s < 2048) ? (p - x) : (x - p);
      int tl = tail + 200;
      if (d >= 0 && d <= tl) { int t = 255 - (d * 255) / tl; v = (t * t) / 255; }
      int hd = 255 - mzw_ramp(mzw_abs(x - p), 0, 90); v = mzw_mx(v, hd); } break;
    case 13: { int h = (u * 2) & 4095; v = mzw_mx(mzw_cometRing(h, x, tail < 1400 ? tail : 1400),
                                                   mzw_cometRing(h + 2048, x, tail < 1400 ? tail : 1400)); } break;
    case 14: { int h = (u * 2) & 4095, tl = tail < 900 ? tail : 900;
               v = mzw_mx(mzw_cometRing(h, x, tl), mzw_mx(mzw_cometRing(h + 1365, x, tl), mzw_cometRing(h + 2731, x, tl))); } break;
    case 15: { int h = ((u * 2) & 4095) * (MZW_U + tail) / MZW_U; v = mzw_comet(h, q,  tail); } break;
    case 16: { int h = ((u * 2) & 4095) * (MZW_U + tail) / MZW_U; v = mzw_comet(h, qr, tail); } break;
    case 17: v = mzw_ramp(mzw_tri(x * 6 - u * 8), 60, 196); break;               // air mengalir
    case 18: v = mzw_ramp(mzw_tri(x * 3 - u * 4), 90, 170); break;               // dash lebar
    case 19: v = mzw_ramp(mzw_tri(x * 6 + u * 8), 60, 196); break;               // mundur
    case 20: {                                                                    // meteor panjang
      int tl = tail + 1000, h = (u & 4095) * (MZW_U + tl) / MZW_U;
      int m = mzw_comet(h, x, tl); uint8_t r = mzw_h((uint32_t)i + (uint32_t)(u >> 5) * 17U);
      v = (m * (200 + (r % 56))) / 255; } break;

    // ===== C. NAPAS / KEDIP (21-26) =====
    case 21: v = 30 + (mzw_smooth(mzw_tri(u * 2)) * 225) / 255; break;
    case 22: v = 30 + (mzw_smooth(mzw_tri(u * 5)) * 225) / 255; break;
    case 23: v = mzw_smooth(mzw_ramp(u, 0, MZW_F(0.45))); break;
    case 24: { int b = (u * 2) & 4095;                                            // denyut jantung
               v = mzw_mx(20, mzw_mx(mzw_pulse(b, 0, MZW_F(0.16)), (mzw_pulse(b, MZW_F(0.20), MZW_F(0.36)) * 180) / 255)); } break;
    case 25: v = (u < MZW_F(0.42)) ? ((u % MZW_F(0.14)) < MZW_F(0.07) ? 255 : 0) : 255; break;
    case 26: {                                                                    // blok berkedip berurutan
      int blk = (x * 8) >> 12; if (blk > 7) blk = 7;
      int tt = u - blk * MZW_F(0.075);
      if (tt < 0) v = 0;
      else if (tt < MZW_F(0.08)) v = (tt < MZW_F(0.03) || (tt >= MZW_F(0.045) && tt < MZW_F(0.075))) ? 255 : 0;
      else v = 110; } break;                                     // lewat kedipan -> menetap redup (beda dari W01)

    // ===== D. ALIRAN PALETTE & GELOMBANG (27-32) =====
    case 27: pos = (x >> 4) - ((u >> 4) * 2); goto flow;
    case 28: pos = (x >> 4) + ((u >> 4) * 2); goto flow;
    case 29: pos = (x >> 5) + (mzw_tri(u * 2) >> 1); goto flow;
    case 30: pos = ((x * 3) >> 4) - ((u >> 4) * 3); goto flow;
    flow:    pos &= 255; v = 140 + (mzw_tri(pos << 4) * 115) / 255; break;
    case 31: v = 40 + (mzw_smooth(mzw_tri(x * 2 - u * 3)) * 215) / 255; break;
    case 32: { int a = mzw_smooth(mzw_tri(x * 2 - u * 3)), b = mzw_smooth(mzw_tri(x * 3 + u * 2));
               v = 30 + (((a * b) / 255) * 225) / 255; } break;

    // ===== E. KERLIP / PERCIK (33-37) =====
    case 33: { int h = mzw_h((uint32_t)i * 7U + 3U); v = 25 + (mzw_smooth(mzw_tri(u * 3 + h * 16)) * 230) / 255; pos = h; } break;
    case 34: { uint32_t step = (uint32_t)((u * 80) >> 12); uint8_t h = mzw_h((uint32_t)i * 131U + step * 977U);
               v = (h > 255 - (wd >> 5)) ? 255 : 0; pos = mzw_h((uint32_t)i * 17U + step * 31U); } break;
    case 35: {                                                                    // bintang jatuh
      for (int k = 0; k < 4; k++) {
        int lp = 1 + (mzw_h((uint32_t)k * 91U) & 1), st = mzw_h((uint32_t)k * 37U + 5U) * 16;
        int ph = (u * lp + st) & 4095, tl = tail / 2 + 250, h = ph * (MZW_U + tl) / MZW_U;
        v = mzw_mx(v, mzw_comet(h, (k & 1) ? xr : x, tl)); } } break;
    case 36: { int f = mzw_fillV(x, u, 0, MZW_F(0.55), soft); v = (f * 170) / 255;
               if (f > 200 && mzw_h((uint32_t)i + (uint32_t)(u >> 5) * 53U) > 240) v = 255; } break;
    case 37: { int h = mzw_h((uint32_t)i * 13U + 7U), s = mzw_tri(u * 5 + h * 16); v = (((s * s) / 255) * s) / 255; pos = h; } break;

    // ===== F. BLOK & POLA (38-45) =====
    case 38: { int blk = (x * 8) >> 12; if (blk > 7) blk = 7; int step = (u * 8) >> 12; v = ((blk + step) & 1) ? 255 : 0; } break;
    case 39: { int blk = (x * 8) >> 12; if (blk > 7) blk = 7;
               int tk = MZW_F(0.04) + blk * MZW_F(0.07); v = (u >= tk) ? 40 + (blk * 215) / 7 : 0; } break;
    case 40: { int blk = (x * 8) >> 12; if (blk > 7) blk = 7;
               int tk = blk * MZW_F(0.055), td = MZW_F(0.55) + blk * MZW_F(0.055);
               v = (u >= tk && u < td) ? 40 + (blk * 215) / 7 : 0; } break;
    case 41: { int blk = (x * 12) >> 12; if (blk > 11) blk = 11;
               int on = blk * MZW_F(0.04), off = MZW_F(0.52) + blk * MZW_F(0.04);
               if (u >= on && u < off) v = ((u - on) < MZW_F(0.03)) ? 255 : 200; } break;
    case 42: { int bw = wd < 700 ? 700 : wd, s = (u * 3) & 4095, cc = s < 2048 ? s * 2 : (4095 - s) * 2;
               v = mzw_smooth(mzw_ramp((bw >> 1) - mzw_abs(x - cc), 0, bw >> 1)); } break;
    case 43: { int th = (u < 2048) ? (u * 2) : ((4095 - u) * 2);
               v = mzw_ramp(q - th, 0, 160); } break;
    case 44: { int th = (u < 2048) ? (u * 2) : ((4095 - u) * 2);
               v = mzw_ramp(th - q, 0, 160); } break;
    case 45: {                                                                    // FINALE
      if (u < MZW_F(0.40))      v = mzw_fillV(x, u, 0, MZW_F(0.40), 200);
      else if (u < MZW_F(0.52)) v = 255;
      else if (u < MZW_F(0.72)) v = ((u - MZW_F(0.52)) % MZW_F(0.10)) < MZW_F(0.05) ? 255 : 90;
      else                      v = 140 + (mzw_smooth(mzw_tri(((u - MZW_F(0.72)) * 4) & 4095)) * 115) / 255; } break;
    default: v = 0; break;
  }
  v = (mzw_cl(v) * c.env) / 255;
  MzwPx o; o.v = (uint8_t)v; o.pos = (uint8_t)(pos & 255); return o;
}

// =====================================================================================================
// Bagian WLED (di-skip saat tes di PC dengan -DMZW_HOST)
// =====================================================================================================
#if !defined(MZW_HOST) && defined(SEGENV)

static void mzwRun(uint8_t mode) {
  const uint16_t n = SEGLEN;
  if (SEGENV.call == 0) SEGENV.step = strip.now;                  // titik nol animasi
  const uint32_t T = mzwPeriod(SEGMENT.speed);
  const uint32_t t = (uint32_t)(strip.now - SEGENV.step) % T;     // siklus berulang (preview di UI)
  MzwCtx c; mzwInit(c, mode, (int)n, t, T, SEGMENT.intensity);
  const uint32_t bg = SEGCOLOR(1);
  for (uint16_t i = 0; i < n; i++) {
    const MzwPx p = mzwPixel(c, i);
    const uint32_t fg = SEGMENT.color_from_palette(p.pos, false, true, 0, 255);
    SEGMENT.setPixelColor(i, p.v == 255 ? fg : (p.v == 0 ? bg : color_blend(bg, fg, p.v)));
  }
}
// WLED 0.15.x : efek bertipe  uint16_t fn()  (mengembalikan FRAMETIME)
// WLED main/0.16: efek bertipe  void fn()
// mode_ptr bersifat private di WS2812FX, jadi tipe yang diterima addEffect dipilih otomatis lewat SFINAE.
template <uint8_t M> static void     mzwFnV() { mzwRun(M); }
template <uint8_t M> static uint16_t mzwFnU() { mzwRun(M); return FRAMETIME; }
template <uint8_t M, typename S> static auto mzwAdd(S &s, const char *d, int)  -> decltype(s.addEffect(255, &mzwFnV<M>, d)) { return s.addEffect(255, &mzwFnV<M>, d); }
template <uint8_t M, typename S> static auto mzwAdd(S &s, const char *d, long) -> decltype(s.addEffect(255, &mzwFnU<M>, d)) { return s.addEffect(255, &mzwFnU<M>, d); }

// Daftar nama (ASCII saja; hindari '@' ';' ',' pada nama). Format data: Nama@slider;warna;palette;flag;default
#define MZW_LIST(X) \
 X(1,"W01 Isi Luar-Dalam")      X(2,"W02 Isi Dalam-Luar")      X(3,"W03 Isi Tengah-Ujung")    X(4,"W04 Isi Ujung-Tengah") \
 X(5,"W05 Hapus Luar-Dalam")    X(6,"W06 Hapus Dalam-Luar")    X(7,"W07 Isi lalu Hapus")      X(8,"W08 Isi Bertahap 6") \
 X(9,"W09 Isi Bertahap 12")     X(10,"W10 Peluru Luar-Dalam")  X(11,"W11 Peluru Dalam-Luar")  X(12,"W12 Knight Rider") \
 X(13,"W13 Dua Peluru")         X(14,"W14 Tiga Peluru")        X(15,"W15 Peluru Tengah-Ujung") X(16,"W16 Peluru Ujung-Tengah") \
 X(17,"W17 Air Mengalir")       X(18,"W18 Dash Lebar")         X(19,"W19 Air Mundur")         X(20,"W20 Meteor Panjang") \
 X(21,"W21 Napas Lambat")       X(22,"W22 Napas Cepat")        X(23,"W23 Fade In Tahan")      X(24,"W24 Denyut Jantung") \
 X(25,"W25 Kedip 3x Tahan")     X(26,"W26 Kedip Bertahap")     X(27,"W27 Aliran Keluar")      X(28,"W28 Aliran Masuk") \
 X(29,"W29 Aliran Pantul")      X(30,"W30 Aliran Rapat")       X(31,"W31 Gelombang")          X(32,"W32 Gelombang Lawan") \
 X(33,"W33 Kerlip Lembut")      X(34,"W34 Percik Cepat")       X(35,"W35 Bintang Jatuh")      X(36,"W36 Isi Berkilau") \
 X(37,"W37 Kerlip Tajam")       X(38,"W38 Blok Bergantian")    X(39,"W39 Tangga Naik")        X(40,"W40 Tangga Naik-Turun") \
 X(41,"W41 Domino")             X(42,"W42 Scan Lebar")         X(43,"W43 Tutup Ujung")        X(44,"W44 Ekspansi Tengah") \
 X(45,"W45 Finale Welcome")

#define MZW_DATA(num, name) static const char mzw_d##num[] PROGMEM = name "@!,Lebar;!,!;!;1;sx=128,ix=128";
MZW_LIST(MZW_DATA)
#undef MZW_DATA

// Cari id efek dari awalan nama (mis. "W45 "). Dipakai untuk default slot Welcoming. -1 jika tidak ada.
static int mizumaFindEffectId(const char* prefix) {
  const size_t L = strlen(prefix);
  for (uint16_t i = 1; i < strip.getModeCount(); i++) {
    const char* d = strip.getModeData(i);
    if (d && strncmp_P(prefix, d, L) == 0) return (int)i;
  }
  return -1;
}

// Panggil SEKALI di setup(), SEBELUM preset boot diterapkan. Mengembalikan jumlah efek yang berhasil didaftarkan.
// PENTING: WLED membatasi total efek 255. Di WLED main (221 efek bawaan) hanya ~39 slot kosong bila 2D aktif ->
// tambahkan  -D WLED_DISABLE_2D  di build_flags agar ke-45 mode muat. Cek hasilnya di /mizuma/debug (welcomeFx harus 45).
static uint8_t mizumaRegisterWelcomeEffects() {
  uint8_t ok = 0;
  #define MZW_REG(num, name) if (mzwAdd<num>(strip, mzw_d##num, 0) != 255) ok++;
  MZW_LIST(MZW_REG)
  #undef MZW_REG
  return ok;
}
#endif  // WLED
