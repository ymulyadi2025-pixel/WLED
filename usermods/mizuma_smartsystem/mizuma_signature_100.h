#pragma once
/*
MIZUMA SIGNATURE 100 — EDISI OTOMOTIF (FINAL v2 + FIX PALETTE)
100 algoritma animasi 1D bertema motor/mobil custom.
Target: WLED 16.x / ESP32-C3.

Kategori (10 × 10):
  0-9   : RPM & Tachometer
  10-19 : Sequential Signal
  20-29 : Turbo & Boost
  30-39 : Exhaust & Fire
  40-49 : Brake & Heat
  50-59 : Race Track
  60-69 : Night Rider
  70-79 : JDM Legend
  80-89 : Premium Signature
  90-99 : Show & Meet

Integrasi:
  #include "mizuma_signature_100.h"
  // di setup() usermod:
  MizumaSignature100::registerWLEDEffect();
*/
#include "wled.h"
#include <stdint.h>
#include <string.h>

namespace MizumaSignature100 {

static const uint8_t EFFECT_COUNT = 100;
static const uint8_t GROUP_COUNT  = 10;

static const char * const EFFECT_GROUPS[GROUP_COUNT] = {
  "RPM & Tachometer ",
  "Sequential Signal ",
  "Turbo & Boost ",
  "Exhaust & Fire ",
  "Brake & Heat ",
  "Race Track ",
  "Night Rider ",
  "JDM Legend ",
  "Premium Signature ",
  "Show & Meet "
};

static const char * const EFFECT_NAMES[EFFECT_COUNT] = {
  // 0-9 RPM & Tachometer
  "Tach Sweep ",         "Redline Pulse ",      "Rev Hang ",
  "Idle Lope ",          "Launch Control ",     "Overrev Flash ",
  "Limp Mode ",          "Cruise Steady ",      "Kickdown ",
  "Top Speed Run ",
  // 10-19 Sequential Signal
  "Audi Signal ",        "BMW Sweep ",          "Merc Cascade ",
  "Lexus Flow ",         "Porsche Pulse ",      "Lambo Slash ",
  "Sequential Dual ",    "Waterfall ",          "Cascade Turn ",
  "Strobe Seq ",
  // 20-29 Turbo & Boost
  "Spool Up ",           "Wastegate ",          "Blowoff ",
  "Boost Creep ",        "Flutter ",            "Diverter ",
  "Turbo Timer ",        "Lag Kick ",           "Boost Gauge ",
  "Twin Scroll ",
  // 30-39 Exhaust & Fire
  "Backfire ",           "Flame Out ",          "Two Step ",
  "Anti Lag ",           "Pops & Bangs ",       "Afterburn ",
  "Cold Start ",         "Warmup ",             "Full Throttle ",
  "Decel Pop ",
  // 40-49 Brake & Heat
  "Disc Glow ",          "Brake Fade ",         "Brake Bias ",
  "ABS Pulse ",          "Brake Light ",        "Emergency Stop ",
  "Trail Brake ",        "Carbon Ceramic ",     "Vented Disc ",
  "Brake Duct ",
  // 50-59 Race Track
  "Start Lights ",       "Pace Lap ",           "Safety Car ",
  "Checkered Flag ",     "Pit Lane ",           "Overtake ",
  "Slipstream ",         "Drag Strip ",         "Burnout ",
  "Podium ",
  // 60-69 Night Rider
  "Knight Rider ",       "Tron Line ",          "Cyber Grid ",
  "Neon Pulse ",         "Synthwave ",          "Vaporwave ",
  "Retrowave ",          "Outrun ",             "Dark Rider ",
  "Shadow Chase ",
  // 70-79 JDM Legend
  "Akira Red ",          "Initial Drift ",      "Wangan Midnight ",
  "Hashiriya ",          "Touge Run ",          "Kanji Pulse ",
  "Rising Sun ",         "Sakura Fall ",        "Bamboo Forest ",
  "Tokyo Neon ",
  // 80-89 Premium Signature
  "Signature DRL ",      "Angel Eye ",          "Demon Eye ",
  "Laser Line ",         "Crystal Flow ",       "Diamond Cut ",
  "Platinum Sweep ",     "Obsidian Glow ",      "Titanium Pulse ",
  "Carbon Weave ",
  // 90-99 Show & Meet
  "RGB Parade ",         "Color Wipe Pro ",     "Strobe Party ",
  "Rave Mode ",          "Crowd Surfer ",       "Mosh Pit ",
  "Encore ",             "Grand Finale ",       "Victory Lap ",
  "Champion "
};

static inline const char *nameAt(uint8_t index) {
  return EFFECT_NAMES[(index < EFFECT_COUNT) ? index : (EFFECT_COUNT - 1U)];
}
static inline const char *groupAt(uint8_t index) {
  const uint8_t safeIndex = (index < EFFECT_COUNT) ? index : (EFFECT_COUNT - 1U);
  return EFFECT_GROUPS[safeIndex / 10U];
}
static inline uint8_t clampIndex(uint8_t index) {
  return (index < EFFECT_COUNT) ? index : (EFFECT_COUNT - 1U);
}

// -----------------------------------------------------------------------------
// Math helpers
// -----------------------------------------------------------------------------
static inline uint8_t min8(uint8_t a, uint8_t b) { return (a < b) ? a : b; }
static inline uint8_t max8(uint8_t a, uint8_t b) { return (a > b) ? a : b; }
static inline uint8_t avg8(uint8_t a, uint8_t b) { return (uint8_t)(((uint16_t)a + b) >> 1); }
static inline uint8_t mul8(uint8_t a, uint8_t b) { return scale8(a, b); }
static inline uint8_t absdiff8(uint8_t a, uint8_t b) { return (a > b) ? (a - b) : (b - a); }
static inline uint8_t dist8(uint8_t a, uint8_t b) {
  uint8_t d = absdiff8(a, b);
  return (d > 127U) ? (uint8_t)(255U - d) : d;
}
static inline uint8_t peak8(uint8_t distance, uint8_t width) {
  if (width == 0U || distance >= width) return 0U;
  return (uint8_t)(255U - ((uint16_t)distance * 255U) / width);
}
static inline uint8_t hash8(uint16_t x, uint8_t t, uint8_t seed) {
  uint8_t v = (uint8_t)(x * 73U + t * 151U + seed * 29U);
  v ^= (uint8_t)(v << 3U);
  v = (uint8_t)(v * 109U + 89U);
  v ^= (uint8_t)(v >> 4U);
  return v;
}

// Local sin/cos 8-bit
static const uint8_t MZ_SIN8_QUARTER[65] PROGMEM = {
  0, 3, 6, 9, 12, 16, 19, 22, 25, 28, 31, 34, 37,
  40, 43, 46, 49, 51, 54, 57, 60, 63, 65, 68, 71, 73,
  76, 78, 81, 83, 85, 88, 90, 92, 94, 96, 98, 100, 102,
  104, 106, 107, 109, 111, 112, 113, 115, 116, 117, 118, 120, 121,
  122, 122, 123, 124, 125, 125, 126, 126, 126, 127, 127, 127, 127
};
static inline uint8_t mz_sin8(uint8_t phase) {
  const uint8_t quadrant = (uint8_t)(phase >> 6U);
  const uint8_t offset = (uint8_t)(phase & 0x3FU);
  int16_t wave = 0;
  const uint8_t *t = MZ_SIN8_QUARTER;
  if (quadrant == 0U)      wave = (int16_t)pgm_read_byte(&t[offset]);
  else if (quadrant == 1U) wave = (int16_t)pgm_read_byte(&t[64U - offset]);
  else if (quadrant == 2U) wave = -(int16_t)pgm_read_byte(&t[offset]);
  else                     wave = -(int16_t)pgm_read_byte(&t[64U - offset]);
  return (uint8_t)(128 + wave);
}
static inline uint8_t mz_cos8(uint8_t phase) {
  return mz_sin8((uint8_t)(phase + 64U));
}
static inline uint8_t mz_tri8(uint8_t phase) {
  const uint8_t p = phase << 1U;
  return (phase < 128U) ? p : (uint8_t)(255U - p);
}
static inline uint8_t mz_ease(uint8_t x) {
  const uint16_t t = x;
  const uint16_t s = t * t;
  const uint32_t c = (s * t) >> 16U;
  return (uint8_t)min8(255U, (uint8_t)(c >> 0U));
}

// -----------------------------------------------------------------------------
// Color helpers — FIX PALETTE ADAPTIF
// -----------------------------------------------------------------------------
// Sample dari palette aktif WLED. Parameter mapping=false agar posisi absolut
// 0..255 di palette (bukan map ke panjang segmen). Fallback ke SEGCOLOR kalau
// palette belum ter-inisialisasi (mis. palette 0 / Default).
static inline uint32_t paletteColor(uint8_t position) {
  uint32_t c = SEGMENT.color_from_palette(position, false, true, 0, 255) & 0x00FFFFFFUL;
  if (c == 0 && position < 64)   c = SEGCOLOR(0) & 0x00FFFFFFUL;
  if (c == 0 && position < 192)  c = SEGCOLOR(1) & 0x00FFFFFFUL;
  if (c == 0)                    c = SEGCOLOR(2) & 0x00FFFFFFUL;
  return c;
}
static inline uint32_t color0() { return paletteColor(18U);  }
static inline uint32_t color1() { return paletteColor(128U); }
static inline uint32_t color2() { return paletteColor(238U); }

// -----------------------------------------------------------------------------
// PATTERN PRIMITIVES
// -----------------------------------------------------------------------------
static inline uint8_t patternSequential(uint8_t pixelPos, uint8_t headPos, uint8_t trailLen) {
  int16_t d = (int16_t)headPos - (int16_t)pixelPos;
  if (d < 0) d += 256;
  if ((uint16_t)d > trailLen) return 0U;
  return (uint8_t)(255U - ((uint16_t)d * 255U) / (trailLen ? trailLen : 1U));
}
static inline uint8_t patternScanner(uint8_t pixelPos, uint8_t phase, uint8_t trailLen) {
  const uint8_t head = mz_tri8(phase);
  return patternSequential(pixelPos, head, trailLen);
}
static inline uint8_t patternStrobe(uint8_t phase, uint8_t duty) {
  return (phase < duty) ? 255U : 0U;
}
static inline uint8_t tachNeedle(uint8_t rpm, uint8_t timePhase) {
  const uint8_t base = rpm;
  const int16_t jitter = (int16_t)mz_sin8(timePhase) * 6 / 128;
  int16_t v = (int16_t)base + jitter;
  if (v < 0) v = 0;
  if (v > 255) v = 255;
  return (uint8_t)v;
}
static inline uint8_t boostCurve(uint8_t phase) {
  if (phase < 180U) {
    const uint16_t t = phase;
    return (uint8_t)((t * t) >> 9U);
  }
  const uint8_t p = (uint8_t)(phase - 180U);
  return (uint8_t)min8(255U, (uint16_t)180U + (uint16_t)p * 5U);
}
static inline uint8_t discGlow(uint8_t pixelPos, uint8_t heat) {
  const uint8_t d = absdiff8(pixelPos, 128U);
  const uint8_t falloff = (uint8_t)(255U - d);
  return mul8(falloff, heat);
}

// -----------------------------------------------------------------------------
// STATEFUL RENDERERS
// -----------------------------------------------------------------------------
static void renderBackfire(uint16_t len, uint32_t now) {
  const unsigned bytes = (unsigned)len;
  if (!SEGENV.allocateData(bytes)) { SEGMENT.fill(color0()); return; }
  uint8_t *fire = SEGENV.data;
  if (SEGENV.call == 0U) { memset(fire, 0, bytes); SEGENV.step = now; }
  const uint16_t interval = (uint16_t)(28U - ((uint16_t)SEGMENT.speed * 20U) / 255U);
  if ((uint32_t)(now - SEGENV.step) >= interval) {
    SEGENV.step = now;
    for (uint16_t i = 0; i < len; i++) fire[i] = scale8(fire[i], 200U);
    const uint8_t intensity = SEGMENT.intensity;
    if (hw_random8() < (uint8_t)(60U + (intensity >> 1U))) {
      const uint16_t p = (hw_random8() & 1U) ? (len - 1U) : 0U;
      fire[p] = max8(fire[p], (uint8_t)(180U + (hw_random8() >> 2U)));
      if (p > 0 && p < len - 1) {
        fire[p + (p == 0 ? 1 : -1)] = max8(fire[p - (p == 0 ? -1 : 1)], 120U);
      }
    }
  }
  const uint32_t c0 = color0(), c2 = color2();
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t h = fire[i];
    uint32_t c = (h > 160U) ? color_blend(c0, c2, (uint8_t)(h - 120U)) : c0;
    SEGMENT.setPixelColor(i, color_fade(c, h));
  }
}

