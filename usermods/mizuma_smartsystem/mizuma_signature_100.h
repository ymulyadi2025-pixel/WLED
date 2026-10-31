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

static inline uint32_t color0() {
  uint32_t c = SEGCOLOR(0);
  return c ? c : 0x00FF8A18UL; // amber Mizuma default (RGB; W=0)
}
static inline uint32_t color1() {
  uint32_t c = SEGCOLOR(1);
  return c ? c : 0x00FFD38AUL; // warm highlight (RGB; W=0)
}
static inline uint32_t color2() {
  uint32_t c = SEGCOLOR(2);
  return c ? c : 0x00FF4A0AUL; // ember accent (RGB; W=0)
}

// -----------------------------------------------------------------------------
// Stateful algorithms: persistent memory is segment-local and allocated only
// while that particular algorithm is selected.
// -----------------------------------------------------------------------------
static void renderEmberline(uint16_t len, uint32_t now) {
  const unsigned bytes = (unsigned)len * 2U;
  if (!SEGENV.allocateData(bytes)) { SEGMENT.fill(color0()); return; }
  uint8_t *heat = SEGENV.data;
  uint8_t *next = SEGENV.data + len;
  if (SEGENV.call == 0U) { memset(heat, 0, bytes); SEGENV.step = now; }
  const uint16_t interval = (uint16_t)(58U - ((uint16_t)SEGMENT.speed * 42U) / 255U);
  if ((uint32_t)(now - SEGENV.step) >= interval) {
    SEGENV.step = now;
    const uint8_t cooling = (uint8_t)(3U + (SEGMENT.custom2 >> 3U));
    for (uint16_t i = 0; i < len; i++) {
      const uint8_t l = heat[(i == 0U) ? i : (i - 1U)];
      const uint8_t r = heat[(i + 1U < len) ? (i + 1U) : i];
      const uint8_t d = (uint8_t)(((uint16_t)l + (uint16_t)heat[i] * 2U + r) >> 2U);
      next[i] = (d > cooling) ? (uint8_t)(d - cooling) : 0U;
    }
    const uint8_t sparks = (uint8_t)(1U + (SEGMENT.intensity >> 6U));
    for (uint8_t s = 0; s < sparks; s++) {
      const uint16_t p = (uint16_t)(((uint16_t)hw_random8() * len) >> 8U);
      next[p] = max8(next[p], (uint8_t)(170U + (hw_random8() >> 2U)));
    }
    memcpy(heat, next, len);
  }
  const uint32_t c0 = color0(), c2 = color2();
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t h = heat[i];
    uint32_t c = (h > 185U) ? color_blend(c0, c2, (uint8_t)(h - 120U)) : c0;
    SEGMENT.setPixelColor(i, color_fade(c, h));
  }
}

static void renderFireflySwarm(uint16_t len, uint32_t now) {
  if (!SEGENV.allocateData(len)) { SEGMENT.fill(color0()); return; }
  uint8_t *trail = SEGENV.data;
  if (SEGENV.call == 0U) { memset(trail, 0, len); SEGENV.step = now; SEGENV.aux0 = len / 2U; }
  const uint16_t interval = (uint16_t)(35U + ((255U - SEGMENT.speed) >> 2U));
  if ((uint32_t)(now - SEGENV.step) >= interval) {
    SEGENV.step = now;
    for (uint16_t i = 0; i < len; i++) trail[i] = scale8(trail[i], 218U);
    const uint8_t count = (uint8_t)(1U + (SEGMENT.intensity >> 5U));
    for (uint8_t s = 0; s < count; s++) {
      uint16_t p = (uint16_t)(((uint16_t)hw_random8() * len) >> 8U);
      trail[p] = 255U;
      if (p > 0U) trail[p - 1U] = max8(trail[p - 1U], 95U);
      if (p + 1U < len) trail[p + 1U] = max8(trail[p + 1U], 95U);
    }
  }
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t v = trail[i];
    uint32_t c = color_blend(color0(), color1(), v);
    SEGMENT.setPixelColor(i, color_fade(c, v));
  }
}

