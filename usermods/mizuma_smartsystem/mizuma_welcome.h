// =====================================================================================================
// MIZUMA WELCOMING ENGINE — PREMIUM 45 MODE (W01..W45)
// -----------------------------------------------------------------------------------------------------
// Drop-in replacement for the previous mizuma_welcome.h.
// Interface intentionally kept compatible with the existing .cpp:
//   MZW_COUNT, mizumaRegisterWelcomeEffects(), mizumaFindEffectId(), mzwModeFn<M>()
//   and effect IDs W01..W45 are unchanged.
//
// Design goals:
// - 45 visibly different automotive welcome patterns (sequential, scanner, comet, burst,
//   chase, strobe, ripple, palette flow and finale).
// - Integer pixel calculations; no floating point in the frame loop.
// - Uses the WLED segment palette for foreground and segment BG (color slot 2) for background.
// - Pixel 0 = outer eyebrow tip, last pixel = inner tip. Use WLED Reverse if wiring differs.
// - Speed controls cycle duration (default sx=128 ~= 6.9 seconds); intensity controls trail/width.
// - Animation timing starts at SEGENV.call == 0 so left/right segments remain synchronized.
// =====================================================================================================
#pragma once
#include <stdint.h>

#define MZW_COUNT 45
#define MZW_U     4096
#define MZW_F(x)  ((int)((x) * 4096.0 + 0.5))

struct MzwPx  { uint8_t v; uint8_t pos; };
struct MzwCtx { int mode; int n; int u; int wd; int soft; int env; };

// ---------- integer helpers ----------
static inline int mzw_cl(int x) { return x < 0 ? 0 : (x > 255 ? 255 : x); }
static inline int mzw_ramp(int x, int a, int b) {
  if (b <= a) return x >= a ? 255 : 0;
  return mzw_cl(((x - a) * 255) / (b - a));
}
static inline int mzw_tri(int x) {
  x &= 4095;
  return x < 2048 ? (x >> 3) : ((4095 - x) >> 3);
}
static inline int mzw_smooth(int s) { return (s * s * (765 - 2 * s)) / 65025; }
static inline int mzw_abs(int x) { return x < 0 ? -x : x; }
static inline int mzw_mx(int a, int b) { return a > b ? a : b; }
static inline int mzw_mn(int a, int b) { return a < b ? a : b; }
static inline int mzw_peak(int distance, int width) {
  return distance >= width ? 0 : mzw_smooth(255 - (distance * 255) / (width ? width : 1));
}
static inline uint8_t mzw_h(uint32_t x) {
  x ^= x >> 15; x *= 0x2c1b3c6dU; x ^= x >> 12; x *= 0x297a2d39U; x ^= x >> 15;
  return (uint8_t)x;
}
static inline int mzw_comet(int head, int x, int tail) {
  int d = head - x;
  if (d < 0 || d > tail) return 0;
  int a = 255 - (d * 255) / (tail ? tail : 1);
  return (a * a) / 255;
}
static inline int mzw_cometRing(int head, int x, int tail) {
  int d = (head - x) & 4095;
  if (d > tail) return 0;
  int a = 255 - (d * 255) / (tail ? tail : 1);
  return (a * a) / 255;
}
static inline int mzw_fillV(int x, int u, int a, int b, int soft) {
  int uu = u < a ? a : (u > b ? b : u);
  int front = -soft + (((uu - a) * (MZW_U + 2 * soft)) / (b > a ? (b - a) : 1));
  return mzw_ramp(front - x, 0, soft);
}
static inline int mzw_gate(int u, int a, int b) { return (u >= a && u < b) ? 255 : 0; }
static inline int mzw_flash(int u, int period, int on) { return (u % period) < on ? 255 : 0; }
static inline int mzw_bounce(int u, int loops) {
  int p = (u * loops) & 4095;
  return p < 2048 ? p * 2 : (4095 - p) * 2;
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
  c.wd = 300 + ((int)intensity * 1500) / 255;   // trail/width: 300..1800 Q12
  c.soft = 100 + c.wd / 9;
  int fin = mzw_ramp(c.u, 0, MZW_F(0.025));
  int fout = 255 - mzw_ramp(c.u, MZW_F(0.965), MZW_U);
  c.env = (fin * fout) / 255;
}

