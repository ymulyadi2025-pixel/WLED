#!/usr/bin/env python3
# =====================================================================================================
# MIZUMA WELCOMING SHOWS — definisi 55 animasi (tahap = efek asli WLED yang dirangkai berurutan)
# Jalankan:  python3 gen_mizuma_shows.py [reg_default.json]
#   -> menulis  mizuma_shows.h  (tabel C++)  dan  katalog_55_animasi.md
# Jika reg_default.json (daftar efek terdaftar di WLED Anda) tersedia, semua nama efek divalidasi.
# =====================================================================================================
import sys, json, os, re

PAL = dict(DEFAULT=0, PARTY=6, CLOUD=7, LAVA=8, OCEAN=9, FOREST=10, RAINBOW=11, BANDS=12, SUNSET=13,
           RIVENDELL=14, BREEZE=15, REDBLUE=16, ANALOG=18, SPLASH=19, PASTEL=20, SUNSET2=21, BEACH=22,
           DEPARTURE=24, SHERBET=27, HULT=28, FIRE=35, ICEFIRE=36, CYANE=37, LIGHTPINK=38, AUTUMN=39,
           MAGRED=41, YELMAG=42, YELBLU=43, ORANGETEAL=44, TIAMAT=45, APRILNIGHT=46, ORANGERY=47,
           SAKURA=49, AURORA=50, ATLANTICA=51, C9NEW=53, TEMPERATURE=54, AURORA2=55, CANDY=57,
           PINKCANDY=61, REDSHIFT=68, REDTIDE=69, FIREWORKS=72)

# ---- kode warna (dipetakan ke uint32 di C++) ----
# 'A1','A2','A3' = warna aksen pengguna (slot 1/2/3 pada UI), 'A1d' = aksen redup. Selain itu hex '#RRGGBB'.
NAMED = dict(BLK='#000000', WHT='#FFFFFF', GLD='#FFB000', RED='#FF1A00', CYN='#00D8FF', ICE='#9FE8FF',
             PNK='#FF4FA0', VIO='#7A2CFF', GRN='#18FF70', BLU='#1E50FF', ORG='#FF5A00', LIM='#B8FF00')

KEEP = 0xFF000000
def col_code(c):
    if c is None: return KEEP
    m = re.fullmatch(r'A([123])(d?)', c)
    if m: return 0xFE000000 | (int(m.group(1)) - 1) | (0x100 if m.group(2) else 0)
    if c in NAMED: c = NAMED[c]
    m = re.fullmatch(r'#([0-9A-Fa-f]{6})', c)
    if not m: raise ValueError('warna tidak dikenal: %r' % c)
    return int(m.group(1), 16)

FLIP, TBJ = 1, 2     # flags: FLIP = balik arah (rev), TBJ = lompatkan timebase (lewati tunggu 2 dtk Tetrix)

class St:
    pass

def S(t, fx, sx=None, ix=None, c1=None, c2=None, c3=None, o=None, pal=None, col=(None, None, None),
      side=0, flip=0, tt=0, tb='auto', tbj=False):
    s = St(); s.t = t; s.fx = fx; s.sx = sx; s.ix = ix; s.c1 = c1; s.c2 = c2; s.c3 = c3; s.o = o
    s.pal = pal if not isinstance(pal, str) else PAL[pal]; s.col = tuple(col); s.side = side
    s.flip = flip; s.tt = tt; s.tb = tb; s.tbj = tbj
    return s

class Show:
    def __init__(s, name, cat, desc, stages):
        s.name = name; s.cat = cat; s.desc = desc; s.stages = sorted(stages, key=lambda x: (x.t, x.side))
        s.duration_ms = 0