static void renderSequential(uint16_t len, uint32_t now, bool reverse) {
  const uint16_t period = (uint16_t)(900U - ((uint16_t)SEGMENT.speed * 700U) / 255U);
  const uint8_t phase = (uint8_t)(((now % period) * 255U) / period);
  const uint8_t trailLen = (uint8_t)(60U + (SEGMENT.intensity >> 2U));
  const uint32_t c0 = color0();
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t p = (uint8_t)(((uint32_t)i * 255U) / (len > 1 ? len - 1 : 1));
    const uint8_t pp = reverse ? (uint8_t)(255U - p) : p;
    const uint8_t level = patternSequential(pp, phase, trailLen);
    SEGMENT.setPixelColor(i, color_fade(c0, level));
  }
}

static void renderKnightRider(uint16_t len, uint32_t now) {
  const uint16_t period = (uint16_t)(1800U - ((uint16_t)SEGMENT.speed * 1400U) / 255U);
  const uint8_t phase = (uint8_t)(((now % period) * 255U) / period);
  const uint8_t trailLen = (uint8_t)(40U + (SEGMENT.intensity >> 2U));
  const uint32_t c0 = color0();
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t p = (uint8_t)(((uint32_t)i * 255U) / (len > 1 ? len - 1 : 1));
    const uint8_t level = patternScanner(p, phase, trailLen);
    SEGMENT.setPixelColor(i, color_fade(c0, level));
  }
}