// ---------- one pixel renderer ----------
static inline MzwPx mzwPixel(const MzwCtx &c, int i) {
  const int u = c.u, wd = c.wd, soft = c.soft;
  const int x  = c.n > 1 ? (int)(((uint32_t)i * 4096U) / (uint32_t)(c.n - 1)) : 0;
  const int xr = MZW_U - x;
  const int q  = mzw_abs(2 * x - MZW_U);       // center=0, tips=4096
  const int qr = MZW_U - q;                   // tips=0, center=4096
  const int tail = wd < 420 ? 420 : wd;
  int v = 0;
  int pos = (x >> 4);

  switch (c.mode) {
    // ===== 01-09 : PREMIUM SEQUENTIAL / IGNITION =====
    case 1: // clean outer-to-inner ignition
      v = mzw_fillV(x, u, 0, MZW_F(0.48), soft + 45);
      if (u > MZW_F(0.48)) v = 255;
      break;
    case 2: // clean inner-to-outer ignition
      v = mzw_fillV(xr, u, 0, MZW_F(0.48), soft + 45);
      if (u > MZW_F(0.48)) v = 255;
      break;
    case 3: // center split / outward ignition
      v = mzw_fillV(q, u, 0, MZW_F(0.48), soft + 40);
      if (u > MZW_F(0.48)) v = 255;
      break;
    case 4: // both tips meet in the center
      v = mzw_fillV(qr, u, 0, MZW_F(0.48), soft + 40);
      if (u > MZW_F(0.48)) v = 255;
      break;
    case 5: { // sharp scanner sweep with a luminous tail
      int head = (u * (MZW_U + tail)) / MZW_U;
      v = mzw_mx(mzw_comet(head, x, tail), mzw_comet(head - 260, x, tail / 2));
      if (u > MZW_F(0.60)) v = mzw_mx(v, 65);
    } break;
    case 6: { // double curtain: two beams travel from tips to center
      int h = (u * (MZW_U + tail)) / MZW_U;
      v = mzw_mx(mzw_comet(h, q, tail), mzw_comet(h - 160, q, tail / 2));
    } break;
    case 7: { // sweep, lock-on, then three confirmation flashes
      if (u < MZW_F(0.54)) v = mzw_fillV(x, u, 0, MZW_F(0.54), soft);
      else if (u < MZW_F(0.68)) v = 255;
      else v = mzw_flash(u - MZW_F(0.68), MZW_F(0.085), MZW_F(0.052));
    } break;
    case 8: // 6 large blocks, sequential ignition
    case 9: { // 12 fine blocks, sequential ignition
      int blocks = c.mode == 8 ? 6 : 12;
      int blk = (x * blocks) >> 12;
      if (blk >= blocks) blk = blocks - 1;
      int local = u * blocks;
      int stage = local / MZW_F(0.58);
      int frac = local % MZW_F(0.58);
      if (blk < stage) v = 125 + (blk * 130) / (blocks - 1);
      else if (blk == stage && frac < MZW_F(0.58)) v = 255;
      else v = 0;
      if (u > MZW_F(0.62)) v = 255;
    } break;

    // ===== 10-20 : COMET / SCANNER / CHASE =====
    case 10: { // single comet outer -> inner
      int h = (u * (MZW_U + tail)) / MZW_U;
      v = mzw_comet(h, x, tail);
    } break;
    case 11: { // single comet inner -> outer
      int h = (u * (MZW_U + tail)) / MZW_U;
      v = mzw_comet(h, xr, tail);
    } break;
    case 12: { // classic Knight Rider scanner with a bright head
      int p = mzw_bounce(u, 2);
      int d = mzw_abs(x - p);
      v = mzw_mx(mzw_peak(d, tail + 250), mzw_peak(d, 110));
    } break;
    case 13: { // twin comets, opposite directions
      int h = (u * (MZW_U + tail)) / MZW_U;
      v = mzw_mx(mzw_comet(h, x, tail), mzw_comet(h, xr, tail));
    } break;
    case 14: { // triple racing comets
      int h = (u * (MZW_U + tail)) / MZW_U;
      int tl = tail * 3 / 4;
      v = mzw_mx(mzw_comet(h, x, tl), mzw_mx(mzw_comet(h - 1365, x, tl), mzw_comet(h - 2730, x, tl)));
    } break;
    case 15: { // twin comets expanding from center to tips
      int h = (u * (MZW_U + tail)) / MZW_U;
      v = mzw_comet(h, q, tail);
    } break;
    case 16: { // twin comets converging from tips to center
      int h = (u * (MZW_U + tail)) / MZW_U;
      v = mzw_comet(h, qr, tail);
    } break;
    case 17: { // three slim racing dashes
      int p1 = (u * 3) & 4095;
      int p2 = (p1 + 1365) & 4095;
      int p3 = (p1 + 2730) & 4095;
      int w = wd / 3 + 160;
      v = mzw_mx(mzw_peak(mzw_abs(x - p1), w), mzw_mx(mzw_peak(mzw_abs(x - p2), w), mzw_peak(mzw_abs(x - p3), w)));
    } break;
    case 18: { // dense short dash chase
      int phase = (x * 5 - u * 7) & 4095;
      int band = mzw_tri(phase);
      v = mzw_ramp(band, 105, 220);
    } break;
    case 19: { // alternating forward/reverse fast chase
      int phase = (u < 2048) ? (x * 8 - u * 11) : (x * 8 + u * 11);
      v = mzw_ramp(mzw_tri(phase), 80, 205);
    } break;
    case 20: { // long meteor with irregular sparkling tail
      int h = (u * (MZW_U + tail + 900)) / MZW_U;
      int m = mzw_comet(h, x, tail + 900);
      uint8_t r = mzw_h((uint32_t)i * 37U + (uint32_t)(u >> 4) * 97U);
      v = (m * (180 + (r % 76))) / 255;
    } break;

    // ===== 21-27 : BREATH / PULSE / RIPPLE =====
    case 21: // slow luxury breathing
      v = 22 + (mzw_smooth(mzw_tri(u * 2)) * 233) / 255;
      break;
    case 22: // fast double breathing
      v = 18 + (mzw_smooth(mzw_tri(u * 4)) * 237) / 255;
      break;
    case 23: { // one-shot soft fade-up, then hold
      int a = mzw_ramp(u, 0, MZW_F(0.42));
      v = mzw_smooth(a);
    } break;
    case 24: { // heartbeat double-pulse
      int p = (u * 2) & 4095;
      int a = (p < 670) ? mzw_peak(mzw_abs(p - 210), 210) : 0;
      int b = (p > 800 && p < 1510) ? (mzw_peak(mzw_abs(p - 1120), 260) * 190) / 255 : 0;
      v = mzw_mx(12, mzw_mx(a, b));
    } break;
    case 25: { // three crisp full-strip welcome flashes, then steady glow
      if (u < MZW_F(0.48)) v = mzw_flash(u, MZW_F(0.16), MZW_F(0.075));
      else v = 230;
    } break;
    case 26: { // sequential pixel ignition with a short afterglow
      int delay = (x * MZW_F(0.52)) / MZW_U;
      int d = u - delay;
      if (d < 0) v = 0;
      else if (d < MZW_F(0.06)) v = (d < MZW_F(0.025)) ? 255 : 95;
      else v = 180;
    } break;
    case 27: { // center-out expanding ripple rings
      int front = (u * 2) & 4095;
      int d = mzw_abs(q - front);
      v = mzw_peak(d, 230 + (wd >> 2));
    } break;

    // ===== 28-33 : PALETTE FLOW / AURORA =====
    case 28: // flowing spectrum, forward
      pos = ((x >> 4) + (u >> 3)) & 255;
      v = 185 + (mzw_tri((x * 2 - u * 3)) * 70) / 255;
      break;
    case 29: // flowing spectrum, reverse
      pos = ((x >> 4) - (u >> 3)) & 255;
      v = 185 + (mzw_tri((x * 2 + u * 3)) * 70) / 255;
      break;
    case 30: { // wide liquid color bands
      int ph = (x * 3 - u * 4) & 4095;
      pos = (ph >> 4) & 255;
      v = 105 + (mzw_smooth(mzw_tri(ph)) * 150) / 255;
    } break;
    case 31: { // aurora curtain, two drifting waves
      int a = mzw_smooth(mzw_tri(x * 2 - u * 3));
      int b = mzw_smooth(mzw_tri(x * 3 + u * 2));
      pos = ((x >> 4) + (u >> 4) + (b >> 2)) & 255;
      v = 28 + (a * 175) / 255 + (b * 52) / 255;
    } break;
    case 32: { // interference / crossing waves
      int a = mzw_tri(x * 4 - u * 5);
      int b = mzw_tri(x * 3 + u * 4);
      int mix = (a * b) / 255;
      pos = ((x >> 4) + (a >> 2) - (u >> 3)) & 255;
      v = 20 + (mix * 235) / 255;
    } break;
    case 33: { // luminous color sweep with a moving hot spot
      int h = (u * (MZW_U + tail)) / MZW_U;
      int beam = mzw_comet(h, x, tail * 2);
      pos = ((x >> 4) + (u >> 4) * 3) & 255;
      v = 65 + (beam * 190) / 255;
    } break;

    // ===== 34-39 : GLITTER / SPARK / PIXEL CHASE =====
    case 34: { // fine random twinkle
      uint32_t step = (uint32_t)(u >> 5);
      uint8_t h = mzw_h((uint32_t)i * 131U + step * 977U);
      v = (h > 222) ? 255 : (h > 184 ? 70 : 8);
      pos = mzw_h((uint32_t)i * 17U + step * 31U);
    } break;
    case 35: { // glitter burst moving behind a comet
      int h = (u * (MZW_U + tail)) / MZW_U;
      int comet = mzw_comet(h, x, tail + 650);
      uint8_t r = mzw_h((uint32_t)i * 113U + (uint32_t)(u >> 4) * 71U);
      v = mzw_mx(comet, (r > 242 && comet > 35) ? 255 : 0);
      pos = (x >> 4) + (r >> 2);
    } break;
    case 36: { // sequential fill with sparkling leading edge
      int f = mzw_fillV(x, u, 0, MZW_F(0.56), soft);
      v = (f * 175) / 255;
      uint8_t r = mzw_h((uint32_t)i * 29U + (uint32_t)(u >> 4) * 53U);
      if (f > 150 && r > 225) v = 255;
    } break;
    case 37: { // random firefly shimmer with palette variation
      uint8_t h = mzw_h((uint32_t)i * 13U + 7U);
      int s = mzw_smooth(mzw_tri(u * 3 + h * 16));
      v = (s * s) / 255;
      pos = h;
    } break;
    case 38: { // 8-cell progressive block chase
      int blk = (x * 8) >> 12; if (blk > 7) blk = 7;
      int step = (u * 8) / MZW_U;
      int d = (step - blk + 8) & 7;
      v = d == 0 ? 255 : (d == 1 ? 95 : (d == 2 ? 28 : 0));
    } break;
    case 39: { // 12-cell domino wave with fading trail
      int blk = (x * 12) >> 12; if (blk > 11) blk = 11;
      int step = (u * 12) / MZW_U;
      int d = (step - blk + 12) % 12;
      v = d == 0 ? 255 : (d == 1 ? 180 : (d == 2 ? 90 : (d == 3 ? 28 : 0)));
    } break;

    // ===== 40-45 : SHOW / STROBE / SIGNATURE FINALE =====
    case 40: { // bouncing segmented scanner
      int blk = (x * 8) >> 12; if (blk > 7) blk = 7;
      int head = (mzw_bounce(u, 2) * 8) >> 12;
      int d = mzw_abs(blk - head);
      v = d == 0 ? 255 : (d == 1 ? 115 : 12);
    } break;
    case 41: { // alternating odd/even pixel groups
      int group = (i & 1) ? 0 : 1;
      int phase = (u * 8) & 4095;
      int on = ((phase >> 10) & 1) == group;
      v = on ? 255 : 18;
    } break;
    case 42: { // triple strobe burst with short dark gaps
      int p = u % MZW_F(0.30);
      v = (p < MZW_F(0.055) || (p > MZW_F(0.085) && p < MZW_F(0.14)) ||
           (p > MZW_F(0.17) && p < MZW_F(0.225))) ? 255 : 0;
    } break;
    case 43: { // center blast / radial burst to both tips
      int front = (u * 2) & 4095;
      int d = mzw_abs(q - front);
      int ring = mzw_peak(d, 300 + (wd >> 2));
      int fill = mzw_ramp(front - q, 0, 230);
      v = mzw_mx(ring, (fill * 95) / 255);
    } break;
    case 44: { // two outer beams converge and lock in center
      int front = (u * (MZW_U + tail)) / MZW_U;
      int beam = mzw_mx(mzw_comet(front, xr, tail), mzw_comet(front, x, tail));
      if (u > MZW_F(0.66)) v = 210;
      else v = beam;
    } break;
    case 45: { // MIZUMA signature: sweep -> double flash -> premium hold
      if (u < MZW_F(0.42)) {
        int h = (u * (MZW_U + tail)) / MZW_F(0.42);
        v = mzw_mx(mzw_comet(h, x, tail + 380), mzw_comet(h - 170, x, tail / 2));
      } else if (u < MZW_F(0.53)) {
        v = 255;
      } else if (u < MZW_F(0.78)) {
        int p = (u - MZW_F(0.53)) % MZW_F(0.125);
        v = (p < MZW_F(0.045)) ? 255 : 72;
      } else {
        int breathe = mzw_smooth(mzw_tri((u - MZW_F(0.78)) * 5));
        v = 170 + (breathe * 85) / 255;
      }
    } break;
    default:
      v = 0;
      break;
  }

  v = (mzw_cl(v) * c.env) / 255;
  MzwPx o;
  o.v = (uint8_t)v;
  o.pos = (uint8_t)(pos & 255);
  return o;
}

