#pragma once

/*
 * MIZUMA SIGNATURE 100 — 100 algoritma animasi orisinal 1D
 * Target: WLED 16.x / ESP32-C3 class hardware.
 *
 * PENTING:
 * - Ini bukan 100 preset efek native. Setiap indeks mempunyai logika render sendiri.
 * - Agar aman terhadap kapasitas ID efek WLED (uint8_t), header ini mendaftarkan
 *   SATU engine WLED bernama "MZ | Signature 100". Indeks algoritma 0..99 dipilih
 *   melalui SEGMENT.custom1 (c1). UI Mizuma nantinya dapat menampilkan 100 nama.
 * - Header ini untuk Usermod Mizuma saja; tidak perlu mengedit FX.cpp / FX.h WLED.
 * - Saat .cpp memanggil registerWLEDEffect(), satu slot Usermod FX ditambahkan melalui
 *   API resmi strip.addEffect(). Ini bukan efek bawaan/core WLED; entry tersebut hanya
 *   ada pada firmware yang memuat Usermod Mizuma.
 * - Satu slot dipakai untuk 100 algoritma internal agar tidak menghabiskan rentang ID
 *   efek 8-bit WLED (pada WLED 16.x sebagian besar rentang ID sudah dipakai).
 * - File ini tidak mengubah .cpp dan tidak mendaftarkan dirinya otomatis.
 *
 * Integrasi nanti di .cpp:
 *   #include "mizuma_signature_100.h"
 *   // panggil SATU KALI dari setup() usermod:
 *   MizumaSignature100::registerWLEDEffect();
 *   // untuk UI / katalog:
 *   MizumaSignature100::nameAt(index); // index 0..99
 *
 * Pemilihan algoritma lewat JSON WLED:
 *   {"seg":[{"id":SEG_ID,"fx":ID_ENGINE_MZ,"c1":0}]}
 *   c1=0 memilih Flowline, c1=99 memilih Signature Finale.
 *   UI Usermod Mizuma dapat menampilkan 100 nama melalui nameAt(index), lalu
 *   mengirim index terpilih ke custom1/c1 pada segmen yang memakai engine ini.
 *
 * Kontrol engine:
 *   speed     = kecepatan waktu animasi
 *   intensity = intensitas / level terang
 *   custom1   = indeks algoritma 0..99
 *   custom2   = lebar / kepadatan / karakter gerak (tergantung algoritma)
 *   custom3   = detail tambahan (0..31; dipakai sejumlah algoritma)
 *   Color 1/2/3 = warna utama, aksen, dan highlight.
 *
 * Beberapa algoritma memakai SEGENV.allocateData() untuk state per-segmen.
 * Jika alokasi gagal, efek fallback ke warna utama; tidak akan mengakses buffer null.
 */

#include "wled.h"
#include <stdint.h>
#include <string.h>