static void renderTachSweep(uint16_t len, uint32_t now) {
  const uint16_t period = (uint16_t)(2200U - ((uint16_t)SEGMENT.speed * 1800U) / 255U);
  const uint32_t t = now % period;
  const uint8_t phase = (uint8_t)((t * 255U) / period);
  uint8_t rpm;
  if (phase < 160U) {
    rpm = (uint8_t)((uint16_t)phase * 255U / 160U);
  } else if (phase < 200U) {
    const uint8_t p = (uint8_t)(phase - 160U);
    rpm = (uint8_t)(255U - (uint16_t)p * 2U);
  } else if (phase < 230U) {
    const uint8_t p = (uint8_t)(phase - 200U);
    rpm = (uint8_t)(180U + (uint16_t)p * 2U);
  } else {
    const uint8_t p = (uint8_t)(phase - 230U);
    rpm = (uint8_t)(240U - (uint16_t)p * 240U / 25U);
  }
  const uint8_t needle = tachNeedle(rpm, (uint8_t)(now >> 3U));
  const uint32_t c0 = color0(), c2 = color2();
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t p = (uint8_t)(((uint32_t)i * 255U) / (len > 1 ? len - 1 : 1));
    int16_t d = (int16_t)needle - (int16_t)p;
    uint8_t level;
    if (d >= 0) {
      level = (uint8_t)max8(40U, 255U - (uint8_t)min8(255, d * 3));
    } else {
      level = (uint8_t)max8(0U, 80U - (uint8_t)min8(80, (-d) * 2));
    }
    uint32_t c = (p > 220U) ? color_blend(c0, c2, (uint8_t)(p - 220U) * 5U) : c0;
    SEGMENT.setPixelColor(i, color_fade(c, level));
  }
}

static void renderTurboSpool(uint16_t len, uint32_t now) {
  const uint16_t period = (uint16_t)(3000U - ((uint16_t)SEGMENT.speed * 2400U) / 255U);
  const uint32_t t = now % period;
  const uint8_t phase = (uint8_t)((t * 255U) / period);
  uint8_t boost;
  if (phase < 180U) {
    boost = boostCurve(phase);
  } else if (phase < 210U) {
    const uint8_t p = (uint8_t)(phase - 180U);
    boost = (uint8_t)(220U + (mz_sin8((uint8_t)(p * 8U)) >> 2U));
  } else {
    const uint8_t p = (uint8_t)(phase - 210U);
    boost = (uint8_t)(255U - (uint16_t)p * 255U / 45U);
  }
  const uint32_t c0 = color0(), c1 = color1();
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t p = (uint8_t)(((uint32_t)i * 255U) / (len > 1 ? len - 1 : 1));
    const uint8_t level = (p < boost) ? mul8(255U, (uint8_t)(255U - (boost - p))) : 0U;
    uint32_t c = color_blend(c0, c1, p);
    SEGMENT.setPixelColor(i, color_fade(c, level));
  }
}

static void renderDiscGlow(uint16_t len, uint32_t now) {
  const uint16_t period = (uint16_t)(1500U - ((uint16_t)SEGMENT.speed * 1100U) / 255U);
  const uint8_t phase = (uint8_t)(((now % period) * 255U) / period);
  const uint8_t heat = (uint8_t)(128U + (mz_sin8(phase) >> 1U) + (mz_sin8((uint8_t)(phase * 3U)) >> 2U));
  const uint32_t c0 = color0(), c2 = color2();
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t p = (uint8_t)(((uint32_t)i * 255U) / (len > 1 ? len - 1 : 1));
    const uint8_t level = discGlow(p, heat);
    uint32_t c = (heat > 200U) ? color_blend(c0, c2, (uint8_t)(heat - 200U) * 5U) : c0;
    SEGMENT.setPixelColor(i, color_fade(c, level));
  }
}

static void renderStartLights(uint16_t len, uint32_t now) {
  const uint16_t period = (uint16_t)(4000U - ((uint16_t)SEGMENT.speed * 3000U) / 255U);
  const uint32_t t = now % period;
  const uint8_t phase = (uint8_t)((t * 255U) / period);
  const uint32_t c0 = color0();
  const uint32_t cGreen = color1();
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t p = (uint8_t)(((uint32_t)i * 255U) / (len > 1 ? len - 1 : 1));
    const uint8_t lightIdx = (uint8_t)(p / 51U);
    const uint8_t lightOn = (uint8_t)(40U + lightIdx * 35U);
    uint8_t level = 0;
    if (phase < 220U) {
      if (phase > lightOn && phase < lightOn + 30U) {
        level = 255U;
      } else if (phase >= lightOn + 30U && phase < 220U) {
        level = 180U;
      }
      SEGMENT.setPixelColor(i, color_fade(c0, level));
    } else {
      level = (phase < 250U) ? 255U : (uint8_t)(255U - (phase - 250U) * 255U / 5U);
      SEGMENT.setPixelColor(i, color_fade(cGreen, level));
    }
  }
}

static void renderTwoStep(uint16_t len, uint32_t now) {
  const uint16_t basePeriod = (uint16_t)(40U + ((255U - SEGMENT.speed) >> 2U));
  const uint32_t t = now;
  const uint8_t jitter = hash8((uint16_t)(t >> 4U), (uint8_t)(t >> 8U), 0x5AU);
  const uint16_t period = basePeriod + (uint16_t)(jitter >> 3U);
  const uint8_t phase = (uint8_t)((t % period) * 255U / period);
  const uint8_t duty = (uint8_t)(60U + (SEGMENT.intensity >> 2U));
  const uint8_t level = patternStrobe(phase, duty);
  const uint32_t c0 = color0(), c2 = color2();
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t p = (uint8_t)(((uint32_t)i * 255U) / (len > 1 ? len - 1 : 1));
    uint32_t c = (hash8(i, (uint8_t)(t >> 6U), 0x3CU) > 200U) ? c2 : c0;
    SEGMENT.setPixelColor(i, color_fade(c, level));
  }
}

