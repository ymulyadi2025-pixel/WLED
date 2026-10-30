// =====================================================================================================
// MIZUMA WELCOMING ENGINE — 50 CHOREOGRAPHED MODES (W01..W50)
// -----------------------------------------------------------------------------------------------------
// Drop-in replacement for mizuma_welcome.h. Public interface kept compatible:
//   MZW_COUNT, mizumaRegisterWelcomeEffects(), mizumaFindEffectId(), mzwModeFn<M>()
//   W01..W50 are registered by this header; existing .cpp integration stays unchanged.
//
// DESIGN:
// - 20 FAST modes (sharp chases / flashes), 20 FLOW modes (smooth visual transitions),
//   and 10 HYBRID modes (smooth arrival + short high-energy accent).
// - Listing order alternates by pairs: FAST x2 -> FLOW x2 -> HYBRID x2, then repeats;
//   after the 10 HYBRID modes are used, FAST and FLOW continue alternating by pairs.
// - Every mode is a multi-stage welcome animation, not a single continuously-running effect.
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

// Every two listings change character: FAST x2, FLOW x2, HYBRID x2.
// kind: 0=fast, 1=flowing beauty, 2=hybrid. rank is 0-based within its family.
static const uint8_t mzwKind[50] = {
  0,0, 1,1, 2,2, 0,0, 1,1, 2,2, 0,0, 1,1, 2,2, 0,0,
  1,1, 2,2, 0,0, 1,1, 2,2, 0,0, 1,1, 0,0, 1,1, 0,0,
  1,1, 0,0, 1,1, 0,0, 1,1
};
static const uint8_t mzwRank[50] = {
  0,1, 0,1, 0,1, 2,3, 2,3, 2,3, 4,5, 4,5, 4,5, 6,7,
  6,7, 6,7, 8,9, 8,9, 8,9, 10,11, 10,11, 12,13, 12,13, 14,15,
  14,15, 16,17, 16,17, 18,19, 18,19
};

