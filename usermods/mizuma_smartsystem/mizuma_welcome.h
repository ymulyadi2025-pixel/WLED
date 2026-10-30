// =====================================================================================================
// MIZUMA WELCOMING ENGINE — 50 CHOREOGRAPHED MODES (W01..W50)
// -----------------------------------------------------------------------------------------------------
// Drop-in replacement for mizuma_welcome.h. Public interface kept compatible:
//   MZW_COUNT, mizumaRegisterWelcomeEffects(), mizumaFindEffectId(), mzwModeFn<M>()
//   W01..W45 names/prefixes remain stable so the existing .cpp does not need editing.
//
// DESIGN:
// - Every mode is a multi-stage welcome animation, not a single continuously-running effect.
// - Typical sequence: ignition / approach -> moving beams or comets -> accent / burst -> signature hold.
// - Time is normalized to 0..4095. Speed sets total sequence duration; default speed 128 ~= 6.9 s.
// - Pixel 0 = outer eyebrow tip, last pixel = inner tip. Reverse the WLED segment if wiring is opposite.
// - Foreground color comes from the WLED segment palette; background from segment color slot 2.
// - Integer pixel math; no floating-point work in the per-pixel animation loop.
// =====================================================================================================
#pragma once
#include <stdint.h>
#include <string.h>

#define MZW_COUNT 50
#define MZW_U     4096
#define MZW_F(x)  ((int)((x) * 4096.0 + 0.5))

struct MzwPx  { uint8_t v; uint8_t pos; };
struct MzwCtx { int mode; int n; int u; int wd; int soft; int env; };

// ---------- integer helpers ----------
static inline int mzw_cl(int x) { return x < 0 ? 0 : (x > 255 ? 255 : x); }
static inline int mzw_mx(int a, int b) { return a > b ? a : b; }
static inline int mzw_mn(int a, int b) { return a < b ? a : b; }
static inline int mzw_abs(int x) { return x < 0 ? -x : x; }
static inline int mzw_ramp(int x, int a, int b) {
  if (b <= a) return x >= a ? 255 : 0;
  return mzw_cl(((x - a) * 255) / (b - a));
}
static inline int mzw_smooth(int s) { s = mzw_cl(s); return (s * s * (765 - 2 * s)) / 65025; }
static inline int mzw_tri(int x) {
  x &= 4095;
  return x < 2048 ? (x >> 3) : ((4095 - x) >> 3);
}
static inline uint8_t mzw_h(uint32_t x) {
  x ^= x >> 15; x *= 0x2c1b3c6dU; x ^= x >> 12; x *= 0x297a2d39U; x ^= x >> 15;
  return (uint8_t)x;
}
static inline int mzw_peak(int d, int width) {
  if (width < 1 || d >= width) return 0;
  return mzw_smooth(255 - (d * 255) / width);
}
static inline int mzw_phase(int u, int a, int b) { // local phase 0..4095, or -1 outside the stage
  if (u < a || u >= b || b <= a) return -1;
  return (int)(((uint32_t)(u - a) * 4095U) / (uint32_t)(b - a));
}
static inline int mzw_flash(int p, int count, int duty) {
  if (p < 0 || count < 1) return 0;
  int period = 4096 / count;
  return (p % period) < duty ? 255 : 0;
}
static inline int mzw_breathe(int p, int loops, int low) {
  if (p < 0) return 0;
  return low + ((mzw_smooth(mzw_tri(p * loops)) * (255 - low)) / 255);
}
static inline int mzw_fillV(int x, int p, int soft) {
  int front = -soft + (((p < 0 ? 0 : p) * (MZW_U + 2 * soft)) / MZW_U);
  return mzw_ramp(front - x, 0, soft);
}
static inline int mzw_comet(int head, int x, int tail) {
  int d = head - x;
  if (d < 0 || d > tail) return 0;
  int a = 255 - (d * 255) / (tail ? tail : 1);
  return (a * a) / 255;
}
static inline int mzw_head(int p, int tail) { // traverse fully through the strip, starting/ending off-strip
  return -tail + (int)(((int64_t)(p < 0 ? 0 : p) * (MZW_U + 2 * tail)) / MZW_U);
}
static inline int mzw_sweep(int x, int p, int tail, int reverse) {
  int coord = reverse ? (MZW_U - x) : x;
  return mzw_comet(mzw_head(p, tail), coord, tail);
}
static inline int mzw_dualSweep(int x, int p, int tail) {
  return mzw_mx(mzw_sweep(x, p, tail, 0), mzw_sweep(x, p, tail, 1));
}
static inline int mzw_centerSweep(int q, int p, int tail) {
  return mzw_comet(mzw_head(p, tail), q, tail);
}
static inline int mzw_bounce(int p, int loops) {
  int z = (p * loops) & 4095;
  return z < 2048 ? z * 2 : (4095 - z) * 2;
}
static inline int mzw_scanner(int x, int p, int loops, int width) {
  int head = mzw_bounce(p, loops);
  int beam = mzw_peak(mzw_abs(x - head), width);
  int tail = (x < head) ? (head - x) : (x - head);
  int trail = tail < width * 3 ? ((width * 3 - tail) * 110) / (width * 3) : 0;
  return mzw_mx(beam, trail);
}
static inline int mzw_ring(int d, int width) { return mzw_peak(d, width); }
static inline int mzw_ripple(int q, int p, int width, int reverse) {
  int front = reverse ? (4095 - p) : p;
  return mzw_ring(mzw_abs(q - front), width);
}
static inline int mzw_wave(int x, int p, int scale, int drift, int lo, int hi) {
  int w = mzw_tri(x * scale - p * drift);
  return lo + (mzw_smooth(w) * (hi - lo)) / 255;
}
static inline int mzw_blocks(int x, int p, int count, int reverse, int tailBlocks) {
  int blk = (x * count) >> 12;
  if (blk >= count) blk = count - 1;
  if (reverse) blk = count - 1 - blk;
  int step = (p * count) >> 12;
  int d = step - blk;
  if (d < 0) return 0;
  if (d == 0) return 255;
  if (d <= tailBlocks) return (255 * (tailBlocks + 1 - d)) / (tailBlocks + 1);
  return 0;
}
static inline int mzw_glitter(int i, int p, int density, uint32_t seed) {
  uint32_t tick = (uint32_t)(p >> 5);
  uint8_t h = mzw_h((uint32_t)i * 131U + tick * 977U + seed);
  return h > (uint8_t)(255 - density) ? 255 : (h > 210 ? 55 : 0);
}
static inline int mzw_pulsePair(int p) {
  int a = mzw_peak(mzw_abs(p - 420), 330);
  int b = (p > 700 && p < 1750) ? (mzw_peak(mzw_abs(p - 1180), 440) * 205) / 255 : 0;
  return mzw_mx(a, b);
}