static void renderAkiraRed(uint16_t len, uint32_t now) {
  const uint16_t period = (uint16_t)(600U - ((uint16_t)SEGMENT.speed * 450U) / 255U);
  const uint8_t phase = (uint8_t)(((now % period) * 255U) / period);
  const uint8_t trailLen = (uint8_t)(80U + (SEGMENT.intensity >> 2U));
  const uint32_t c0 = color0();
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t p = (uint8_t)(((uint32_t)i * 255U) / (len > 1 ? len - 1 : 1));
    const uint8_t level = patternSequential(p, phase, trailLen);
    SEGMENT.setPixelColor(i, color_fade(c0, level));
  }
}

static void renderSignatureDRL(uint16_t len, uint32_t now) {
  const uint16_t period = (uint16_t)(3500U - ((uint16_t)SEGMENT.speed * 2500U) / 255U);
  const uint8_t phase = (uint8_t)(((now % period) * 255U) / period);
  const uint8_t breathe = (uint8_t)(180U + (mz_sin8(phase) >> 2U));
  const uint32_t c0 = color0();
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t p = (uint8_t)(((uint32_t)i * 255U) / (len > 1 ? len - 1 : 1));
    const uint8_t h1 = mz_sin8((uint8_t)(p * 2U));
    const uint8_t h2 = mz_sin8((uint8_t)(p * 3U + 40U));
    const uint8_t shape = avg8(h1, h2);
    const uint8_t level = mul8(shape, breathe);
    SEGMENT.setPixelColor(i, color_fade(c0, level));
  }
}

static void renderRGBParade(uint16_t len, uint32_t now) {
  const uint16_t period = (uint16_t)(1200U - ((uint16_t)SEGMENT.speed * 900U) / 255U);
  const uint8_t phase = (uint8_t)(((now % period) * 255U) / period);
  const uint32_t c0 = color0(), c1 = color1(), c2 = color2();
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t p = (uint8_t)(((uint32_t)i * 255U) / (len > 1 ? len - 1 : 1));
    const uint8_t r = patternSequential(p, phase, 80U);
    const uint8_t g = patternSequential(p, (uint8_t)(phase + 85U), 80U);
    const uint8_t b = patternSequential(p, (uint8_t)(phase + 170U), 80U);
    const uint32_t cr = color_fade(c0, r);
    const uint32_t cg = color_fade(c1, g);
    const uint32_t cb = color_fade(c2, b);
    SEGMENT.setPixelColor(i, color_add(color_add(cr, cg), cb));
  }
}

static void renderBurnout(uint16_t len, uint32_t now) {
  const unsigned bytes = (unsigned)len * 2U;
  if (!SEGENV.allocateData(bytes)) { SEGMENT.fill(color0()); return; }
  uint8_t *smoke = SEGENV.data;
  uint8_t *fire  = SEGENV.data + len;
  if (SEGENV.call == 0U) { memset(smoke, 0, bytes); SEGENV.step = now; }
  const uint16_t interval = (uint16_t)(35U - ((uint16_t)SEGMENT.speed * 25U) / 255U);
  if ((uint32_t)(now - SEGENV.step) >= interval) {
    SEGENV.step = now;
    for (uint16_t i = 0; i < len; i++) {
      smoke[i] = scale8(smoke[i], 230U);
      fire[i]  = scale8(fire[i],  180U);
    }
    const uint8_t sparks = (uint8_t)(1U + (SEGMENT.intensity >> 6U));
    for (uint8_t s = 0; s < sparks; s++) {
      const uint16_t p = (uint16_t)(((uint16_t)hw_random8() * len) >> 8U);
      fire[p] = max8(fire[p], (uint8_t)(180U + (hw_random8() >> 2U)));
      smoke[p] = max8(smoke[p], (uint8_t)(100U + (hw_random8() >> 3U)));
    }
  }
  const uint32_t c0 = color0(), c1 = color1();
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t f = fire[i];
    const uint8_t s = smoke[i];
    uint32_t cf = color_fade(c0, f);
    uint32_t cs = color_fade(c1, s);
    SEGMENT.setPixelColor(i, color_add(cf, cs));
  }
}

static void renderSynthwave(uint16_t len, uint32_t now) {
  const uint16_t period = (uint16_t)(800U - ((uint16_t)SEGMENT.speed * 600U) / 255U);
  const uint8_t phase = (uint8_t)(((now % period) * 255U) / period);
  const uint32_t c0 = color0(), c1 = color1(), c2 = color2();
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t p = (uint8_t)(((uint32_t)i * 255U) / (len > 1 ? len - 1 : 1));
    const uint8_t grid = (uint8_t)((p + phase) & 31U);
    const uint8_t line = (grid < 3U) ? 255U : 0U;
    const uint8_t horizon = (p < 40U) ? (uint8_t)(255U - p * 6U) : 0U;
    uint32_t c = (p < 80U) ? color_blend(c2, c1, (uint8_t)(p * 3U)) : c0;
    const uint8_t level = max8(line, horizon);
    SEGMENT.setPixelColor(i, color_fade(c, level));
  }
}

static void renderGrandFinale(uint16_t len, uint32_t now) {
  const uint16_t period = 3000U;
  const uint32_t t = now % period;
  const uint8_t phase = (uint8_t)((t * 255U) / period);
  const uint32_t c0 = color0(), c1 = color1(), c2 = color2();
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t p = (uint8_t)(((uint32_t)i * 255U) / (len > 1 ? len - 1 : 1));
    uint8_t level;
    uint32_t c;
    if (phase < 85U) {
      const uint8_t wipe = (uint8_t)(phase * 3U);
      level = (p < wipe) ? 255U : 0U;
      c = (p < 85U) ? c0 : ((p < 170U) ? c1 : c2);
    } else if (phase < 170U) {
      const uint8_t strobePhase = (uint8_t)((t * 16U) & 255U);
      level = patternStrobe(strobePhase, 128U);
      c = color_blend(c0, c1, mz_sin8((uint8_t)(t >> 3U)));
    } else {
      const uint8_t r = patternSequential(p, (uint8_t)(phase * 3U), 60U);
      const uint8_t g = patternSequential(p, (uint8_t)(phase * 3U + 85U), 60U);
      const uint8_t b = patternSequential(p, (uint8_t)(phase * 3U + 170U), 60U);
      level = max8(r, max8(g, b));
      c = (r > g && r > b) ? c0 : ((g > b) ? c1 : c2);
    }
    SEGMENT.setPixelColor(i, color_fade(c, level));
  }
}