# ---------- resep per efek (parameter mengikuti metadata fxdata WLED) ----------
def solid(t, c='A1', side=0, **k):      return S(t, 'Solid', pal=0, col=(c, None, None), side=side, **k)
def off(t, side=0, **k):                return S(t, 'Solid', pal=0, col=('BLK', None, None), side=side, **k)
def strobe(t, sx=248, c=('A2', 'BLK'), **k):  return S(t, 'Strobe', sx=sx, pal=0, col=(c[0], c[1], None), **k)
def smega(t, sx=238, ix=30, c=('A2', 'BLK'), **k): return S(t, 'Strobe Mega', sx=sx, ix=ix, pal=0, col=(c[0], c[1], None), **k)
def blink(t, sx=240, ix=100, c=('A1', 'BLK'), **k): return S(t, 'Blink', sx=sx, ix=ix, pal=0, col=(c[0], c[1], None), **k)
def blinkr(t, sx=240, ix=100, **k):     return S(t, 'Blink Rainbow', sx=sx, ix=ix, col=(None, 'BLK', None), **k)
def strober(t, sx=246, **k):            return S(t, 'Strobe Rainbow', sx=sx, col=(None, 'BLK', None), **k)
def lightn(t, sx=190, ix=170, c=('WHT', 'BLK'), **k): return S(t, 'Lightning', sx=sx, ix=ix, col=(c[0], c[1], None), **k)
def chflash(t, sx=245, c=('A1', 'WHT'), **k): return S(t, 'Chase Flash', sx=sx, pal=0, col=(c[0], c[1], None), **k)
def chflashr(t, sx=245, **k):           return S(t, 'Chase Flash Rnd', sx=sx, col=(None, None, None), **k)
def dissolve(t, sx=230, ix=225, c=('A1', 'BLK'), o=None, **k): return S(t, 'Dissolve', sx=sx, ix=ix, o=o, pal=0, col=(c[0], c[1], None), **k)
def glitter(t, sx=200, ix=190, c=('A1', 'BLK', 'WHT'), **k): return S(t, 'Glitter', sx=sx, ix=ix, pal=0, col=c, **k)
def sparkle(t, sx=240, c=('A1', 'BLK'), **k): return S(t, 'Sparkle', sx=sx, pal=0, col=(c[0], c[1], None), **k)
def twinkle(t, sx=200, ix=190, c=('A2', 'A1d'), **k): return S(t, 'Twinkle', sx=sx, ix=ix, pal=0, col=(c[0], c[1], None), **k)
def ctwink(t, pal='RAINBOW', sx=170, ix=170, **k): return S(t, 'Colortwinkles', sx=sx, ix=ix, pal=pal, **k)
def tfox(t, pal='SAKURA', sx=100, ix=120, **k): return S(t, 'Twinklefox', sx=sx, ix=ix, pal=pal, **k)
def popcorn(t, sx=170, pal='FIRE', **k): return S(t, 'Popcorn', sx=sx, pal=pal, **k)
def scan(t, sx=250, ix=60, c=('A1', 'BLK'), **k): return S(t, 'Scan', sx=sx, ix=ix, pal=0, col=(c[0], c[1], None), **k)
def scand(t, sx=250, ix=70, c=('A1', 'BLK'), **k): return S(t, 'Scan Dual', sx=sx, ix=ix, pal=0, col=(c[0], c[1], None), **k)
def chase2(t, sx=244, ix=90, c=('A1', 'A2'), **k): return S(t, 'Chase 2', sx=sx, ix=ix, pal=0, col=(c[0], c[1], None), **k)
def chase3(t, sx=244, ix=90, c=('A1', 'A2', 'A3'), **k): return S(t, 'Chase 3', sx=sx, ix=ix, pal=0, col=c, **k)
def chaser(t, sx=240, ix=100, pal='RAINBOW', **k): return S(t, 'Chase Rainbow', sx=sx, ix=ix, pal=pal, **k)
def rundual(t, sx=225, ix=90, c=('A1', 'BLK', 'A2'), **k): return S(t, 'Running Dual', sx=sx, ix=ix, pal=0, col=c, **k)
def twodots(t, sx=225, ix=100, c=('A1', 'A2', 'BLK'), **k): return S(t, 'Two Dots', sx=sx, ix=ix, pal=0, col=c, **k)
def mcomet(t, sx=215, **k):             return S(t, 'Multi Comet', sx=sx, pal=0, col=('A1', 'BLK', None), **k)
def sinedual(t, sx=190, ix=90, c=('A1', 'A2', 'A3'), **k): return S(t, 'Sinelon Dual', sx=sx, ix=ix, pal=0, col=c, **k)
def sinerb(t, sx=190, ix=110, pal='RAINBOW', **k): return S(t, 'Sinelon Rainbow', sx=sx, ix=ix, pal=pal, **k)
def meteor(t, sx=190, ix=140, pal=0, c=('A1', 'BLK'), **k): return S(t, 'Meteor', sx=sx, ix=ix, pal=pal, col=(c[0], c[1], None), **k)
def fw1d(t, pal=None, **k):             return S(t, 'Fireworks 1D', pal=pal, **k)
def fwork(t, pal='FIREWORKS', ix=190, **k): return S(t, 'Fireworks', ix=ix, pal=pal, **k)
def starburst(t, pal=None, **k):        return S(t, 'PS Starburst', pal=pal, **k)
def spray1d(t, pal=None, **k):          return S(t, 'PS Spray 1D', pal=pal, **k)
def balance(t, pal=None, **k):          return S(t, 'PS 1D Balance', pal=pal, **k)
def bballs(t, pal='PARTY', **k):        return S(t, 'Bouncing Balls', pal=pal, **k)
def drip(t, pal='OCEAN', c=('BLK', 'BLK'), **k): return S(t, 'Drip', pal=pal, col=(c[0], c[1], None), **k)
def pacman(t, **k):                     return S(t, 'PacMan', **k)
def wavesins(t, pal='OCEAN', sx=90, **k): return S(t, 'Wavesins', sx=sx, pal=pal, **k)
def shimmer(t, pal='SAKURA', c=('A1', 'BLK', 'A2'), **k): return S(t, 'Shimmer', pal=pal, col=c, **k)
def sunrise(t, sx=174, pal='FIRE', **k): return S(t, 'Sunrise', sx=sx, pal=pal, **k)   # sx>120: naik-turun cepat; tb=0 menyelaraskan fasa
def fade(t, sx=200, c=('A1', 'A1d'), **k): return S(t, 'Fade', sx=sx, pal=0, col=(c[0], c[1], None), **k)
def breathe(t, sx=150, c=('A1', 'BLK'), **k): return S(t, 'Breathe', sx=sx, pal=0, col=(c[0], c[1], None), **k)
def heartb(t, sx=190, ix=170, c=('A3', 'BLK'), **k): return S(t, 'Heartbeat', sx=sx, ix=ix, pal=0, col=(c[0], c[1], None), **k)
def candle(t, c=('A1', 'BLK'), **k):    return S(t, 'Candle', pal=0, col=(c[0], c[1], None), **k)
def fire12(t, **k):                     return S(t, 'Fire 2012', **k)
def aurora(t, pal='AURORA', **k):       return S(t, 'Aurora', pal=pal, **k)
def pacifica(t, pal='ATLANTICA', **k):  return S(t, 'Pacifica', pal=pal, **k)
def cwaves(t, pal='SUNSET', **k):       return S(t, 'Colorwaves', pal=pal, **k)
def lake(t, pal='OCEAN', c=('A1', None), **k): return S(t, 'Lake', pal=pal, col=(c[0], c[1], None), **k)
def ripple(t, pal='OCEAN', sx=150, ix=130, c=(None, 'BLK'), **k): return S(t, 'Ripple', sx=sx, ix=ix, pal=pal, col=(c[0], c[1], None), **k)
def rippler(t, sx=150, ix=120, **k):    return S(t, 'Ripple Rainbow', sx=sx, ix=ix, **k)
def rainbow(t, sx=120, ix=60, **k):     return S(t, 'Rainbow', sx=sx, ix=ix, **k)