namespace MizumaSignature100 {

static const uint8_t EFFECT_COUNT = 100;
static const uint8_t GROUP_COUNT = 10;

static const char * const EFFECT_GROUPS[GROUP_COUNT] = {
  "Arah & Kecepatan",
  "Gelombang & Resonansi",
  "Partikel & Bara",
  "Geometri & Pola",
  "Atmosfer & Cairan",
  "Mesin & Racing",
  "Cahaya Premium",
  "Gerbang & Simetri",
  "Eksperimental",
  "Signature & Energi"
};

static const char * const EFFECT_NAMES[EFFECT_COUNT] = {
  "Flowline",
  "Twin Orbit",
  "Slipstream",
  "Crosswind",
  "Pulse Rail",
  "Helix Run",
  "Stagger Dash",
  "Comet Arc",
  "Vortex Gate",
  "Drift Current",
  "Resonant Field",
  "Standing Bloom",
  "Beat Engine",
  "Phase Lattice",
  "Harmonic Crown",
  "Ripple Origin",
  "Interference Knot",
  "Pendulum Arc",
  "Tidal Glass",
  "Polar Pulse",
  "Emberline",
  "Firefly Swarm",
  "Spark Cascade",
  "Molten Veins",
  "Ashfall",
  "Plasma Bloom",
  "Lava Pulse",
  "Starfall",
  "Meteor Choir",
  "Aurora Sparks",
  "Prism March",
  "Binary Rain",
  "Morse Beacon",
  "Barcode Shift",
  "Chevron Drive",
  "Pixel Quilt",
  "Fractal Fold",
  "Fibonacci Trace",
  "Parallax Stripes",
  "Kaleido Steps",
  "Nebula Drift",
  "Aurora Veil",
  "Silk Current",
  "Liquid Chrome",
  "Vapor Wake",
  "Cloudfront",
  "Ocean Tide",
  "Rain Curtain",
  "Haze Bloom",
  "Opal Flow",
  "Heartbeat Core",
  "Engine Idle",
  "Tach Sweep",
  "Rev Pulse",
  "Gearshift",
  "Ignition Ladder",
  "Startline",
  "Afterburn",
  "Turbo Spiral",
  "Pitlane Flash",
  "Champagne Glow",
  "Amber Velvet",
  "Golden Hour",
  "Pearl Cascade",
  "Satin Wave",
  "Opal Shimmer",
  "Moonlit Chrome",
  "Candle Crown",
  "Halo Rise",
  "Aurora Crown",
  "Split Converge",
  "Split Diverge",
  "Center Bloom",
  "Edge Bloom",
  "Twin Portal",
  "Moving Window",
  "Sliding Aperture",
  "Gate Runner",
  "Beacon Sweep",
  "Cross Beacon",
  "Quantum Flicker",
  "Phase Shifter",
  "Noise Sculpt",
  "Cellular Rule",
  "Binary Drift",
  "Reaction Filament",
  "Brownian Trail",
  "Logistic Cascade",
  "Lorenz Ribbon",
  "Cellular Pulse",
  "Lightning Trace",
  "Solar Flare",
  "Magnetic Field",
  "Ripple Lattice",
  "Time Warp",
  "Spectral Bands",
  "Ion Stream",
  "Gravity Well",
  "Horizon Sweep",
  "Signature Finale"
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

static inline uint8_t addSat8(uint8_t a, uint8_t b) {
  uint16_t v = (uint16_t)a + b;
  return (v > 255U) ? 255U : (uint8_t)v;
}

// Local 8-bit sine/cosine helpers. Some WLED 16.x fastled_slim builds do not
// expose FastLED mz_sin8()/mz_cos8() in the global namespace. Phase 0..255 maps
// one full cycle; output uses the usual 0..255 unsigned sine range.
static const uint8_t MZ_SIN8_QUARTER[65] = {
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
  if (quadrant == 0U)      wave = (int16_t)MZ_SIN8_QUARTER[offset];
  else if (quadrant == 1U) wave = (int16_t)MZ_SIN8_QUARTER[64U - offset];
  else if (quadrant == 2U) wave = -(int16_t)MZ_SIN8_QUARTER[offset];
  else                    wave = -(int16_t)MZ_SIN8_QUARTER[64U - offset];
  return (uint8_t)(128 + wave);
}

static inline uint8_t mz_cos8(uint8_t phase) {
  return mz_sin8((uint8_t)(phase + 64U));
}

// Short aliases used by the 100 rendering equations below.
static inline uint8_t mz_min(uint8_t a, uint8_t b) { return min8(a, b); }
static inline uint8_t mz_max(uint8_t a, uint8_t b) { return max8(a, b); }
static inline uint8_t mz_avg(uint8_t a, uint8_t b) { return avg8(a, b); }
static inline uint8_t mz_mul(uint8_t a, uint8_t b) { return mul8(a, b); }
static inline uint8_t mz_absdiff(uint8_t a, uint8_t b) { return absdiff8(a, b); }
static inline uint8_t mz_dist(uint8_t a, uint8_t b) { return dist8(a, b); }
static inline uint8_t mz_peak(uint8_t distance, uint8_t width) { return peak8(distance, width); }
static inline uint8_t mz_hash(uint16_t x, uint8_t t, uint8_t seed) { return hash8(x, t, seed); }

// Sample three stable points from the active WLED palette. Palette 0 (Default)
// still follows the segment color slots; all other built-in/custom palettes are
// sampled directly, so changing the palette changes the Signature 100 output.
static inline uint32_t paletteColor(uint8_t position) {
  return SEGMENT.color_from_palette(position, false, true, 0, 255) & 0x00FFFFFFUL;
}
static inline uint32_t color0() { return paletteColor(18U); }
static inline uint32_t color1() { return paletteColor(128U); }
static inline uint32_t color2() { return paletteColor(238U); }

// -----------------------------------------------------------------------------
// MIZUMA SIGNATURE 100 — premium 1D render core
// 10 visual families x 10 variations. Each family has a distinct visual language:
// motion ribbons, resonance, embers/particles, geometry, atmosphere, racing,
// premium illumination, portals, experimental fields, and signature composites.
// All outputs are full-width light fields; particles are layered over a luminous
// body so the result does not collapse into isolated dots on short LED strips.
// -----------------------------------------------------------------------------
static inline uint8_t mz_satAdd(uint8_t a, uint8_t b) {
  const uint16_t v = (uint16_t)a + (uint16_t)b;
  return (v > 255U) ? 255U : (uint8_t)v;
}

static inline uint8_t mz_lerp8(uint8_t a, uint8_t b, uint8_t amount) {
  return (uint8_t)(((uint16_t)a * (255U - amount) + (uint16_t)b * amount) / 255U);
}

static inline uint8_t mz_blob(uint8_t position, uint8_t center, uint8_t radius) {
  return peak8(dist8(position, center), radius);
}

static inline uint8_t mz_tail(uint8_t position, uint8_t head, uint8_t length) {
  const uint8_t behind = (uint8_t)(head - position);
  if (behind >= length) return 0U;
  return (uint8_t)(255U - ((uint16_t)behind * 255U) / (length ? length : 1U));
}

static inline uint8_t mz_smoothNoise(uint16_t x, uint8_t time, uint8_t seed, uint8_t cellSize) {
  if (cellSize < 2U) cellSize = 2U;
  const uint16_t cell = x / cellSize;
  const uint8_t fraction = (uint8_t)(((uint16_t)(x % cellSize) * 255U) / cellSize);
  const uint8_t a = hash8(cell, time, seed);
  const uint8_t b = hash8((uint16_t)(cell + 1U), time, seed);
  return mz_lerp8(a, b, fraction);
}

static inline uint8_t mz_bed(uint8_t wave, uint8_t floorLevel, uint8_t gain) {
  return mz_satAdd(floorLevel, scale8(wave, gain));
}

static inline uint8_t mz_softMask(uint8_t value, uint8_t threshold, uint8_t width) {
  if (value <= threshold) return 0U;
  const uint16_t rise = (uint16_t)(value - threshold) * 255U;
  return (uint8_t)((rise / (width ? width : 1U) > 255U) ? 255U : rise / (width ? width : 1U));
}

static void renderAlgorithm(uint8_t id) {
  const uint16_t len = SEGLEN;
  if (len == 0U) return;
  if (len == 1U) {
    SEGMENT.setPixelColor(0, SEGMENT.color_from_palette(128U, false, true, 0, 255));
    return;
  }

  const uint8_t family = (uint8_t)(id / 10U);
  const uint8_t variant = (uint8_t)(id % 10U);
  const uint8_t speed = SEGMENT.speed;
  const uint8_t intensity = SEGMENT.intensity;
  const uint8_t density = SEGMENT.custom2;
  const uint8_t detail = SEGMENT.custom3;

  // At maximum speed the phase clock advances roughly 4x faster than at the
  // minimum. The real LED refresh rate remains controlled by WLED's main loop.
  const uint16_t frameDiv = (uint16_t)(18U - ((uint16_t)speed * 14U) / 255U); // 18..4 ms
  const uint32_t tick32 = strip.now / frameDiv;
  const uint8_t tick = (uint8_t)tick32;
  const uint8_t slowTick = (uint8_t)(strip.now / (uint16_t)(frameDiv * 4U + 11U));
  const uint8_t spatialScale = (uint8_t)(2U + (density >> 5U)); // 2..9
  const uint8_t width = (uint8_t)(14U + (density >> 3U) + detail); // broad form width
  const uint8_t levelGain = (uint8_t)(88U + ((uint16_t)intensity * 167U) / 255U);
  const uint8_t noiseCell = (uint8_t)(2U + ((255U - density) >> 5U));

  // Scratch layout: raw light, palette coordinate and persistent afterglow.
  // At 300 LEDs this consumes 900 bytes, acceptable for ESP32-C3 class boards.
  const unsigned renderBytes = (unsigned)len * 3U;
  if (!SEGENV.allocateData(renderBytes)) {
    SEGMENT.fill(SEGMENT.color_from_palette(128U, false, true, 0, 255));
    return;
  }
  uint8_t *rawLevel = SEGENV.data;
  uint8_t *rawHue = SEGENV.data + len;
  uint8_t *afterglow = SEGENV.data + (unsigned)len * 2U;
  if (SEGENV.call == 0U) memset(afterglow, 0, len);

  const uint8_t phase = (uint8_t)(tick * (uint8_t)(1U + (variant % 3U)));
  const uint8_t phase2 = (uint8_t)(tick * (uint8_t)(2U + (variant % 4U)) + 73U);
  const uint8_t slowPhase = (uint8_t)(slowTick * (uint8_t)(1U + (variant % 2U)));
  const uint8_t trailFade = (family == 2U || family == 8U) ? 207U :
                            (family == 6U || family == 4U) ? 235U : 222U;

  for (uint16_t x = 0; x < len; x++) {
    const uint8_t p = (uint8_t)(((uint32_t)x * 255U) / (len - 1U));
    const uint8_t q = (uint8_t)(p * spatialScale);
    const uint8_t w1 = mz_sin8((uint8_t)(q + phase));
    const uint8_t w2 = mz_sin8((uint8_t)(q * 2U - phase2 + 41U));
    const uint8_t w3 = mz_cos8((uint8_t)(q * 3U + phase / 2U + 91U));
    const uint8_t w4 = mz_sin8((uint8_t)(q / 2U - slowPhase + 17U));
    const uint8_t tri = triwave8((uint8_t)(q + phase2));
    const uint8_t noise = mz_smoothNoise(x, slowTick, (uint8_t)(id + 17U), noiseCell);
    const uint8_t grain = hash8((uint16_t)(x + variant * 11U), (uint8_t)(slowTick + phase / 5U), id);
    const uint8_t edge = (uint8_t)(255U - peak8(absdiff8(p, 128U), 128U));
    uint8_t level = 0U;
    uint8_t hue = (uint8_t)(p + phase / 3U + id * 13U);

    switch (family) {
      // 0 — Arah & Kecepatan: broad moving ribbons, swept beams and streams.
      case 0U:
        switch (variant) {
          case 0U: level = mz_bed(mz_avg(w1, w4), 44U, 172U); level = mz_satAdd(level, scale8(mz_blob(p, (uint8_t)(phase + 128U), width), 150U)); hue = (uint8_t)(p + phase); break;
          case 1U: level = mz_satAdd(24U, mz_max(scale8(mz_blob(p, phase, width), 235U), scale8(mz_blob(p, (uint8_t)(phase + 128U), width), 210U))); hue = (uint8_t)(p * 2U + phase); break;
          case 2U: { const uint8_t tail = mz_tail(p, phase, 150U); level = mz_satAdd(18U, scale8(tail, 225U)); level = mz_satAdd(level, scale8(mz_blob(p, phase, 22U), 100U)); hue = (uint8_t)(phase + tail / 3U); break; }
          case 3U: level = mz_satAdd(28U, scale8(mz_absdiff(w1, w2), 205U)); level = mz_satAdd(level, scale8(mz_blob(p, (uint8_t)(phase + 64U), 32U), 80U)); hue = mz_avg(w1, w2); break;
          case 4U: { const uint8_t rail = mz_sin8((uint8_t)(q * 2U + phase * 2U)); const uint8_t gate = mz_softMask(rail, 155U, 90U); level = mz_bed(w1, 20U, 75U); level = mz_satAdd(level, scale8(gate, 220U)); hue = (uint8_t)(q + phase * 2U); break; }
          case 5U: { const uint8_t warp = (uint8_t)(q + mz_sin8((uint8_t)(q * 2U - phase)) / 3U); level = mz_bed(mz_sin8((uint8_t)(warp + phase)), 22U, 190U); level = mz_satAdd(level, scale8(mz_sin8((uint8_t)(warp * 3U - phase2)), 65U)); hue = warp; break; }
          case 6U: { const uint8_t cell = (uint8_t)((q + phase * 2U) & 63U); const uint8_t dash = (cell < 38U) ? (uint8_t)(255U - cell * 5U) : 0U; level = mz_satAdd(28U, scale8(dash, 205U)); level = mz_satAdd(level, scale8(w2, 70U)); hue = (uint8_t)(cell * 4U + variant * 17U); break; }
          case 7U: { const uint8_t head = (uint8_t)(phase + mz_sin8((uint8_t)(phase / 2U)) / 4U); const uint8_t tail = mz_tail(p, head, 112U); level = mz_satAdd(16U, scale8(tail, 225U)); level = mz_satAdd(level, scale8(mz_blob(p, head, 18U), 115U)); hue = (uint8_t)(head + tail / 2U); break; }
          case 8U: level = mz_satAdd(26U, scale8(mz_mul(w1, w2), 225U)); level = mz_satAdd(level, scale8(mz_blob(p, (uint8_t)(phase + 128U), 34U), 100U)); hue = mz_avg(w1, w3); break;
          default: level = mz_bed(mz_avg(w1, w3), 38U, 190U); level = mz_satAdd(level, scale8(mz_absdiff(w2, w4), 92U)); hue = (uint8_t)(phase + q / 2U); break;
        }
        break;

      // 1 — Gelombang & Resonansi: standing waves, beats and layered harmonics.
      case 1U:
        switch (variant) {
          case 0U: level = mz_bed(mz_avg(w1, w2), 35U, 205U); level = mz_satAdd(level, scale8(mz_absdiff(w1, w3), 80U)); hue = mz_avg(w1, w2); break;
          case 1U: level = mz_satAdd(18U, scale8(mz_mul(w1, mz_sin8((uint8_t)(q + 128U))), 238U)); level = mz_satAdd(level, scale8(w3, 52U)); hue = q; break;
          case 2U: { const uint8_t beat = mz_absdiff(mz_sin8((uint8_t)(phase * 2U)), mz_sin8((uint8_t)(phase * 2U + 31U))); level = mz_bed(w1, 26U, 105U); level = mz_satAdd(level, scale8(mz_mul(w2, beat), 210U)); hue = (uint8_t)(q + beat / 2U); break; }
          case 3U: level = mz_satAdd(22U, scale8(mz_mul(mz_sin8((uint8_t)(q * 2U + phase)), mz_sin8((uint8_t)(q * 3U - phase2))), 230U)); hue = mz_sin8((uint8_t)(q * 2U + phase)); break;
          case 4U: level = mz_bed((uint8_t)((w1 / 3U + w2 / 3U + w3 / 3U)), 24U, 210U); hue = (uint8_t)(q * 3U + phase); break;
          case 5U: { const uint8_t r = mz_blob(p, phase, 48U); const uint8_t r2 = mz_blob(p, (uint8_t)(phase + 78U), 30U); level = mz_satAdd(12U, mz_max(r, scale8(r2, 190U))); level = mz_satAdd(level, scale8(w4, 60U)); hue = (uint8_t)(phase + r2); break; }
          case 6U: { const uint8_t knot = mz_absdiff(w1, w3); level = mz_bed(knot, 24U, 220U); level = mz_satAdd(level, scale8(mz_mul(w2, w4), 70U)); hue = (uint8_t)(knot + q); break; }
          case 7U: { const uint8_t pend = mz_sin8((uint8_t)(phase / 2U)); level = mz_satAdd(20U, scale8(mz_blob(p, pend, 54U), 210U)); level = mz_satAdd(level, scale8(mz_blob(p, (uint8_t)(pend + 128U), 24U), 75U)); hue = pend; break; }
          case 8U: level = mz_bed(mz_mul(mz_sin8((uint8_t)(q + phase / 3U)), mz_sin8((uint8_t)(q / 2U - phase / 4U))), 30U, 210U); hue = (uint8_t)(w1 + w4 / 2U); break;
          default: { const uint8_t polar = mz_max(mz_blob(p, 0U, 44U), mz_blob(p, 128U, 44U)); level = mz_satAdd(18U, scale8(mz_mul(polar, mz_sin8((uint8_t)(phase + q))), 220U)); hue = (uint8_t)(q + phase); break; }
        }
        break;

      // 2 — Partikel & Bara: particles are clusters/streaks over continuous heat.
      case 2U:
        switch (variant) {
          case 0U: { const uint8_t heat = mz_smoothNoise(x, slowTick, 20U, 5U); const uint8_t ember = mz_softMask(heat, 158U, 70U); level = mz_bed(mz_sin8((uint8_t)(q / 2U - phase / 3U)), 34U, 92U); level = mz_satAdd(level, scale8(ember, 188U)); hue = (uint8_t)(190U + heat / 4U); break; }
          case 1U: { const uint8_t aPos = (uint8_t)(phase * 2U); const uint8_t bPos = (uint8_t)(255U - phase * 3U); const uint8_t aGlow = mz_blob(p, aPos, 28U); const uint8_t bGlow = mz_blob(p, bPos, 23U); level = mz_satAdd(24U, mz_max(scale8(aGlow, 220U), scale8(bGlow, 205U))); level = mz_satAdd(level, scale8(w2, 60U)); hue = (uint8_t)(aGlow > bGlow ? aPos : bPos); break; }
          case 2U: { const uint8_t cascade = mz_sin8((uint8_t)(q + phase * 3U)); const uint8_t shards = mz_softMask(cascade, 174U, 54U); level = mz_bed(w1, 18U, 95U); level = mz_satAdd(level, scale8(shards, 225U)); level = mz_satAdd(level, scale8(mz_blob(p, (uint8_t)(phase + 100U), 25U), 85U)); hue = (uint8_t)(cascade + phase); break; }
          case 3U: { const uint8_t vein = mz_sin8((uint8_t)(q * 2U + mz_sin8((uint8_t)(q + phase)) / 2U)); const uint8_t core = mz_softMask(vein, 170U, 55U); level = mz_bed(vein, 26U, 130U); level = mz_satAdd(level, scale8(core, 115U)); hue = (uint8_t)(150U + vein / 3U); break; }
          case 4U: { const uint8_t ash = mz_smoothNoise(x, (uint8_t)(slowTick + x / 3U), 24U, 3U); const uint8_t streak = mz_blob(p, (uint8_t)(phase + ash / 5U), 32U); level = mz_bed(w4, 20U, 70U); level = mz_satAdd(level, scale8(streak, 145U)); level = mz_satAdd(level, scale8(mz_softMask(ash, 205U, 45U), 120U)); hue = (uint8_t)(ash + 80U); break; }
          case 5U: { const uint8_t neb = mz_mul(mz_sin8((uint8_t)(q + phase)), mz_sin8((uint8_t)(q * 3U - phase2))); const uint8_t flare = mz_softMask(noise, 186U, 60U); level = mz_bed(neb, 20U, 170U); level = mz_satAdd(level, scale8(flare, 165U)); hue = (uint8_t)(neb + noise / 3U); break; }
          case 6U: { const uint8_t lava = mz_sin8((uint8_t)(q + mz_sin8((uint8_t)(q / 2U + phase)) / 3U - phase / 2U)); level = mz_bed(lava, 35U, 185U); level = mz_satAdd(level, scale8(mz_blob(p, (uint8_t)(phase * 2U), 37U), 95U)); hue = (uint8_t)(175U + lava / 4U); break; }
          case 7U: { const uint8_t meteorHead = (uint8_t)(phase * 3U); const uint8_t meteorTail = mz_tail(p, meteorHead, 86U); level = mz_satAdd(10U, scale8(meteorTail, 245U)); level = mz_satAdd(level, scale8(mz_blob(p, meteorHead, 13U), 160U)); hue = (uint8_t)(meteorHead + 24U); break; }
          case 8U: { const uint8_t p1 = (uint8_t)(phase * 2U); const uint8_t p2 = (uint8_t)(phase * 2U + 91U); const uint8_t tails = mz_max(mz_tail(p, p1, 72U), mz_tail(p, p2, 54U)); level = mz_satAdd(16U, scale8(tails, 215U)); hue = (uint8_t)(p1 + tails / 2U); break; }
          default: { const uint8_t curtain = mz_avg(mz_sin8((uint8_t)(q + phase / 2U)), mz_sin8((uint8_t)(q / 2U - phase))); const uint8_t sparks = mz_softMask(grain, 222U, 33U); level = mz_bed(curtain, 32U, 175U); level = mz_satAdd(level, scale8(sparks, 145U)); hue = (uint8_t)(curtain + grain / 2U); break; }
        }
        break;

      // 3 — Geometri & Pola: deliberate blocks, chevrons, bands and symmetry.
      case 3U:
        switch (variant) {
          case 0U: { const uint8_t band = (uint8_t)(q + phase); level = mz_satAdd(26U, scale8(mz_sin8(band), 205U)); hue = band; break; }
          case 1U: { const uint8_t block = (uint8_t)(((x / (2U + (density >> 5U))) + tick / 5U) & 7U); const uint8_t digital = (block & 1U) ? 205U : 55U; level = mz_satAdd(digital, scale8(w2, 55U)); hue = (uint8_t)(block * 32U + phase); break; }
          case 2U: { const uint8_t symbol = (uint8_t)(((q / 8U) + tick / 7U) & 15U); const bool on = (symbol == 1U || symbol == 2U || symbol == 5U || symbol == 6U || symbol == 9U || symbol == 10U); level = on ? 238U : 24U; level = mz_satAdd(level, scale8(w3, 35U)); hue = (uint8_t)(symbol * 16U + phase); break; }
          case 3U: { const uint8_t bar = (uint8_t)((q / 3U + phase) & 63U); level = (bar < 22U) ? (uint8_t)(235U - bar * 4U) : 20U; hue = (uint8_t)(bar * 4U + q); break; }
          case 4U: { const uint8_t fold = (p < 128U) ? p : (uint8_t)(255U - p); const uint8_t chevron = mz_sin8((uint8_t)(fold * 3U + phase)); level = mz_satAdd(32U, scale8(mz_peak(absdiff8(chevron, 128U), 86U), 220U)); hue = (uint8_t)(fold * 2U + phase); break; }
          case 5U: { const uint8_t tile = (uint8_t)(((x / 3U) + (tick / 9U)) & 7U); const uint8_t quilt = (uint8_t)(tile * 27U + mz_sin8((uint8_t)(q + phase)) / 3U); level = mz_satAdd(34U, quilt); hue = (uint8_t)(tile * 31U + phase); break; }
          case 6U: { const uint8_t fold = (p < 128U) ? p : (uint8_t)(255U - p); const uint8_t fract = mz_sin8((uint8_t)(fold * 7U + phase)); level = mz_bed(mz_mul(fract, tri), 24U, 220U); hue = (uint8_t)(fract + fold); break; }
          case 7U: { const uint8_t fib = (uint8_t)(x * 13U + x / 2U + phase); level = mz_satAdd(24U, scale8(mz_peak(dist8(fib, 128U), 72U), 220U)); hue = fib; break; }
          case 8U: { const uint8_t a = mz_sin8((uint8_t)(q + phase / 2U)); const uint8_t b = mz_sin8((uint8_t)(q / 2U - phase)); level = mz_satAdd(24U, scale8(mz_absdiff(a, b), 215U)); hue = a; break; }
          default: { const uint8_t mirror = (p < 128U) ? p : (uint8_t)(255U - p); const uint8_t step = (uint8_t)(((mirror / 8U) + phase / 8U) & 15U); const uint8_t diamond = (step < 8U) ? (uint8_t)(step * 29U) : (uint8_t)((15U - step) * 29U); level = mz_satAdd(32U, diamond); hue = (uint8_t)(step * 17U + phase); break; }
        }
        break;

      // 4 — Atmosfer & Cairan: slow cloud volumes, curtains and liquid contours.
      case 4U:
        switch (variant) {
          case 0U: { const uint8_t cloudA = mz_smoothNoise(x, (uint8_t)(slowTick + phase / 4U), 40U, 9U); const uint8_t cloudB = mz_smoothNoise(x, (uint8_t)(slowTick / 2U + phase / 7U), 41U, 17U); level = mz_bed(mz_avg(cloudA, cloudB), 34U, 195U); hue = mz_avg(cloudA, cloudB); break; }
          case 1U: { const uint8_t veil = mz_sin8((uint8_t)(q / 2U + phase / 3U)); const uint8_t curtain = mz_sin8((uint8_t)(q / 5U - phase / 2U + veil / 3U)); level = mz_bed(mz_avg(veil, curtain), 38U, 195U); level = mz_satAdd(level, scale8(mz_peak(absdiff8(veil, 128U), 45U), 60U)); hue = (uint8_t)(curtain + q / 4U); break; }
          case 2U: { const uint8_t silk = mz_sin8((uint8_t)(q + mz_sin8((uint8_t)(phase + q / 5U)) / 2U)); level = mz_bed(silk, 42U, 190U); level = mz_satAdd(level, scale8(mz_sin8((uint8_t)(q * 2U - phase / 2U)), 65U)); hue = silk; break; }
          case 3U: { const uint8_t chrome = mz_sin8((uint8_t)(q * 3U + phase + mz_sin8((uint8_t)(q + phase / 2U)) / 3U)); const uint8_t glint = mz_peak(absdiff8(chrome, 128U), 28U); level = mz_bed(chrome, 28U, 145U); level = mz_satAdd(level, scale8(glint, 145U)); hue = (uint8_t)(chrome + glint); break; }
          case 4U: { const uint8_t vapor = mz_avg(mz_sin8((uint8_t)(q + phase / 2U)), mz_sin8((uint8_t)(q / 3U - phase))); const uint8_t turbulence = mz_smoothNoise(x, slowTick, 44U, 7U); level = mz_bed(vapor, 35U, 178U); level = mz_satAdd(level, scale8(turbulence, 45U)); hue = (uint8_t)(vapor + turbulence / 3U); break; }
          case 5U: { const uint8_t cloud = mz_smoothNoise(x, (uint8_t)(slowTick / 2U + phase / 6U), 45U, 13U); level = mz_bed(cloud, 48U, 190U); level = mz_satAdd(level, scale8(mz_sin8((uint8_t)(q / 3U + phase / 4U)), 42U)); hue = cloud; break; }
          case 6U: { const uint8_t ocean = mz_sin8((uint8_t)(q / 2U + phase / 3U)); const uint8_t swell = mz_sin8((uint8_t)(q + phase / 2U + ocean / 4U)); level = mz_bed(mz_avg(ocean, swell), 48U, 185U); hue = (uint8_t)(150U + swell / 3U); break; }
          case 7U: { const uint8_t rainNoise = mz_smoothNoise(x, (uint8_t)(slowTick + x / 4U), 47U, 3U); const uint8_t streak = mz_blob(p, (uint8_t)(phase + rainNoise / 3U), 30U); level = mz_bed(w4, 24U, 70U); level = mz_satAdd(level, scale8(streak, 185U)); hue = (uint8_t)(rainNoise + phase); break; }
          case 8U: { const uint8_t haze = mz_avg(mz_sin8((uint8_t)(q / 3U + phase / 5U)), mz_sin8((uint8_t)(q / 5U - phase / 7U))); level = mz_bed(haze, 64U, 130U); hue = (uint8_t)(haze + phase / 4U); break; }
          default: { const uint8_t opalA = mz_sin8((uint8_t)(q + phase / 3U)); const uint8_t opalB = mz_sin8((uint8_t)(q * 2U - phase / 4U)); const uint8_t opalC = mz_sin8((uint8_t)(q / 3U + phase / 2U)); level = mz_bed(mz_avg(opalA, mz_avg(opalB, opalC)), 42U, 190U); hue = (uint8_t)(opalA / 3U + opalB / 3U + opalC / 3U); break; }
        }
        break;

      // 5 — Mesin & Racing: assertive rev sweeps, torque surges and start lights.
      case 5U:
        switch (variant) {
          case 0U: { const uint8_t beat = mz_sin8((uint8_t)(phase * 2U)); const uint8_t thump = mz_softMask(beat, 176U, 58U); level = mz_bed(w1, 24U, 80U); level = mz_satAdd(level, scale8(thump, 220U)); hue = (uint8_t)(q + thump); break; }
          case 1U: { const uint8_t idle = mz_sin8((uint8_t)(q * 3U + phase / 3U + mz_sin8((uint8_t)(phase / 2U)) / 3U)); level = mz_bed(idle, 58U, 160U); level = mz_satAdd(level, scale8(w2, 48U)); hue = (uint8_t)(idle + 32U); break; }
          case 2U: { const uint8_t sweep = (uint8_t)(phase * 2U); const uint8_t band = mz_blob(p, sweep, 42U); const uint8_t tail = mz_tail(p, sweep, 72U); level = mz_satAdd(12U, mz_max(scale8(band, 220U), scale8(tail, 115U))); hue = (uint8_t)(sweep + band); break; }
          case 3U: { const uint8_t rev = mz_sin8((uint8_t)(phase * 3U)); const uint8_t strips = mz_sin8((uint8_t)(q * 2U + phase * 2U)); level = mz_bed(strips, 20U, 80U); level = mz_satAdd(level, scale8(mz_softMask(rev, 170U, 65U), 205U)); hue = (uint8_t)(strips + rev / 2U); break; }
          case 4U: { const uint8_t gear = (uint8_t)((tick / 24U) % 5U); const uint8_t zone = mz_blob(p, (uint8_t)(gear * 51U), 42U); level = mz_bed(w1, 25U, 65U); level = mz_satAdd(level, scale8(zone, 210U)); hue = (uint8_t)(gear * 48U + phase); break; }
          case 5U: { const uint8_t rung = (uint8_t)((q + phase * 3U) & 63U); const uint8_t ladder = (rung < 40U) ? 230U : 18U; level = mz_satAdd(ladder, scale8(w3, 35U)); hue = (uint8_t)(rung * 4U + phase); break; }
          case 6U: { const uint8_t launch = (uint8_t)((tick / 10U) % 8U); const uint8_t grid = (uint8_t)((x / 2U + launch) % 8U); const uint8_t start = (grid <= launch) ? (uint8_t)(235U - grid * 18U) : 20U; level = mz_satAdd(start, scale8(w2, 55U)); hue = (uint8_t)(grid * 30U + phase); break; }
          case 7U: { const uint8_t after = mz_sin8((uint8_t)(q * 2U + phase * 3U)); const uint8_t flame = mz_mul(after, mz_sin8((uint8_t)(q / 2U - phase))); level = mz_bed(flame, 28U, 220U); level = mz_satAdd(level, scale8(mz_blob(p, phase, 38U), 75U)); hue = (uint8_t)(185U + flame / 4U); break; }
          case 8U: { const uint8_t turbo = mz_sin8((uint8_t)(q * 3U + phase + mz_sin8((uint8_t)(q * 5U - phase)) / 2U)); level = mz_bed(turbo, 20U, 225U); hue = (uint8_t)(turbo + q); break; }
          default: { const uint8_t flash = mz_sin8((uint8_t)(tick * 5U)); const uint8_t lane = mz_sin8((uint8_t)(q * 2U + phase)); level = mz_bed(lane, 16U, 55U); level = mz_satAdd(level, scale8(mz_softMask(flash, 194U, 42U), 230U)); hue = (uint8_t)(lane + flash); break; }
        }
        break;

      // 6 — Cahaya Premium: no hard black gaps; silk, satin, pearl and warm glow.
      case 6U:
        switch (variant) {
          case 0U: { const uint8_t champagne = mz_avg(w1, mz_sin8((uint8_t)(q / 3U - phase / 4U))); level = mz_bed(champagne, 72U, 155U); level = mz_satAdd(level, scale8(mz_peak(absdiff8(grain, 128U), 18U), 60U)); hue = (uint8_t)(32U + champagne / 2U); break; }
          case 1U: { const uint8_t velvet = mz_sin8((uint8_t)(q / 2U + phase / 7U)); level = mz_bed(velvet, 82U, 148U); level = mz_satAdd(level, scale8(edge, 34U)); hue = (uint8_t)(16U + velvet / 3U); break; }
          case 2U: { const uint8_t sun = mz_sin8((uint8_t)(phase / 3U)); const uint8_t horizon = mz_sin8((uint8_t)(q / 3U + phase / 8U)); level = mz_bed(mz_avg(sun, horizon), 68U, 165U); hue = (uint8_t)(24U + sun / 3U); break; }
          case 3U: { const uint8_t pearl = mz_sin8((uint8_t)(q + phase / 2U + mz_sin8((uint8_t)(phase + q / 4U)) / 3U)); level = mz_bed(pearl, 64U, 170U); level = mz_satAdd(level, scale8(mz_peak(absdiff8(pearl, 128U), 22U), 55U)); hue = (uint8_t)(pearl + 64U); break; }
          case 4U: { const uint8_t satin = mz_sin8((uint8_t)(q / 2U + mz_sin8((uint8_t)(phase + q / 5U)) / 2U)); level = mz_bed(satin, 82U, 150U); hue = (uint8_t)(q + phase / 3U); break; }
          case 5U: { const uint8_t opal = mz_avg(w1, mz_avg(w3, w4)); const uint8_t shimmer = mz_softMask(grain, 220U, 36U); level = mz_bed(opal, 72U, 155U); level = mz_satAdd(level, scale8(shimmer, 48U)); hue = (uint8_t)(opal + grain / 4U); break; }
          case 6U: { const uint8_t chrome = mz_sin8((uint8_t)(q * 3U + phase / 2U)); const uint8_t shine = mz_peak(absdiff8(chrome, 128U), 25U); level = mz_bed(chrome, 58U, 130U); level = mz_satAdd(level, scale8(shine, 115U)); hue = (uint8_t)(chrome + 100U); break; }
          case 7U: { const uint8_t flame = mz_sin8((uint8_t)(q / 2U + phase + mz_sin8((uint8_t)(q * 3U - phase)) / 3U)); level = mz_bed(flame, 65U, 160U); hue = (uint8_t)(12U + flame / 3U); break; }
          case 8U: { const uint8_t halo = mz_blob(p, (uint8_t)(phase / 2U), 100U); const uint8_t breath = mz_sin8((uint8_t)(phase / 3U)); level = mz_bed(mz_max(halo, breath), 48U, 178U); hue = (uint8_t)(128U + halo / 3U); break; }
          default: { const uint8_t crownA = mz_sin8((uint8_t)(q / 2U + phase / 3U)); const uint8_t crownB = mz_sin8((uint8_t)(q / 5U - phase / 2U)); level = mz_bed(mz_avg(crownA, crownB), 68U, 180U); hue = (uint8_t)(crownA + crownB / 3U); break; }
        }
        break;

      // 7 — Gerbang & Simetri: mirrored motion, apertures and broad portal rings.
      case 7U:
        switch (variant) {
          case 0U: { const uint8_t d = (p < 128U) ? p : (uint8_t)(255U - p); const uint8_t front = (uint8_t)(phase / 2U); level = mz_satAdd(18U, scale8(mz_blob(d, front, 48U), 230U)); hue = (uint8_t)(d + phase); break; }
          case 1U: { const uint8_t front = (uint8_t)(phase / 2U); const uint8_t d = (p < 128U) ? p : (uint8_t)(255U - p); level = mz_satAdd(18U, scale8(mz_blob(d, (uint8_t)(128U - front), 42U), 228U)); hue = (uint8_t)(255U - d + phase); break; }
          case 2U: { const uint8_t bloom = mz_blob(p, 128U, (uint8_t)(18U + (phase / 3U))); level = mz_bed(bloom, 26U, 225U); hue = (uint8_t)(bloom + phase); break; }
          case 3U: { const uint8_t d = (p < 128U) ? p : (uint8_t)(255U - p); const uint8_t bloom = mz_blob(d, (uint8_t)(phase / 3U), 46U); level = mz_bed(bloom, 34U, 210U); hue = (uint8_t)(d * 2U + phase); break; }
          case 4U: { const uint8_t portalA = mz_blob(p, (uint8_t)(phase + 64U), 24U); const uint8_t portalB = mz_blob(p, (uint8_t)(phase + 192U), 24U); level = mz_satAdd(22U, mz_max(scale8(portalA, 220U), scale8(portalB, 220U))); level = mz_satAdd(level, scale8(w1, 65U)); hue = (uint8_t)(portalA > portalB ? 30U : 190U); break; }
          case 5U: { const uint8_t winCenter = (uint8_t)(phase); const uint8_t win = mz_blob(p, winCenter, 74U); level = mz_bed(win, 24U, 220U); hue = (uint8_t)(winCenter + p / 3U); break; }
          case 6U: { const uint8_t open = mz_sin8((uint8_t)(phase / 2U)); const uint8_t aperture = (uint8_t)(18U + open / 3U); const uint8_t d = absdiff8(p, 128U); level = mz_satAdd(20U, scale8(mz_peak(absdiff8(d, aperture), 34U), 220U)); hue = (uint8_t)(open + d); break; }
          case 7U: { const uint8_t gate = mz_sin8((uint8_t)(q * 2U + phase * 2U)); const uint8_t beam = mz_softMask(gate, 165U, 60U); level = mz_bed(w2, 22U, 70U); level = mz_satAdd(level, scale8(beam, 210U)); hue = (uint8_t)(q + beam); break; }
          case 8U: { const uint8_t sweep = mz_blob(p, phase, 30U); const uint8_t tail = mz_tail(p, phase, 100U); level = mz_satAdd(16U, mz_max(scale8(sweep, 215U), scale8(tail, 110U))); hue = (uint8_t)(phase + sweep); break; }
          default: { const uint8_t a = mz_blob(p, phase, 20U); const uint8_t b = mz_blob(p, (uint8_t)(phase + 128U), 20U); level = mz_satAdd(20U, mz_max(a, b)); level = mz_satAdd(level, scale8(mz_absdiff(w1, w2), 70U)); hue = (uint8_t)(a + b + phase); break; }
        }
        break;

      // 8 — Eksperimental: clustered noise, cellular pulses, reaction and chaos.
      case 8U:
        switch (variant) {
          case 0U: { const uint8_t quantum = mz_smoothNoise(x, (uint8_t)(slowTick + phase), 80U, 4U); const uint8_t cluster = mz_softMask(quantum, 142U, 85U); level = mz_bed(mz_avg(w1, w3), 30U, 105U); level = mz_satAdd(level, scale8(cluster, 145U)); hue = quantum; break; }
          case 1U: { const uint8_t a = mz_sin8((uint8_t)(q + phase)); const uint8_t b = mz_sin8((uint8_t)(q - phase / 2U)); level = mz_bed(mz_absdiff(a, b), 26U, 220U); hue = (uint8_t)(b + phase); break; }
          case 2U: { const uint8_t n1 = mz_smoothNoise(x, slowTick, 82U, 11U); const uint8_t n2 = mz_smoothNoise(x, (uint8_t)(slowTick + 31U), 83U, 5U); const uint8_t sculpt = mz_mul(n1, n2); level = mz_bed(sculpt, 34U, 205U); hue = (uint8_t)(n1 + n2 / 2U); break; }
          case 3U: { const uint8_t cell = (uint8_t)((q + phase) & 63U); const uint8_t neighbor = mz_sin8((uint8_t)(q * 2U - phase)); const uint8_t live = (cell < 38U) ? 225U : 18U; level = mz_satAdd(live, scale8(neighbor, 50U)); hue = (uint8_t)(cell * 4U + phase); break; }
          case 4U: { const uint8_t binary = (uint8_t)(((x / 2U) ^ (tick / 7U)) & 15U); const uint8_t block = (binary & 1U) ? 205U : 28U; level = mz_satAdd(block, scale8(w2, 62U)); hue = (uint8_t)(binary * 17U + phase); break; }
          case 5U: { const uint8_t reactionA = mz_sin8((uint8_t)(q + phase / 2U)); const uint8_t reactionB = mz_sin8((uint8_t)(q * 3U - phase)); const uint8_t chemical = mz_mul(reactionA, reactionB); level = mz_bed(mz_avg(chemical, mz_absdiff(reactionA, reactionB)), 20U, 225U); hue = chemical; break; }
          case 6U: { const uint8_t wander = (uint8_t)(phase + grain / 3U); const uint8_t trail = mz_tail(p, wander, 110U); level = mz_bed(mz_smoothNoise(x, slowTick, 86U, 8U), 14U, 70U); level = mz_satAdd(level, scale8(trail, 195U)); level = mz_satAdd(level, scale8(mz_blob(p, wander, 18U), 90U)); hue = (uint8_t)(wander + trail / 3U); break; }
          case 7U: { const uint8_t logistic = mz_sin8((uint8_t)(q * 3U + phase + mz_sin8((uint8_t)(q * 5U - phase2)) / 2U)); level = mz_bed(mz_mul(logistic, (uint8_t)(255U - logistic / 3U)), 24U, 215U); hue = logistic; break; }
          case 8U: { const uint8_t lorenzA = mz_sin8((uint8_t)(q + phase)); const uint8_t lorenzB = mz_sin8((uint8_t)(q * 2U - phase2)); const uint8_t ribbon = mz_absdiff(lorenzA, lorenzB); level = mz_bed(mz_max(ribbon, mz_mul(w3, w4)), 22U, 220U); hue = (uint8_t)(lorenzA + lorenzB / 2U); break; }
          default: { const uint8_t cell = mz_sin8((uint8_t)(q / 2U + phase)); const uint8_t pulse = mz_softMask(cell, 150U, 75U); level = mz_bed(w2, 32U, 75U); level = mz_satAdd(level, scale8(pulse, 190U)); hue = (uint8_t)(cell + pulse); break; }
        }
        break;

      // 9 — Signature & Energi: high-impact layered compositions, not one-layer FX.
      default:
        switch (variant) {
          case 0U: { const uint8_t bolt = mz_smoothNoise(x, (uint8_t)(slowTick + phase / 2U), 90U, 4U); const uint8_t core = mz_softMask(bolt, 190U, 54U); level = mz_bed(mz_sin8((uint8_t)(q / 2U + phase)), 28U, 110U); level = mz_satAdd(level, scale8(core, 195U)); hue = (uint8_t)(bolt + phase); break; }
          case 1U: { const uint8_t solar = mz_sin8((uint8_t)(q / 2U + phase / 3U)); const uint8_t flare = mz_blob(p, (uint8_t)(phase + 128U), 68U); level = mz_bed(solar, 34U, 165U); level = mz_satAdd(level, scale8(flare, 145U)); hue = (uint8_t)(solar + flare / 3U); break; }
          case 2U: { const uint8_t fieldA = mz_sin8((uint8_t)(q + phase)); const uint8_t fieldB = mz_cos8((uint8_t)(q * 2U - phase / 2U)); level = mz_bed(mz_avg(fieldA, fieldB), 32U, 185U); level = mz_satAdd(level, scale8(mz_absdiff(fieldA, fieldB), 70U)); hue = fieldA; break; }
          case 3U: { const uint8_t ringA = mz_blob(p, phase, 35U); const uint8_t ringB = mz_blob(p, (uint8_t)(phase + 64U), 35U); const uint8_t lattice = mz_sin8((uint8_t)(q * 2U + phase)); level = mz_bed(mz_max(ringA, ringB), 22U, 170U); level = mz_satAdd(level, scale8(lattice, 78U)); hue = (uint8_t)(ringA + ringB + lattice / 3U); break; }
          case 4U: { const uint8_t warped = (uint8_t)(q + mz_sin8((uint8_t)(q * 2U + phase)) / 2U); const uint8_t warpA = mz_sin8((uint8_t)(warped + phase)); const uint8_t warpB = mz_sin8((uint8_t)(warped * 3U - phase2)); level = mz_bed(mz_avg(warpA, warpB), 25U, 210U); hue = (uint8_t)(warped + warpB / 3U); break; }
          case 5U: { const uint8_t spectral = mz_sin8((uint8_t)(q * 2U + phase)); const uint8_t band = mz_sin8((uint8_t)(q / 2U - phase / 2U)); level = mz_bed(mz_avg(spectral, band), 32U, 195U); hue = (uint8_t)(q + phase * 2U); break; }
          case 6U: { const uint8_t ion = mz_sin8((uint8_t)(q * 4U + phase)); const uint8_t packet = mz_smoothNoise(x, (uint8_t)(slowTick + phase / 6U), 96U, 6U); level = mz_bed(mz_mul(ion, packet), 20U, 220U); hue = (uint8_t)(ion + packet / 2U); break; }
          case 7U: { const uint8_t gravity = mz_blob(p, (uint8_t)(128U + mz_sin8((uint8_t)(phase / 3U)) / 4U), 108U); const uint8_t orbit = mz_sin8((uint8_t)(phase + q / 2U)); level = mz_bed(mz_mul(gravity, orbit), 25U, 220U); hue = (uint8_t)(gravity + phase); break; }
          case 8U: { const uint8_t horizon = mz_sin8((uint8_t)(q / 2U + phase)); const uint8_t line = mz_blob(p, phase, 38U); level = mz_bed(horizon, 32U, 150U); level = mz_satAdd(level, scale8(line, 120U)); hue = (uint8_t)(horizon + line / 2U); break; }
          default: { const uint8_t silk = mz_sin8((uint8_t)(q + phase / 2U)); const uint8_t storm = mz_sin8((uint8_t)(q * 3U - phase2)); const uint8_t pulse = mz_blob(p, (uint8_t)(phase + 128U), 70U); const uint8_t energy = mz_max(mz_mul(silk, storm), mz_avg(w3, w4)); level = mz_bed(energy, 42U, 175U); level = mz_satAdd(level, scale8(pulse, 160U)); hue = (uint8_t)(silk + storm / 2U + phase); break; }
        }
        break;
    }

    // Detail adds a fine moving texture, not isolated spark dots. The amount
    // stays bounded so it enriches the base composition without flattening it.
    const uint8_t microTexture = mz_sin8((uint8_t)(q * 5U + tick * 3U + id * 23U));
    level = mz_satAdd(level, scale8(microTexture, (uint8_t)(detail * 2U)));
    hue = (uint8_t)(hue + scale8(microTexture, (uint8_t)(detail >> 1U)));

    // Family-specific floor keeps visual continuity without washing out contrast.
    const uint8_t floorLevel = (family == 6U) ? 42U : (family == 4U) ? 28U : 12U;
    level = (level < floorLevel) ? floorLevel : level;
    level = scale8(level, levelGain);
    rawLevel[x] = level;
    rawHue[x] = hue;
  }

  // Motion persistence is deliberately different by family: embers/chaos leave
  // shorter trails; premium and atmospheric effects retain a slower silk afterglow.
  for (uint16_t x = 0; x < len; x++) {
    const uint8_t old = scale8(afterglow[x], trailFade);
    afterglow[x] = mz_max(rawLevel[x], old);
  }

  for (uint16_t x = 0; x < len; x++) {
    const uint16_t left = (x == 0U) ? x : (uint16_t)(x - 1U);
    const uint16_t right = (x + 1U < len) ? (uint16_t)(x + 1U) : x;
    const uint16_t sum = (uint16_t)afterglow[left] +
                         (uint16_t)afterglow[x] * 2U +
                         (uint16_t)afterglow[right];
    const uint8_t soft = (uint8_t)(sum >> 2U);
    const uint8_t shoulder = mz_max(scale8(afterglow[left], 158U), scale8(afterglow[right], 158U));
    const uint8_t outLevel = mz_max(afterglow[x], mz_max(scale8(soft, 190U), shoulder));
    const uint32_t paletteColor = SEGMENT.color_from_palette(rawHue[x], false, true, 0, 255);
    SEGMENT.setPixelColor(x, color_fade(paletteColor, outLevel));
  }
}

// One WLED FX slot exposes all 100 algorithms through SEGMENT.custom1 (c1).
static void mode_mizuma_signature_100(void) {
  const uint8_t index = clampIndex(SEGMENT.custom1);
  // Reset segment-local scratch whenever the internal algorithm changes.
  const uint16_t algorithmTag = (uint16_t)(0xA500U | ((uint16_t)(index & 0x7FU) << 1U));
  if ((SEGENV.aux1 & 0xFFFEU) != algorithmTag) {
    SEGENV.call = 0U;
    SEGENV.step = 0U;
    SEGENV.aux0 = 0U;
    SEGENV.aux1 = algorithmTag;
  }
  SEGMENT.beginDraw();
  renderAlgorithm(index);
}

// Keep one registered WLED effect: c1 selects algorithm 0..99,
// c2 controls spatial density, c3 controls detail; WLED palette remains active.
static const char MZ_SIGNATURE_100_METADATA[] PROGMEM =
  "MZ | Signature 100@!,Intensity,Algorithm ID,Density,Detail;!,!;!;1;sx=128,ix=180,c1=0,c2=160,c3=16,pal=0";

static inline uint8_t registerWLEDEffect(void) {
  return strip.addEffect(255, &mode_mizuma_signature_100, MZ_SIGNATURE_100_METADATA);
}

} // namespace MizumaSignature100