// -----------------------------------------------------------------------------
// GENERIC ANALYTIC RENDERER
// -----------------------------------------------------------------------------
static void renderAlgorithm(uint8_t id) {
  const uint16_t len = SEGLEN;
  if (len == 0U) return;
  const uint32_t now = strip.now;
  if (len < 2U) { SEGMENT.setPixelColor(0, color0()); return; }

  // Stateful effects
  if (id == 30U) { renderBackfire(len, now); return; }
  if (id == 10U) { renderSequential(len, now, false); return; }
  if (id == 11U) { renderSequential(len, now, true); return; }
  if (id == 60U) { renderKnightRider(len, now); return; }
  if (id == 0U)  { renderTachSweep(len, now); return; }
  if (id == 20U) { renderTurboSpool(len, now); return; }
  if (id == 40U) { renderDiscGlow(len, now); return; }
  if (id == 50U) { renderStartLights(len, now); return; }
  if (id == 32U) { renderTwoStep(len, now); return; }
  if (id == 70U) { renderAkiraRed(len, now); return; }
  if (id == 80U) { renderSignatureDRL(len, now); return; }
  if (id == 90U) { renderRGBParade(len, now); return; }
  if (id == 58U) { renderBurnout(len, now); return; }
  if (id == 64U) { renderSynthwave(len, now); return; }
  if (id == 97U) { renderGrandFinale(len, now); return; }

  // Analytic effects
  const uint8_t speed = SEGMENT.speed;
  const uint8_t intensity = SEGMENT.intensity;
  const uint8_t density = (uint8_t)(1U + (SEGMENT.custom2 >> 4U));
  const uint8_t frameDiv = (uint8_t)(20U + ((255U - speed) * 2U) / 7U);
  const uint8_t ph = (uint8_t)(now / frameDiv);
  const uint8_t slow = (uint8_t)(now / (uint16_t)(90U + ((255U - speed) >> 1U)));
  const uint8_t ph2 = (uint8_t)(now / (uint16_t)(31U + ((255U - speed) >> 2U)));
  const uint32_t c0 = color0(), c1 = color1(), c2 = color2();

  for (uint16_t x = 0; x < len; x++) {
    const uint8_t p = (uint8_t)(((uint32_t)x * 255U) / (len - 1U));
    const uint8_t u = (uint8_t)(p * density);
    const uint8_t a = mz_sin8((uint8_t)(u + ph));
    const uint8_t b = mz_sin8((uint8_t)(u * 2U - ph * 2U + 37U));
    const uint8_t c = mz_cos8((uint8_t)(u * 3U + ph / 2U + 91U));
    const uint8_t e = mz_sin8((uint8_t)(u * 5U + ph2 * 3U + 19U));
    const uint8_t tri = mz_tri8((uint8_t)(u + ph2));
    const uint8_t hash = hash8(x, slow, id);
    const uint8_t center = peak8(absdiff8(p, 128U), 128U);
    const uint8_t edge = (uint8_t)(255U - center);

    uint8_t level = 0U, mix = 0U;
    switch (id) {
      // === RPM & TACHOMETER (1-9) ===
      case 1U: { uint8_t rpm = mz_sin8((uint8_t)(ph * 2U)); level = (rpm > 220U) ? 255U : ((rpm > 180U) ? (uint8_t)(rpm * 1.4f) : 30U); mix = (rpm > 220U) ? 245U : 70U; break; }
      case 2U: { uint8_t rpm = (uint8_t)(180U + (mz_sin8((uint8_t)(ph / 3U)) >> 2U)); level = (uint8_t)(150U + rpm / 3U); mix = rpm; break; }
      case 3U: { uint8_t lope = mz_sin8((uint8_t)(ph / 4U)); uint8_t jitter = hash8(x, (uint8_t)(slow / 2U), id); level = (uint8_t)(80U + lope / 2U + (jitter >> 4U)); mix = lope; break; }
      case 4U: { uint8_t hold = (uint8_t)((ph * 3U) & 255U); uint8_t flash = (hold < 40U) ? 255U : ((hold < 80U) ? 180U : 60U); level = mul8(flash, (uint8_t)(200U + (mz_sin8((uint8_t)(ph * 8U)) >> 2U))); mix = hold; break; }
      case 5U: { uint8_t rpm = (uint8_t)(200U + (mz_sin8((uint8_t)(ph * 3U)) >> 1U)); uint8_t flash = (rpm > 240U) ? 255U : 0U; level = flash; mix = (uint8_t)(rpm - 200U); break; }
      case 6U: { uint8_t lim = (uint8_t)(80U + (hash8(x, (uint8_t)(slow / 4U), id) >> 3U)); level = (uint8_t)(lim + (mz_sin8((uint8_t)(ph / 6U)) >> 3U)); mix = lim; break; }
      case 7U: { uint8_t cruise = (uint8_t)(160U + (mz_sin8((uint8_t)(ph / 8U)) >> 3U)); level = cruise; mix = (uint8_t)(cruise - 160U); break; }
      case 8U: { uint8_t kd = mz_tri8((uint8_t)(ph * 2U)); level = mz_ease(kd); mix = kd; break; }
      case 9U: { uint8_t run = (uint8_t)((u + ph * 2U) & 255U); uint8_t streak = (run < 30U) ? 255U : ((run < 60U) ? 180U : 40U); level = streak; mix = run; break; }

      // === SEQUENTIAL SIGNAL (12-19) ===
      case 12U: { uint8_t head = (uint8_t)((ph * 2U) & 255U); level = patternSequential(p, head, 70U); mix = head; break; }
      case 13U: { uint8_t head = (uint8_t)((ph * 3U) & 255U); level = patternSequential((uint8_t)(255U - p), head, 50U); mix = head; break; }
      case 14U: { uint8_t head = (uint8_t)((ph * 2U) & 255U); level = patternSequential(p, head, 40U); level = mul8(level, (uint8_t)(200U + (mz_sin8((uint8_t)(ph * 4U)) >> 2U))); mix = head; break; }
      case 15U: { uint8_t head = (uint8_t)((ph * 4U) & 255U); level = patternSequential(p, head, 25U); mix = head; break; }
      case 16U: { uint8_t head1 = (uint8_t)((ph * 2U) & 255U); uint8_t head2 = (uint8_t)((ph * 2U + 128U) & 255U); uint8_t l1 = patternSequential(p, head1, 50U); uint8_t l2 = patternSequential(p, head2, 50U); level = max8(l1, l2); mix = (l1 > l2) ? head1 : head2; break; }
      case 17U: { uint8_t head = (uint8_t)((ph * 3U) & 255U); level = patternSequential(p, head, 80U); level = mul8(level, (uint8_t)(180U + (p >> 1U))); mix = head; break; }
      case 18U: { uint8_t head = (uint8_t)((ph * 2U) & 255U); uint8_t trail = patternSequential(p, head, 60U); uint8_t pulse = (uint8_t)(200U + (mz_sin8((uint8_t)(ph * 6U)) >> 2U)); level = mul8(trail, pulse); mix = head; break; }
      case 19U: { uint8_t head = (uint8_t)((ph * 3U) & 255U); uint8_t trail = patternSequential(p, head, 30U); uint8_t strobe = patternStrobe((uint8_t)(ph * 8U), 80U); level = mul8(trail, strobe); mix = head; break; }

      // === TURBO & BOOST (21-29) ===
      case 21U: { uint8_t boost = mz_sin8((uint8_t)(ph * 2U)); uint8_t flutter = (boost > 200U) ? (uint8_t)(mz_sin8((uint8_t)(ph * 16U)) >> 1U) : 0U; level = (uint8_t)(boost + flutter); mix = boost; break; }
      case 22U: { uint8_t phase = (uint8_t)((ph * 2U) & 255U); uint8_t boost = (phase < 200U) ? (uint8_t)(phase * 255U / 200U) : (uint8_t)(255U - (phase - 200U) * 255U / 55U); uint8_t burst = (phase > 190U && phase < 220U) ? 255U : 0U; level = max8(boost, burst); mix = phase; break; }
      case 23U: { uint8_t creep = (uint8_t)min8(255U, (uint16_t)slow + (uint16_t)(mz_sin8((uint8_t)(ph / 4U)) >> 3U)); level = creep; mix = creep; break; }
      case 24U: { uint8_t flutter = mz_sin8((uint8_t)(ph * 12U)); level = (uint8_t)(180U + (flutter >> 2U)); mix = (uint8_t)(128U + flutter); break; }
      case 25U: { uint8_t div = mz_tri8((uint8_t)(ph * 3U)); level = mz_ease(div); mix = div; break; }
      case 26U: { uint8_t timer = (uint8_t)(200U - (uint16_t)slow / 4U); uint8_t pulse = (uint8_t)(180U + (mz_sin8((uint8_t)(ph / 2U)) >> 2U)); level = mul8(timer, pulse); mix = timer; break; }
      case 27U: { uint8_t lag = (uint8_t)((ph < 180U) ? 0U : (uint8_t)((ph - 180U) * 255U / 75U)); level = lag; mix = lag; break; }
      case 28U: { uint8_t gauge = mz_sin8((uint8_t)(ph * 2U)); uint8_t needle = tachNeedle(gauge, (uint8_t)(now >> 3U)); int16_t d = (int16_t)needle - (int16_t)p; level = (d >= 0) ? (uint8_t)max8(40U, 255U - (uint8_t)min8(255, d * 3)) : 0U; mix = needle; break; }
      case 29U: { uint8_t s1 = mz_sin8((uint8_t)(u + ph)); uint8_t s2 = mz_sin8((uint8_t)(u * 2U + ph + 85U)); level = avg8(s1, s2); mix = (s1 > s2) ? s1 : s2; break; }

      // === EXHAUST & FIRE (31, 33-39) ===
      case 31U: { uint8_t flame = mz_sin8((uint8_t)(u + ph)); uint8_t flicker = hash8(x, (uint8_t)(slow + u), id); level = mul8(flame, (uint8_t)(170U + flicker / 3U)); mix = flicker; break; }
      case 33U: { uint8_t ag = (uint8_t)((ph * 6U) & 255U); uint8_t bang = (ag < 30U) ? 255U : ((ag < 60U) ? 180U : 40U); level = mul8(bang, (uint8_t)(200U + (hash8(x, (uint8_t)(ph >> 2U), id) >> 3U))); mix = ag; break; }
      case 34U: { uint8_t pop = hash8(x, (uint8_t)(slow / 2U), id); uint8_t bang = (pop > 220U) ? 255U : ((pop > 180U) ? 120U : 20U); level = bang; mix = pop; break; }
      case 35U: { uint8_t burn = mz_sin8((uint8_t)(u * 3U + ph)); uint8_t tail = mz_sin8((uint8_t)(u / 2U + ph / 2U)); level = mul8(burn, (uint8_t)(150U + tail / 2U)); mix = burn; break; }
      case 36U: { uint8_t cold = (uint8_t)(60U + (mz_sin8((uint8_t)(ph / 6U)) >> 2U)); uint8_t rough = hash8(x, (uint8_t)(slow / 2U), id); level = (uint8_t)(cold + (rough >> 4U)); mix = cold; break; }
      case 37U: { uint8_t warm = (uint8_t)(120U + (mz_sin8((uint8_t)(ph / 4U)) >> 2U)); level = warm; mix = warm; break; }
      case 38U: { uint8_t ft = (uint8_t)(220U + (mz_sin8((uint8_t)(ph * 2U)) >> 3U)); level = ft; mix = ft; break; }
      case 39U: { uint8_t decel = (uint8_t)((ph * 4U) & 255U); uint8_t pop = (decel < 20U) ? 255U : 0U; level = pop; mix = decel; break; }

      // === BRAKE & HEAT (41-49) ===
      case 41U: { uint8_t fade = (uint8_t)(200U - (uint16_t)slow / 8U); uint8_t pulse = (uint8_t)(180U + (mz_sin8((uint8_t)(ph / 3U)) >> 2U)); level = mul8(fade, pulse); mix = fade; break; }
      case 42U: { uint8_t bias = (p < 128U) ? (uint8_t)(200U + (mz_sin8((uint8_t)(ph / 2U)) >> 2U)) : (uint8_t)(160U + (mz_sin8((uint8_t)(ph / 3U)) >> 2U)); level = bias; mix = bias; break; }
      case 43U: { uint8_t abs = patternStrobe((uint8_t)(ph * 8U), 100U); uint8_t base = (uint8_t)(180U + (mz_sin8((uint8_t)(ph / 2U)) >> 2U)); level = mul8(base, (uint8_t)(128U + (abs >> 1U))); mix = abs; break; }
      case 44U: { uint8_t bl = (uint8_t)(220U + (mz_sin8((uint8_t)(ph / 4U)) >> 3U)); level = bl; mix = bl; break; }
      case 45U: { uint8_t es = patternStrobe((uint8_t)(ph * 12U), 80U); level = es; mix = es; break; }
      case 46U: { uint8_t tb = mz_sin8((uint8_t)(ph / 2U)); uint8_t trail = patternSequential(p, (uint8_t)(255U - tb), 80U); level = trail; mix = tb; break; }
      case 47U: { uint8_t cc = (uint8_t)(200U + (hash8(x, (uint8_t)(slow / 3U), id) >> 3U)); uint8_t shimmer = mz_sin8((uint8_t)(ph + u)); level = mul8(cc, (uint8_t)(200U + (shimmer >> 3U))); mix = cc; break; }
      case 48U: { uint8_t vd = (uint8_t)((u * 5U + ph) & 255U); uint8_t vent = (vd < 30U) ? 255U : ((vd < 60U) ? 180U : 60U); level = vent; mix = vd; break; }
      case 49U: { uint8_t bd = mz_sin8((uint8_t)(u + ph)); uint8_t cool = (uint8_t)(150U + (bd >> 2U)); level = cool; mix = bd; break; }

      // === RACE TRACK (51-57, 59) ===
      case 51U: { uint8_t pace = mz_sin8((uint8_t)(ph / 2U)); uint8_t steady = (uint8_t)(160U + (pace >> 2U)); level = steady; mix = pace; break; }
      case 52U: { uint8_t sc = patternStrobe((uint8_t)(ph * 4U), 128U); uint8_t amber = (uint8_t)(200U + (mz_sin8((uint8_t)(ph / 3U)) >> 2U)); level = mul8(amber, (uint8_t)(128U + (sc >> 1U))); mix = sc; break; }
      case 53U: { uint8_t cf = (uint8_t)(((x / 4U) ^ (ph / 16U)) & 1U); level = cf ? 255U : 40U; mix = cf ? 240U : 30U; break; }
      case 54U: { uint8_t pl = (uint8_t)(120U + (mz_sin8((uint8_t)(ph / 4U)) >> 2U)); uint8_t limit = (p > 200U) ? 255U : 0U; level = (uint8_t)(pl + (limit >> 2U)); mix = pl; break; }
      case 55U: { uint8_t over = mz_tri8((uint8_t)(ph * 3U)); level = mz_ease(over); mix = over; break; }
      case 56U: { uint8_t ss = mz_sin8((uint8_t)(u + ph)); uint8_t draft = mz_sin8((uint8_t)(u * 2U - ph)); level = avg8(ss, draft); mix = (ss > draft) ? ss : draft; break; }
      case 57U: { uint8_t ds = (uint8_t)((ph * 4U) & 255U); uint8_t streak = (ds < 40U) ? 255U : ((ds < 80U) ? 180U : 40U); level = streak; mix = ds; break; }
      case 59U: { uint8_t pod = (uint8_t)(200U + (mz_sin8((uint8_t)(ph / 2U)) >> 2U)); uint8_t sparkle = hash8(x, (uint8_t)(slow / 2U), id); level = (uint8_t)(pod + (sparkle >> 4U)); mix = pod; break; }

      // === NIGHT RIDER (61-69) ===
      case 61U: { uint8_t line = (uint8_t)((p + ph * 2U) & 255U); uint8_t sharp = (line < 10U) ? 255U : ((line < 20U) ? 180U : 0U); level = sharp; mix = line; break; }
      case 62U: { uint8_t grid = (uint8_t)((u + ph) & 31U); uint8_t line = (grid < 3U) ? 255U : 0U; level = line; mix = grid; break; }
      case 63U: { uint8_t neon = mz_sin8((uint8_t)(u + ph)); uint8_t pulse = (uint8_t)(200U + (mz_sin8((uint8_t)(ph * 4U)) >> 2U)); level = mul8(neon, pulse); mix = neon; break; }
      case 65U: { uint8_t vw = mz_sin8((uint8_t)(u / 2U + ph / 3U)); uint8_t pink = (uint8_t)(180U + (vw >> 2U)); level = pink; mix = vw; break; }
      case 66U: { uint8_t rw = mz_tri8((uint8_t)(u + ph)); uint8_t grad = (uint8_t)(160U + (rw >> 2U)); level = grad; mix = rw; break; }
      case 67U: { uint8_t or1 = mz_sin8((uint8_t)(u * 2U + ph)); uint8_t or2 = mz_sin8((uint8_t)(u * 3U - ph)); level = avg8(or1, or2); mix = (or1 > or2) ? or1 : or2; break; }
      case 68U: { uint8_t dr = mz_sin8((uint8_t)(u + ph / 2U)); uint8_t shadow = (uint8_t)(100U + (dr >> 2U)); level = shadow; mix = dr; break; }
      case 69U: { uint8_t head = (uint8_t)((ph * 3U) & 255U); uint8_t trail = patternSequential(p, head, 50U); uint8_t shadow = mul8(trail, (uint8_t)(180U + (mz_sin8((uint8_t)(ph * 2U)) >> 2U))); level = shadow; mix = head; break; }

      // === JDM LEGEND (71-79) ===
      case 71U: { uint8_t drift = mz_sin8((uint8_t)(u + ph + mz_sin8((uint8_t)(u * 2U + slow)) / 4U)); level = drift; mix = drift; break; }
      case 72U: { uint8_t wm = (uint8_t)((u + ph * 3U) & 255U); uint8_t streak = (wm < 20U) ? 255U : ((wm < 40U) ? 180U : 40U); level = streak; mix = wm; break; }
      case 73U: { uint8_t hash = hash8(x, (uint8_t)(slow + ph / 4U), id); uint8_t flash = (hash > 220U) ? 255U : ((hash > 180U) ? 120U : 20U); level = flash; mix = hash; break; }
      case 74U: { uint8_t touge = mz_sin8((uint8_t)(u * 2U + ph)); uint8_t corner = (uint8_t)(180U + (touge >> 2U)); level = corner; mix = touge; break; }
      case 75U: { uint8_t kp = (uint8_t)((u / 8U + ph / 12U) & 15U); uint8_t pulse = ((kp == 1U || kp == 4U || kp == 7U || kp == 10U) ? 255U : ((kp == 2U || kp == 5U) ? 90U : 0U)); level = pulse; mix = kp; break; }
      case 76U: { uint8_t rs = mz_sin8((uint8_t)(ph / 3U)); uint8_t glow = discGlow(p, (uint8_t)(180U + (rs >> 2U))); level = glow; mix = rs; break; }
      case 77U: { uint8_t sf = hash8(x, (uint8_t)(slow / 2U), id); uint8_t fall = (uint8_t)(p + ph / 2U); level = (sf > 210U) ? (uint8_t)sf : patternSequential(fall, (uint8_t)(ph / 2U), 30U); mix = sf; break; }
      case 78U: { uint8_t bf = (uint8_t)((u / 3U + ph / 2U) & 31U); uint8_t stalk = (bf < 13U) ? (uint8_t)(255U - bf * 18U) : 0U; level = stalk; mix = bf; break; }
      case 79U: { uint8_t tn = mz_sin8((uint8_t)(u + ph)); uint8_t neon = (uint8_t)(200U + (tn >> 2U)); uint8_t flicker = hash8(x, (uint8_t)(slow + u), id); level = mul8(neon, (uint8_t)(200U + (flicker >> 4U))); mix = tn; break; }

      // === PREMIUM SIGNATURE (81-89) ===
      case 81U: { uint8_t ae = mz_sin8((uint8_t)(u + ph / 2U)); uint8_t ring = (uint8_t)(200U + (ae >> 3U)); level = ring; mix = ae; break; }
      case 82U: { uint8_t de = mz_sin8((uint8_t)(u * 2U + ph)); uint8_t glow = (uint8_t)(180U + (de >> 2U)); level = glow; mix = de; break; }
      case 83U: { uint8_t ll = (uint8_t)((u * 4U + ph * 2U) & 255U); uint8_t sharp = (ll < 8U) ? 255U : ((ll < 16U) ? 180U : 0U); level = sharp; mix = ll; break; }
      case 84U: { uint8_t cf = mz_sin8((uint8_t)(u + ph / 3U)); uint8_t shimmer = hash8(x, (uint8_t)(slow / 3U), id); level = (uint8_t)(180U + (cf >> 2U) + (shimmer >> 5U)); mix = cf; break; }
      case 85U: { uint8_t dc = (uint8_t)((u * 5U + ph) & 255U); uint8_t facet = (dc < 20U) ? 255U : ((dc < 40U) ? 180U : 60U); level = facet; mix = dc; break; }
      case 86U: { uint8_t head = (uint8_t)((ph * 2U) & 255U); uint8_t trail = patternSequential(p, head, 60U); uint8_t shine = (uint8_t)(200U + (mz_sin8((uint8_t)(ph * 3U)) >> 3U)); level = mul8(trail, shine); mix = head; break; }
      case 87U: { uint8_t og = mz_sin8((uint8_t)(u + ph / 4U)); uint8_t deep = (uint8_t)(100U + (og >> 2U)); level = deep; mix = og; break; }
      case 88U: { uint8_t tp = mz_sin8((uint8_t)(u * 2U + ph)); uint8_t pulse = (uint8_t)(180U + (tp >> 2U)); level = pulse; mix = tp; break; }
      case 89U: { uint8_t cw = (uint8_t)(((x / 2U) ^ (ph / 8U)) & 7U); uint8_t weave = (uint8_t)(cw * 32U + (uint8_t)(mz_sin8((uint8_t)(u + ph)) >> 3U)); level = weave; mix = cw; break; }

      // === SHOW & MEET (91-96, 98-99) ===
      case 91U: { uint8_t head = (uint8_t)((ph * 2U) & 255U); uint8_t wipe = (p < head) ? 255U : 0U; level = wipe; mix = head; break; }
      case 92U: { uint8_t sp = patternStrobe((uint8_t)(ph * 10U), 80U); uint8_t color = hash8(x, (uint8_t)(slow / 2U), id); level = sp; mix = color; break; }
      case 93U: { uint8_t rave = patternStrobe((uint8_t)(ph * 12U), 60U); uint8_t flash = hash8(x, (uint8_t)(ph >> 2U), id); level = mul8(rave, (uint8_t)(200U + (flash >> 3U))); mix = flash; break; }
      case 94U: { uint8_t cs = mz_sin8((uint8_t)(u + ph)); uint8_t wave = (uint8_t)(180U + (cs >> 2U)); level = wave; mix = cs; break; }
      case 95U: { uint8_t mp = hash8(x, (uint8_t)(slow + ph / 2U), id); uint8_t chaos = (mp > 200U) ? 255U : ((mp > 150U) ? 180U : 40U); level = chaos; mix = mp; break; }
      case 96U: { uint8_t enc = (uint8_t)(220U + (mz_sin8((uint8_t)(ph / 2U)) >> 3U)); uint8_t sparkle = hash8(x, (uint8_t)(slow / 2U), id); level = (uint8_t)(enc + (sparkle >> 4U)); mix = enc; break; }
      case 98U: { uint8_t vl = mz_sin8((uint8_t)(u + ph)); uint8_t lap = (uint8_t)(200U + (vl >> 2U)); level = lap; mix = vl; break; }
      case 99U: { uint8_t champ = (uint8_t)(220U + (mz_sin8((uint8_t)(ph / 2U)) >> 3U)); uint8_t gold = hash8(x, (uint8_t)(slow / 3U), id); level = (uint8_t)(champ + (gold >> 5U)); mix = champ; break; }

      default: level = a; mix = b; break;
    }

    level = scale8(level, (uint8_t)(96U + ((uint16_t)intensity * 159U) / 255U));
    uint32_t color = color_blend(c0, c1, mix);
    if (mix > 208U) color = color_blend(color, c2, (uint8_t)((mix - 208U) * 5U));
    SEGMENT.setPixelColor(x, color_fade(color, level));
  }
}