// ---------- extra choreography primitives inspired by WLED 1D / PS effects ----------
// These are native integer renderers: no dependency on WLED's Particle System user feature.
static inline int mzw_sine255(int phase) {
  int x = phase & 4095;
  if (x > 2047) x = 4095 - x;
  return (x * (2048 - x) * 255) / 1048576;
}
static inline int mzw_waveSins(int x, int p, int scale, int drift, int lo, int hi) {
  int a = mzw_sine255(x * scale - p * drift);
  int b = mzw_sine255(x * (scale + 1) + p * (drift - 1));
  int mix = (a * 3 + b) / 4;
  return lo + (mix * (hi - lo)) / 255;
}
static inline int mzw_bouncingBalls(int x, int p, int count, int width) {
  int out = 0;
  if (count < 1) count = 1;
  if (count > 4) count = 4;
  for (int k = 0; k < count; k++) {
    int ph = (p + (k * 4096) / count) & 4095;
    int head = mzw_bounce(ph, 1);
    out = mzw_mx(out, mzw_peak(mzw_abs(x - head), width));
  }
  return out;
}
static inline int mzw_chaseRainbow(int x, int p, int tail, int reverse, int &pos) {
  pos = ((x >> 4) + (reverse ? -(p >> 2) : (p >> 2))) & 255;
  return mzw_sweep(x, p, tail, reverse);
}
static inline int mzw_colorTwinkles(int i, int p, int density, uint32_t seed, int &hue) {
  uint32_t tick = (uint32_t)(p >> 5);
  uint8_t r = mzw_h((uint32_t)i * 131U + tick * 977U + seed);
  hue = mzw_h((uint32_t)i * 37U + tick * 71U + seed * 3U);
  if (r < (uint8_t)(255 - density)) return 0;
  int ph = (p * 2 + (int)mzw_h((uint32_t)i * 19U + seed) * 16) & 4095;
  return 45 + (mzw_smooth(mzw_tri(ph)) * 210) / 255;
}
static inline int mzw_dissolve(int i, int p, uint32_t seed, int reverse) {
  uint8_t rank = mzw_h((uint32_t)i * 197U + seed * 29U);
  if (reverse) rank = (uint8_t)(255 - rank);
  int progress = p >> 4;
  int d = progress - (int)rank;
  if (d >= 0) return 255;
  if (d > -28) return ((28 + d) * 150) / 28;
  return 0;
}
static inline int mzw_drip(int x, int p, int tail, uint32_t seed, int reverse) {
  int out = 0;
  int coord = reverse ? (4095 - x) : x;
  for (int k = 0; k < 4; k++) {
    int delay = (int)(mzw_h(seed + (uint32_t)k * 73U) % 1500U);
    int local = p - delay;
    if (local < 0 || local > 3300) continue;
    int flight = local > 2500 ? 4095 : (local * 4095) / 2500;
    int head = mzw_head(flight, tail / 2 + 80);
    out = mzw_mx(out, mzw_comet(head, coord, tail / 2 + 80));
    if (local > 2050) {
      int radius = ((local - 2050) * 620) / 1250;
      int edge = mzw_abs(coord - 4095);
      out = mzw_mx(out, mzw_peak(mzw_abs(edge - radius), 95));
    }
  }
  return out;
}
static inline int mzw_fireworks1D(int x, int p, uint32_t seed) {
  int out = 0;
  for (int k = 0; k < 3; k++) {
    int delay = k * 1320;
    int local = p - delay;
    if (local < 0 || local > 2050) continue;
    int center = 400 + ((int)mzw_h(seed + (uint32_t)k * 97U) * 3295) / 255;
    int radius = (local * 1500) / 2050;
    int ring = mzw_peak(mzw_abs(mzw_abs(x - center) - radius), 115);
    out = mzw_mx(out, ring);
    for (int j = 0; j < 4; j++) {
      int frac = 1 + j;
      int offset = (radius * frac) / 4;
      int a = mzw_peak(mzw_abs(x - (center + offset)), 72);
      int b = mzw_peak(mzw_abs(x - (center - offset)), 72);
      int tw = mzw_h(seed + (uint32_t)(k * 19 + j * 7));
      if (tw > 80) out = mzw_mx(out, mzw_mx(a, b));
    }
  }
  return out;
}
static inline int mzw_pacman(int x, int p, int tail, int reverse) {
  int coord = reverse ? (4095 - x) : x;
  int head = mzw_head(p, tail / 2 + 100);
  int chop = ((p >> 7) & 1) ? 255 : 175;
  int out = (mzw_peak(mzw_abs(coord - head), 150) * chop) / 255;
  for (int k = 1; k <= 4; k++) {
    int pellet = head - k * 330;
    if (pellet >= 0 && pellet <= 4095)
      out = mzw_mx(out, mzw_peak(mzw_abs(coord - pellet), 58));
  }
  return out;
}
static inline int mzw_psBalance(int x, int p, int count, int width) {
  int out = 0;
  if (count < 1) count = 1;
  if (count > 5) count = 5;
  int balance = mzw_bounce(p, 1);
  for (int k = 0; k < count; k++) {
    int offset = (k - (count - 1) / 2) * 245;
    int head = mzw_cl(balance + offset) * 4095 / 255;
    out = mzw_mx(out, mzw_peak(mzw_abs(x - head), width));
  }
  return out;
}
static inline int mzw_psSpray1D(int x, int p, int width, int reverse) {
  int out = 0;
  int center = 2048;
  int coord = reverse ? 4095 - x : x;
  for (int k = 0; k < 5; k++) {
    int delay = k * 500;
    int local = p - delay;
    if (local < 0) continue;
    if (local > 3000) local = 3000;
    int radius = (local * 1950) / 3000;
    int sign = (k & 1) ? -1 : 1;
    int head = center + sign * radius;
    out = mzw_mx(out, mzw_peak(mzw_abs(coord - head), width));
    int trail = mzw_abs(coord - head);
    if (trail < radius && ((trail / 95) % 3 == 0)) out = mzw_mx(out, 65);
  }
  return out;
}
static inline int mzw_psStarburst(int x, int p, uint32_t seed) {
  int out = 0;
  for (int k = 0; k < 3; k++) {
    int delay = k * 1360;
    int local = p - delay;
    if (local < 0 || local > 1800) continue;
    int center = 700 + ((int)mzw_h(seed + (uint32_t)k * 151U) * 2695) / 255;
    int radius = (local * 1650) / 1800;
    out = mzw_mx(out, mzw_peak(mzw_abs(mzw_abs(x - center) - radius), 80));
    uint8_t r = mzw_h(seed + (uint32_t)k * 41U + (uint32_t)(local >> 5));
    if (r > 100) {
      int a = mzw_peak(mzw_abs(x - (center + radius)), 85);
      int b = mzw_peak(mzw_abs(x - (center - radius)), 85);
      out = mzw_mx(out, mzw_mx(a, b));
    }
  }
  return out;
}
static inline int mzw_shimmer(int x, int p, int scale, int flow, int &pos) {
  int phase = (x * scale - p * flow) & 4095;
  int grain = ((x >> 5) & 1) ? 36 : 0;
  pos = ((x >> 4) + (p >> 3)) & 255;
  return mzw_cl(30 + grain + (mzw_sine255(phase) * 225) / 255);
}
static inline int mzw_sinelonDual(int x, int p, int tail) {
  int head = mzw_sine255(p * 2) * 16;
  int a = mzw_comet(head, x, tail);
  int b = mzw_comet(4095 - head, x, tail);
  return mzw_mx(a, b);
}
static inline int mzw_tetrix(int x, int p, uint32_t seed) {
  int cell = (x * 12) >> 12;
  if (cell > 11) cell = 11;
  int step = (p * 14) >> 12;
  int lane = (cell + (int)(mzw_h(seed + (uint32_t)cell * 43U) % 5U)) % 14;
  int settled = (step > lane) ? 105 + (mzw_h(seed + (uint32_t)cell * 71U) % 75) : 0;
  int fallingCell = (step + 3) % 12;
  int falling = (cell == fallingCell) ? 255 : 0;
  int flash = ((p >> 7) & 1) && (cell == ((step + 8) % 12)) ? 190 : 0;
  return mzw_mx(settled, mzw_mx(falling, flash));
}

// ---------- per-frame context ----------
static inline uint32_t mzwPeriod(uint8_t speed) {
  int T = 12000 - (int)speed * 40;
  return (uint32_t)(T < 1500 ? 1500 : T);
}
static inline void mzwInit(MzwCtx &c, int mode, int n, uint32_t t, uint32_t T, uint8_t intensity) {
  c.mode = mode;
  c.n = n;
  c.u = (int)(((uint64_t)t * 4095ULL) / (T ? T : 1));
  if (c.u > 4095) c.u = 4095;
  c.wd = 300 + ((int)intensity * 1500) / 255;
  c.soft = 100 + c.wd / 9;
  int fin = mzw_ramp(c.u, 0, MZW_F(0.018));
  int fout = 255 - mzw_ramp(c.u, MZW_F(0.985), MZW_U);
  c.env = (fin * fout) / 255;
}