def wipe(t, sx=252, c=('A1', 'BLK'), half=False, flip=0, **k):
    cyc = 750 + (255 - sx) * 150
    return S(t, 'Wipe', sx=sx, ix=128, pal=0, col=(c[0], c[1], None), flip=flip, tb=(cyc // 2 if half else 'auto'), **k)
def sweep(t, sx=252, c=('A1', 'BLK'), half=False, flip=0, **k):
    cyc = 750 + (255 - sx) * 150
    return S(t, 'Sweep', sx=sx, ix=128, pal=0, col=(c[0], c[1], None), flip=flip, tb=(cyc // 2 if half else 'auto'), **k)
def tetrix(t, sx, ix=128, pal='FIRE', one=False, c=('A1', 'BLK'), flip=0, **k):
    return S(t, 'Tetrix', sx=sx, ix=ix, pal=pal, o=(1 if one else 0), col=(c[0], c[1], None), flip=flip, **k)

def alt(t0, n, step, c=('A1', 'A2')):
    """kiri-kanan bergantian: n kali, tiap 'step' ms (Solid menyala di satu sisi, sisi lain mati)."""
    out = []
    for i in range(n):
        a = 1 + (i % 2); b = 3 - a
        out += [solid(t0 + i * step, c[i % len(c)], side=a), off(t0 + i * step, side=b)]
    return out

def glow(t, **k): return fade(t, **k)   # penutup standar: menyala penuh, berdenyut halus (identitas DRL)

SHOWS = []
def add(name, cat, desc, *stages):
    flat = []
    for s in stages:
        flat += s if isinstance(s, list) else [s]
    SHOWS.append(Show(name, cat, desc, flat))

# =====================================================================================================
# A. SIGNATURE TETRIX (MZ01-MZ05) — permintaan khusus
# =====================================================================================================
add('Mizuma Ignition', 'Signature Tetrix',
    'Tetrix lambat ke cepat (kanan ke kiri), Wipe mati kiri ke kanan, Sweep kanan ke kiri, Fireworks 1D, ditutup Fade menyala penuh.',
    tetrix(0, 160, ix=128, tbj=True), tetrix(900, 200), tetrix(1700, 235), tetrix(2300, 255),
    wipe(3800, sx=252, c=('A1', 'BLK'), half=True),            # langsung masuk setengah siklus "mati" (kiri ke kanan)
    sweep(4450, sx=252, c=('A1', 'BLK'), flip=1),             # isi kanan ke kiri
    fw1d(5100), glow(6450))
add('Mizuma Afterburn', 'Signature Tetrix',
    'Alur Ignition dibalik: Fade menyala, Fireworks 1D, Sweep, Wipe mati (arah dibalik), lalu Tetrix cepat ke lambat.',
    glow(0, sx=215), fw1d(750),
    sweep(2050, sx=252, c=('A1', 'BLK')),                      # isi kiri ke kanan
    wipe(2700, sx=252, c=('A1', 'BLK'), half=True, flip=1),    # mati kanan ke kiri
    tetrix(3350, 255, ix=128, flip=1, tbj=True), tetrix(4150, 240, flip=1), tetrix(4950, 215, flip=1), tetrix(5800, 185, flip=1))
add('Golden Dawn', 'Signature Tetrix',
    'Sunrise fajar emas, Fireworks 1D, Twinkle, ditutup Shimmer.',
    sunrise(0, sx=174), fw1d(2350, pal='FIRE'),
    twinkle(4150, sx=190, ix=200, c=('A2', 'A1d')), shimmer(5500, pal='FIRE', c=('A1', 'BLK', 'A2')))
add('Prism Finale', 'Signature Tetrix',
    'Shimmer, Sinelon Rainbow, Chase Rainbow, Ripple Rainbow lalu penutup Fade amber.',
    shimmer(0, pal='RAINBOW', c=('A1', 'BLK', 'A2')), sinerb(1700, sx=200, ix=110),
    chaser(3400, sx=238), rippler(4700, sx=160), glow(5900, sx=190))
add('Stack Royale', 'Signature Tetrix',
    'Tetrix campuran: pelangi cepat, ledakan PS Starburst, Tetrix satu warna arah balik, Colortwinkles, Fade.',
    tetrix(0, 250, ix=128, pal='RAINBOW', tbj=True),
    starburst(1950, pal='FIREWORKS'),
    tetrix(2900, 250, ix=160, pal='DEFAULT', one=True, c=('A1', 'BLK'), flip=1, tbj=True),
    ctwink(4900, pal='PARTY'), glow(6050, sx=210))

# =====================================================================================================
# B. KEDIPAN CEPAT (MZ06-MZ25) — 20 animasi
# =====================================================================================================
add('Raijin Strobe', 'Kedipan Cepat', 'Dewa petir: burst Strobe Mega putih, Strobe amber, Lightning, ditutup menyala.',
    smega(0, 236, 30), strobe(1700, 249, ('A1', 'BLK')), lightn(3400, 185, 180), smega(5100, 240, 20, ('A1', 'BLK')), glow(6200))
add('Katana Slash', 'Kedipan Cepat', 'Tebasan beruntun Wipe/Sweep bergantian arah dan warna.',
    wipe(0, 254, ('A2', 'BLK')), sweep(500, 254, ('A1', 'BLK'), flip=1), wipe(1000, 254, ('A3', 'BLK')),
    sweep(1500, 254, ('A2', 'BLK'), flip=1), wipe(2000, 254, ('A1', 'BLK')), chflash(2500, 246, ('A1', 'WHT')),
    strobe(4300, 250, ('WHT', 'BLK')), glow(5800))
add('Shinobi Bergantian', 'Kedipan Cepat', 'Kiri dan kanan menyala bergantian makin cepat seperti ninja.',
    alt(0, 4, 240), alt(960, 6, 170, ('A2', 'A1')), alt(1980, 8, 120), strobe(3000, 249, ('A1', 'BLK')),
    smega(4600, 238, 40, ('WHT', 'BLK')), glow(6000))
add('Oni Heartbeat', 'Kedipan Cepat', 'Detak jantung oni merah-oranye yang makin liar.',
    heartb(0, 190, 170, ('A3', 'BLK')), heartb(2000, 220, 190, ('A3', 'BLK')), blink(3400, 244, 70, ('A3', 'BLK')),
    strobe(4600, 250, ('A1', 'BLK')), glow(5900, c=('A3', 'A3d')))
add('Kitsune Dash', 'Kedipan Cepat', 'Rubah berlari: Chase 2 cepat, Scan Dual, Chase 3 lalu menyala.',
    chase2(0, 246, 80, ('A1', 'A2')), scand(1500, 252, 80, ('A1', 'BLK')), chase3(3000, 246, 70),
    chflash(4500, 247, ('A1', 'WHT')), glow(5900))
add('Tengu Burst', 'Kedipan Cepat', 'Dissolve cepat, Glitter, Strobe Mega: ledakan acak berkilau.',
    dissolve(0, 235, 235, ('A1', 'BLK')), glitter(1700, 220, 220), smega(3300, 238, 30, ('A2', 'A1d')),
    dissolve(4700, 245, 245, ('A2', 'BLK')), glow(5900))
add('Hayate Rush', 'Kedipan Cepat', 'Angin kencang: Chase Flash, Running Dual, Two Dots berkejaran.',
    chflash(0, 247, ('A1', 'WHT')), rundual(1500, 238, 100, ('A1', 'BLK', 'A2')), twodots(3000, 238, 110, ('A1', 'A2', 'BLK')),
    chflash(4500, 250, ('A2', 'A1')), glow(5800))
add('Ronin Spark', 'Kedipan Cepat', 'Percikan pedang ronin: Sparkle, Glitter, Twinkle rapat.',
    sparkle(0, 250, ('A2', 'BLK')), glitter(1500, 225, 225, ('A1', 'BLK', 'WHT')), twinkle(3000, 230, 230, ('A2', 'A1d')),
    sparkle(4400, 252, ('A1', 'BLK')), glow(5700))
add('Kabuto Pulse', 'Kedipan Cepat', 'Pola Blink berirama seperti genderang di helm kabuto.',
    blink(0, 236, 60, ('A1', 'BLK')), blink(1100, 242, 90, ('A2', 'BLK')), blink(2200, 247, 110, ('A3', 'BLK')),
    blinkr(3300, 246, 120), strobe(4700, 250, ('A1', 'BLK')), glow(5900))
add('Tsunami Flash', 'Kedipan Cepat', 'Riak cepat berwarna lalu kilat Strobe Rainbow.',
    ripple(0, pal='OCEAN', sx=210, ix=170), strober(1700, 247), ripple(3200, pal='RAINBOW', sx=220, ix=190),
    smega(4600, 238, 40, ('A2', 'BLK')), glow(5900, c=('A1', 'A1d')))
add('Yami Glitch', 'Kedipan Cepat', 'Efek glitch kegelapan: Glitter, Dissolve Rnd, Strobe patah-patah.',
    glitter(0, 230, 230, ('A3', 'BLK', 'WHT')), S(1400, 'Dissolve Rnd', sx=235, ix=235, col=(None, 'BLK', None)),
    strobe(2800, 249, ('A2', 'BLK')), glitter(3800, 245, 245, ('A1', 'BLK', 'WHT')), smega(5000, 236, 50, ('A3', 'BLK')), glow(6100))
add('Hanabi Rapid', 'Kedipan Cepat', 'Kembang api beruntun: Fireworks 1D, Popcorn, Sparkle lalu menyala.',
    fw1d(0), popcorn(1800, 190, 'FIRE'), sparkle(3300, 248, ('A2', 'BLK')), fw1d(4300, pal='RAINBOW'), glow(5900))
add('Taiko Beat', 'Kedipan Cepat', 'Dentuman taiko: kedip berirama dengan Bouncing Balls.',
    smega(0, 232, 20, ('A1', 'BLK')), smega(1000, 236, 20, ('A2', 'BLK')), bballs(2000, 'PARTY'),
    smega(3600, 240, 30, ('A3', 'BLK')), blink(4700, 246, 120, ('A1', 'BLK')), glow(5900))
add('Kaen Burst', 'Kedipan Cepat', 'Api membara: Chase Flash Rnd, Strobe oranye, Lightning hangat.',
    chflashr(0, 246), strobe(1500, 248, ('A3', 'BLK')), lightn(2800, 190, 190, ('A1', 'BLK')),
    chflash(4200, 248, ('A3', 'A2')), smega(5200, 238, 30, ('A1', 'BLK')), glow(6100, c=('A3', 'A1d')))
add('Shuriken Spin', 'Kedipan Cepat', 'Bintang lempar: Scan, Scan Dual, Two Dots dan Sinelon Dual cepat.',
    scan(0, 252, 90, ('A2', 'BLK')), scand(1300, 254, 90, ('A1', 'BLK')), twodots(2600, 245, 90, ('A1', 'A2', 'BLK')),
    sinedual(3900, 215, 70), strobe(5200, 250, ('A1', 'BLK')), glow(6100))
add('Nitro Flicker', 'Kedipan Cepat', 'Kelip nitro: Colortwinkles cepat, Twinkle, Strobe pendek.',
    ctwink(0, 'PARTY', 220, 220), twinkle(1500, 240, 240, ('A1', 'BLK')), strobe(2800, 250, ('WHT', 'BLK')),
    ctwink(3500, 'FIRE', 230, 230), smega(4800, 240, 30, ('A1', 'BLK')), glow(6000))
add('Dragon Roar', 'Kedipan Cepat', 'Raungan naga: ledakan PS Starburst berulang dan Strobe Mega.',
    starburst(0, pal='FIREWORKS'), smega(1500, 236, 40, ('A1', 'BLK')), starburst(2900, pal='FIRE'),
    smega(4400, 240, 30, ('A3', 'BLK')), glow(5600, c=('A3', 'A1d')))
add('Samurai Triple', 'Kedipan Cepat', 'Tiga kedipan tegas per kelompok dengan tiga warna aksen.',
    smega(0, 234, 20, ('A1', 'BLK')), smega(1000, 234, 20, ('A2', 'BLK')), smega(2000, 234, 20, ('A3', 'BLK')),
    smega(3000, 240, 20, ('A1', 'BLK')), smega(3900, 240, 20, ('A2', 'BLK')), smega(4800, 240, 20, ('A3', 'BLK')), glow(5800))
add('Thunder Chase', 'Kedipan Cepat', 'Kilat dan kejaran pelangi cepat.',
    lightn(0, 180, 190), chaser(1500, 244, 90, 'RAINBOW'), strobe(2900, 249, ('A2', 'BLK')), chaser(3700, 250, 100, 'PARTY'),
    lightn(5000, 200, 200, ('A1', 'BLK')), glow(6100))
add('Zenkai Flash', 'Kedipan Cepat', 'Puncak kedipan: gabungan Wipe, Strobe, Glitter, Chase Flash lalu ledakan terang.',
    wipe(0, 254, ('A1', 'BLK')), strobe(500, 250, ('A2', 'BLK')), glitter(1200, 235, 235), chflash(2200, 248, ('A1', 'WHT')),
    smega(3200, 238, 40, ('A2', 'BLK')), dissolve(4300, 245, 245, ('A1', 'BLK')), alt(5200, 4, 120), glow(5900))

# =====================================================================================================
# C. KEINDAHAN (MZ26-MZ45) — 20 animasi
# =====================================================================================================
add('Sakura Drift', 'Keindahan', 'Kelopak sakura melayang: Colortwinkles, Meteor lembut, Shimmer muda.',
    ctwink(0, 'SAKURA', 90, 110), meteor(1900, 150, 160, 'SAKURA'), shimmer(3600, 'SAKURA', ('A1', 'BLK', 'A2')),
    breathe(5400, 120, ('A1', 'A1d')))
add('Fuji Sunrise', 'Keindahan', 'Fajar di Gunung Fuji: Sunrise, Colorwaves, Fade hangat.',
    sunrise(0, 174), cwaves(2350, 'SUNSET'), glow(5000, sx=120, c=('A1', 'A1d')))
add('Mizu Ripple', 'Keindahan', 'Riak air (mizu): Ripple, Pacifica, Lake.',
    ripple(0, 'OCEAN', 110, 120), pacifica(2100, 'ATLANTICA'), lake(4200, 'OCEAN', ('A1', None)), glow(6000, c=('A1', 'A1d')))
add('Hotaru Fireflies', 'Keindahan', 'Kunang-kunang malam: Twinkle kuning, Fairy, Breathe hangat.',
    twinkle(0, 80, 110, ('A1', 'BLK')), S(2000, 'Fairy', sx=110, ix=110, pal=0, col=('A1', 'BLK', None)),
    twinkle(3800, 70, 160, ('GLD', 'BLK')), breathe(5200, 110, ('A1', 'A1d')))
add('Tsuki Moonlight', 'Keindahan', 'Cahaya bulan: Breathe putih kebiruan, Shimmer es, Fade.',
    breathe(0, 90, ('ICE', 'BLK')), shimmer(2300, 'CLOUD', ('ICE', 'BLK', 'A2')), glow(4600, sx=110, c=('ICE', 'A1d')),
    glow(5600, sx=110, c=('A1', 'A1d')))
add('Ginga Galaxy', 'Keindahan', 'Galaksi: Sinelon Dual, Twinklefox, Shimmer ungu.',
    sinedual(0, 120, 120, ('VIO', 'ICE', 'A1')), tfox(2100, 'TIAMAT', 90, 130), shimmer(4000, 'APRILNIGHT', ('VIO', 'BLK', 'A2')),
    glow(5800, sx=120, c=('A1', 'A1d')))
add('Nami Wave', 'Keindahan', 'Ombak berlapis: Wavesins, Pacifica, Colorwaves laut.',
    wavesins(0, 'OCEAN', 90), pacifica(2200, 'ATLANTICA'), cwaves(4200, 'BEACH'), glow(6000, c=('A1', 'A1d')))
add('Kasumi Mist', 'Keindahan', 'Kabut pagi: Noise Pal pastel, Lake, Breathe.',
    S(0, 'Noise Pal', sx=70, ix=100, pal='PASTEL'), lake(2300, 'PASTEL', ('A1', None)), breathe(4300, 100, ('A1', 'A1d')),
    glow(5800, sx=110))
add('Akari Bloom', 'Keindahan', 'Cahaya lampion mekar: Wipe lambat, Fade, Shimmer hangat.',
    wipe(0, 160, ('A1', 'BLK')), fade(2300, 150, ('A1', 'A1d')), shimmer(4000, 'AUTUMN', ('A1', 'BLK', 'A2')), glow(5900, sx=130))
add('Yuki Shimmer', 'Keindahan', 'Salju berkilau: Shimmer es, Twinkle putih, Sparkle.',
    shimmer(0, 'ICEFIRE', ('ICE', 'BLK', 'WHT')), twinkle(2200, 90, 160, ('WHT', 'BLK')), sparkle(4000, 200, ('ICE', 'BLK')),
    glow(5600, sx=110, c=('ICE', 'A1d')), glow(6300, sx=110, c=('A1', 'A1d')))
add('Koi Pond', 'Keindahan', 'Ikan koi berenang: Sinelon Dual oranye-putih, Ripple, Pacifica.',
    sinedual(0, 130, 130, ('ORG', 'WHT', 'A1')), ripple(2200, 'ORANGETEAL', 120, 120), pacifica(4200, 'ORANGETEAL'),
    glow(6000, c=('A1', 'A1d')))
add('Hanabi Garden', 'Keindahan', 'Taman kembang api: Fireworks 1D, PS Starburst, Colortwinkles.',
    fw1d(0, pal='FIREWORKS'), starburst(2000, pal='FIREWORKS'), ctwink(3800, 'PARTY', 130, 150), glow(5600, sx=170))
add('Tanabata Stars', 'Keindahan', 'Festival bintang: Twinklefox, Colortwinkles, Shimmer.',
    tfox(0, 'APRILNIGHT', 90, 130), ctwink(2000, 'TIAMAT', 90, 130), shimmer(3900, 'TIAMAT', ('VIO', 'BLK', 'A2')),
    glow(5700, sx=120, c=('A1', 'A1d')))
add('Aurora Kaze', 'Keindahan', 'Aurora tertiup angin: Aurora, Colorwaves aurora, Fade.',
    aurora(0, 'AURORA'), cwaves(2800, 'AURORA2'), shimmer(4500, 'AURORA', ('A1', 'BLK', 'A2')), glow(6000, c=('A1', 'A1d')))
add('Ame Neon', 'Keindahan', 'Hujan neon kota: Drip, Bouncing Balls, Ripple Rainbow.',
    drip(0, 'PARTY'), bballs(2100, 'PARTY'), rippler(3900, 120, 130), glow(5800, c=('A1', 'A1d')))
add('Kaze Whisper', 'Keindahan', 'Bisikan angin: Sinelon lembut, Wavesins, Fade.',
    S(0, 'Sinelon', sx=110, ix=130, pal=0, col=('A1', 'A2', 'BLK')), wavesins(2100, 'PASTEL', 70),
    breathe(4100, 100, ('A1', 'A1d')), glow(5600, sx=120))
add('Hoshi Cascade', 'Keindahan', 'Hujan bintang: Meteor, PS Spray 1D, Twinkle.',
    meteor(0, 140, 170, 'DEFAULT', ('A1', 'BLK')), spray1d(2100, pal='FIREWORKS'), twinkle(4000, 110, 170, ('A2', 'A1d')),
    glow(5700, c=('A1', 'A1d')))
add('Asagiri Dawn', 'Keindahan', 'Fajar berkabut: Sunrise, Pacifica lembut, Fade.',
    sunrise(0, 163, 'SUNSET2'), pacifica(2700, 'SUNSET2'), glow(5200, sx=120, c=('A1', 'A1d')))
add('Kyoto Lantern', 'Keindahan', 'Lentera Kyoto: Candle, Fire 2012, Breathe hangat.',
    candle(0, ('A1', 'BLK')), fire12(2100, pal='FIRE'), breathe(4300, 100, ('A1', 'A1d')), glow(5700, sx=110))
add('Mizuma Eternal', 'Keindahan', 'Tanda tangan Mizuma yang abadi: Colorwaves emas, Shimmer, Fade.',
    cwaves(0, 'FIRE'), shimmer(2300, 'FIRE', ('A1', 'BLK', 'A2')), meteor(4300, 170, 150, 'DEFAULT', ('A1', 'BLK')), glow(5600, sx=130))

# =====================================================================================================
# D. CAMPURAN KEDIPAN + KEINDAHAN (MZ46-MZ55) — 10 animasi
# =====================================================================================================
add('Raijin Bloom', 'Campuran', 'Kilat Strobe yang mekar menjadi Shimmer sakura.',
    smega(0, 236, 30), strobe(1300, 250, ('A2', 'BLK')), shimmer(2600, 'SAKURA', ('A1', 'BLK', 'A2')), breathe(4900, 120, ('A1', 'A1d')))
add('Hanabi Rush', 'Campuran', 'Kembang api cepat yang berakhir lembut.',
    fw1d(0), smega(1700, 238, 30, ('A1', 'BLK')), ctwink(2900, 'FIREWORKS', 180, 190), glow(5000, sx=160))
add('Oni Prism', 'Campuran', 'Strobe Rainbow menuju Sinelon Rainbow yang indah.',
    strober(0, 247), blinkr(1300, 244, 120), sinerb(2500, 190, 120), rippler(4400, 150), glow(5900))
add('Kitsune Aurora', 'Campuran', 'Kejaran rubah lalu aurora tenang.',
    chase2(0, 246, 80, ('A1', 'A2')), scand(1400, 252, 80, ('A1', 'BLK')), aurora(2800, 'AURORA'), glow(5800, c=('A1', 'A1d')))
add('Katana Dawn', 'Campuran', 'Tebasan cepat yang berakhir dalam fajar.',
    wipe(0, 254, ('A2', 'BLK')), sweep(480, 254, ('A1', 'BLK'), flip=1), wipe(960, 254, ('A3', 'BLK')),
    sweep(1440, 254, ('A1', 'BLK'), flip=1), sunrise(2000, 174), glow(4600, sx=150))
add('Tsunami Spark', 'Campuran', 'Riak dan percikan yang bermuara ke Pacifica.',
    ripple(0, 'OCEAN', 200, 160), sparkle(1500, 245, ('A2', 'BLK')), pacifica(2800, 'ATLANTICA'), glow(5600, c=('A1', 'A1d')))
add('Yokai Glow', 'Campuran', 'Kilatan yokai yang berubah menjadi cahaya bernapas.',
    glitter(0, 235, 235, ('A3', 'BLK', 'WHT')), dissolve(1400, 240, 240, ('A3', 'BLK')), breathe(2800, 120, ('A3', 'A3d')),
    shimmer(4300, 'REDTIDE', ('A3', 'BLK', 'A1')), glow(5800, c=('A1', 'A1d')))
add('Shogun Pulse', 'Campuran', 'Denyut shogun, ledakan PS Starburst lalu Colorwaves megah.',
    heartb(0, 200, 180, ('A3', 'BLK')), starburst(1800, pal='FIRE'), cwaves(3300, 'FIRE'), shimmer(5100, 'FIRE', ('A1', 'BLK', 'A2')))
add('Sakura Storm', 'Campuran', 'Badai sakura: Dissolve, Glitter, Colortwinkles, Shimmer.',
    dissolve(0, 238, 235, ('A1', 'BLK')), glitter(1400, 225, 225, ('A1', 'BLK', 'WHT')), ctwink(2800, 'SAKURA', 150, 170),
    shimmer(4400, 'SAKURA', ('A1', 'BLK', 'A2')), glow(5900, c=('A1', 'A1d')))
add('Mizuma Grand Finale', 'Campuran', 'Final megah: Strobe, Fireworks 1D, Sinelon Rainbow, Shimmer, Fade.',
    smega(0, 236, 30), fw1d(1300), sinerb(2800, 195, 120), shimmer(4400, 'RAINBOW', ('A1', 'BLK', 'A2')), glow(5800, sx=200))

# =====================================================================================================
# validasi, pembuatan header C++ dan katalog
# =====================================================================================================
# Durasi dihitung per animasi: cue terakhir + waktu penutup. Tidak ada
# pemampatan timestamp, pemotongan stage, atau lock global 7 detik.
SHOW_END_HOLD_MS = 1500

TRANSITION_BASE_MS = {
    'Signature Tetrix': 620,
    'Kedipan Cepat': 140,
    'Keindahan': 700,
    'Campuran': 480,
}

def prepare_show_timing(shows):
    """Pertahankan waktu resep dan hitung crossfade hingga cue terakhir."""
    for sh in shows:
        sh.stages.sort(key=lambda x: (x.t, x.side))
        last_cue = max((st.t for st in sh.stages), default=0)
        sh.duration_ms = last_cue + SHOW_END_HOLD_MS

        times = sorted({st.t for st in sh.stages})
        base = TRANSITION_BASE_MS.get(sh.cat, 420)
        for st in sh.stages:
            next_times = [t for t in times if t > st.t]
            next_t = next_times[0] if next_times else sh.duration_ms
            gap = max(0, next_t - st.t)
            # Transisi diatur per kategori dan dibatasi agar selesai sebelum cue berikutnya.
            if gap < 100:
                transition_ms = 0
            else:
                transition_ms = min(base, int(gap * 0.42))
            transition_ms = max(0, (transition_ms // 10) * 10)
            st.tt = min(255, transition_ms // 10)


def validate(reg):
    errs, warns = [], []
    names = set(reg.keys()) if reg else None
    for si, sh in enumerate(SHOWS, 1):
        tag = 'MZ%02d %s' % (si, sh.name)
        prev = {1: None, 2: None}; last = {}
        if not sh.stages or sh.stages[0].t != 0: errs.append(tag + ': tahap pertama harus t=0')
        for st in sh.stages:
            if st.t < 0 or st.t > 6600: errs.append('%s: t=%d di luar 0..6600 ms' % (tag, st.t))
            if names is not None and st.fx != 'Solid' and st.fx not in names: errs.append('%s: efek "%s" tidak terdaftar' % (tag, st.fx))
            for k in ('sx', 'ix', 'c1', 'c2'):
                v = getattr(st, k)
                if v is not None and not 0 <= v <= 255: errs.append('%s: %s=%s' % (tag, k, v))
            if st.c3 is not None and not 0 <= st.c3 <= 31: errs.append('%s: c3=%s (0..31)' % (tag, st.c3))
            if st.pal is not None and not 0 <= st.pal <= 72: errs.append('%s: pal=%s' % (tag, st.pal))
            if st.side not in (0, 1, 2): errs.append(tag + ': side')
            if not 0 <= st.tt <= 255: errs.append('%s: transition tt=%s (0..255)' % (tag, st.tt))
        ts = sorted({s.t for s in sh.stages})
        for a, b in zip(ts, ts[1:]):
            if b - a < 80: warns.append('%s: jarak tahap %d ms (%d->%d) sangat pendek' % (tag, b - a, a, b))
        both = [s for s in sh.stages if s.side == 0]
        final = [s for s in sh.stages if s.t == ts[-1]]
        if ts[-1] < 4200: warns.append('%s: tahap terakhir mulai %d ms (cukup awal?)' % (tag, ts[-1]))
        if len(sh.stages) > 48: errs.append(tag + ': terlalu banyak tahap')
    return errs, warns

def resolve_defaults(shows):
    """tb otomatis: 0 jika efek berganti pada tahap 'kedua sisi'; None jika hanya update parameter / satu sisi."""
    for sh in shows:
        cur = {1: None, 2: None}
        for st in sh.stages:
            sides = (1, 2) if st.side == 0 else (st.side,)
            changed = any(cur[s] != st.fx for s in sides)
            if st.tb == 'auto':
                st.tb = 0 if (st.side == 0 and changed) else -1
            for s in sides: cur[s] = st.fx

def cint(v, d=-1): return d if v is None else int(v)

def emit(shows, fx_names):
    fxi = {n: i for i, n in enumerate(fx_names)}
    L = []
    L.append('// =====================================================================================================')
    L.append('// FILE OTOMATIS dari gen_mizuma_shows.py — jangan edit tangan; ubah definisi di generator lalu jalankan ulang.')
    L.append('// 55 animasi welcoming Mizuma. Tiap "tahap" = satu efek ASLI WLED + parameter; dijalankan berurutan oleh engine.')
    L.append('// =====================================================================================================')
    L.append('#pragma once')
    L.append('#include <stdint.h>')
    L.append('')
    L.append('#define MZ_SHOW_COUNT %d' % len(shows))
    L.append('#define MZ_SHOW_END_HOLD_MS %d' % SHOW_END_HOLD_MS)
    L.append('#define MZ_SHOW_MAX_DURATION_MS %d' % max((sh.duration_ms for sh in shows), default=0))
    L.append('// Nama efek WLED yang dipakai (dicari berdasarkan NAMA saat boot, bukan nomor, jadi tahan terhadap pergeseran ID).')
    L.append('static const char* const MZ_FXN[] = {')
    for n in fx_names: L.append('  "%s",' % n)
    L.append('};')
    L.append('#define MZ_FXN_COUNT %d' % len(fx_names))
    for n in fx_names:
        L.append('#define MZFX_%s %d' % (re.sub(r'[^A-Za-z0-9]', '_', n).upper(), fxi[n]))
    L.append('')
    L.append('// Struktur satu tahap. -1 pada parameter = pakai default efek (fxdef). Warna: RGB 0x00RRGGBB,')
    L.append('// 0xFF000000 = biarkan, 0xFE000000|n = warna aksen pengguna n (bit 0x100 = versi redup).')
    L.append('struct MzStage {')
    L.append('  uint16_t t;  uint8_t fx;  uint8_t flags;   // flags: 1 = balik arah (rev), 2 = lompat timebase (lewati tunggu Tetrix)')
    L.append('  int16_t sx, ix, c1, c2, c3, pal;  int8_t o;  uint8_t side;  uint8_t tt;  int16_t tb; // tt = transisi ms/10')
    L.append('  uint32_t col[3];')
    L.append('};')
    L.append('struct MzShow { const char* name; const MzStage* st; uint8_t n; uint16_t durationMs; };')
    L.append('')
    for i, sh in enumerate(shows, 1):
        L.append('// MZ%02d %s  [%s]' % (i, sh.name, sh.cat))
        L.append('static const MzStage mzs%02d[] = {' % i)
        for st in sh.stages:
            flags = (FLIP if st.flip else 0) | (TBJ if st.tbj else 0)
            o = -1 if st.o is None else int(st.o)
            L.append('  {%d,%d,%d, %d,%d,%d,%d,%d,%d, %d,%d,%d,%d, {0x%08X,0x%08X,0x%08X}},' % (
                st.t, fxi[st.fx], flags, cint(st.sx), cint(st.ix), cint(st.c1), cint(st.c2), cint(st.c3), cint(st.pal),
                o, st.side, st.tt, st.tb, col_code(st.col[0]), col_code(st.col[1]), col_code(st.col[2])))
        L.append('};')
    L.append('')
    L.append('static const MzShow MZ_SHOWS[MZ_SHOW_COUNT] = {')
    for i, sh in enumerate(shows, 1):
        L.append('  {"MZ%02d %s", mzs%02d, (uint8_t)(sizeof(mzs%02d)/sizeof(mzs%02d[0])), %d},' % (i, sh.name, i, i, i, sh.duration_ms))
    L.append('};')
    return '\n'.join(L) + '\n'

def catalog(shows):
    cats = []
    out = ['# Katalog 55 animasi Welcoming Mizuma', '',
           'Setiap animasi adalah **rangkaian efek asli WLED** (tahap demi tahap). Durasi tiap show dihitung dari cue terakhir + %d ms waktu penutup; tidak dikunci ke 7 detik.' % SHOW_END_HOLD_MS,
           'Transisi antartahap memakai crossfade WLED yang disesuaikan dengan kategori dan jarak cue. Durasi berbeda untuk setiap animasi.',
           'Palette yang dipilih pengguna di UI menjadi palette global saat show berjalan. Nilai palette di resep dipertahankan sebagai metadata/original look, bukan memaksa pilihan pengguna.',
           'Warna `A1/A2/A3` menjadi aksen yang dipetakan ke palette aktif; hitam dan putih tetap menjadi latar/highlight.', '']
    for i, sh in enumerate(shows, 1):
        if sh.cat not in cats:
            cats.append(sh.cat); out += ['', '## %s' % sh.cat, '']
        out.append('### MZ%02d — %s' % (i, sh.name)); out.append(sh.desc); out.append('')
        out.append('**Durasi: %.2f detik**' % (sh.duration_ms / 1000.0)); out.append('')
        out.append('| t (ms) | Efek | Sisi | Catatan |'); out.append('|---:|---|:-:|---|')
        for st in sh.stages:
            notes = []
            if st.flip: notes.append('arah dibalik')
            if st.tbj: notes.append('lewati tunggu 2 dtk Tetrix')
            if st.tb not in ('auto', -1, None) and st.tb != 0: notes.append('mulai di fasa %d ms' % st.tb)
            for k in ('sx', 'ix'):
                if getattr(st, k) is not None: notes.append('%s=%d' % (k, getattr(st, k)))
            if st.pal not in (None,): notes.append('resep-pal=%d' % st.pal)
            if st.tt: notes.append('crossfade=%dms' % (st.tt * 10))
            side = {0: 'L+R', 1: 'Kanan', 2: 'Kiri'}[st.side]
            out.append('| %d | %s | %s | %s |' % (st.t, st.fx, side, ', '.join(notes)))
        out.append('')
    return '\n'.join(out)

if __name__ == '__main__':
    here = os.path.dirname(os.path.abspath(__file__))
    regp = sys.argv[1] if len(sys.argv) > 1 else os.path.join(here, 'reg_default.json')
    reg = json.load(open(regp)) if os.path.exists(regp) else None
    prepare_show_timing(SHOWS)
    resolve_defaults(SHOWS)
    errs, warns = validate(reg)
    print('show: %d | validasi efek: %s' % (len(SHOWS), 'AKTIF (%d efek terdaftar)' % len(reg) if reg else 'dilewati'))
    for w in warns: print('PERINGATAN', w)
    for e in errs: print('GALAT    ', e)
    if errs: sys.exit(1)
    used = []
    for sh in SHOWS:
        for st in sh.stages:
            if st.fx not in used: used.append(st.fx)
    used.sort(key=lambda n: (n != 'Solid', n))
    open(os.path.join(here, 'mizuma_shows.h'), 'w', encoding='utf-8').write(emit(SHOWS, used))
    open(os.path.join(here, 'katalog_55_animasi.md'), 'w', encoding='utf-8').write(catalog(SHOWS))
    cnt = {}
    for sh in SHOWS: cnt[sh.cat] = cnt.get(sh.cat, 0) + 1
    durations = [sh.duration_ms for sh in SHOWS]
    print('OK  efek unik dipakai: %d | tahap total: %d | per kategori: %s | durasi: %.2f–%.2f dtk' % (len(used), sum(len(s.stages) for s in SHOWS), cnt, min(durations)/1000.0, max(durations)/1000.0))