// =====================================================================================================
// WLED integration — deliberately unchanged so the existing .cpp needs no edits.
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
    const MzwPx p = mzwPixel(c, i);
    const uint32_t fg = SEGMENT.color_from_palette(p.pos, false, true, 0, 255);
    SEGMENT.setPixelColor(i, p.v == 255 ? fg : (p.v == 0 ? bg : color_blend(bg, fg, p.v)));
  }
}
template <uint8_t M> static void mzwModeFn() { mzwRun(M); }

// Names keep the W01..W45 prefixes so existing .cpp preset/effect lookup remains valid.
// WLED effect data format: Name@slider;colors;palette;flags;defaults
#define MZW_LIST(X) \
 X(1,"W01 Ignite Outer-Inner")    X(2,"W02 Ignite Inner-Outer")    X(3,"W03 Center Expansion")      X(4,"W04 Tips Converge") \
 X(5,"W05 Laser Sweep")           X(6,"W06 Twin Curtain")          X(7,"W07 Sweep Lock Flash")      X(8,"W08 Block Ignite 6") \
 X(9,"W09 Block Ignite 12")       X(10,"W10 Comet Forward")       X(11,"W11 Comet Reverse")        X(12,"W12 Knight Rider Pro") \
 X(13,"W13 Twin Comet")           X(14,"W14 Triple Comet")         X(15,"W15 Center Comets Out")     X(16,"W16 Tips Comets In") \
 X(17,"W17 Triple Dash Race")     X(18,"W18 Dense Dash Chase")     X(19,"W19 Flip Speed Chase")     X(20,"W20 Meteor Glitter") \
 X(21,"W21 Luxury Breathing")     X(22,"W22 Double Breathing")     X(23,"W23 Soft Fade Hold")        X(24,"W24 Heartbeat Double") \
 X(25,"W25 Triple Flash Hold")    X(26,"W26 Pixel Ignition")       X(27,"W27 Center Ripple")         X(28,"W28 Spectrum Flow") \
 X(29,"W29 Reverse Spectrum")     X(30,"W30 Liquid Color Bands")   X(31,"W31 Aurora Flow")           X(32,"W32 Wave Interference") \
 X(33,"W33 Color Laser")          X(34,"W34 Random Twinkle")       X(35,"W35 Glitter Comet")         X(36,"W36 Sparkle Ignite") \
 X(37,"W37 Firefly Shimmer")      X(38,"W38 8 Cell Chase")         X(39,"W39 12 Cell Domino")        X(40,"W40 Segment Scanner") \
 X(41,"W41 Alternating Strobe")   X(42,"W42 Triple Strobe")        X(43,"W43 Center Blast")          X(44,"W44 Twin Lock Beam") \
 X(45,"W45 MIZUMA Signature")

#define MZW_DATA(num, name) static const char mzw_d##num[] PROGMEM = name "@!,Lebar;!,!;!;1;sx=128,ix=160";
MZW_LIST(MZW_DATA)
#undef MZW_DATA

// Find effect ID by prefix, e.g. "W45 ". -1 if not found.
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