// ---------- one-pixel renderer: 50 multi-stage choreographies ----------
static inline MzwPx mzwPixel(const MzwCtx &c, int i) {
  const int u = c.u, wd = c.wd, soft = c.soft;
  const int x = c.n > 1 ? (int)(((uint32_t)i * 4096U) / (uint32_t)(c.n - 1)) : 0;
  const int xr = MZW_U - x;
  const int q = mzw_abs(2 * x - MZW_U); // 0 center, 4096 tips
  const int qr = MZW_U - q;
  const int tail = wd < 460 ? 460 : wd;
  int v = 0;
  int pos = (x >> 4);
  int p = -1;

  switch (c.mode) {
    // ===== 01-09: IGNITION, SCAN, LOCK =====
    case 1: { // IGNITION SIGNATURE: fill -> twin laser -> 3 flash -> premium glow
      if ((p = mzw_phase(u, 0, MZW_F(.22))) >= 0) v = mzw_fillV(x, p, soft);
      else if ((p = mzw_phase(u, MZW_F(.22), MZW_F(.52))) >= 0) v = mzw_mx(mzw_sweep(x, p, tail, 0), mzw_sweep(x, p, tail / 2, 1));
      else if ((p = mzw_phase(u, MZW_F(.52), MZW_F(.72))) >= 0) v = mzw_mx(mzw_flash(p, 3, 560), mzw_mx(mzw_scanner(x, p, 1, 130), mzw_dissolve(i, p, 11, 0)));
      else { p = mzw_phase(u, MZW_F(.72), MZW_U); v = 145 + mzw_breathe(p, 2, 0) * 110 / 255; }
    } break;
    case 2: { // REVERSE IGNITION: inner fill -> reverse laser -> sparkle lock
      if ((p = mzw_phase(u, 0, MZW_F(.22))) >= 0) v = mzw_fillV(xr, p, soft);
      else if ((p = mzw_phase(u, MZW_F(.22), MZW_F(.50))) >= 0) v = mzw_mx(mzw_sweep(x, p, tail, 1), mzw_sweep(x, p, tail / 2, 0));
      else if ((p = mzw_phase(u, MZW_F(.50), MZW_F(.73))) >= 0) { int h = 0; v = mzw_mx(mzw_flash(p, 2, 720), mzw_mx(mzw_glitter(i, p, 32, 17), mzw_colorTwinkles(i, p, 48, 19, h))); pos = h; }
      else { p = mzw_phase(u, MZW_F(.73), MZW_U); v = 165 + mzw_breathe(p, 3, 0) * 90 / 255; }
    } break;
    case 3: { // CENTER EXPANSION: radial ignition -> outward twin beams -> ripple -> hold
      if ((p = mzw_phase(u, 0, MZW_F(.24))) >= 0) v = mzw_fillV(q, p, soft);
      else if ((p = mzw_phase(u, MZW_F(.24), MZW_F(.53))) >= 0) v = mzw_mx(mzw_centerSweep(q, p, tail), mzw_ripple(q, p, 210, 0));
      else if ((p = mzw_phase(u, MZW_F(.53), MZW_F(.72))) >= 0) v = mzw_mx(mzw_ripple(q, p, 180, 1), mzw_flash(p, 2, 580));
      else { p = mzw_phase(u, MZW_F(.72), MZW_U); v = 130 + mzw_breathe(p, 2, 0) * 125 / 255; }
    } break;
    case 4: { // TIPS CONVERGE: two tips meet -> center blast -> double confirmation -> hold
      if ((p = mzw_phase(u, 0, MZW_F(.27))) >= 0) v = mzw_fillV(qr, p, soft);
      else if ((p = mzw_phase(u, MZW_F(.27), MZW_F(.53))) >= 0) v = mzw_mx(mzw_centerSweep(qr, p, tail), mzw_peak(qr, 210));
      else if ((p = mzw_phase(u, MZW_F(.53), MZW_F(.70))) >= 0) v = mzw_flash(p, 2, 520);
      else { p = mzw_phase(u, MZW_F(.70), MZW_U); v = 175 + mzw_breathe(p, 2, 0) * 80 / 255; }
    } break;
    case 5: { // LASER CROSS: crossing beams -> scan -> flash lock -> color breathing
      if ((p = mzw_phase(u, 0, MZW_F(.30))) >= 0) v = mzw_dualSweep(x, p, tail / 2);
      else if ((p = mzw_phase(u, MZW_F(.30), MZW_F(.58))) >= 0) v = mzw_mx(mzw_scanner(x, p, 2, 150), mzw_sweep(x, p, tail, (p > 2048)));
      else if ((p = mzw_phase(u, MZW_F(.58), MZW_F(.74))) >= 0) v = mzw_mx(mzw_flash(p, 3, 430), mzw_peak(q, 700));
      else { p = mzw_phase(u, MZW_F(.74), MZW_U); v = mzw_breathe(p, 2, 95); }
    } break;
    case 6: { // KNIGHT RIDER PRO: bounce scan -> twin counter chase -> red-car-style pulse lock
      if ((p = mzw_phase(u, 0, MZW_F(.34))) >= 0) v = mzw_scanner(x, p, 2, 190);
      else if ((p = mzw_phase(u, MZW_F(.34), MZW_F(.62))) >= 0) v = mzw_mx(mzw_sweep(x, p, tail, 0), mzw_sweep(x, p, tail, 1));
      else if ((p = mzw_phase(u, MZW_F(.62), MZW_F(.78))) >= 0) v = mzw_mx(mzw_pulsePair(p), mzw_flash(p, 2, 440));
      else { p = mzw_phase(u, MZW_F(.78), MZW_U); v = 145 + mzw_breathe(p, 2, 0) * 110 / 255; }
    } break;
    case 7: { // DIGITAL IGNITION: 8 blocks -> pixel chase -> scanner -> full lock
      if ((p = mzw_phase(u, 0, MZW_F(.32))) >= 0) v = mzw_blocks(x, p, 8, 0, 2);
      else if ((p = mzw_phase(u, MZW_F(.32), MZW_F(.58))) >= 0) v = mzw_mx(mzw_blocks(x, p, 12, 1, 3), mzw_glitter(i, p, 20, 31));
      else if ((p = mzw_phase(u, MZW_F(.58), MZW_F(.74))) >= 0) v = mzw_mx(mzw_scanner(x, p, 1, 180), mzw_flash(p, 2, 480));
      else v = 205;
    } break;
    case 8: { // TRIPLE COMET IGNITION: triple runners -> sparkle trail -> triple flash -> hold
      if ((p = mzw_phase(u, 0, MZW_F(.42))) >= 0) {
        int a = mzw_sweep(x, p, tail, 0), b = mzw_sweep(x, (p + 1365) & 4095, tail, 0), d = mzw_sweep(x, (p + 2730) & 4095, tail, 0);
        v = mzw_mx(a, mzw_mx(b, d));
      } else if ((p = mzw_phase(u, MZW_F(.42), MZW_F(.63))) >= 0) v = mzw_mx(mzw_glitter(i, p, 40, 41), mzw_mx(mzw_sweep(x, p, tail * 2, 0), mzw_bouncingBalls(x, p, 2, 115)));
      else if ((p = mzw_phase(u, MZW_F(.63), MZW_F(.78))) >= 0) v = mzw_flash(p, 3, 500);
      else { p = mzw_phase(u, MZW_F(.78), MZW_U); v = mzw_breathe(p, 2, 150); }
    } break;
    case 9: { // 12-STEP MATRIX: domino ignition -> center ripple -> twin beams -> stable glow
      if ((p = mzw_phase(u, 0, MZW_F(.35))) >= 0) v = mzw_blocks(x, p, 12, 0, 3);
      else if ((p = mzw_phase(u, MZW_F(.35), MZW_F(.58))) >= 0) v = mzw_mx(mzw_ripple(q, p, 230, 0), mzw_blocks(x, p, 6, 1, 1));
      else if ((p = mzw_phase(u, MZW_F(.58), MZW_F(.78))) >= 0) v = mzw_dualSweep(x, p, tail);
      else v = 190;
    } break;

    // ===== 10-18: COMET / CHASE CHOREOGRAPHIES =====
    case 10: { // COMET ARRIVAL: single comet -> dual comet -> burst -> warm hold
      if ((p = mzw_phase(u, 0, MZW_F(.30))) >= 0) v = mzw_sweep(x, p, tail * 2, 0);
      else if ((p = mzw_phase(u, MZW_F(.30), MZW_F(.56))) >= 0) v = mzw_mx(mzw_sweep(x, p, tail, 0), mzw_sweep(x, p, tail, 1));
      else if ((p = mzw_phase(u, MZW_F(.56), MZW_F(.74))) >= 0) v = mzw_mx(mzw_flash(p, 2, 520), mzw_mx(mzw_ripple(q, p, 260, 0), mzw_fireworks1D(x, p, 29)));
      else { p = mzw_phase(u, MZW_F(.74), MZW_U); v = mzw_breathe(p, 2, 125); }
    } break;
    case 11: { // REVERSE COMET: reverse meteor -> center meeting -> sparkle lock -> fade glow
      if ((p = mzw_phase(u, 0, MZW_F(.31))) >= 0) v = mzw_sweep(x, p, tail * 2, 1);
      else if ((p = mzw_phase(u, MZW_F(.31), MZW_F(.57))) >= 0) v = mzw_centerSweep(qr, p, tail);
      else if ((p = mzw_phase(u, MZW_F(.57), MZW_F(.76))) >= 0) v = mzw_mx(mzw_glitter(i, p, 48, 53), mzw_mx(mzw_flash(p, 2, 500), mzw_drip(x, p, tail, 31, 1)));
      else { p = mzw_phase(u, MZW_F(.76), MZW_U); v = mzw_breathe(p, 3, 130); }
    } break;
    case 12: { // DOUBLE KNIGHT RIDER: fast scan -> split scan -> flash confirmation -> lock
      if ((p = mzw_phase(u, 0, MZW_F(.30))) >= 0) v = mzw_scanner(x, p, 3, 120);
      else if ((p = mzw_phase(u, MZW_F(.30), MZW_F(.59))) >= 0) v = mzw_mx(mzw_scanner(x, p, 2, 230), mzw_scanner(x, (4095 - p), 2, 100));
      else if ((p = mzw_phase(u, MZW_F(.59), MZW_F(.75))) >= 0) v = mzw_flash(p, 3, 420);
      else v = 215;
    } break;
    case 13: { // TWIN COMET COLLISION: opposing comets -> center collision -> radial flash -> breathe
      if ((p = mzw_phase(u, 0, MZW_F(.38))) >= 0) v = mzw_dualSweep(x, p, tail * 2);
      else if ((p = mzw_phase(u, MZW_F(.38), MZW_F(.60))) >= 0) v = mzw_mx(mzw_peak(q, 360), mzw_ripple(q, p, 260, 0));
      else if ((p = mzw_phase(u, MZW_F(.60), MZW_F(.77))) >= 0) v = mzw_mx(235, mzw_flash(p, 2, 650));
      else { p = mzw_phase(u, MZW_F(.77), MZW_U); v = mzw_breathe(p, 2, 140); }
    } break;
    case 14: { // TRIPLE RACE: three comets -> sweep tail -> glitter burst -> hold
      if ((p = mzw_phase(u, 0, MZW_F(.43))) >= 0) {
        int a = mzw_sweep(x, p, tail, 0), b = mzw_sweep(x, (p + 1365) & 4095, tail, 1), d = mzw_sweep(x, (p + 2730) & 4095, tail, 0);
        v = mzw_mx(a, mzw_mx(b, d));
      } else if ((p = mzw_phase(u, MZW_F(.43), MZW_F(.64))) >= 0) v = mzw_mx(mzw_sweep(x, p, tail * 3, 0), mzw_mx(mzw_glitter(i, p, 35, 61), mzw_psStarburst(x, p, 37)));
      else if ((p = mzw_phase(u, MZW_F(.64), MZW_F(.80))) >= 0) v = mzw_flash(p, 3, 470);
      else v = 200;
    } break;
    case 15: { // CENTER-OUT TURBO: center ignition -> twin radial beams -> outer-edge flash -> hold
      if ((p = mzw_phase(u, 0, MZW_F(.25))) >= 0) v = mzw_fillV(q, p, soft);
      else if ((p = mzw_phase(u, MZW_F(.25), MZW_F(.58))) >= 0) v = mzw_mx(mzw_centerSweep(q, p, tail), mzw_ripple(q, p, 180, 0));
      else if ((p = mzw_phase(u, MZW_F(.58), MZW_F(.75))) >= 0) v = mzw_mx(mzw_peak(q, 240), mzw_flash(p, 2, 450));
      else { p = mzw_phase(u, MZW_F(.75), MZW_U); v = mzw_breathe(p, 2, 145); }
    } break;
    case 16: { // CENTER LOCK-IN: tips ignite -> twin beams converge -> central lock -> palette shimmer
      if ((p = mzw_phase(u, 0, MZW_F(.25))) >= 0) v = mzw_fillV(qr, p, soft);
      else if ((p = mzw_phase(u, MZW_F(.25), MZW_F(.58))) >= 0) v = mzw_centerSweep(qr, p, tail);
      else if ((p = mzw_phase(u, MZW_F(.58), MZW_F(.76))) >= 0) v = mzw_mx(mzw_flash(p, 2, 600), mzw_peak(qr, 420));
      else { p = mzw_phase(u, MZW_F(.76), MZW_U); pos = ((x >> 4) + (p >> 4)) & 255; v = mzw_breathe(p, 2, 135); }
    } break;
    case 17: { // TRIPLE DASH: 3-lane race -> reverse race -> scan lock -> glow
      if ((p = mzw_phase(u, 0, MZW_F(.34))) >= 0) {
        int a = mzw_sweep(x, p, tail / 2, 0), b = mzw_sweep(x, (p + 1365) & 4095, tail / 2, 0), d = mzw_sweep(x, (p + 2730) & 4095, tail / 2, 0);
        v = mzw_mx(a, mzw_mx(b, d));
      } else if ((p = mzw_phase(u, MZW_F(.34), MZW_F(.61))) >= 0) v = mzw_mx(mzw_dualSweep(x, p, tail / 2), mzw_wave(x, p, 5, 6, 0, 100));
      else if ((p = mzw_phase(u, MZW_F(.61), MZW_F(.78))) >= 0) v = mzw_scanner(x, p, 1, 150);
      else v = 205;
    } break;
    case 18: { // PIXEL RACE: segmented chase -> full-width laser -> double pulse -> afterglow
      if ((p = mzw_phase(u, 0, MZW_F(.31))) >= 0) v = mzw_blocks(x, p, 16, 0, 2);
      else if ((p = mzw_phase(u, MZW_F(.31), MZW_F(.57))) >= 0) v = mzw_mx(mzw_sweep(x, p, tail, 1), mzw_scanner(x, p, 1, 120));
      else if ((p = mzw_phase(u, MZW_F(.57), MZW_F(.77))) >= 0) v = mzw_pulsePair(p);
      else { p = mzw_phase(u, MZW_F(.77), MZW_U); v = mzw_breathe(p, 2, 125); }
    } break;

    // ===== 19-27: PULSE / RIPPLE / BURST =====
    case 19: { // SPLIT LANE: odd/even ignition -> lanes cross -> center impact -> hold
      if ((p = mzw_phase(u, 0, MZW_F(.28))) >= 0) {
        int lane = ((i & 1) == 0) ? mzw_fillV(x, p, soft) : mzw_fillV(xr, p, soft);
        v = lane;
      } else if ((p = mzw_phase(u, MZW_F(.28), MZW_F(.56))) >= 0) v = mzw_mx(mzw_dualSweep(x, p, tail), mzw_wave(x, p, 3, 5, 0, 130));
      else if ((p = mzw_phase(u, MZW_F(.56), MZW_F(.76))) >= 0) v = mzw_mx(mzw_peak(q, 360), mzw_flash(p, 2, 520));
      else v = 205;
    } break;
    case 20: { // METEOR STRIKE: long meteor -> debris glitter -> impact flash -> glowing ember
      if ((p = mzw_phase(u, 0, MZW_F(.42))) >= 0) v = mzw_mx(mzw_sweep(x, p, tail * 3, 0), mzw_glitter(i, p, 24, 73));
      else if ((p = mzw_phase(u, MZW_F(.42), MZW_F(.64))) >= 0) v = mzw_mx(mzw_glitter(i, p, 65, 79), mzw_ripple(q, p, 240, 0));
      else if ((p = mzw_phase(u, MZW_F(.64), MZW_F(.76))) >= 0) v = mzw_flash(p, 2, 750);
      else { p = mzw_phase(u, MZW_F(.76), MZW_U); v = mzw_breathe(p, 3, 100); }
    } break;
    case 21: { // LUXURY BREATH: light builds -> aurora rolls -> twin glints -> soft hold
      if ((p = mzw_phase(u, 0, MZW_F(.24))) >= 0) v = mzw_breathe(p, 1, 5);
      else if ((p = mzw_phase(u, MZW_F(.24), MZW_F(.58))) >= 0) { pos = ((x >> 4) + (p >> 3)) & 255; v = mzw_mx(mzw_wave(x, p, 2, 3, 20, 205), mzw_mx(mzw_sweep(x, p, tail / 2, 0), mzw_shimmer(x, p, 2, 5, pos))); }
      else if ((p = mzw_phase(u, MZW_F(.58), MZW_F(.76))) >= 0) v = mzw_mx(mzw_peak(x, 200), mzw_peak(xr, 200));
      else { p = mzw_phase(u, MZW_F(.76), MZW_U); v = mzw_breathe(p, 2, 135); }
    } break;
    case 22: { // DOUBLE HEARTBEAT: heart pulses -> twin comet -> flash -> breathing hold
      if ((p = mzw_phase(u, 0, MZW_F(.30))) >= 0) v = mzw_pulsePair(p);
      else if ((p = mzw_phase(u, MZW_F(.30), MZW_F(.58))) >= 0) v = mzw_dualSweep(x, p, tail);
      else if ((p = mzw_phase(u, MZW_F(.58), MZW_F(.76))) >= 0) v = mzw_mx(mzw_flash(p, 2, 580), mzw_pulsePair(p));
      else { p = mzw_phase(u, MZW_F(.76), MZW_U); v = mzw_breathe(p, 2, 125); }
    } break;
    case 23: { // SOFT ARRIVAL: gradual fade -> expanding ripple -> laser pass -> calm glow
      if ((p = mzw_phase(u, 0, MZW_F(.25))) >= 0) v = mzw_smooth(p >> 4);
      else if ((p = mzw_phase(u, MZW_F(.25), MZW_F(.56))) >= 0) v = mzw_ripple(q, p, 230, 0);
      else if ((p = mzw_phase(u, MZW_F(.56), MZW_F(.76))) >= 0) v = mzw_sweep(x, p, tail, 0);
      else { p = mzw_phase(u, MZW_F(.76), MZW_U); v = mzw_breathe(p, 1, 150); }
    } break;
    case 24: { // HEARTBEAT ARRIVAL: double pulse -> sweeping accent -> triple flash -> hold
      if ((p = mzw_phase(u, 0, MZW_F(.34))) >= 0) v = mzw_pulsePair(p);
      else if ((p = mzw_phase(u, MZW_F(.34), MZW_F(.62))) >= 0) v = mzw_mx(mzw_sweep(x, p, tail, 0), mzw_mx(mzw_sweep(x, p, tail / 2, 1), mzw_sinelonDual(x, p, tail)));
      else if ((p = mzw_phase(u, MZW_F(.62), MZW_F(.79))) >= 0) v = mzw_flash(p, 3, 460);
      else v = 210;
    } break;
    case 25: { // TRIPLE FLASH SHOW: flash salvo -> radial expansion -> laser lock -> breathe
      if ((p = mzw_phase(u, 0, MZW_F(.28))) >= 0) v = mzw_flash(p, 3, 500);
      else if ((p = mzw_phase(u, MZW_F(.28), MZW_F(.58))) >= 0) v = mzw_mx(mzw_ripple(q, p, 280, 0), mzw_fillV(q, p, soft));
      else if ((p = mzw_phase(u, MZW_F(.58), MZW_F(.76))) >= 0) v = mzw_mx(mzw_scanner(x, p, 1, 150), mzw_flash(p, 2, 560));
      else { p = mzw_phase(u, MZW_F(.76), MZW_U); v = mzw_breathe(p, 2, 135); }
    } break;
    case 26: { // PIXEL IGNITION PRO: granular ignition -> double sweep -> sparkling lock -> hold
      if ((p = mzw_phase(u, 0, MZW_F(.34))) >= 0) {
        int delay = (x * 1700) / MZW_U;
        int local = p - delay;
        v = local < 0 ? 0 : (local < 500 ? 255 : 145);
      } else if ((p = mzw_phase(u, MZW_F(.34), MZW_F(.62))) >= 0) v = mzw_dualSweep(x, p, tail);
      else if ((p = mzw_phase(u, MZW_F(.62), MZW_F(.80))) >= 0) v = mzw_mx(mzw_glitter(i, p, 45, 89), 120);
      else v = 200;
    } break;
    case 27: { // DOUBLE RIPPLE: center ring -> reverse ring -> crossing comet -> light lock
      if ((p = mzw_phase(u, 0, MZW_F(.30))) >= 0) v = mzw_ripple(q, p, 250, 0);
      else if ((p = mzw_phase(u, MZW_F(.30), MZW_F(.56))) >= 0) v = mzw_mx(mzw_ripple(q, p, 220, 1), mzw_ripple(q, (p + 2048) & 4095, 160, 0));
      else if ((p = mzw_phase(u, MZW_F(.56), MZW_F(.77))) >= 0) v = mzw_dualSweep(x, p, tail / 2);
      else { p = mzw_phase(u, MZW_F(.77), MZW_U); v = mzw_breathe(p, 2, 150); }
    } break;

    // ===== 28-36: COLOR FLOW / AURORA / SPARK =====
    case 28: { // SPECTRUM IGNITION: color wave -> comet highlight -> glitter -> gradient hold
      if ((p = mzw_phase(u, 0, MZW_F(.32))) >= 0) { pos = ((x >> 4) + (p >> 3)) & 255; v = mzw_wave(x, p, 2, 4, 55, 225); }
      else if ((p = mzw_phase(u, MZW_F(.32), MZW_F(.59))) >= 0) { v = mzw_mx(90, mzw_chaseRainbow(x, p, tail * 2, 0, pos)); }
      else if ((p = mzw_phase(u, MZW_F(.59), MZW_F(.78))) >= 0) { pos = (mzw_h((uint32_t)i * 17U + (uint32_t)(p >> 4)) + (p >> 4)) & 255; v = mzw_mx(mzw_glitter(i, p, 38, 97), 90); }
      else { p = mzw_phase(u, MZW_F(.78), MZW_U); pos = ((x >> 4) + (p >> 4)) & 255; v = mzw_breathe(p, 2, 150); }
    } break;
    case 29: { // REVERSE SPECTRUM: reverse color flow -> inward beam -> dual pulse -> color hold
      if ((p = mzw_phase(u, 0, MZW_F(.32))) >= 0) { pos = ((x >> 4) - (p >> 3)) & 255; v = mzw_wave(x, 4095 - p, 3, 4, 65, 230); }
      else if ((p = mzw_phase(u, MZW_F(.32), MZW_F(.58))) >= 0) { pos = ((x >> 4) - (p >> 2)) & 255; v = mzw_sweep(x, p, tail * 2, 1); }
      else if ((p = mzw_phase(u, MZW_F(.58), MZW_F(.77))) >= 0) v = mzw_flash(p, 2, 600);
      else { p = mzw_phase(u, MZW_F(.77), MZW_U); pos = ((x >> 4) - (p >> 4)) & 255; v = mzw_breathe(p, 2, 140); }
    } break;
    case 30: { // LIQUID CHROME: flowing bands -> twin scanner -> color ripple -> final glow
      if ((p = mzw_phase(u, 0, MZW_F(.35))) >= 0) { int ph = (x * 3 - p * 3) & 4095; pos = (ph >> 4) & 255; v = 95 + mzw_smooth(mzw_tri(ph)) * 160 / 255; }
      else if ((p = mzw_phase(u, MZW_F(.35), MZW_F(.61))) >= 0) { pos = ((x >> 4) + (p >> 3)) & 255; v = mzw_mx(mzw_scanner(x, p, 2, 210), mzw_wave(x, p, 2, 4, 0, 105)); }
      else if ((p = mzw_phase(u, MZW_F(.61), MZW_F(.79))) >= 0) { pos = ((x >> 4) + (p >> 2)) & 255; v = mzw_ripple(q, p, 260, 0); }
      else { p = mzw_phase(u, MZW_F(.79), MZW_U); v = mzw_breathe(p, 2, 145); }
    } break;
    case 31: { // AURORA CURTAIN: layered waves -> comet through aurora -> sparkle rain -> hold
      if ((p = mzw_phase(u, 0, MZW_F(.36))) >= 0) { pos = ((x >> 4) + (p >> 4)) & 255; v = mzw_mx(mzw_wave(x, p, 2, 3, 25, 190), mzw_wave(x, 4095 - p, 3, 2, 0, 105)); }
      else if ((p = mzw_phase(u, MZW_F(.36), MZW_F(.62))) >= 0) { pos = ((x >> 4) + (p >> 3)) & 255; v = mzw_mx(mzw_sweep(x, p, tail * 2, 0), mzw_mx(mzw_wave(x, p, 2, 5, 0, 120), mzw_waveSins(x, p, 3, 3, 0, 95))); }
      else if ((p = mzw_phase(u, MZW_F(.62), MZW_F(.80))) >= 0) v = mzw_mx(mzw_glitter(i, p, 43, 101), mzw_wave(x, p, 4, 2, 0, 120));
      else { p = mzw_phase(u, MZW_F(.80), MZW_U); pos = ((x >> 4) + (p >> 4)) & 255; v = mzw_breathe(p, 2, 130); }
    } break;
    case 32: { // WAVE INTERFERENCE: crossing waves -> sharp scan -> radial burst -> steady
      if ((p = mzw_phase(u, 0, MZW_F(.34))) >= 0) { int a = mzw_wave(x, p, 3, 4, 0, 220); int b = mzw_wave(x, 4095 - p, 4, 3, 0, 170); v = mzw_mx(a, b); pos = ((x >> 4) + (p >> 3)) & 255; }
      else if ((p = mzw_phase(u, MZW_F(.34), MZW_F(.60))) >= 0) v = mzw_mx(mzw_scanner(x, p, 2, 110), mzw_sweep(x, p, tail, 1));
      else if ((p = mzw_phase(u, MZW_F(.60), MZW_F(.78))) >= 0) v = mzw_mx(mzw_ripple(q, p, 200, 0), mzw_flash(p, 2, 500));
      else v = 200;
    } break;
    case 33: { // COLOR LASER: rainbow beam -> reverse beam -> triple flash -> spectrum breathe
      if ((p = mzw_phase(u, 0, MZW_F(.36))) >= 0) { pos = ((x >> 4) + (p >> 2)) & 255; v = mzw_mx(35, mzw_sweep(x, p, tail * 2, 0)); }
      else if ((p = mzw_phase(u, MZW_F(.36), MZW_F(.62))) >= 0) { pos = ((x >> 4) - (p >> 2)) & 255; v = mzw_mx(55, mzw_sweep(x, p, tail * 2, 1)); }
      else if ((p = mzw_phase(u, MZW_F(.62), MZW_F(.79))) >= 0) v = mzw_flash(p, 3, 450);
      else { p = mzw_phase(u, MZW_F(.79), MZW_U); pos = ((x >> 4) + (p >> 4)) & 255; v = mzw_breathe(p, 2, 150); }
    } break;
    case 34: { // STAR DUST: twinkle awakening -> glitter sweep -> sparkle burst -> luxury hold
      if ((p = mzw_phase(u, 0, MZW_F(.27))) >= 0) v = mzw_glitter(i, p, 27, 107);
      else if ((p = mzw_phase(u, MZW_F(.27), MZW_F(.58))) >= 0) v = mzw_mx(mzw_glitter(i, p, 48, 109), mzw_sweep(x, p, tail * 2, 0));
      else if ((p = mzw_phase(u, MZW_F(.58), MZW_F(.79))) >= 0) { int h = 0; v = mzw_mx(mzw_glitter(i, p, 72, 113), mzw_mx(mzw_flash(p, 2, 450), mzw_colorTwinkles(i, p, 88, 117, h))); pos = h; }
      else { p = mzw_phase(u, MZW_F(.79), MZW_U); v = mzw_breathe(p, 2, 125); }
    } break;
    case 35: { // GLITTER COMET: comet launch -> two cross tails -> star burst -> soft hold
      if ((p = mzw_phase(u, 0, MZW_F(.38))) >= 0) v = mzw_mx(mzw_sweep(x, p, tail * 3, 0), mzw_glitter(i, p, 20, 127));
      else if ((p = mzw_phase(u, MZW_F(.38), MZW_F(.63))) >= 0) v = mzw_mx(mzw_dualSweep(x, p, tail), mzw_glitter(i, p, 53, 131));
      else if ((p = mzw_phase(u, MZW_F(.63), MZW_F(.80))) >= 0) v = mzw_mx(mzw_glitter(i, p, 85, 137), mzw_mx(mzw_ripple(q, p, 220, 0), mzw_psStarburst(x, p, 139)));
      else { p = mzw_phase(u, MZW_F(.80), MZW_U); v = mzw_breathe(p, 2, 135); }
    } break;
    case 36: { // SPARK IGNITION: spark trails -> sequential fill -> laser edge -> bright hold
      if ((p = mzw_phase(u, 0, MZW_F(.25))) >= 0) v = mzw_glitter(i, p, 35, 139);
      else if ((p = mzw_phase(u, MZW_F(.25), MZW_F(.55))) >= 0) v = mzw_mx(mzw_fillV(x, p, soft), mzw_glitter(i, p, 24, 149));
      else if ((p = mzw_phase(u, MZW_F(.55), MZW_F(.76))) >= 0) v = mzw_mx(mzw_sweep(x, p, tail, 0), mzw_glitter(i, p, 52, 151));
      else v = 220;
    } break;

    // ===== 37-45: SHOWTIME / SIGNATURE FINISHES =====
    case 37: { // FIREFLY PARADE: random glows -> firefly wave -> twin comet -> flash hold
      if ((p = mzw_phase(u, 0, MZW_F(.29))) >= 0) { uint8_t h = mzw_h((uint32_t)i * 13U + 7U); v = mzw_smooth(mzw_tri(p * 3 + h * 16)); }
      else if ((p = mzw_phase(u, MZW_F(.29), MZW_F(.58))) >= 0) { uint8_t h = mzw_h((uint32_t)i * 13U + 7U); v = mzw_mx(mzw_smooth(mzw_tri(p * 3 + h * 16)), mzw_mx(mzw_wave(x, p, 3, 4, 0, 100), mzw_psSpray1D(x, p, 90, 0))); pos = h; }
      else if ((p = mzw_phase(u, MZW_F(.58), MZW_F(.78))) >= 0) v = mzw_dualSweep(x, p, tail);
      else { p = mzw_phase(u, MZW_F(.78), MZW_U); v = mzw_mx(150, mzw_flash(p, 2, 430)); }
    } break;
    case 38: { // 8-CELL SHOW: block ignition -> cascading domino -> scanner -> lock flash
      if ((p = mzw_phase(u, 0, MZW_F(.30))) >= 0) v = mzw_blocks(x, p, 8, 0, 1);
      else if ((p = mzw_phase(u, MZW_F(.30), MZW_F(.57))) >= 0) v = mzw_blocks(x, p, 8, 1, 3);
      else if ((p = mzw_phase(u, MZW_F(.57), MZW_F(.78))) >= 0) v = mzw_mx(mzw_scanner(x, p, 1, 210), mzw_mx(mzw_flash(p, 2, 440), mzw_tetrix(x, p, 151)));
      else v = 210;
    } break;
    case 39: { // 12-CELL DOMINO: segmented cascade -> ripple -> triple flash -> solid glow
      if ((p = mzw_phase(u, 0, MZW_F(.34))) >= 0) v = mzw_blocks(x, p, 12, 1, 2);
      else if ((p = mzw_phase(u, MZW_F(.34), MZW_F(.60))) >= 0) v = mzw_mx(mzw_blocks(x, p, 6, 0, 2), mzw_ripple(q, p, 200, 0));
      else if ((p = mzw_phase(u, MZW_F(.60), MZW_F(.78))) >= 0) v = mzw_flash(p, 3, 480);
      else v = 220;
    } break;
    case 40: { // SEGMENT SCANNER: segmented bounce -> twin beam cross -> pulse -> premium hold
      if ((p = mzw_phase(u, 0, MZW_F(.33))) >= 0) { int b = mzw_bounce(p, 2) * 8 / 4096; int blk = (x * 8) >> 12; int d = mzw_abs(blk - b); v = d == 0 ? 255 : (d == 1 ? 150 : 12); }
      else if ((p = mzw_phase(u, MZW_F(.33), MZW_F(.60))) >= 0) v = mzw_mx(mzw_dualSweep(x, p, tail), mzw_psBalance(x, p, 3, 105));
      else if ((p = mzw_phase(u, MZW_F(.60), MZW_F(.78))) >= 0) v = mzw_flash(p, 2, 520);
      else { p = mzw_phase(u, MZW_F(.78), MZW_U); v = mzw_breathe(p, 2, 150); }
    } break;
    case 41: { // ALTERNATING SHOW: odd/even strobes -> opposing fill -> laser sweep -> lock
      if ((p = mzw_phase(u, 0, MZW_F(.27))) >= 0) { int phase = (p * 6) & 4095; v = (((((i & 1) ? 2048 : 0) + phase) & 4095) < 1500) ? 255 : 12; }
      else if ((p = mzw_phase(u, MZW_F(.27), MZW_F(.56))) >= 0) v = mzw_mx(((i & 1) ? mzw_fillV(xr, p, soft) : mzw_fillV(x, p, soft)), mzw_psSpray1D(x, p, 75, 0));
      else if ((p = mzw_phase(u, MZW_F(.56), MZW_F(.77))) >= 0) v = mzw_mx(mzw_sweep(x, p, tail, 0), mzw_sweep(x, p, tail, 1));
      else v = 215;
    } break;
    case 42: { // TRIPLE STROBE ARRIVAL: salvo -> radial expansion -> triple comet -> glow
      if ((p = mzw_phase(u, 0, MZW_F(.24))) >= 0) v = mzw_flash(p, 3, 560);
      else if ((p = mzw_phase(u, MZW_F(.24), MZW_F(.52))) >= 0) v = mzw_mx(mzw_ripple(q, p, 300, 0), mzw_fillV(q, p, soft));
      else if ((p = mzw_phase(u, MZW_F(.52), MZW_F(.78))) >= 0) { int a = mzw_sweep(x, p, tail, 0), b = mzw_sweep(x, (p + 1365) & 4095, tail, 1); v = mzw_mx(mzw_mx(a, b), mzw_fireworks1D(x, p, 163)); }
      else { p = mzw_phase(u, MZW_F(.78), MZW_U); v = mzw_breathe(p, 2, 140); }
    } break;
    case 43: { // CENTER BLAST: center ignition -> shockwave -> twin scanners -> bright lock
      if ((p = mzw_phase(u, 0, MZW_F(.25))) >= 0) v = mzw_fillV(q, p, soft);
      else if ((p = mzw_phase(u, MZW_F(.25), MZW_F(.54))) >= 0) v = mzw_mx(mzw_ripple(q, p, 310, 0), mzw_peak(mzw_abs(q - p), 180));
      else if ((p = mzw_phase(u, MZW_F(.54), MZW_F(.77))) >= 0) v = mzw_mx(mzw_scanner(x, p, 2, 150), mzw_scanner(xr, p, 2, 150));
      else v = 230;
    } break;
    case 44: { // TWIN LOCK BEAM: converge -> center lock -> cross laser -> double pulse finish
      if ((p = mzw_phase(u, 0, MZW_F(.34))) >= 0) v = mzw_centerSweep(qr, p, tail * 2);
      else if ((p = mzw_phase(u, MZW_F(.34), MZW_F(.56))) >= 0) v = mzw_mx(mzw_peak(qr, 330), mzw_flash(p, 2, 600));
      else if ((p = mzw_phase(u, MZW_F(.56), MZW_F(.78))) >= 0) v = mzw_dualSweep(x, p, tail / 2);
      else { p = mzw_phase(u, MZW_F(.78), MZW_U); v = mzw_mx(145, mzw_pulsePair(p)); }
    } break;
    case 45: { // MIZUMA SIGNATURE: ignition -> twin laser -> triple accent -> luxury breathing lock
      if ((p = mzw_phase(u, 0, MZW_F(.30))) >= 0) v = mzw_mx(mzw_fillV(x, p, soft), mzw_sweep(x, p, tail, 0));
      else if ((p = mzw_phase(u, MZW_F(.30), MZW_F(.56))) >= 0) v = mzw_mx(mzw_sweep(x, p, tail * 2, 0), mzw_sweep(x, p, tail, 1));
      else if ((p = mzw_phase(u, MZW_F(.56), MZW_F(.77))) >= 0) v = mzw_mx(mzw_flash(p, 3, 500), mzw_scanner(x, p, 1, 150));
      else { p = mzw_phase(u, MZW_F(.77), MZW_U); v = 165 + mzw_breathe(p, 2, 0) * 90 / 255; }
    } break;
    // ===== 46-50: PARTICLE SHOW / DMX-STYLE HYBRIDS =====
    case 46: { // PARTICLE SYMPHONY: bouncing balls -> balance/sinelon -> spray/drips -> starburst lock
      if ((p = mzw_phase(u, 0, MZW_F(.25))) >= 0) {
        v = mzw_mx(mzw_bouncingBalls(x, p, 3, 145), mzw_waveSins(x, p, 2, 3, 0, 70));
      } else if ((p = mzw_phase(u, MZW_F(.25), MZW_F(.52))) >= 0) {
        v = mzw_mx(mzw_psBalance(x, p, 4, 115), mzw_sinelonDual(x, p, tail));
      } else if ((p = mzw_phase(u, MZW_F(.52), MZW_F(.77))) >= 0) {
        v = mzw_mx(mzw_psSpray1D(x, p, 95, 0), mzw_drip(x, p, tail, 167, 1));
      } else {
        p = mzw_phase(u, MZW_F(.77), MZW_U);
        v = mzw_mx(mzw_psStarburst(x, p, 173), mzw_breathe(p, 2, 120));
      }
    } break;
    case 47: { // RAINBOW PACMAN RUN: rainbow chase -> chomping runner -> dual sinelon -> colorful twinkle
      if ((p = mzw_phase(u, 0, MZW_F(.27))) >= 0) {
        v = mzw_mx(65, mzw_chaseRainbow(x, p, tail * 2, 0, pos));
      } else if ((p = mzw_phase(u, MZW_F(.27), MZW_F(.53))) >= 0) {
        pos = ((x >> 4) + (p >> 2)) & 255;
        v = mzw_mx(mzw_pacman(x, p, tail, 0), mzw_shimmer(x, p, 3, 4, pos));
      } else if ((p = mzw_phase(u, MZW_F(.53), MZW_F(.76))) >= 0) {
        v = mzw_mx(mzw_sinelonDual(x, p, tail * 2), mzw_sweep(x, p, tail / 2, 1));
      } else {
        p = mzw_phase(u, MZW_F(.76), MZW_U);
        v = mzw_colorTwinkles(i, p, 95, 179, pos);
      }
    } break;
    case 48: { // FIREWORK CASCADE: dissolve ignition -> fireworks 1D -> spray/drip -> starburst finale
      if ((p = mzw_phase(u, 0, MZW_F(.23))) >= 0) {
        v = mzw_dissolve(i, p, 181, 0);
      } else if ((p = mzw_phase(u, MZW_F(.23), MZW_F(.54))) >= 0) {
        v = mzw_mx(mzw_fireworks1D(x, p, 191), mzw_psStarburst(x, p, 193));
      } else if ((p = mzw_phase(u, MZW_F(.54), MZW_F(.78))) >= 0) {
        v = mzw_mx(mzw_psSpray1D(x, p, 115, 1), mzw_drip(x, p, tail, 197, 0));
      } else {
        p = mzw_phase(u, MZW_F(.78), MZW_U);
        v = mzw_mx(mzw_flash(p, 2, 420), mzw_breathe(p, 2, 145));
      }
    } break;
    case 49: { // TETRIX AURORA: falling blocks -> wavesins -> shimmer sweep -> color twinkle lock
      if ((p = mzw_phase(u, 0, MZW_F(.29))) >= 0) {
        v = mzw_mx(mzw_tetrix(x, p, 199), mzw_blocks(x, p, 12, 1, 2));
      } else if ((p = mzw_phase(u, MZW_F(.29), MZW_F(.55))) >= 0) {
        pos = ((x >> 4) + (p >> 3)) & 255;
        v = mzw_mx(mzw_waveSins(x, p, 3, 4, 15, 215), mzw_waveSins(x, 4095 - p, 2, 3, 0, 120));
      } else if ((p = mzw_phase(u, MZW_F(.55), MZW_F(.78))) >= 0) {
        v = mzw_mx(mzw_shimmer(x, p, 4, 6, pos), mzw_sweep(x, p, tail, 0));
      } else {
        p = mzw_phase(u, MZW_F(.78), MZW_U);
        v = mzw_colorTwinkles(i, p, 75, 211, pos);
      }
    } break;
    case 50: { // MIZUMA DMX SHOWCASE: dissolve -> rainbow/sinelon -> spray/drip -> fireworks -> shimmer aura
      if ((p = mzw_phase(u, 0, MZW_F(.19))) >= 0) {
        v = mzw_mx(mzw_dissolve(i, p, 223, 0), mzw_sweep(x, p, tail, 0));
      } else if ((p = mzw_phase(u, MZW_F(.19), MZW_F(.42))) >= 0) {
        v = mzw_mx(mzw_chaseRainbow(x, p, tail * 2, 0, pos), mzw_sinelonDual(x, p, tail));
      } else if ((p = mzw_phase(u, MZW_F(.42), MZW_F(.62))) >= 0) {
        v = mzw_mx(mzw_psSpray1D(x, p, 105, 0), mzw_mx(mzw_drip(x, p, tail, 227, 1), mzw_pacman(x, p, tail, 1)));
      } else if ((p = mzw_phase(u, MZW_F(.62), MZW_F(.82))) >= 0) {
        v = mzw_mx(mzw_fireworks1D(x, p, 229), mzw_psStarburst(x, p, 233));
      } else {
        p = mzw_phase(u, MZW_F(.82), MZW_U);
        pos = ((x >> 4) + (p >> 3)) & 255;
        v = mzw_mx(mzw_shimmer(x, p, 3, 4, pos), mzw_mx(mzw_waveSins(x, p, 2, 4, 25, 155), mzw_breathe(p, 2, 110)));
      }
    } break;
    default: v = 0; break;
  }

  v = (mzw_cl(v) * c.env) / 255;
  MzwPx out;
  out.v = (uint8_t)v;
  out.pos = (uint8_t)(pos & 255);
  return out;
}