static void renderCellularRule(uint16_t len, uint32_t now) {
  const unsigned bytes = (unsigned)len * 2U;
  if (!SEGENV.allocateData(bytes)) { SEGMENT.fill(color0()); return; }
  uint8_t *state = SEGENV.data;
  uint8_t *next = SEGENV.data + len;
  if (SEGENV.call == 0U) {
    memset(state, 0, bytes);
    state[len / 2U] = 1U;
    if (len > 8U) { state[len / 2U - 3U] = 1U; state[len / 2U + 3U] = 1U; }
    SEGENV.step = now;
  }
  const uint16_t interval = (uint16_t)(110U - ((uint16_t)SEGMENT.speed * 82U) / 255U);
  if ((uint32_t)(now - SEGENV.step) >= interval) {
    SEGENV.step = now;
    for (uint16_t i = 0; i < len; i++) {
      const uint8_t l = state[(i == 0U) ? (len - 1U) : (i - 1U)];
      const uint8_t c = state[i];
      const uint8_t r = state[(i + 1U == len) ? 0U : (i + 1U)];
      // Elementary cellular automaton Rule 30, not a palette animation.
      next[i] = (uint8_t)(l ^ (c | r));
    }
    memcpy(state, next, len);
  }
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t on = state[i] ? 255U : 0U;
    const uint32_t c = color_blend(color0(), color1(), (uint8_t)(on ? 220U : 0U));
    SEGMENT.setPixelColor(i, color_fade(c, on));
  }
}

static void renderReactionFilament(uint16_t len, uint32_t now) {
  const unsigned bytes = (unsigned)len * 4U;
  if (!SEGENV.allocateData(bytes)) { SEGMENT.fill(color0()); return; }
  uint8_t *u = SEGENV.data;
  uint8_t *v = u + len;
  uint8_t *un = v + len;
  uint8_t *vn = un + len;
  if (SEGENV.call == 0U) {
    memset(u, 220, len); memset(v, 0, len);
    for (uint8_t k = 0; k < 3U; k++) {
      const uint16_t p = (uint16_t)(((uint16_t)(70U + k * 61U) * len) / 255U);
      v[(p < len) ? p : (len - 1U)] = 210U;
    }
    SEGENV.step = now;
  }
  const uint16_t interval = (uint16_t)(70U - ((uint16_t)SEGMENT.speed * 48U) / 255U);
  if ((uint32_t)(now - SEGENV.step) >= interval) {
    SEGENV.step = now;
    const int feed = 5 + (SEGMENT.custom2 >> 5U);
    const int kill = 28 + (SEGMENT.custom3 >> 1U);
    for (uint16_t i = 0; i < len; i++) {
      const uint16_t il = (i == 0U) ? (len - 1U) : (i - 1U);
      const uint16_t ir = (i + 1U == len) ? 0U : (i + 1U);
      const int lapU = (int)u[il] + (int)u[ir] - 2 * (int)u[i];
      const int lapV = (int)v[il] + (int)v[ir] - 2 * (int)v[i];
      const int reaction = ((int)u[i] * (int)v[i] * (int)v[i]) / 65025;
      int du = (lapU * 3) / 4 - reaction + (feed * (255 - (int)u[i])) / 255;
      int dv = (lapV * 1) / 3 + reaction - ((feed + kill) * (int)v[i]) / 255;
      int nu = (int)u[i] + du;
      int nv = (int)v[i] + dv;
      un[i] = (uint8_t)((nu < 0) ? 0 : ((nu > 255) ? 255 : nu));
      vn[i] = (uint8_t)((nv < 0) ? 0 : ((nv > 255) ? 255 : nv));
    }
    memcpy(u, un, len); memcpy(v, vn, len);
  }
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t chemical = v[i];
    uint32_t c = color_blend(color0(), color2(), chemical);
    SEGMENT.setPixelColor(i, color_fade(c, (uint8_t)(55U + chemical * 200U / 255U)));
  }
}

static void renderBrownianTrail(uint16_t len, uint32_t now) {
  if (!SEGENV.allocateData(len)) { SEGMENT.fill(color0()); return; }
  uint8_t *trail = SEGENV.data;
  if (SEGENV.call == 0U) { memset(trail, 0, len); SEGENV.aux0 = len / 2U; SEGENV.aux1 = 1U; SEGENV.step = now; }
  const uint16_t interval = (uint16_t)(42U - ((uint16_t)SEGMENT.speed * 25U) / 255U);
  if ((uint32_t)(now - SEGENV.step) >= interval) {
    SEGENV.step = now;
    for (uint16_t i = 0; i < len; i++) trail[i] = scale8(trail[i], 232U);
    int pos = (int)SEGENV.aux0;
    int dir = (SEGENV.aux1 & 1U) ? 1 : -1;
    const uint8_t rnd = hw_random8();
    if (rnd < 86U) dir = -dir;
    pos += dir * (1 + (rnd >> 6U));
    while (pos < 0) pos += len;
    while (pos >= len) pos -= len;
    SEGENV.aux0 = (uint16_t)pos;
    SEGENV.aux1 = (dir > 0) ? 1U : 0U;
    trail[pos] = 255U;
    if (pos > 0) trail[pos - 1] = max8(trail[pos - 1], 130U);
    if (pos + 1 < len) trail[pos + 1] = max8(trail[pos + 1], 130U);
  }
  for (uint16_t i = 0; i < len; i++) {
    uint32_t c = color_blend(color0(), color1(), trail[i]);
    SEGMENT.setPixelColor(i, color_fade(c, trail[i]));
  }
}