// ---------- one-pixel renderer: 20 FAST + 20 FLOW + 10 HYBRID ----------
static inline MzwPx mzwPixel(const MzwCtx &c, int i) {
  const int u = c.u, wd = c.wd, soft = c.soft;
  const int x = c.n > 1 ? (int)(((uint32_t)i * 4096U) / (uint32_t)(c.n - 1)) : 0;
  const int xr = MZW_U - x;
  const int q = mzw_abs(2 * x - MZW_U); // center -> tips
  const int tail = wd < 460 ? 460 : wd;
  const uint8_t kind = mzwKind[(c.mode >= 1 && c.mode <= 50) ? c.mode - 1 : 0];
  const uint8_t k = mzwRank[(c.mode >= 1 && c.mode <= 50) ? c.mode - 1 : 0];
  int v = 0, pos = (x >> 4), p = -1;

  if (kind == 0) {
    // ==================== FAST SHOW / SHARP BLINKS ====================
    switch (k) {
      case 0: // FAST Twin Flash: sweep -> 5-shot pulse -> sharp lock
        if ((p=mzw_phase(u,0,MZW_F(.43)))>=0) v=mzw_mx(mzw_scanner(x,p,4,145),mzw_sweep(x,p,tail,0));
        else if ((p=mzw_phase(u,MZW_F(.43),MZW_F(.78)))>=0) v=mzw_mx(mzw_flash(p,5,900),mzw_scanner(x,p,2,100));
        else { p=mzw_phase(u,MZW_F(.78),MZW_U); v=mzw_mx(205,mzw_flash(p,3,600)); }
        break;
      case 1: // FAST Split Strobe: alternating halves -> counter-sweep -> strobe
        if ((p=mzw_phase(u,0,MZW_F(.32)))>=0) { int ph=(p*8)&4095; int blk=(x*8)>>12; v=(((blk+(ph>>9))&1)?255:12); }
        else if ((p=mzw_phase(u,MZW_F(.32),MZW_F(.68)))>=0) v=mzw_mx(mzw_dualSweep(x,p,tail/2),mzw_flash(p,4,780));
        else { p=mzw_phase(u,MZW_F(.68),MZW_U); v=mzw_flash(p,5,850); }
        break;
      case 2: // FAST Triple Comet: 3 runners -> sparkle strike -> flash lock
        if ((p=mzw_phase(u,0,MZW_F(.58)))>=0) {
          v=mzw_mx(mzw_sweep(x,p,tail,0),mzw_mx(mzw_sweep(x,(p+1365)&4095,tail,0),mzw_sweep(x,(p+2730)&4095,tail,0)));
        } else if ((p=mzw_phase(u,MZW_F(.58),MZW_F(.82)))>=0) v=mzw_mx(mzw_psStarburst(x,p,13),mzw_flash(p,4,700));
        else { p=mzw_phase(u,MZW_F(.82),MZW_U); v=mzw_flash(p,3,760); }
        break;
      case 3: // FAST Knight Rider X: multi-scan -> cross beams -> pulse
        if ((p=mzw_phase(u,0,MZW_F(.48)))>=0) v=mzw_scanner(x,p,5,125);
        else if ((p=mzw_phase(u,MZW_F(.48),MZW_F(.78)))>=0) v=mzw_mx(mzw_scanner(x,p,3,210),mzw_scanner(xr,4095-p,3,110));
        else { p=mzw_phase(u,MZW_F(.78),MZW_U); v=mzw_mx(mzw_pulsePair(p),mzw_flash(p,3,650)); }
        break;
      case 4: // FAST Digital Matrix: rapid blocks -> scan burst -> hard hold
        if ((p=mzw_phase(u,0,MZW_F(.45)))>=0) v=mzw_blocks(x,p,12,0,1);
        else if ((p=mzw_phase(u,MZW_F(.45),MZW_F(.79)))>=0) v=mzw_mx(mzw_scanner(x,p,4,155),mzw_flash(p,4,730));
        else v=235;
        break;
      case 5: // FAST Pacman Run: chomping chase -> rainbow pellets -> strobe
        if ((p=mzw_phase(u,0,MZW_F(.62)))>=0) { v=mzw_mx(mzw_pacman(x,p,tail,0),mzw_chaseRainbow(x,p,tail/2,0,pos)); }
        else if ((p=mzw_phase(u,MZW_F(.62),MZW_F(.82)))>=0) v=mzw_mx(mzw_colorTwinkles(i,p,120,31,pos),mzw_flash(p,4,780));
        else { p=mzw_phase(u,MZW_F(.82),MZW_U); v=mzw_flash(p,4,800); }
        break;
      case 6: // FAST Split Lane: odd/even race -> collision -> triple pulse
        if ((p=mzw_phase(u,0,MZW_F(.48)))>=0) {
          int a=mzw_sweep(x,p,tail,0), b=mzw_sweep(xr,p,tail,0);
          v=(i&1)?b:a;
        } else if ((p=mzw_phase(u,MZW_F(.48),MZW_F(.76)))>=0) v=mzw_mx(mzw_peak(q,360),mzw_flash(p,3,720));
        else { p=mzw_phase(u,MZW_F(.76),MZW_U); v=mzw_flash(p,4,820); }
        break;
      case 7: // FAST Quad Beam Race: four staggered beams -> scanner burst
        if ((p=mzw_phase(u,0,MZW_F(.67)))>=0) {
          int a=mzw_sweep(x,p,tail,0), b=mzw_sweep(x,(p+1024)&4095,tail,1);
          int d=mzw_sweep(x,(p+2048)&4095,tail,0), e=mzw_sweep(x,(p+3072)&4095,tail,1);
          v=mzw_mx(mzw_mx(a,b),mzw_mx(d,e));
        } else { p=mzw_phase(u,MZW_F(.67),MZW_U); v=mzw_mx(mzw_scanner(x,p,4,125),mzw_flash(p,4,800)); }
        break;
      case 8: // FAST Heartbeat Salvo: double pulse -> rapid flash -> hard lock
        if ((p=mzw_phase(u,0,MZW_F(.56)))>=0) v=mzw_mx(mzw_pulsePair(p),mzw_flash(p,5,560));
        else if ((p=mzw_phase(u,MZW_F(.56),MZW_F(.82)))>=0) v=mzw_mx(220,mzw_flash(p,4,820));
        else v=235;
        break;
      case 9: // FAST Meteor Impact: comet -> particle blast -> repeated impact
        if ((p=mzw_phase(u,0,MZW_F(.52)))>=0) v=mzw_sweep(x,p,tail*3,0);
        else if ((p=mzw_phase(u,MZW_F(.52),MZW_F(.81)))>=0) v=mzw_mx(mzw_fireworks1D(x,p,47),mzw_psSpray1D(x,p,115,0));
        else { p=mzw_phase(u,MZW_F(.81),MZW_U); v=mzw_flash(p,5,780); }
        break;
      case 10: // FAST Rainbow Chase: high-speed palette runner -> twin scan -> strobe
        if ((p=mzw_phase(u,0,MZW_F(.58)))>=0) v=mzw_mx(mzw_chaseRainbow(x,p,tail,0,pos),mzw_sinelonDual(x,p,tail));
        else if ((p=mzw_phase(u,MZW_F(.58),MZW_F(.82)))>=0) { pos=((x>>4)-(p>>1))&255; v=mzw_mx(mzw_scanner(x,p,3,130),mzw_flash(p,4,700)); }
        else { p=mzw_phase(u,MZW_F(.82),MZW_U); v=mzw_flash(p,4,850); }
        break;
      case 11: // FAST Color Twinkle: dense spark -> dissolve chase -> flash finale
        if ((p=mzw_phase(u,0,MZW_F(.48)))>=0) v=mzw_colorTwinkles(i,p,150,59,pos);
        else if ((p=mzw_phase(u,MZW_F(.48),MZW_F(.78)))>=0) v=mzw_mx(mzw_dissolve(i,p,61,0),mzw_flash(p,4,740));
        else { p=mzw_phase(u,MZW_F(.78),MZW_U); v=mzw_flash(p,5,830); }
        break;
      case 12: // FAST Domino Rush: segmented cascade -> reverse chase -> strobe
        if ((p=mzw_phase(u,0,MZW_F(.42)))>=0) v=mzw_blocks(x,p,12,0,1);
        else if ((p=mzw_phase(u,MZW_F(.42),MZW_F(.76)))>=0) v=mzw_mx(mzw_blocks(x,p,8,1,2),mzw_scanner(x,p,3,145));
        else { p=mzw_phase(u,MZW_F(.76),MZW_U); v=mzw_flash(p,5,820); }
        break;
      case 13: // FAST Firework Burst: quick starbursts -> flash rain -> hard finish
        if ((p=mzw_phase(u,0,MZW_F(.68)))>=0) v=mzw_mx(mzw_fireworks1D(x,p,73),mzw_psStarburst(x,p,79));
        else if ((p=mzw_phase(u,MZW_F(.68),MZW_F(.86)))>=0) v=mzw_mx(mzw_colorTwinkles(i,p,160,83,pos),mzw_flash(p,4,800));
        else v=245;
        break;
      case 14: // FAST Scanner Assault: opposing scanner -> twin sweep -> impact flash
        if ((p=mzw_phase(u,0,MZW_F(.48)))>=0) v=mzw_mx(mzw_scanner(x,p,4,110),mzw_scanner(xr,p,4,110));
        else if ((p=mzw_phase(u,MZW_F(.48),MZW_F(.78)))>=0) v=mzw_dualSweep(x,p,tail/2);
        else { p=mzw_phase(u,MZW_F(.78),MZW_U); v=mzw_flash(p,5,840); }
        break;
      case 15: // FAST Alternating Strobe: odd/even flashes -> split beam -> triple flash
        if ((p=mzw_phase(u,0,MZW_F(.38)))>=0) { int z=(p*10)&4095; v=((((i&1)?z+2048:z)&4095)<1900)?255:0; }
        else if ((p=mzw_phase(u,MZW_F(.38),MZW_F(.72)))>=0) v=mzw_mx(mzw_sweep(x,p,tail,0),mzw_sweep(x,p,tail,1));
        else { p=mzw_phase(u,MZW_F(.72),MZW_U); v=mzw_flash(p,5,850); }
        break;
      case 16: // FAST Starburst Punch: starburst -> scanner -> pulse salvo
        if ((p=mzw_phase(u,0,MZW_F(.55)))>=0) v=mzw_psStarburst(x,p,97);
        else if ((p=mzw_phase(u,MZW_F(.55),MZW_F(.80)))>=0) v=mzw_mx(mzw_scanner(x,p,4,130),mzw_fireworks1D(x,p,101));
        else { p=mzw_phase(u,MZW_F(.80),MZW_U); v=mzw_flash(p,5,850); }
        break;
      case 17: // FAST Comet Barrage: four staggered comets -> repeated strobe
        if ((p=mzw_phase(u,0,MZW_F(.72)))>=0) {
          int a=mzw_sweep(x,p,tail,0), b=mzw_sweep(x,(p+1024)&4095,tail,0);
          int d=mzw_sweep(x,(p+2048)&4095,tail,1), e=mzw_sweep(x,(p+3072)&4095,tail,1);
          v=mzw_mx(mzw_mx(a,b),mzw_mx(d,e));
        } else { p=mzw_phase(u,MZW_F(.72),MZW_U); v=mzw_flash(p,5,850); }
        break;
      case 18: // FAST Dissolve Spark: random dissolve -> fast chase -> strobe
        if ((p=mzw_phase(u,0,MZW_F(.48)))>=0) v=mzw_dissolve(i,p,109,0);
        else if ((p=mzw_phase(u,MZW_F(.48),MZW_F(.78)))>=0) v=mzw_mx(mzw_sweep(x,p,tail,0),mzw_colorTwinkles(i,p,145,113,pos));
        else { p=mzw_phase(u,MZW_F(.78),MZW_U); v=mzw_flash(p,5,850); }
        break;
      default: // FAST DMX Turbo: all-out scan, particles, flash lock
        if ((p=mzw_phase(u,0,MZW_F(.34)))>=0) v=mzw_mx(mzw_scanner(x,p,5,130),mzw_chaseRainbow(x,p,tail,0,pos));
        else if ((p=mzw_phase(u,MZW_F(.34),MZW_F(.72)))>=0) v=mzw_mx(mzw_fireworks1D(x,p,127),mzw_psSpray1D(x,p,100,0));
        else { p=mzw_phase(u,MZW_F(.72),MZW_U); v=mzw_flash(p,6,850); }
        break;
    }
  } else if (kind == 1) {
    // ==================== FLOW / BEAUTIFUL TRANSITIONS ====================
    switch (k) {
      case 0: // FLOW Velvet Arrival: long fill -> flowing wave -> soft breathing
        if ((p=mzw_phase(u,0,MZW_F(.38)))>=0) v=mzw_fillV(x,p,soft);
        else if ((p=mzw_phase(u,MZW_F(.38),MZW_F(.72)))>=0) { pos=((x>>4)+(p>>4))&255; v=mzw_mx(mzw_waveSins(x,p,2,2,15,180),mzw_sweep(x,p,tail*2,0)/2); }
        else { p=mzw_phase(u,MZW_F(.72),MZW_U); v=mzw_breathe(p,1,115); }
        break;
      case 1: // FLOW Center Bloom: radial fill -> slow expanding ripple -> aura
        if ((p=mzw_phase(u,0,MZW_F(.34)))>=0) v=mzw_fillV(q,p,soft);
        else if ((p=mzw_phase(u,MZW_F(.34),MZW_F(.77)))>=0) v=mzw_mx(mzw_ripple(q,p,260,0),mzw_wave(q,p,2,2,15,110));
        else { p=mzw_phase(u,MZW_F(.77),MZW_U); v=mzw_breathe(p,1,125); }
        break;
      case 2: // FLOW Aurora Silk: layered sinus waves -> color drift -> calm hold
        if ((p=mzw_phase(u,0,MZW_F(.55)))>=0) { pos=((x>>4)+(p>>4))&255; v=mzw_mx(mzw_waveSins(x,p,2,2,15,180),mzw_waveSins(x,4095-p,3,1,0,100)); }
        else { p=mzw_phase(u,MZW_F(.55),MZW_U); pos=((x>>4)+(p>>5))&255; v=mzw_breathe(p,1,110); }
        break;
      case 3: // FLOW Liquid Spectrum: broad color bands -> smooth palette wave -> fade
        if ((p=mzw_phase(u,0,MZW_F(.48)))>=0) { pos=((x>>4)+(p>>5))&255; v=65+mzw_smooth(mzw_tri(x*2-p*2))*170/255; }
        else if ((p=mzw_phase(u,MZW_F(.48),MZW_F(.82)))>=0) { pos=((x>>4)-(p>>5))&255; v=mzw_waveSins(x,p,2,1,50,205); }
        else { p=mzw_phase(u,MZW_F(.82),MZW_U); v=mzw_breathe(p,1,120); }
        break;
      case 4: // FLOW Comet Ballet: one long soft comet -> two slow comets -> shimmer
        if ((p=mzw_phase(u,0,MZW_F(.43)))>=0) v=mzw_sweep(x,p,tail*3,0);
        else if ((p=mzw_phase(u,MZW_F(.43),MZW_F(.78)))>=0) v=mzw_mx(mzw_sweep(x,p,tail*2,0),mzw_sweep(x,p,tail*2,1));
        else { p=mzw_phase(u,MZW_F(.78),MZW_U); v=mzw_shimmer(x,p,2,2,pos); }
        break;
      case 5: // FLOW Dissolve Dream: gradual dissolve -> aurora veil -> breathing
        if ((p=mzw_phase(u,0,MZW_F(.38)))>=0) v=mzw_dissolve(i,p,137,0);
        else if ((p=mzw_phase(u,MZW_F(.38),MZW_F(.78)))>=0) { pos=((x>>4)+(p>>5))&255; v=mzw_mx(mzw_waveSins(x,p,2,2,20,160),mzw_shimmer(x,p,2,2,pos)/2); }
        else { p=mzw_phase(u,MZW_F(.78),MZW_U); v=mzw_breathe(p,1,105); }
        break;
      case 6: // FLOW Waterfall: slow droplets -> expanding water rings -> soft glow
        if ((p=mzw_phase(u,0,MZW_F(.53)))>=0) v=mzw_drip(x,p,tail,149,0);
        else if ((p=mzw_phase(u,MZW_F(.53),MZW_F(.82)))>=0) v=mzw_mx(mzw_ripple(q,p,320,0),mzw_wave(x,p,2,1,0,95));
        else { p=mzw_phase(u,MZW_F(.82),MZW_U); v=mzw_breathe(p,1,115); }
        break;
      case 7: // FLOW Firefly Garden: slow fireflies -> gentle wave -> lingering glow
        if ((p=mzw_phase(u,0,MZW_F(.48)))>=0) { int h=0; v=mzw_colorTwinkles(i,p,62,157,h); pos=h; }
        else if ((p=mzw_phase(u,MZW_F(.48),MZW_F(.82)))>=0) { pos=((x>>4)+(p>>5))&255; v=mzw_mx(mzw_waveSins(x,p,2,2,10,135),mzw_glitter(i,p,32,163)); }
        else { p=mzw_phase(u,MZW_F(.82),MZW_U); v=mzw_breathe(p,1,95); }
        break;
      case 8: // FLOW Rainbow Veil: slow rainbow chase -> silk waves -> soft hold
        if ((p=mzw_phase(u,0,MZW_F(.56)))>=0) { v=mzw_chaseRainbow(x,p,tail*2,0,pos)/2+55; }
        else if ((p=mzw_phase(u,MZW_F(.56),MZW_F(.84)))>=0) { pos=((x>>4)+(p>>5))&255; v=mzw_waveSins(x,p,3,1,35,190); }
        else { p=mzw_phase(u,MZW_F(.84),MZW_U); v=mzw_breathe(p,1,115); }
        break;
      case 9: // FLOW Glass Ripple: layered concentric rings -> shimmer reflection -> hold
        if ((p=mzw_phase(u,0,MZW_F(.58)))>=0) v=mzw_mx(mzw_ripple(q,p,230,0),mzw_ripple(q,p,480,1)/2);
        else if ((p=mzw_phase(u,MZW_F(.58),MZW_F(.83)))>=0) v=mzw_shimmer(x,p,3,2,pos);
        else { p=mzw_phase(u,MZW_F(.83),MZW_U); v=mzw_breathe(p,1,125); }
        break;
      case 10: // FLOW Floating Pearls: bouncing orbs -> slow crossing -> breath
        if ((p=mzw_phase(u,0,MZW_F(.58)))>=0) v=mzw_bouncingBalls(x,p,3,190);
        else if ((p=mzw_phase(u,MZW_F(.58),MZW_F(.84)))>=0) v=mzw_mx(mzw_sinelonDual(x,p,tail*2),mzw_wave(x,p,2,1,0,70));
        else { p=mzw_phase(u,MZW_F(.84),MZW_U); v=mzw_breathe(p,1,105); }
        break;
      case 11: // FLOW Sinelon Dance: paired sinus comets -> gradient wave -> settle
        if ((p=mzw_phase(u,0,MZW_F(.61)))>=0) v=mzw_sinelonDual(x,p,tail*3);
        else if ((p=mzw_phase(u,MZW_F(.61),MZW_F(.84)))>=0) { pos=((x>>4)+(p>>5))&255; v=mzw_waveSins(x,p,2,2,35,175); }
        else { p=mzw_phase(u,MZW_F(.84),MZW_U); v=mzw_breathe(p,1,120); }
        break;
      case 12: // FLOW Starburst Bloom: slow expanding starbursts -> soft sparkle -> aura
        if ((p=mzw_phase(u,0,MZW_F(.62)))>=0) v=mzw_psStarburst(x,p,173);
        else if ((p=mzw_phase(u,MZW_F(.62),MZW_F(.85)))>=0) v=mzw_mx(mzw_glitter(i,p,38,179),mzw_waveSins(x,p,2,1,0,100));
        else { p=mzw_phase(u,MZW_F(.85),MZW_U); v=mzw_breathe(p,1,110); }
        break;
      case 13: // FLOW Shimmer Tide: flowing shimmer -> long ribbon sweep -> gentle hold
        if ((p=mzw_phase(u,0,MZW_F(.62)))>=0) { v=mzw_shimmer(x,p,2,2,pos); }
        else if ((p=mzw_phase(u,MZW_F(.62),MZW_F(.86)))>=0) v=mzw_mx(mzw_sweep(x,p,tail*3,1),mzw_waveSins(x,p,2,1,10,100));
        else { p=mzw_phase(u,MZW_F(.86),MZW_U); v=mzw_breathe(p,1,110); }
        break;
      case 14: // FLOW Aurora Ripple: wide waves -> ripple passes -> color breathing
        if ((p=mzw_phase(u,0,MZW_F(.57)))>=0) { pos=((x>>4)+(p>>5))&255; v=mzw_waveSins(x,p,3,2,25,180); }
        else if ((p=mzw_phase(u,MZW_F(.57),MZW_F(.85)))>=0) v=mzw_mx(mzw_ripple(q,p,300,0),mzw_wave(x,p,2,1,0,90));
        else { p=mzw_phase(u,MZW_F(.85),MZW_U); v=mzw_breathe(p,1,120); }
        break;
      case 15: // FLOW Center-to-Tip: center glow -> outward radial waves -> soft finish
        if ((p=mzw_phase(u,0,MZW_F(.36)))>=0) v=mzw_fillV(q,p,soft*2);
        else if ((p=mzw_phase(u,MZW_F(.36),MZW_F(.82)))>=0) v=mzw_mx(mzw_ripple(q,p,280,0),mzw_ripple(q,p,600,1)/2);
        else { p=mzw_phase(u,MZW_F(.82),MZW_U); v=mzw_breathe(p,1,115); }
        break;
      case 16: // FLOW Crystal Drip: droplets -> reflected trails -> glimmer
        if ((p=mzw_phase(u,0,MZW_F(.58)))>=0) v=mzw_drip(x,p,tail*2,181,1);
        else if ((p=mzw_phase(u,MZW_F(.58),MZW_F(.86)))>=0) { pos=((x>>4)+(p>>5))&255; v=mzw_mx(mzw_shimmer(x,p,2,1,pos),mzw_wave(x,p,2,1,0,90)); }
        else { p=mzw_phase(u,MZW_F(.86),MZW_U); v=mzw_breathe(p,1,105); }
        break;
      case 17: // FLOW Stardust Veil: slow dissolve -> color twinkles -> silk hold
        if ((p=mzw_phase(u,0,MZW_F(.38)))>=0) v=mzw_dissolve(i,p,191,1);
        else if ((p=mzw_phase(u,MZW_F(.38),MZW_F(.82)))>=0) { v=mzw_colorTwinkles(i,p,72,193,pos); }
        else { p=mzw_phase(u,MZW_F(.82),MZW_U); pos=((x>>4)+(p>>5))&255; v=mzw_mx(mzw_breathe(p,1,95),mzw_waveSins(x,p,2,1,0,75)); }
        break;
      case 18: // FLOW Falling Jewels: gentle block cascade -> wave shimmer -> aura
        if ((p=mzw_phase(u,0,MZW_F(.55)))>=0) v=mzw_tetrix(x,p,197);
        else if ((p=mzw_phase(u,MZW_F(.55),MZW_F(.84)))>=0) { pos=((x>>4)+(p>>5))&255; v=mzw_mx(mzw_waveSins(x,p,2,1,10,150),mzw_shimmer(x,p,2,1,pos)/2); }
        else { p=mzw_phase(u,MZW_F(.84),MZW_U); v=mzw_breathe(p,1,115); }
        break;
      default: // FLOW Mizuma Aura: progressive illumination -> aurora -> luxury breathing
        if ((p=mzw_phase(u,0,MZW_F(.32)))>=0) v=mzw_fillV(x,p,soft*2);
        else if ((p=mzw_phase(u,MZW_F(.32),MZW_F(.78)))>=0) { pos=((x>>4)+(p>>5))&255; v=mzw_mx(mzw_waveSins(x,p,2,2,25,190),mzw_sweep(x,p,tail*3,0)/2); }
        else { p=mzw_phase(u,MZW_F(.78),MZW_U); v=mzw_breathe(p,1,130); }
        break;
    }
  } else {
    // ==================== HYBRID / BEAUTY + FAST ACCENT ====================
    switch (k) {
      case 0: // HYBRID Pulse Aurora: soft fill -> quick pulse accent -> aurora lock
        if ((p=mzw_phase(u,0,MZW_F(.42)))>=0) { v=mzw_fillV(x,p,soft); }
        else if ((p=mzw_phase(u,MZW_F(.42),MZW_F(.68)))>=0) v=mzw_mx(mzw_flash(p,3,650),mzw_scanner(x,p,2,110));
        else { p=mzw_phase(u,MZW_F(.68),MZW_U); pos=((x>>4)+(p>>5))&255; v=mzw_breathe(p,1,115); }
        break;
      case 1: // HYBRID Laser Bloom: slow radial bloom -> twin fast laser -> soft finish
        if ((p=mzw_phase(u,0,MZW_F(.42)))>=0) v=mzw_fillV(q,p,soft*2);
        else if ((p=mzw_phase(u,MZW_F(.42),MZW_F(.70)))>=0) v=mzw_mx(mzw_dualSweep(x,p,tail),mzw_flash(p,2,620));
        else { p=mzw_phase(u,MZW_F(.70),MZW_U); v=mzw_breathe(p,1,120); }
        break;
      case 2: // HYBRID Dissolve Chase: soft dissolve -> sharp chase -> shimmer
        if ((p=mzw_phase(u,0,MZW_F(.42)))>=0) v=mzw_dissolve(i,p,211,0);
        else if ((p=mzw_phase(u,MZW_F(.42),MZW_F(.70)))>=0) v=mzw_mx(mzw_sweep(x,p,tail,0),mzw_flash(p,3,600));
        else { p=mzw_phase(u,MZW_F(.70),MZW_U); v=mzw_shimmer(x,p,2,1,pos); }
        break;
      case 3: // HYBRID Ripple Scanner: slow rings -> brief scanner burst -> breathing
        if ((p=mzw_phase(u,0,MZW_F(.44)))>=0) v=mzw_ripple(q,p,260,0);
        else if ((p=mzw_phase(u,MZW_F(.44),MZW_F(.72)))>=0) v=mzw_mx(mzw_scanner(x,p,3,130),mzw_flash(p,3,650));
        else { p=mzw_phase(u,MZW_F(.72),MZW_U); v=mzw_breathe(p,1,120); }
        break;
      case 4: // HYBRID Particle Wave: floating particles -> triple accent -> wave afterglow
        if ((p=mzw_phase(u,0,MZW_F(.46)))>=0) v=mzw_mx(mzw_bouncingBalls(x,p,3,150),mzw_waveSins(x,p,2,2,0,90));
        else if ((p=mzw_phase(u,MZW_F(.46),MZW_F(.72)))>=0) v=mzw_mx(mzw_psStarburst(x,p,223),mzw_flash(p,3,620));
        else { p=mzw_phase(u,MZW_F(.72),MZW_U); v=mzw_waveSins(x,p,2,1,35,155); }
        break;
      case 5: // HYBRID Rainbow Impact: flowing rainbow -> comet rush -> color breathing
        if ((p=mzw_phase(u,0,MZW_F(.43)))>=0) { v=mzw_chaseRainbow(x,p,tail*2,0,pos)/2+50; }
        else if ((p=mzw_phase(u,MZW_F(.43),MZW_F(.72)))>=0) v=mzw_mx(mzw_sweep(x,p,tail,1),mzw_flash(p,3,680));
        else { p=mzw_phase(u,MZW_F(.72),MZW_U); pos=((x>>4)+(p>>5))&255; v=mzw_breathe(p,1,115); }
        break;
      case 6: // HYBRID Waterfall Firework: smooth drips -> firework burst -> soft aura
        if ((p=mzw_phase(u,0,MZW_F(.48)))>=0) v=mzw_drip(x,p,tail,229,0);
        else if ((p=mzw_phase(u,MZW_F(.48),MZW_F(.74)))>=0) v=mzw_mx(mzw_fireworks1D(x,p,233),mzw_flash(p,2,560));
        else { p=mzw_phase(u,MZW_F(.74),MZW_U); v=mzw_breathe(p,1,120); }
        break;
      case 7: // HYBRID Twin Comet Aurora: comet ballet -> fast twin pass -> aurora glow
        if ((p=mzw_phase(u,0,MZW_F(.43)))>=0) v=mzw_sweep(x,p,tail*3,0);
        else if ((p=mzw_phase(u,MZW_F(.43),MZW_F(.72)))>=0) v=mzw_mx(mzw_dualSweep(x,p,tail),mzw_flash(p,2,600));
        else { p=mzw_phase(u,MZW_F(.72),MZW_U); pos=((x>>4)+(p>>5))&255; v=mzw_waveSins(x,p,2,1,35,160); }
        break;
      case 8: // HYBRID Firefly Scanner: slow twinkles -> sharp scanner accent -> shimmer
        if ((p=mzw_phase(u,0,MZW_F(.46)))>=0) v=mzw_colorTwinkles(i,p,72,239,pos);
        else if ((p=mzw_phase(u,MZW_F(.46),MZW_F(.72)))>=0) v=mzw_mx(mzw_scanner(x,p,3,135),mzw_flash(p,3,640));
        else { p=mzw_phase(u,MZW_F(.72),MZW_U); v=mzw_shimmer(x,p,2,1,pos); }
        break;
      default: // HYBRID MIZUMA Signature: fill -> dual laser -> 3-shot accent -> luxury aura
        if ((p=mzw_phase(u,0,MZW_F(.34)))>=0) v=mzw_mx(mzw_fillV(x,p,soft),mzw_waveSins(x,p,2,1,0,85));
        else if ((p=mzw_phase(u,MZW_F(.34),MZW_F(.62)))>=0) v=mzw_mx(mzw_dualSweep(x,p,tail*2),mzw_chaseRainbow(x,p,tail,0,pos));
        else if ((p=mzw_phase(u,MZW_F(.62),MZW_F(.80)))>=0) v=mzw_mx(mzw_flash(p,3,600),mzw_psStarburst(x,p,251));
        else { p=mzw_phase(u,MZW_F(.80),MZW_U); pos=((x>>4)+(p>>5))&255; v=mzw_breathe(p,1,135); }
        break;
    }
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

// WLED effect list order alternates in pairs: FAST x2, FLOW x2, HYBRID x2.
// WLED effect data format: Name@slider;colors;palette;flags;defaults
#define MZW_LIST(X) \
 X(1,"W01 FAST Twin Flash")       X(2,"W02 FAST Split Strobe")      X(3,"W03 FLOW Velvet Arrival")    X(4,"W04 FLOW Center Bloom") \
 X(5,"W05 MIX Pulse Aurora")      X(6,"W06 MIX Laser Bloom")        X(7,"W07 FAST Triple Comet")      X(8,"W08 FAST Knight Rider X") \
 X(9,"W09 FLOW Aurora Silk")      X(10,"W10 FLOW Liquid Spectrum")   X(11,"W11 MIX Dissolve Chase")     X(12,"W12 MIX Ripple Scanner") \
 X(13,"W13 FAST Digital Matrix")  X(14,"W14 FAST Pacman Run")         X(15,"W15 FLOW Comet Ballet")      X(16,"W16 FLOW Dissolve Dream") \
 X(17,"W17 MIX Particle Wave")    X(18,"W18 MIX Rainbow Impact")      X(19,"W19 FAST Split Lane")        X(20,"W20 FAST Quad Beam Race") \
 X(21,"W21 FLOW Waterfall")       X(22,"W22 FLOW Firefly Garden")     X(23,"W23 MIX Waterfall Firework") X(24,"W24 MIX Twin Comet Aurora") \
 X(25,"W25 FAST Heartbeat Salvo") X(26,"W26 FAST Meteor Impact")      X(27,"W27 FLOW Rainbow Veil")      X(28,"W28 FLOW Glass Ripple") \
 X(29,"W29 MIX Firefly Scanner") X(30,"W30 MIX MIZUMA Signature")     X(31,"W31 FAST Rainbow Chase")    X(32,"W32 FAST Color Twinkle") \
 X(33,"W33 FLOW Floating Pearls") X(34,"W34 FLOW Sinelon Dance")      X(35,"W35 FAST Domino Rush")       X(36,"W36 FAST Firework Burst") \
 X(37,"W37 FLOW Starburst Bloom") X(38,"W38 FLOW Shimmer Tide")       X(39,"W39 FAST Scanner Assault")   X(40,"W40 FAST Alternating Strobe") \
 X(41,"W41 FLOW Aurora Ripple")   X(42,"W42 FLOW Center-to-Tip")      X(43,"W43 FAST Starburst Punch")   X(44,"W44 FAST Comet Barrage") \
 X(45,"W45 FLOW Crystal Drip")    X(46,"W46 FLOW Stardust Veil")      X(47,"W47 FAST Dissolve Spark")    X(48,"W48 FAST DMX Turbo") \
 X(49,"W49 FLOW Falling Jewels") X(50,"W50 FLOW Mizuma Aura")

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