// =====================================================================================================
// WLED integration — callback signature matches WS2812FX::mode_ptr (void (*)()).
// =====================================================================================================
#if !defined(MZW_HOST) && defined(SEGENV)
static void mzwRun(uint8_t mode) {
  const uint16_t n = SEGLEN;
  if (SEGENV.call == 0) SEGENV.step = strip.now;
  const uint32_t T = mzwPeriod(SEGMENT.speed);
  const uint32_t t = (uint32_t)(strip.now - SEGENV.step) % T;
  MzwCtx c;
  mzwInit(c, mode, (int)n, t, T, SEGMENT.intensity);
  const uint32_t bg = SEGCOLOR(1);
  for (uint16_t i = 0; i < n; i++) {
    const MzwPx px = mzwPixel(c, i);
    const uint32_t fg = SEGMENT.color_from_palette(px.pos, false, true, 0, 255);
    SEGMENT.setPixelColor(i, px.v == 255 ? fg : (px.v == 0 ? bg : color_blend(bg, fg, px.v)));
  }
}
template <uint8_t M> static void mzwModeFn() { mzwRun(M); }

// Keep W01..W45 prefixes stable; W46..W50 are the added particle-show recipes.
// WLED effect data format: Name@slider;colors;palette;flags;defaults
#define MZW_LIST(X) \
 X(1,"W01 Ignition Signature")    X(2,"W02 Reverse Ignition")       X(3,"W03 Center Expansion")       X(4,"W04 Tips Converge") \
 X(5,"W05 Laser Cross")           X(6,"W06 Knight Rider Pro")        X(7,"W07 Digital Ignition")       X(8,"W08 Triple Comet Ignition") \
 X(9,"W09 12-Step Matrix")        X(10,"W10 Comet Arrival")          X(11,"W11 Reverse Comet")          X(12,"W12 Double Knight Rider") \
 X(13,"W13 Twin Comet Collision") X(14,"W14 Triple Race")            X(15,"W15 Center-Out Turbo")        X(16,"W16 Center Lock-In") \
 X(17,"W17 Triple Dash Race")     X(18,"W18 Pixel Race")              X(19,"W19 Split Lane")              X(20,"W20 Meteor Strike") \
 X(21,"W21 Luxury Breath")        X(22,"W22 Double Heartbeat")        X(23,"W23 Soft Arrival")            X(24,"W24 Heartbeat Arrival") \
 X(25,"W25 Triple Flash Show")    X(26,"W26 Pixel Ignition Pro")       X(27,"W27 Double Ripple")           X(28,"W28 Spectrum Ignition") \
 X(29,"W29 Reverse Spectrum")     X(30,"W30 Liquid Chrome")            X(31,"W31 Aurora Curtain")          X(32,"W32 Wave Interference") \
 X(33,"W33 Color Laser")          X(34,"W34 Star Dust")                X(35,"W35 Glitter Comet")           X(36,"W36 Spark Ignition") \
 X(37,"W37 Firefly Parade")       X(38,"W38 8-Cell Show")              X(39,"W39 12-Cell Domino")           X(40,"W40 Segment Scanner") \
 X(41,"W41 Alternating Show")     X(42,"W42 Triple Strobe Arrival")   X(43,"W43 Center Blast")            X(44,"W44 Twin Lock Beam") \
 X(45,"W45 MIZUMA Signature")     X(46,"W46 Particle Symphony")        X(47,"W47 Rainbow Pacman Run")      X(48,"W48 Firework Cascade") \
 X(49,"W49 Tetrix Aurora")        X(50,"W50 MIZUMA DMX Showcase")

#define MZW_DATA(num, name) static const char mzw_d##num[] PROGMEM = name "@!,Lebar;!,!;!;1;sx=128,ix=160";
MZW_LIST(MZW_DATA)
#undef MZW_DATA

static int mizumaFindEffectId(const char* prefix) {
  const size_t L = strlen(prefix);
  for (uint16_t i = 1; i < strip.getModeCount(); i++) {
    const char* d = strip.getModeData(i);
    if (d && strncmp_P(prefix, d, L) == 0) return (int)i;
  }
  return -1;
}

// Call once in setup(), before the boot preset is applied.
static uint8_t mizumaRegisterWelcomeEffects() {
  uint8_t ok = 0;
  #define MZW_REG(num, name) if (strip.addEffect(255, &mzwModeFn<num>, mzw_d##num) != 255) ok++;
  MZW_LIST(MZW_REG)
  #undef MZW_REG
  return ok;
}
#endif // WLED