// -----------------------------------------------------------------------------
// WLED registration — FIX PALETTE: beginDraw() sebelum sampling
// -----------------------------------------------------------------------------
static void mode_mizuma_signature_100(void) {
  // WAJIB: beginDraw() me-rebuild palette cache dari currentPalette global.
  // Tanpa ini, color_from_palette() membaca cache stale -> warna tidak adaptif.
  SEGMENT.beginDraw();

  const uint8_t index = clampIndex(SEGMENT.custom1);
  const uint16_t algorithmTag = (uint16_t)(0xA500U | ((uint16_t)(index & 0x7FU) << 1U));
  if ((SEGENV.aux1 & 0xFFFEU) != algorithmTag) {
    SEGENV.call = 0U;
    SEGENV.step = 0U;
    SEGENV.aux0 = 0U;
    SEGENV.aux1 = algorithmTag;
  }
  renderAlgorithm(index);
}

static const char MZ_SIGNATURE_100_METADATA[] PROGMEM =
  "MZ | Signature 100@!,Intensity,Algorithm ID,Density,Detail;!,!;!;1;sx=128,ix=180,c1=0,c2=160,c3=16,pal=0";

static inline uint8_t registerWLEDEffect(void) {
  return strip.addEffect(255, &mode_mizuma_signature_100, MZ_SIGNATURE_100_METADATA);
}

} // namespace MizumaSignature100
