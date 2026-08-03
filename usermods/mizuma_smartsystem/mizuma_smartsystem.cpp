#include "wled.h"

const char MIZUMA_SHARED_CSS[] PROGMEM = R"rawliteral(
* { box-sizing: border-box; }
body { margin:0; font-family: -apple-system, Roboto, Arial, sans-serif; background:#111; color:#eee; padding-bottom:62px; }

.mzheader { display:flex; align-items:center; justify-content:space-between; padding:12px 16px;
  background:#151515; border-bottom:1px solid #262626; position:sticky; top:0; z-index:30; }
.mzheader .brand { display:flex; align-items:center; gap:10px; }
.mzheader .logo { width:34px; height:34px; border-radius:8px; background:linear-gradient(135deg,#3ddc84,#1a8a4a);
  display:flex; align-items:center; justify-content:center; font-weight:800; color:#0a0a0a; font-size:16px; }
.mzheader .brandtext { line-height:1.1; }
.mzheader .b1 { font-size:13px; font-weight:800; color:#fff; letter-spacing:0.5px; }
.mzheader .b2 { font-size:9px; color:#3ddc84; letter-spacing:1.5px; }
.mzheader .status { display:flex; align-items:center; gap:8px; }
.mzheader .volt { font-size:12px; font-weight:700; color:#eee; background:#1e1e1e; padding:4px 9px; border-radius:20px; border:1px solid #2a2a2a; }
.mzheader .conn { font-size:15px; }

.bottomnav { position:fixed; bottom:0; left:0; right:0; display:flex;
  background:#181818; border-top:1px solid #2a2a2a; z-index:40; padding-bottom:env(safe-area-inset-bottom); }
.bottomnav a { flex:1; display:flex; flex-direction:column; align-items:center; justify-content:center;
  padding:8px 4px; text-decoration:none; color:#777; font-size:10px; gap:3px; }
.bottomnav a.active { color:#3ddc84; }
.bottomnav a.disabled { color:#444; }
.bottomnav .bn-icon { font-size:19px; }

.card { background:#1a1a1a; border-radius:12px; padding:14px; border:1px solid #242424; margin-bottom:12px; }
.card-title { font-size:12px; color:#999; text-transform:uppercase; letter-spacing:0.5px; margin-bottom:10px; font-weight:700; }
.placeholder-text { color:#666; font-size:12px; text-align:center; padding:10px 0; }
.badge { font-size:10px; padding:3px 8px; border-radius:20px; font-weight:700; }
.badge.gray { background:#2a2a2a; color:#999; }
.badge.green { background:#183d24; color:#3ddc84; }
.badge.orange { background:#3d2e18; color:#e8a33d; }
)rawliteral";

const char MIZUMA_HEADER_HTML[] PROGMEM = R"rawliteral(
<div class="mzheader">
  <div class="brand">
    <div class="logo">M</div>
    <div class="brandtext"><div class="b1">MIZUMA</div><div class="b2">PERFORMANCE</div></div>
  </div>
  <div class="status">
    <span class="volt" id="hdrVolt">-- V</span>
    <span class="conn" id="hdrConn">&#128246;</span>
  </div>
</div>
)rawliteral";

const char MIZUMA_HEADER_SCRIPT[] PROGMEM = R"rawliteral(
async function mzRefreshHeader() {
  try {
    const res = await fetch('/mizuma/status');
    const data = await res.json();
    document.getElementById('hdrConn').textContent = data.sta ? '\uD83C\uDF10' : '\uD83D\uDCF6';
  } catch (e) {}
}
mzRefreshHeader();
setInterval(mzRefreshHeader, 5000);
)rawliteral";

const char MIZUMA_BOTTOMNAV_HTML[] PROGMEM = R"rawliteral(
<div class="bottomnav">
  <a href="/app" class="__ACTIVE_BERANDA__"><span class="bn-icon">&#127968;</span>Beranda</a>
  <a href="/led" class="__ACTIVE_LAMPU__"><span class="bn-icon">&#128161;</span>Lampu</a>
  <a href="/servis" class="__ACTIVE_SERVIS__"><span class="bn-icon">&#128736;</span>Servis</a>
  <a href="/keamanan" class="__ACTIVE_KEAMANAN__"><span class="bn-icon">&#128274;</span>Keamanan</a>
  <a href="/pengaturan" class="__ACTIVE_PENGATURAN__"><span class="bn-icon">&#9881;&#65039;</span>Pengaturan</a>
</div>
)rawliteral";

const char MIZUMA_HOME_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Mizuma Smart System</title>
<style>
  %SHARED_CSS%
  .greet-name { font-size:20px; font-weight:800; color:#fff; }
  .greet-sub { font-size:12px; color:#999; margin-top:2px; }
  .stat-row { display:grid; grid-template-columns:1fr 1fr 1fr; gap:8px; margin-bottom:12px; }
  .stat-box { background:#1a1a1a; border-radius:10px; padding:10px; text-align:center; border:1px solid #242424; }
  .stat-box .si { font-size:18px; margin-bottom:4px; }
  .stat-box .sl { font-size:9px; color:#888; text-transform:uppercase; }
  .stat-box .sv { font-size:12px; font-weight:700; color:#eee; margin-top:2px; }
  .volt-big { font-size:30px; font-weight:800; color:#fff; }
  .volt-unit { font-size:14px; color:#888; }
  .gauge { height:8px; border-radius:4px; background:linear-gradient(90deg,#e05555,#e8a33d,#3ddc84); margin:10px 0 4px; position:relative; }
  .gauge-mark { position:absolute; top:-3px; width:2px; height:14px; background:#fff; border-radius:1px; }
  .quick-tiles { display:grid; grid-template-columns:1fr 1fr; gap:10px; margin-bottom:12px; }
  .tile { background:#1a1a1a; border-radius:12px; padding:16px; text-align:center; text-decoration:none; color:#eee; border:1px solid #242424; }
  .tile .ti { font-size:22px; margin-bottom:6px; }
  .tile .tl { font-size:12px; }
  .container { padding:16px; max-width:520px; margin:0 auto; }
</style>
</head>
<body>
%HEADER%
<div class="container">

  <div class="card">
    <div class="greet-name" id="greetText">Halo!</div>
    <div class="greet-sub">%VEHICLE_BRAND% &bull; %VEHICLE_YEAR% &bull; %VEHICLE_PLATE%</div>
  </div>

  <div class="stat-row">
    <div class="stat-box"><div class="si">&#128337;</div><div class="sl">Odometer</div><div class="sv">--</div></div>
    <div class="stat-box"><div class="si">&#128274;</div><div class="sl">Alarm</div><div class="sv">--</div></div>
    <div class="stat-box"><div class="si">&#128161;</div><div class="sl">Lampu</div><div class="sv">--</div></div>
  </div>

  <div class="card">
    <div class="card-title">Tegangan Aki</div>
    <span class="volt-big">--</span><span class="volt-unit"> V</span>
    <span class="badge gray" style="float:right; margin-top:8px;">Sensor belum terpasang</span>
    <div class="gauge"></div>
    <div class="placeholder-text">Data akan muncul setelah sensor tegangan terpasang</div>
  </div>

  <div class="card">
    <div class="card-title">Reminder Servis</div>
    <div class="placeholder-text">Belum ada data servis &mdash; lihat tab Servis</div>
  </div>

  <div class="quick-tiles">
    <a class="tile" href="/led"><div class="ti">&#128161;</div><div class="tl">Kontrol Lampu</div></a>
    <a class="tile" href="/keamanan"><div class="ti">&#128205;</div><div class="tl">Lokasi GPS</div></a>
  </div>

  <div class="card">
    <div class="card-title">Riwayat Kejadian</div>
    <div class="placeholder-text">Belum ada riwayat</div>
  </div>

</div>
%BOTTOMNAV%
<script>
%HEADER_SCRIPT%
const h = new Date().getHours();
let greet = 'Selamat malam';
if (h >= 4 && h < 11) greet = 'Selamat pagi';
else if (h >= 11 && h < 15) greet = 'Selamat siang';
else if (h >= 15 && h < 18) greet = 'Selamat sore';
document.getElementById('greetText').textContent = greet + ', %VEHICLE_NAME%';
</script>
</body>
</html>
)rawliteral";

const char MIZUMA_SETTINGS_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Pengaturan - Mizuma</title>
<style>
  %SHARED_CSS%
  .container { padding:16px; max-width:520px; margin:0 auto; }
  .info-row { display:flex; justify-content:space-between; padding:7px 0; font-size:13px; border-bottom:1px solid #202020; }
  .info-row:last-child { border-bottom:none; }
  .info-row .k { color:#999; }
  .info-row .v { color:#eee; font-weight:600; }
  .btn { display:block; width:100%; text-align:center; padding:10px; background:#232323; color:#eee; border-radius:8px;
    text-decoration:none; font-size:13px; font-weight:700; margin-top:10px; border:1px solid #333; }
  .danger { border:1px solid #4a1f1f; background:#1a1010; }
  .danger .card-title { color:#e05555; }
  .danger .btn { background:#3d1414; color:#ff8080; border-color:#5a1c1c; }
</style>
</head>
<body>
%HEADER%
<div class="container">

  <div class="card">
    <div class="card-title">Informasi Motor</div>
    <div class="info-row"><span class="k">Nama Motor</span><span class="v">%VEHICLE_NAME%</span></div>
    <div class="info-row"><span class="k">Brand</span><span class="v">%VEHICLE_BRAND%</span></div>
    <div class="info-row"><span class="k">Tahun</span><span class="v">%VEHICLE_YEAR%</span></div>
    <div class="info-row"><span class="k">No. Polisi</span><span class="v">%VEHICLE_PLATE%</span></div>
    <a class="btn" href="/settings/um">Ubah Data Motor</a>
  </div>

  <div class="card">
    <div class="card-title">Koneksi</div>
    <div class="info-row"><span class="k">Mode</span><span class="v" id="connMode">Memuat...</span></div>
    <div class="info-row"><span class="k">Nama WiFi</span><span class="v" id="connSSID">--</span></div>
    <div class="info-row"><span class="k">Alamat IP</span><span class="v" id="connIP">--</span></div>
    <a class="btn" href="/settings/wifi">Ubah Pengaturan Jaringan</a>
  </div>

  <div class="card">
    <div class="card-title">Perangkat & Firmware</div>
    <div class="info-row"><span class="k">Firmware</span><span class="v">MIZUMA WLED 16.0.1</span></div>
    <div class="info-row"><span class="k">Jumlah LED</span><span class="v">96 LED</span></div>
  </div>

  <div class="card danger">
    <div class="card-title">Reset Pabrik</div>
    <div class="placeholder-text" style="color:#c99;">Menghapus semua konfigurasi dan mengembalikan ke pengaturan awal</div>
    <a class="btn" href="/settings/sec">Buka Halaman Reset</a>
  </div>

</div>
%BOTTOMNAV%
<script>
%HEADER_SCRIPT%
async function loadConn() {
  try {
    const res = await fetch('/mizuma/status');
    const data = await res.json();
    document.getElementById('connMode').textContent = data.sta ? 'Mode Internet (Hotspot HP)' : 'Mode Kontrol (WiFi Mizuma)';
    document.getElementById('connIP').textContent = data.sta ? data.staIP : '4.3.2.1';
    document.getElementById('connSSID').textContent = data.sta ? '(Hotspot HP)' : 'Mizuma Smart System';
  } catch (e) {}
}
loadConn();
</script>
</body>
</html>
)rawliteral";

const char MIZUMA_PLACEHOLDER_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>%PAGE_TITLE% - Mizuma</title>
<style>
  %SHARED_CSS%
  .container { padding:16px; max-width:520px; margin:0 auto; }
  .center-card { text-align:center; padding:40px 20px; }
  .center-card .ci { font-size:40px; margin-bottom:14px; }
  .center-card .ct { font-size:15px; font-weight:700; color:#fff; margin-bottom:6px; }
</style>
</head>
<body>
%HEADER%
<div class="container">
  <div class="card center-card">
    <div class="ci">%PAGE_ICON%</div>
    <div class="ct">%PAGE_TITLE%</div>
    <div class="placeholder-text">Fitur ini sedang dikembangkan dan akan tersedia di tahap pengembangan berikutnya.</div>
  </div>
</div>
%BOTTOMNAV%
<script>%HEADER_SCRIPT%</script>
</body>
</html>
)rawliteral";

const char MIZUMA_LED_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Lampu - Mizuma</title>
<style>
  %SHARED_CSS%

  .freeze { position:sticky; top:0; z-index:20; background:#111; }

  .tabbar { display:flex; overflow-x:auto; background:#151515; border-bottom:1px solid #232323; }
  .tabbar button { flex:1 0 auto; padding:11px 10px; background:none; border:none; color:#999; font-size:12.5px; font-weight:600; white-space:nowrap; border-bottom:3px solid transparent; }
  .tabbar button.active { color:#fff; border-bottom-color:#3ddc84; }

  .preview-grid { display:grid; grid-template-columns:1fr 62px 78px; grid-template-rows:34px 34px; gap:5px; padding:10px 12px; background:#111; }
  .pv-bar { border-radius:6px; background:#000; display:flex; align-items:center; padding:0 3px; gap:1px; overflow:hidden; }
  .pv-bar .seg { flex:1; height:20px; background:#2a2a2a; border-radius:1px; }
  .pv-bar.kanan { grid-column:1; grid-row:1; }
  .pv-bar.kiri  { grid-column:1; grid-row:2; }
  .side-btn { grid-column:2; background:#1e1e1e; border:1px solid #333; color:#999; border-radius:6px; font-size:11px; font-weight:600; }
  .side-btn.kanan-btn { grid-row:1; } .side-btn.kiri-btn { grid-row:2; }
  .side-btn.active { background:#2a5; color:#fff; border-color:#2a5; }
  .side-btn-both { grid-column:3; grid-row:1 / span 2; background:#1e1e1e; border:1px solid #333; color:#999; border-radius:6px; font-size:11px; font-weight:600; }
  .side-btn-both.active { background:#2a5; color:#fff; border-color:#2a5; }

  .body-layout { display:flex; }
  .subnav { width:74px; flex-shrink:0; background:#161616; border-right:1px solid #232323; overflow-y:hidden; }
  .subnav button { display:block; width:100%; padding:16px 4px; background:none; border:none; color:#888; font-size:10.5px; text-align:center; border-left:3px solid transparent; }
  .subnav button.active { color:#fff; border-left-color:#3ddc84; background:#1e1e1e; }
  .subnav .icon { display:block; font-size:19px; margin-bottom:4px; }

  .content-wrap { flex:1; min-width:0; overflow-y:auto; }
  .content-sticky { position:sticky; top:0; z-index:15; background:#111; padding:10px 12px; border-bottom:1px solid #202020; }

  .toggle-pair { display:flex; gap:6px; }
  .toggle-pair button { flex:1; padding:9px; background:#1a1a1a; border:1px solid #2a2a2a; color:#999; border-radius:8px; font-size:12px; font-weight:700; }
  .toggle-pair button.active { background:#3ddc84; color:#0a0a0a; border-color:#3ddc84; }

  .brightness-row { display:flex; align-items:center; gap:10px; }
  .brightness-row .bi { font-size:16px; }
  .brightness-row input[type=range] { flex:1; }

  .content-scroll { padding:14px; }

  .wheel-wrap { display:flex; flex-direction:column; align-items:center; margin-bottom:14px; }
  #colorWheel { border-radius:50%; touch-action:none; }
  .slots-row { display:flex; gap:10px; margin-top:12px; }
  .slot { width:38px; height:38px; border-radius:8px; border:2px solid #333; cursor:pointer; }
  .slot.active { border-color:#3ddc84; }

  .search-box { width:100%; padding:9px 12px; background:#1a1a1a; border:1px solid #2a2a2a; border-radius:8px; color:#eee; font-size:13px; margin-bottom:12px; }
  .palette-grid { display:grid; grid-template-columns:repeat(3,1fr); gap:8px; }
  .palette-card { border-radius:8px; overflow:hidden; border:2px solid #262626; cursor:pointer; }
  .palette-card.active { border-color:#3ddc84; }
  .palette-card .swatch { height:34px; }
  .palette-card .pname { font-size:9px; text-align:center; padding:3px; background:#161616; color:#999; }
  .download-link { display:block; text-align:center; margin-top:14px; padding:10px; background:#1a1a1a; border-radius:8px; color:#3ddc84; text-decoration:none; font-size:12px; border:1px dashed #2a2a2a; }

  .fx-list { display:flex; flex-direction:column; gap:6px; margin-bottom:14px; }
  .fx-item { padding:11px 12px; background:#1a1a1a; border-radius:8px; border:1px solid #242424; font-size:13px; color:#ddd; }
  .fx-item.active { border-color:#3ddc84; background:#132a1e; color:#fff; }

  .fx-params { background:#161616; border-radius:10px; padding:14px; border:1px solid #242424; }
  .fx-params.hidden { display:none; }
  .param-row { margin-bottom:14px; }
  .param-label { display:flex; justify-content:space-between; font-size:11px; color:#999; margin-bottom:5px; }
  .param-row input[type=range] { width:100%; }
  .toggle-row { display:flex; justify-content:space-between; align-items:center; font-size:12px; color:#ccc; padding:8px 0; border-bottom:1px solid #222; }
  .switch { position:relative; width:38px; height:22px; background:#333; border-radius:20px; cursor:pointer; }
  .switch.on { background:#3ddc84; }
  .switch .knob { position:absolute; width:18px; height:18px; background:#fff; border-radius:50%; top:2px; left:2px; transition:0.2s; }
  .switch.on .knob { left:18px; }

  .save-btn { display:block; width:100%; text-align:center; padding:12px; background:#3ddc84; color:#0a0a0a; border-radius:10px; font-weight:800; font-size:14px; border:none; margin-top:10px; }
  .save-hint { font-size:11px; color:#888; text-align:center; margin-top:8px; }
</style>
</head>
<body>

<div class="freeze">
  %HEADER%
  <div class="tabbar" id="tabbar">
    <button data-tab="welcoming" class="active">Welcoming</button>
    <button data-tab="riding">Riding</button>
    <button data-tab="sein">Sein</button>
    <button data-tab="rem">Rem</button>
    <button data-tab="hazard">Hazard</button>
  </div>
  <div class="preview-grid">
    <div class="pv-bar kanan" id="pvKanan"></div>
    <div class="pv-bar kiri" id="pvKiri"></div>
    <button class="side-btn kanan-btn active" data-side="kanan">Kanan</button>
    <button class="side-btn kiri-btn" data-side="kiri">Kiri</button>
    <button class="side-btn-both" data-side="both">Kanan<br>+ Kiri</button>
  </div>
</div>

<div class="body-layout">
  <div class="subnav" id="subnav">
    <button data-sub="warna" class="active"><span class="icon">&#127912;</span>Pola Warna</button>
    <button data-sub="efek"><span class="icon">&#10024;</span>Efek</button>
    <button data-sub="simpan"><span class="icon">&#128190;</span>Simpan</button>
  </div>

  <div class="content-wrap" id="contentWrap">

    <!-- ==== PANE: POLA WARNA ==== -->
    <div id="paneWarna">
      <div class="content-sticky">
        <div class="toggle-pair" id="colorToggle">
          <button data-ct="custom" class="active">Custom</button>
          <button data-ct="template">Template</button>
        </div>
      </div>
      <div class="content-scroll">
        <div id="ctCustom">
          <div class="wheel-wrap">
            <canvas id="colorWheel" width="220" height="220"></canvas>
            <div class="slots-row">
              <div class="slot active" style="background:#ff3b3b;" data-slot="0"></div>
              <div class="slot" style="background:#3ddc84;" data-slot="1"></div>
              <div class="slot" style="background:#3b8cff;" data-slot="2"></div>
            </div>
          </div>
        </div>
        <div id="ctTemplate" style="display:none;">
          <input class="search-box" placeholder="Cari palette...">
          <div class="palette-grid" id="paletteGrid"></div>
          <a class="download-link" href="#">&#8681; Download Palette Lainnya</a>
        </div>
      </div>
    </div>

    <!-- ==== PANE: EFEK ==== -->
    <div id="paneEfek" style="display:none;">
      <div class="content-sticky">
        <div class="brightness-row">
          <span class="bi">&#9728;&#65039;</span>
          <input type="range" min="0" max="255" value="180" id="brightSlider">
          <span id="brightVal" style="font-size:12px; color:#999; width:30px;">180</span>
        </div>
      </div>
      <div class="content-scroll">
        <div class="fx-list" id="fxList"></div>
        <div class="fx-params hidden" id="fxParams">
          <div class="param-row">
            <div class="param-label"><span>Speed</span><span>128</span></div>
            <input type="range" min="0" max="255" value="128">
          </div>
          <div class="param-row">
            <div class="param-label"><span>Intensity</span><span>128</span></div>
            <input type="range" min="0" max="255" value="128">
          </div>
          <div class="param-row">
            <div class="param-label"><span>Size</span><span>50</span></div>
            <input type="range" min="0" max="255" value="128">
          </div>
          <div class="param-row">
            <div class="param-label"><span>Blur</span><span>64</span></div>
            <input type="range" min="0" max="255" value="64">
          </div>
          <div class="toggle-row"><span>Freeze</span><div class="switch" id="swFreeze"><div class="knob"></div></div></div>
          <div class="toggle-row"><span>Reverse</span><div class="switch" id="swReverse"><div class="knob"></div></div></div>
        </div>
      </div>
    </div>

    <!-- ==== PANE: SIMPAN ==== -->
    <div id="paneSimpan" style="display:none;">
      <div class="content-scroll">
        <div class="card">
          <div class="card-title">Simpan Pengaturan</div>
          <div class="placeholder-text" id="saveContext">Welcoming &mdash; Kanan</div>
          <button class="save-btn">Simpan Sekarang</button>
          <div class="save-hint">Menyimpan permanen ke memori ESP32</div>
        </div>
      </div>
    </div>

  </div>
</div>

%BOTTOMNAV%
<script>
%HEADER_SCRIPT%

let activeTab = 'welcoming';
let activeSide = 'kanan';
let activeSub = 'warna';

function buildPreviewBar(id) {
  const el = document.getElementById(id); el.innerHTML = '';
  for (let i = 0; i < 48; i++) { const s = document.createElement('div'); s.className = 'seg'; el.appendChild(s); }
}
buildPreviewBar('pvKanan'); buildPreviewBar('pvKiri');

document.getElementById('tabbar').addEventListener('click', e => {
  if (e.target.tagName !== 'BUTTON') return;
  document.querySelectorAll('#tabbar button').forEach(b => b.classList.remove('active'));
  e.target.classList.add('active');
  activeTab = e.target.dataset.tab;
  updateSaveContext();
});

document.querySelector('.preview-grid').addEventListener('click', e => {
  const btn = e.target.closest('button'); if (!btn) return;
  document.querySelectorAll('.side-btn, .side-btn-both').forEach(b => b.classList.remove('active'));
  btn.classList.add('active');
  activeSide = btn.dataset.side;
  updateSaveContext();
});

document.getElementById('subnav').addEventListener('click', e => {
  const btn = e.target.closest('button'); if (!btn) return;
  document.querySelectorAll('#subnav button').forEach(b => b.classList.remove('active'));
  btn.classList.add('active');
  activeSub = btn.dataset.sub;
  document.getElementById('paneWarna').style.display = activeSub === 'warna' ? 'block' : 'none';
  document.getElementById('paneEfek').style.display = activeSub === 'efek' ? 'block' : 'none';
  document.getElementById('paneSimpan').style.display = activeSub === 'simpan' ? 'block' : 'none';
  updateSaveContext();
});

function updateSaveContext() {
  const tabLabels = { welcoming:'Welcoming', riding:'Riding', sein:'Sein', rem:'Rem', hazard:'Hazard' };
  const sideLabels = { kanan:'Kanan', kiri:'Kiri', both:'Kanan + Kiri' };
  document.getElementById('saveContext').textContent = tabLabels[activeTab] + ' \u2014 ' + sideLabels[activeSide];
}
updateSaveContext();

// Toggle Custom / Template
document.getElementById('colorToggle').addEventListener('click', e => {
  if (e.target.tagName !== 'BUTTON') return;
  document.querySelectorAll('#colorToggle button').forEach(b => b.classList.remove('active'));
  e.target.classList.add('active');
  const isCustom = e.target.dataset.ct === 'custom';
  document.getElementById('ctCustom').style.display = isCustom ? 'block' : 'none';
  document.getElementById('ctTemplate').style.display = isCustom ? 'none' : 'block';
});

// Slot warna custom
document.querySelectorAll('.slot').forEach(s => {
  s.addEventListener('click', () => {
    document.querySelectorAll('.slot').forEach(x => x.classList.remove('active'));
    s.classList.add('active');
  });
});

// Color wheel (HSV) sederhana via canvas
const wheel = document.getElementById('colorWheel');
const wctx = wheel.getContext('2d');
const wr = 110;
const img = wctx.createImageData(220, 220);
for (let y = 0; y < 220; y++) {
  for (let x = 0; x < 220; x++) {
    const dx = x - wr, dy = y - wr;
    const dist = Math.sqrt(dx*dx + dy*dy);
    const idx = (y * 220 + x) * 4;
    if (dist <= wr) {
      const angle = Math.atan2(dy, dx) * 180 / Math.PI + 180;
      const sat = dist / wr;
      const [r,g,b] = hsvToRgb(angle, sat, 1);
      img.data[idx]=r; img.data[idx+1]=g; img.data[idx+2]=b; img.data[idx+3]=255;
    } else { img.data[idx+3] = 0; }
  }
}
wctx.putImageData(img, 0, 0);

function hsvToRgb(h, s, v) {
  const c = v*s, x = c*(1-Math.abs((h/60)%2-1)), m = v-c;
  let r,g,b;
  if (h<60){r=c;g=x;b=0;} else if(h<120){r=x;g=c;b=0;} else if(h<180){r=0;g=c;b=x;}
  else if(h<240){r=0;g=x;b=c;} else if(h<300){r=x;g=0;b=c;} else {r=c;g=0;b=x;}
  return [Math.round((r+m)*255), Math.round((g+m)*255), Math.round((b+m)*255)];
}

function pickWheelColor(clientX, clientY) {
  const rect = wheel.getBoundingClientRect();
  const x = clientX - rect.left, y = clientY - rect.top;
  const dx = x - wr, dy = y - wr;
  if (Math.sqrt(dx*dx+dy*dy) > wr) return;
  const angle = Math.atan2(dy, dx) * 180 / Math.PI + 180;
  const sat = Math.min(1, Math.sqrt(dx*dx+dy*dy) / wr);
  const [r,g,b] = hsvToRgb(angle, sat, 1);
  const active = document.querySelector('.slot.active');
  if (active) active.style.background = 'rgb(' + r + ',' + g + ',' + b + ')';
}
wheel.addEventListener('click', e => pickWheelColor(e.clientX, e.clientY));

// Dummy palette grid (Fase 6-8 nanti diisi data asli dari WLED)
const paletteGrid = document.getElementById('paletteGrid');
const dummyGradients = ['linear-gradient(90deg,#ff3b3b,#ffd23b)','linear-gradient(90deg,#3ddc84,#3b8cff)',
  'linear-gradient(90deg,#a83bff,#ff3bd6)','linear-gradient(90deg,#3bfff0,#3b5cff)',
  'linear-gradient(90deg,#ffae3b,#ff3b3b)','linear-gradient(90deg,#3bff5c,#d6ff3b)'];
dummyGradients.forEach((g, i) => {
  const card = document.createElement('div');
  card.className = 'palette-card' + (i === 0 ? ' active' : '');
  card.innerHTML = '<div class="swatch" style="background:' + g + ';"></div><div class="pname">Palette ' + (i+1) + '</div>';
  card.addEventListener('click', () => { document.querySelectorAll('.palette-card').forEach(c=>c.classList.remove('active')); card.classList.add('active'); });
  paletteGrid.appendChild(card);
});

// Dummy fx list (Fase 7 diisi daftar efek WLED asli)
const fxList = document.getElementById('fxList');
const dummyFx = ['Solid','Blink','Breathe','Wipe','Chase','Rainbow','Sparkle','Fade'];
dummyFx.forEach((name, i) => {
  const item = document.createElement('div');
  item.className = 'fx-item' + (i === 0 ? ' active' : '');
  item.textContent = name;
  item.addEventListener('click', () => {
    document.querySelectorAll('.fx-item').forEach(x=>x.classList.remove('active'));
    item.classList.add('active');
    document.getElementById('fxParams').classList.remove('hidden');
  });
  fxList.appendChild(item);
});

document.getElementById('brightSlider').addEventListener('input', e => {
  document.getElementById('brightVal').textContent = e.target.value;
});

document.querySelectorAll('.switch').forEach(sw => {
  sw.addEventListener('click', () => sw.classList.toggle('on'));
});

// Set tinggi content-wrap agar scroll internal, bukan scroll seluruh halaman
function fitContentHeight() {
  const freezeH = document.querySelector('.freeze').offsetHeight;
  const wrap = document.getElementById('contentWrap');
  wrap.style.height = 'calc(100vh - ' + freezeH + 'px - 62px)';
  document.getElementById('subnav').style.height = wrap.style.height;
}
fitContentHeight();
window.addEventListener('resize', fitContentHeight);
</script>
</body>
</html>
)rawliteral";

class MizumaSmartSystem : public Usermod {
  private:
    // ===== Data kendaraan =====
    String vehicleName  = "";
    String vehicleBrand = "";
    uint16_t vehicleYear = 0;
    String vehiclePlate = "";

    // ===== Reminder servis =====
    struct ReminderItem {
      unsigned long lastServiceEpoch = 0;
      uint16_t intervalDays = 0;
    };
    ReminderItem oliMesin, oliRem, oliGardan, cvt, filter;

    // ===== Mapping preset LED =====
    uint8_t presetWelcomingKanan = 1, presetWelcomingKiri = 2;
    uint8_t presetRidingKanan    = 3, presetRidingKiri    = 4;
    uint8_t presetSeinKanan      = 5, presetSeinKiri      = 6;
    uint8_t presetRemKanan       = 7, presetRemKiri       = 8;
    uint8_t presetHazardKanan    = 9, presetHazardKiri    = 10;

    // Helper: sisipkan header + bottom nav + data umum ke template halaman manapun
    String renderPage(const char* pageTemplate, const char* activeKey) {
      String html = FPSTR(pageTemplate);

      html.replace("%SHARED_CSS%", FPSTR(MIZUMA_SHARED_CSS));
      html.replace("%HEADER%", FPSTR(MIZUMA_HEADER_HTML));
      html.replace("%HEADER_SCRIPT%", FPSTR(MIZUMA_HEADER_SCRIPT));

      String navHtml = FPSTR(MIZUMA_BOTTOMNAV_HTML);
      navHtml.replace("__ACTIVE_BERANDA__",    strcmp(activeKey, "beranda")    == 0 ? "active" : "");
      navHtml.replace("__ACTIVE_LAMPU__",      strcmp(activeKey, "lampu")      == 0 ? "active" : "");
      navHtml.replace("__ACTIVE_SERVIS__",     strcmp(activeKey, "servis")     == 0 ? "active" : "");
      navHtml.replace("__ACTIVE_KEAMANAN__",   strcmp(activeKey, "keamanan")   == 0 ? "active" : "");
      navHtml.replace("__ACTIVE_PENGATURAN__", strcmp(activeKey, "pengaturan") == 0 ? "active" : "");
      html.replace("%BOTTOMNAV%", navHtml);

      html.replace("%VEHICLE_NAME%",  vehicleName.length()  ? vehicleName  : "Motor Anda");
      html.replace("%VEHICLE_BRAND%", vehicleBrand.length() ? vehicleBrand : "-");
      html.replace("%VEHICLE_YEAR%",  vehicleYear ? String(vehicleYear) : "-");
      html.replace("%VEHICLE_PLATE%", vehiclePlate.length() ? vehiclePlate : "-");

      return html;
    }

    String renderPlaceholder(const char* title, const char* icon, const char* activeKey) {
      String html = FPSTR(MIZUMA_PLACEHOLDER_HTML);
      html.replace("%PAGE_TITLE%", title);
      html.replace("%PAGE_ICON%", icon);
      return renderPage(html.c_str(), activeKey);
    }

  public:
    void setup() override {
      apBehavior = AP_BEHAVIOR_ALWAYS;

      DEBUG_PRINTLN(F("[Mizuma] Usermod utama siap"));

      server.on("/app", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/html", renderPage(MIZUMA_HOME_HTML, "beranda"));
      });

      server.on("/led", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/html", renderPage(MIZUMA_LED_HTML, "lampu"));
      });

      server.on("/pengaturan", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/html", renderPage(MIZUMA_SETTINGS_HTML, "pengaturan"));
      });

      server.on("/servis", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/html", renderPlaceholder("Servis", "&#128736;", "servis"));
      });

      server.on("/keamanan", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/html", renderPlaceholder("Keamanan & GPS", "&#128274;", "keamanan"));
      });

      server.on("/mizuma/status", HTTP_GET, [](AsyncWebServerRequest *request){
        bool apOn  = (WiFi.softAPgetStationNum() > 0);
        bool staOn = (WiFi.status() == WL_CONNECTED);
        String staIP = staOn ? WiFi.localIP().toString() : "";

        String json = "{\"ap\":";
        json += apOn ? "true" : "false";
        json += ",\"sta\":";
        json += staOn ? "true" : "false";
        json += ",\"staIP\":\"";
        json += staIP;
        json += "\"}";

        request->send(200, "application/json", json);
      });
    }

    void loop() override {}

    void addToConfig(JsonObject& root) override {
      JsonObject top = root.createNestedObject("Mizuma");
      JsonObject vehicle = top.createNestedObject("vehicle");
      vehicle["name"]  = vehicleName;
      vehicle["brand"] = vehicleBrand;
      vehicle["year"]  = vehicleYear;
      vehicle["plate"] = vehiclePlate;

      JsonObject rem = top.createNestedObject("reminder");
      rem["oliMesin_last"]   = oliMesin.lastServiceEpoch;
      rem["oliMesin_int"]    = oliMesin.intervalDays;
      rem["oliRem_last"]     = oliRem.lastServiceEpoch;
      rem["oliRem_int"]      = oliRem.intervalDays;
      rem["oliGardan_last"]  = oliGardan.lastServiceEpoch;
      rem["oliGardan_int"]   = oliGardan.intervalDays;
      rem["cvt_last"]        = cvt.lastServiceEpoch;
      rem["cvt_int"]         = cvt.intervalDays;
      rem["filter_last"]     = filter.lastServiceEpoch;
      rem["filter_int"]      = filter.intervalDays;

      JsonObject pm = top.createNestedObject("presetMap");
      pm["welcomingKanan"] = presetWelcomingKanan;
      pm["welcomingKiri"]  = presetWelcomingKiri;
      pm["ridingKanan"]    = presetRidingKanan;
      pm["ridingKiri"]     = presetRidingKiri;
      pm["seinKanan"]      = presetSeinKanan;
      pm["seinKiri"]       = presetSeinKiri;
      pm["remKanan"]       = presetRemKanan;
      pm["remKiri"]        = presetRemKiri;
      pm["hazardKanan"]    = presetHazardKanan;
      pm["hazardKiri"]     = presetHazardKiri;
    }

    bool readFromConfig(JsonObject& root) override {
      JsonObject top = root["Mizuma"];
      if (top.isNull()) return false;

      JsonObject vehicle = top["vehicle"];
      vehicleName  = vehicle["name"]  | "";
      vehicleBrand = vehicle["brand"] | "";
      vehicleYear  = vehicle["year"]  | 0;
      vehiclePlate = vehicle["plate"] | "";

      JsonObject rem = top["reminder"];
      oliMesin.lastServiceEpoch  = rem["oliMesin_last"]  | 0;
      oliMesin.intervalDays      = rem["oliMesin_int"]   | 0;
      oliRem.lastServiceEpoch    = rem["oliRem_last"]    | 0;
      oliRem.intervalDays        = rem["oliRem_int"]     | 0;
      oliGardan.lastServiceEpoch = rem["oliGardan_last"] | 0;
      oliGardan.intervalDays     = rem["oliGardan_int"]  | 0;
      cvt.lastServiceEpoch       = rem["cvt_last"]       | 0;
      cvt.intervalDays           = rem["cvt_int"]        | 0;
      filter.lastServiceEpoch    = rem["filter_last"]    | 0;
      filter.intervalDays        = rem["filter_int"]     | 0;

      JsonObject pm = top["presetMap"];
      presetWelcomingKanan = pm["welcomingKanan"] | 1;
      presetWelcomingKiri  = pm["welcomingKiri"]  | 2;
      presetRidingKanan    = pm["ridingKanan"]    | 3;
      presetRidingKiri     = pm["ridingKiri"]     | 4;
      presetSeinKanan      = pm["seinKanan"]      | 5;
      presetSeinKiri       = pm["seinKiri"]       | 6;
      presetRemKanan       = pm["remKanan"]       | 7;
      presetRemKiri        = pm["remKiri"]        | 8;
      presetHazardKanan    = pm["hazardKanan"]    | 9;
      presetHazardKiri     = pm["hazardKiri"]     | 10;

      return true;
    }
};

static MizumaSmartSystem mizuma_smartsystem;
REGISTER_USERMOD(mizuma_smartsystem);