// -----------------------------------------------------------------------------
// Generic analytic renderer. Each case below is a separate spatial/temporal
// equation and has its own named motion structure. It does not invoke native FX.
// -----------------------------------------------------------------------------
static void renderAlgorithm(uint8_t id) {
  const uint16_t len = SEGLEN;
  if (len == 0U) return;
  const uint32_t now = strip.now;
  if (len < 2U) { SEGMENT.setPixelColor(0, color0()); return; }

  if (id == 20U) { renderEmberline(len, now); return; }
  if (id == 21U) { renderFireflySwarm(len, now); return; }
  if (id == 83U) { renderCellularRule(len, now); return; }
  if (id == 85U) { renderReactionFilament(len, now); return; }
  if (id == 86U) { renderBrownianTrail(len, now); return; }

  const uint8_t speed = SEGMENT.speed;
  const uint8_t intensity = SEGMENT.intensity;
  const uint8_t density = (uint8_t)(1U + (SEGMENT.custom2 >> 4U));
  const uint8_t frameDiv = (uint8_t)(20U + ((255U - speed) * 2U) / 7U);
  const uint8_t ph = (uint8_t)(now / frameDiv);
  const uint8_t slow = (uint8_t)(now / (uint16_t)(90U + ((255U - speed) >> 1U)));
  const uint8_t ph2 = (uint8_t)(now / (uint16_t)(31U + ((255U - speed) >> 2U)));
  const uint32_t c0 = color0(), c1 = color1(), c2 = color2();
  const uint8_t width = (uint8_t)(10U + (SEGMENT.custom2 >> 2U));
  const uint8_t detail = SEGMENT.custom3;

  for (uint16_t x = 0; x < len; x++) {
    const uint8_t p = (uint8_t)(((uint32_t)x * 255U) / (len - 1U));
    const uint8_t u = (uint8_t)(p * density);
    const uint8_t a = mz_sin8((uint8_t)(u + ph));
    const uint8_t b = mz_sin8((uint8_t)(u * 2U - ph * 2U + 37U));
    const uint8_t c = mz_cos8((uint8_t)(u * 3U + ph / 2U + 91U));
    const uint8_t triWave = triwave8((uint8_t)(u + ph2));
    const uint8_t e = mz_sin8((uint8_t)(u * 5U + ph2 * 3U + 19U));
    const uint8_t ramp = (uint8_t)(u + ph);
    const uint8_t hash = hash8(x, slow, id);
    const uint8_t center = peak8(absdiff8(p, 128U), 128U);
    const uint8_t edge = (uint8_t)(255U - center);
    uint8_t level = 0U, mix = 0U;

    switch (id) {
      case 0U: { uint8_t d=mz_dist(p,ph); level=mz_peak(d, uint8_t(24U+(intensity>>2))); mix=(d<18U)?220U:35U; break; }
      case 1U: { uint8_t d1=mz_dist(p,ph), d2=mz_dist(p,uint8_t(ph+128U)); level=mz_max(mz_peak(d1,34),mz_peak(d2,34)); mix=(d1<d2)?48U:218U; break; }
      case 2U: { uint8_t tail=(uint8_t)(uint8_t(p-ph)); level=(tail<110U)?(uint8_t)(255U-(uint16_t)tail*2U):0U; mix=(tail<18U)?240U:80U; break; }
      case 3U: { uint8_t cross=mz_absdiff(a,b); level=(uint8_t)(255U-cross); mix=a; break; }
      case 4U: { uint8_t cell=(uint8_t)((u+ph)&63U); level=mz_peak(cell, width); mix=(u&64U)?190U:55U; break; }
      case 5U: {
        const uint8_t h1 = mz_sin8((uint8_t)(u + ph));
        const uint8_t h2 = mz_sin8((uint8_t)(u + ph + 85U));
        level = mz_avg(h1, h2);
        mix = (h1 > h2) ? 220U : 35U;
        break;
      }
      case 6U: { uint8_t cell=(uint8_t)((u/3U+ph/2U)&31U); level=(cell<13U)?uint8_t(255U-cell*18U):0U; mix=(uint8_t)((u/32U&1U)?210U:40U); break; }
      case 7U: { uint8_t arc=(uint8_t)(p+((uint16_t)(p*(255U-p))>>7)+ph); level=mz_peak(mz_dist(arc,128U),42U); mix=arc; break; }
      case 8U: { uint8_t gate=mz_min(a,b); level=mz_mul(gate,c); mix=mz_max(a,b); break; }
      case 9U: { uint8_t drift=mz_sin8((uint8_t)(p+ph/2U+mz_sin8((uint8_t)(p*3U+slow))/3U)); level=drift; mix=mz_sin8((uint8_t)(p*2U-ph)); break; }
      case 10U: { level=mz_avg(mz_sin8((uint8_t)(u+ph)),mz_sin8((uint8_t)(u*2U-ph*2U+71U))); mix=b; break; }
      case 11U: { level=mz_mul(mz_sin8((uint8_t)(u+ph)),mz_sin8((uint8_t)(u+128U))); mix=level; break; }
      case 12U: {
        const uint8_t slowWave = mz_sin8((uint8_t)(u + slow));
        const uint8_t fastWave = mz_sin8((uint8_t)(u + ph));
        level = mz_peak(mz_absdiff(slowWave, fastWave), 110U);
        mix = slowWave;
        break;
      }
      case 13U: { uint8_t lattice=mz_sin8((uint8_t)(u*2U+ph/2U)); level=mz_mul(lattice,mz_sin8((uint8_t)(u*3U-ph))); mix=lattice; break; }
      case 14U: { uint8_t h=(uint8_t)((a/3U+b/3U+c/4U+e/4U)); level=h; mix=(uint8_t)(mz_sin8((uint8_t)(u*5U+ph))); break; }
      case 15U: { uint8_t d=mz_dist(p,ph), pulse=mz_peak(d,38U); level=mz_max(pulse,mz_peak(mz_dist(p,uint8_t(ph+96U)),24U)); mix=(d<38U)?230U:70U; break; }
      case 16U: { uint8_t knot=mz_absdiff(a,c); level=mz_peak(knot,90U); mix=(uint8_t)(a^b); break; }
      case 17U: { uint8_t pend=mz_sin8((uint8_t)(ph/2U)); uint8_t d=mz_dist(p,pend); level=mz_peak(d,42U); mix=mz_sin8((uint8_t)(pend+64U)); break; }
      case 18U: { uint8_t tide=mz_sin8((uint8_t)(ph/3U)); level=mz_mul(mz_sin8((uint8_t)(u+tide)),mz_sin8((uint8_t)(u/2U-ph/4U))); mix=tide; break; }
      case 19U: { uint8_t polar=mz_max(mz_peak(p,36U),mz_peak(uint8_t(255U-p),36U)); level=mz_mul(polar,edge); level=mz_mul(level,mz_sin8((uint8_t)(ph+u*2U))); mix=uint8_t(ph+u); break; }
      case 22U: { uint8_t cascade=mz_peak(uint8_t((u+ph)&31U),uint8_t(4U+(intensity>>5)+(detail>>2U))); level=mz_max(cascade,mz_peak(uint8_t((u+ph+11U)&63U),10U)); mix=(u&32U)?230U:70U; break; }
      case 23U: { uint8_t vein=mz_sin8((uint8_t)(u+mz_sin8((uint8_t)(u/2U+ph)))); level=mz_peak(vein,70U); mix=uint8_t(100U+vein/2U); break; }
      case 24U: { uint8_t ash=mz_hash(x,uint8_t(slow/2U),id); uint8_t fall=uint8_t(p+ph/2U); level=(ash>210U)?uint8_t(ash):mz_peak(uint8_t(fall&63U),12U); mix=ash; break; }
      case 25U: { uint8_t plasma=mz_avg(mz_sin8((uint8_t)(u+ph)),mz_sin8((uint8_t)(u*3U-ph))); level=mz_max(plasma,mz_peak(mz_absdiff(a,b),70U)); mix=c; break; }
      case 26U: { uint8_t lava=mz_peak(mz_dist(p,uint8_t(ph/2U)),52U); level=mz_max(lava,mz_peak(mz_dist(p,uint8_t(255U-ph/3U)),29U)); mix=(uint8_t)(a/2U+128U); break; }
      case 27U: { uint8_t star=mz_hash(x,uint8_t(slow+ph/5U),id); level=(star>230U)?255U:((star>205U)?120U:8U); mix=(star&1U)?250U:45U; break; }
      case 28U: { uint8_t m1=mz_peak(mz_dist(p,ph),20U),m2=mz_peak(mz_dist(p,uint8_t(ph+53U)),15U),m3=mz_peak(mz_dist(p,uint8_t(ph+139U)),23U); level=mz_max(m1,mz_max(m2,m3)); mix=(m2>m1)?180U:245U; break; }
      case 29U: { uint8_t aur=mz_sin8((uint8_t)(u+ph)); uint8_t spark=mz_hash(x,slow,id); level=mz_mul(aur,uint8_t(100U+(spark>>1U))); mix=spark; break; }
      case 30U: { uint8_t prism=uint8_t((u/2U+ph)&255U); level=mz_sin8(prism); mix=prism; break; }
      case 31U: { uint8_t bits=uint8_t((x^(uint16_t)(ph/3U))&0x55U); level=(bits)?220U:12U; mix=(bits&4U)?235U:32U; break; }
      case 32U: { uint8_t morse=uint8_t((u/8U+ph/12U)%16U); level=((morse==1U||morse==4U||morse==7U||morse==10U)?255U:((morse==2U||morse==5U)?90U:0U)); mix=(morse&1U)?220U:40U; break; }
      case 33U: { uint8_t bar=uint8_t((u*3U+ph)&31U); level=(bar<5U||bar>27U)?255U:25U; mix=(u&32U)?210U:30U; break; }
      case 34U: { uint8_t chev=mz_sin8((uint8_t)(u+ph+(p<128U?p:uint8_t(255U-p)))); level=mz_peak(mz_absdiff(chev,128U),80U); mix=chev; break; }
      case 35U: { uint8_t tile=uint8_t(((x/3U)^(ph/16U))&7U); level=uint8_t((tile*25U)+ramp/4U); mix=uint8_t(tile*32U); break; }
      case 36U: { uint8_t fold=mz_min(p,uint8_t(255U-p)); uint8_t fract=mz_sin8(uint8_t(fold*7U+ph)); level=mz_mul(fract,triWave); mix=fract; break; }
      case 37U: { uint8_t fib=uint8_t((x*13U+x/2U+ph)&255U); level=mz_peak(mz_dist(fib,128U),46U); mix=fib; break; }
      case 38U: { uint8_t para=mz_sin8(uint8_t(u+ph/2U)); uint8_t para2=mz_sin8(uint8_t(u/2U-ph)); level=mz_absdiff(para,para2); mix=para; break; }
      case 39U: { uint8_t step=uint8_t(((u/16U)+(ph/16U))&15U); level=uint8_t((step<8U)?step*32U:(15U-step)*32U); mix=uint8_t(step*17U); break; }
      case 40U: { uint8_t neb=mz_sin8(uint8_t(u+mz_sin8(uint8_t(u*2U+slow))/2U+ph)); level=mz_mul(neb,hash); mix=neb; break; }
      case 41U: { uint8_t veil=mz_sin8(uint8_t(u+ph/3U)); level=mz_avg(veil,mz_sin8(uint8_t(u/2U-ph/5U))); mix=mz_sin8(uint8_t(ph+u/3U)); break; }
      case 42U: { uint8_t silk=mz_sin8(uint8_t(u+mz_sin8(uint8_t(ph+u/4U)))); level=mz_mul(silk,mz_sin8(uint8_t(u*2U-ph/2U))); mix=silk; break; }
      case 43U: { uint8_t chrome=mz_sin8(uint8_t(u*4U+ph)); uint8_t glint=mz_peak(mz_absdiff(chrome,240U),36U); level=mz_max(chrome/3U,glint); mix=glint; break; }
      case 44U: { uint8_t vapor=mz_sin8(uint8_t(u+ph)); uint8_t tail=mz_sin8(uint8_t(u/3U-ph/2U)); level=mz_mul(vapor,tail); mix=tail; break; }
      case 45U: { uint8_t cloud=mz_avg(mz_sin8(uint8_t(u+ph/2U)),mz_sin8(uint8_t(u/3U+ph))); level=mz_max(cloud,mz_peak(hash,60U)); mix=cloud; break; }
      case 46U: { uint8_t ocean=uint8_t(80U+((uint16_t)mz_sin8(uint8_t(u+ph/3U))*175U)/255U); level=mz_mul(ocean,mz_sin8(uint8_t(u*2U-ph/4U))); mix=uint8_t(130U+mz_sin8(uint8_t(ph+u))/2U); break; }
      case 47U: { uint8_t rain=mz_hash(x,uint8_t(slow+x/4U),id); uint8_t streak=uint8_t(u+ph/2U+e/6U); level=(rain>210U)?255U:mz_peak(uint8_t(streak&31U),7U); mix=rain; break; }
      case 48U: { uint8_t haze=mz_sin8(uint8_t(u/2U+ph/4U)); level=uint8_t(30U+(uint16_t)haze*(3U+(detail>>3U))/4U); mix=mz_sin8(uint8_t(u+ph)); break; }
      case 49U: { uint8_t opal=mz_sin8(uint8_t(u+ph)); uint8_t sheen=mz_cos8(uint8_t(u*3U-ph/2U)); level=mz_avg(opal,sheen); mix=mz_avg(opal,hash); break; }
      case 50U: { uint8_t beat=mz_sin8(uint8_t(ph*2U)); level=(beat>210U)?255U:((beat>165U)?uint8_t(beat):25U); mix=(beat>210U)?245U:70U; break; }
      case 51U: { uint8_t idle=mz_sin8(uint8_t(ph/5U+mz_sin8(uint8_t(ph/2U)))); level=uint8_t(100U+idle/2U); mix=idle; break; }
      case 52U: { uint8_t tach=mz_sin8(uint8_t(u*2U+ph)); level=(u<ph)?255U:((u<uint8_t(ph+32U))?150U:22U); mix=tach; break; }
      case 53U: { uint8_t rev=uint8_t((u+ph*2U)&255U); level=mz_peak(uint8_t(rev&63U),width); mix=(rev>192U)?245U:80U; break; }
      case 54U: { uint8_t gear=uint8_t((ph/20U)%5U); uint8_t zone=mz_peak(mz_absdiff(u,uint8_t(gear*51U)),25U); level=mz_max(zone,uint8_t((gear==4U)?a/2U:15U)); mix=uint8_t(gear*50U); break; }
      case 55U: { uint8_t ladder=uint8_t((u+ph)&255U); level=(ladder<32U)?255U:((ladder<64U)?uint8_t(160U):0U); mix=ladder; break; }
      case 56U: { uint8_t count=uint8_t((ph/32U)%4U); uint8_t d=mz_dist(u,uint8_t(count*64U)); level=mz_peak(d,18U); mix=(count&1U)?210U:60U; break; }
      case 57U: { uint8_t burn=mz_sin8(uint8_t(u*3U+ph)); level=mz_mul(burn,uint8_t(120U+mz_sin8(uint8_t(ph+u/2U))/2U)); mix=burn; break; }
      case 58U: { uint8_t spiral=mz_sin8(uint8_t(u*2U+ph+mz_sin8(uint8_t(u*4U-ph))/3U)); level=spiral; mix=mz_sin8(uint8_t(u*3U-ph)); break; }
      case 59U: { uint8_t pit=uint8_t((u+ph)&127U); level=(pit<8U)?255U:((pit<20U)?90U:14U); mix=(pit<8U)?240U:30U; break; }
      case 60U: { uint8_t bubbles=mz_hash(x,uint8_t(slow+x/9U),id); level=(bubbles>224U)?255U:uint8_t(30U+bubbles/5U); mix=uint8_t(170U+bubbles/3U); break; }
      case 61U: { uint8_t velvet=mz_sin8(uint8_t(u+ph/7U)); level=uint8_t(35U+velvet/3U+edge/5U); mix=uint8_t(80U+velvet/2U); break; }
      case 62U: { uint8_t sun=mz_sin8(uint8_t(ph/3U)); level=mz_mul(uint8_t(120U+sun/2U),mz_sin8(uint8_t(u+ph/8U))); mix=uint8_t(150U+sun/3U); break; }
      case 63U: { uint8_t pearl=mz_peak(mz_dist(p,uint8_t(ph+mz_sin8(uint8_t(ph/4U))/3U)),32U); level=mz_max(55U,pearl); mix=pearl; break; }
      case 64U: { uint8_t satin=mz_sin8(uint8_t(u+mz_sin8(uint8_t(ph+u/5U))/2U)); level=uint8_t(75U+((uint16_t)satin*180U)/255U); mix=mz_sin8(uint8_t(u*2U+ph)); break; }
      case 65U: { uint8_t opal=mz_sin8(uint8_t(u+ph)); uint8_t shimmer=mz_hash(x,uint8_t(slow/3U),id); level=mz_max(opal,uint8_t(shimmer>220U?255U:0U)); mix=shimmer; break; }
      case 66U: { uint8_t chrome=mz_sin8(uint8_t(u*5U+ph/2U)); level=mz_peak(mz_absdiff(chrome,128U),48U); mix=uint8_t(128U+chrome/2U); break; }
      case 67U: { uint8_t flame=mz_sin8(uint8_t(u+ph)); uint8_t candle=mz_hash(x,uint8_t(slow+u),id); level=mz_mul(flame,uint8_t(170U+candle/3U)); mix=candle; break; }
      case 68U: { uint8_t halo=mz_peak(mz_dist(p,128U),128U); uint8_t breathe=mz_sin8(uint8_t(ph/3U)); level=mz_mul(halo,breathe); mix=breathe; break; }
      case 69U: { uint8_t crown=mz_peak(mz_absdiff(p,128U),96U); level=mz_mul(crown,mz_sin8(uint8_t(ph+u))); mix=uint8_t(200U-crown/2U); break; }
      case 70U: { uint8_t d=mz_absdiff(p,128U); uint8_t wave=mz_sin8(uint8_t(ph+u)); level=mz_peak(d, uint8_t(20U+wave/3U)); mix=wave; break; }
      case 71U: { uint8_t d=mz_absdiff(p,128U); uint8_t open=mz_sin8(uint8_t(ph/2U)); level=mz_peak(mz_absdiff(d,open/2U),36U); mix=open; break; }
      case 72U: { uint8_t mzCenter72=mz_peak(mz_absdiff(p,128U),128U); level=mz_mul(mzCenter72,mz_sin8(uint8_t(ph/2U))); mix=mzCenter72; break; }
      case 73U: { uint8_t mzEdge73=mz_peak(mz_min(p,uint8_t(255U-p)),128U); level=mz_mul(mzEdge73,mz_sin8(uint8_t(ph/2U))); mix=mzEdge73; break; }
      case 74U: { uint8_t l=mz_peak(mz_dist(p,ph),20U),r=mz_peak(mz_dist(p,uint8_t(255U-ph)),20U); level=mz_max(l,r); mix=(l>r)?40U:220U; break; }
      case 75U: { uint8_t w=ramp; level=(w>70U&&w<184U)?255U:18U; mix=w; break; }
      case 76U: { uint8_t aperture=mz_sin8(uint8_t(ph/2U)); uint8_t apertureDist76=mz_absdiff(p,aperture); level=mz_peak(apertureDist76,28U); mix=aperture; break; }
      case 77U: { uint8_t gate=uint8_t((u+ph)&63U); level=(gate<5U)?255U:((gate<14U)?100U:0U); mix=uint8_t(gate*4U); break; }
      case 78U: { uint8_t beacon=mz_dist(p,ph); level=mz_peak(beacon, uint8_t(12U+(intensity>>3U)+(detail>>1U))); mix=(beacon<10U)?250U:110U; break; }
      case 79U: { uint8_t x1=mz_peak(mz_dist(p,ph),16U),x2=mz_peak(mz_dist(p,uint8_t(ph+85U)),12U),x3=mz_peak(mz_dist(p,uint8_t(ph+170U)),16U); level=mz_max(x1,mz_max(x2,x3)); mix=mz_avg(x1,x3); break; }
      case 80U: { uint8_t q=mz_hash(x,uint8_t(slow+ph),id); level=(q>180U)?uint8_t(q):uint8_t(q/6U); mix=(q&32U)?225U:25U; break; }
      case 81U: { uint8_t phaseA=mz_sin8(uint8_t(u+ph)); uint8_t phaseB=mz_sin8(uint8_t(u-ph/2U)); level=mz_absdiff(phaseA,phaseB); mix=phaseB; break; }
      case 82U: { uint8_t noise1=mz_hash(x,uint8_t(slow),id),noise2=mz_hash(uint16_t(x+17U),uint8_t(slow+23U),id+9U); level=mz_mul(noise1,noise2); mix=noise2; break; }
      case 84U: { uint8_t bit=uint8_t((x*5U+ph/7U)^(x>>1U)); level=(bit&8U)?220U:20U; mix=uint8_t(bit*13U); break; }
      case 87U: { uint8_t logistic=mz_sin8(uint8_t(u*3U+ph)); level=mz_mul(logistic,uint8_t(255U-logistic/2U)); mix=logistic; break; }
      case 88U: { const uint8_t mz88Left=mz_sin8(uint8_t(u+ph)); const uint8_t mz88Right=mz_sin8(uint8_t(u*2U-ph/2U)); level=mz_avg(mz_mul(mz88Left,mz88Right),mz_absdiff(mz88Left,mz88Right)); level=mz_avg(level,e); mix=mz88Right; break; }
      case 89U: { uint8_t cell=uint8_t((u+ph)&31U); level=(cell<16U)?uint8_t(cell*16U):uint8_t((31U-cell)*16U); mix=hash; break; }
      case 90U: { uint8_t bolt=mz_hash(x,uint8_t(slow+x/3U),id); level=(bolt>242U)?255U:((bolt>226U)?190U:0U); mix=(bolt>230U)?245U:80U; break; }
      case 91U: { uint8_t flare=mz_peak(mz_dist(p,ph),45U); level=mz_max(flare,uint8_t(mz_sin8(uint8_t(u+ph/3U))/3U)); mix=(flare>170U)?245U:130U; break; }
      case 92U: { uint8_t field=mz_sin8(uint8_t(u+ph)); uint8_t pole=mz_cos8(uint8_t(u*2U-ph)); level=mz_absdiff(field,pole); mix=field; break; }
      case 93U: { uint8_t rippleA=mz_peak(mz_dist(p,ph),22U); uint8_t rippleB=mz_peak(mz_dist(p,uint8_t(ph+64U)),16U); level=mz_max(rippleA,rippleB); mix=mz_avg(rippleA,rippleB); break; }
      case 94U: { uint8_t warped=uint8_t(u+mz_sin8(uint8_t(u*2U+ph))/2U); level=mz_sin8(uint8_t(warped+ph)); mix=warped; break; }
      case 95U: { uint8_t bands=uint8_t((u+ph)&127U); level=mz_peak(uint8_t(bands&31U),18U); mix=uint8_t((bands/16U)*36U); break; }
      case 96U: { uint8_t ion=mz_sin8(uint8_t(u*4U+ph)); uint8_t packet=mz_hash(x,uint8_t(slow+ph/7U),id); level=mz_mul(ion,packet); mix=packet; break; }
      case 97U: { uint8_t gravity=mz_peak(mz_absdiff(p,uint8_t(128U+mz_sin8(uint8_t(ph/3U))/4U)),120U); level=mz_mul(gravity,mz_sin8(uint8_t(ph+u/2U))); mix=gravity; break; }
      case 98U: { uint8_t horizon=mz_sin8(uint8_t(u/2U+ph)); uint8_t line=mz_peak(mz_dist(p,ph),14U); level=mz_max(uint8_t(horizon/2U),line); mix=line; break; }
      case 99U: {
        // Gunakan deklarasi terpisah dan nama unik agar tidak berbenturan
        // dengan macro/simbol sigB atau sigC dari header lain.
        const uint8_t mzSigA99 = mz_sin8(uint8_t(u + ph));
        const uint8_t mzSigB99 = mz_sin8(uint8_t(u * 3U - ph));
        const uint8_t mzSigC99 = mz_hash(x, slow, id);
        level = mz_max(mz_mul(mzSigA99, mzSigB99),
                       uint8_t(mzSigC99 > 238U ? 255U : 0U));
        mix = mz_avg(mzSigA99, mzSigC99);
        break;
      }
      default: level = a; mix = b; break;
    }

    // Intensity scales the computed field without changing the algorithm shape.
    level = scale8(level, (uint8_t)(96U + ((uint16_t)intensity * 159U) / 255U));
    uint32_t color = color_blend(c0, c1, mix);
    if (mix > 208U) color = color_blend(color, c2, (uint8_t)((mix - 208U) * 5U));
    SEGMENT.setPixelColor(x, color_fade(color, level));
  }
}

// A single WLED effect entry exposes all 100 algorithms via c1 (0..99).
static void mode_mizuma_signature_100(void) {
  const uint8_t index = clampIndex(SEGMENT.custom1);
  renderAlgorithm(index);
}

static const char MZ_SIGNATURE_100_METADATA[] PROGMEM =
  "MZ | Signature 100@!,Intensity,Algorithm ID,Detail;!,!;;;ix=180,c1=0,c2=160";

static inline uint8_t registerWLEDEffect(void) {
  // Call once from MizumaSmartSystem::setup().
  return strip.addEffect(255, &mode_mizuma_signature_100, MZ_SIGNATURE_100_METADATA);
}

} // namespace MizumaSignature100
