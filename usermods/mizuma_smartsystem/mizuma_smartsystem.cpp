#include "wled.h"

// ===== Komponen bottom nav (dipakai di semua halaman) =====
const char MIZUMA_BOTTOMNAV_CSS[] PROGMEM = R"rawliteral(
.bottomnav { position:fixed; bottom:0; left:0; right:0; display:flex; overflow-x:auto;
  background:#181818; border-top:1px solid #2a2a2a; z-index:20; padding-bottom:env(safe-area-inset-bottom); }
.bottomnav a { flex:1 0 60px; display:flex; flex-direction:column; align-items:center; justify-content:center;
  padding:8px 4px; text-decoration:none; color:#777; font-size:10px; gap:3px; }
.bottomnav a.active { color:#3ddc84; }
.bottomnav a.disabled { color:#444; pointer-events:none; }
.bottomnav .bn-icon { font-size:18px; }
body { padding-bottom: 62px; }
)rawliteral";

const char MIZUMA_BOTTOMNAV_HTML[] PROGMEM = R"rawliteral(
<div class="bottomnav">
  <a href="/app" class="__ACTIVE_HOME__"><span class="bn-icon">&#127968;</span>Beranda</a>
  <a href="/led" class="__ACTIVE_LED__"><span class="bn-icon">&#128161;</span>LED</a>
  <a href="#" class="disabled"><span class="bn-icon">&#128267;</span>Aki</a>
  <a href="#" class="disabled"><span class="bn-icon">&#128736;</span>Servis</a>
  <a href="#" class="disabled"><span class="bn-icon">&#128274;</span>Anti-Theft</a>
  <a href="#" class="disabled"><span class="bn-icon">&#128205;</span>GPS</a>
  <a href="/settings/um" class=""><span class="bn-icon">&#127961;</span>Motor</a>
  <a href="/settings/wifi" class=""><span class="bn-icon">&#128246;</span>Jaringan</a>
</div>
)rawliteral";

const char MIZUMA_SHELL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Mizuma Smart System</title>
<style>
  * { box-sizing: border-box; }
  body { margin:0; font-family: -apple-system, Roboto, Arial, sans-serif; background:#111; color:#eee; }
  header { padding:16px; text-align:center; background:#1a1a1a; border-bottom: 1px solid #222; }
  header h1 { margin:0; font-size:20px; font-weight:600; color:#fff; }
  .container { padding: 16px; max-width: 500px; margin: 0 auto; }
  .mode-card { background:#1e1e1e; border-radius:12px; padding:16px; border:1px solid #2a2a2a; margin-bottom:16px; }
  .mode-header { display:flex; justify-content:space-between; align-items:center; gap:12px; }
  .mode-info { display:flex; align-items:center; gap:12px; }
  .mode-icon { font-size:26px; line-height:1; }
  .mode-label { font-size:11px; color:#aaa; text-transform:uppercase; letter-spacing:0.5px; }
  .mode-title { font-size:13px; font-weight:bold; color:#3ddc84; margin-top:2px; }
  .btn-switch { padding:8px 14px; background:#2a2a2a; color:#fff; border:1px solid #444; border-radius:8px; font-size:12px; font-weight:600; cursor:pointer; white-space:nowrap; }
  .switch-panel { margin-top:14px; padding-top:14px; border-top:1px solid #2a2a2a; }
  .target-title { font-size:13px; font-weight:bold; color:#fff; margin-bottom:8px; }
  .guide-box { font-size:12px; color:#ccc; background:#141414; padding:10px 12px; border-radius:8px; margin-bottom:12px; line-height:1.5; border-left:3px solid #007acc; }
  .status-row { display:flex; align-items:center; gap:8px; font-size:13px; margin-bottom:12px; color:#aaa; }
  .dot { width:10px; height:10px; border-radius:50%; background:#555; display:inline-block; flex-shrink:0; }
  .dot.on { background:#3ddc84; box-shadow: 0 0 8px #3ddc84; }
  .btn-action { display:block; width:100%; text-align:center; padding:10px; background:#2a5; color:#fff; border-radius:8px; text-decoration:none; font-size:13px; font-weight:bold; }
  .grid { display:grid; grid-template-columns: 1fr 1fr; gap:12px; }
  .card { background:#1e1e1e; border-radius:12px; padding:18px 12px; text-align:center; text-decoration:none; color:#eee; display:block; border:1px solid #252525; }
  .card .icon { font-size:26px; margin-bottom:6px; }
  .card .label { font-size:13px; }
  %BOTTOMNAV_CSS%
</style>
</head>
<body>

<header><h1>Mizuma Smart System</h1></header>

<div class="container">
  <div class="mode-card">
    <div class="mode-header">
      <div class="mode-info">
        <span class="mode-icon" id="modeIcon">📶</span>
        <div>
          <div class="mode-label">Mode Aktif:</div>
          <div class="mode-title" id="modeTitle">Memuat...</div>
        </div>
      </div>
      <button class="btn-switch" onclick="toggleSwitchPanel()">Ganti Mode</button>
    </div>
    <div class="switch-panel" id="switchPanel" style="display:none;">
      <div class="target-title" id="targetTitle">Beralih Mode</div>
      <div class="guide-box" id="guideBox">Memuat panduan...</div>
      <div class="status-row"><span class="dot" id="statusDot"></span><span id="statusText">Memeriksa koneksi...</span></div>
      <a class="btn-action" id="btnSwitchAction" style="display:none;" href="#">Beralih Mode Sekarang</a>
    </div>
  </div>

  <div class="grid">
    <a class="card" href="/led"><div class="icon">&#128161;</div><div class="label">LED Alis</div></a>
    <a class="card" href="#"><div class="icon">&#128267;</div><div class="label">Kondisi Aki</div></a>
    <a class="card" href="#"><div class="icon">&#128736;</div><div class="label">Reminder Servis</div></a>
    <a class="card" href="#"><div class="icon">&#128274;</div><div class="label">Anti-Theft</div></a>
    <a class="card" href="#"><div class="icon">&#128205;</div><div class="label">Lokasi GPS</div></a>
    <a class="card" href="/settings/um"><div class="icon">&#127961;</div><div class="label">Data Motor</div></a>
    <a class="card" href="/settings/wifi"><div class="icon">&#128246;</div><div class="label">Jaringan</div></a>
  </div>
</div>

%BOTTOMNAV_HTML%

<script>
const isAPMode = (window.location.hostname === '4.3.2.1');
const modeIcon = document.getElementById('modeIcon');
const modeTitle = document.getElementById('modeTitle');
const targetTitle = document.getElementById('targetTitle');
const guideBox = document.getElementById('guideBox');

if (isAPMode) {
  modeIcon.textContent = '📶';
  modeTitle.textContent = 'Mode Kontrol Only (Offline)';
  targetTitle.textContent = 'Pilihan: Mode Kontrol + Internet (Hotspot)';
  guideBox.innerHTML = '<b>Panduan:</b><br>1. Aktifkan Hotspot / Tethering di HP Anda.<br>2. Tunggu hingga sistem terhubung ke Hotspot HP.<br>3. Tombol beralih akan muncul otomatis saat terhubung.';
} else {
  modeIcon.textContent = '🌐';
  modeTitle.textContent = 'Mode Kontrol + Internet (Hotspot)';
  targetTitle.textContent = 'Pilihan: Mode Kontrol Only (Offline)';
  guideBox.innerHTML = '<b>Panduan:</b><br>1. Aktifkan Wi-Fi di HP Anda.<br>2. Sambungkan HP Anda ke Wi-Fi <b>Mizuma Smart System</b>.<br>3. Tombol beralih akan muncul otomatis saat terhubung.';
}

function toggleSwitchPanel() {
  const panel = document.getElementById('switchPanel');
  panel.style.display = (panel.style.display === 'none') ? 'block' : 'none';
}

async function refreshStatus() {
  try {
    const res = await fetch('/mizuma/status');
    const data = await res.json();
    const dot = document.getElementById('statusDot');
    const statusText = document.getElementById('statusText');
    const btnAction = document.getElementById('btnSwitchAction');
    if (isAPMode) {
      if (data.sta && data.staIP) {
        dot.className = 'dot on'; statusText.textContent = 'Sistem terhubung ke Hotspot HP!'; statusText.style.color = '#3ddc84';
        btnAction.href = 'http://' + data.staIP + '/app'; btnAction.textContent = 'Beralih ke Mode Kontrol + Internet (Hotspot)'; btnAction.style.display = 'block';
      } else {
        dot.className = 'dot'; statusText.textContent = 'Menunggu ESP32 terhubung ke Hotspot HP...'; statusText.style.color = '#aaa'; btnAction.style.display = 'none';
      }
    } else {
      if (data.ap) {
        dot.className = 'dot on'; statusText.textContent = 'HP Anda terhubung ke Wi-Fi Mizuma!'; statusText.style.color = '#3ddc84';
        btnAction.href = 'http://4.3.2.1/app'; btnAction.textContent = 'Beralih ke Mode Kontrol Only (Offline)'; btnAction.style.display = 'block';
      } else {
        dot.className = 'dot'; statusText.textContent = 'Menunggu HP terhubung ke Wi-Fi Mizuma...'; statusText.style.color = '#aaa'; btnAction.style.display = 'none';
      }
    }
  } catch (e) {}
}
refreshStatus();
setInterval(refreshStatus, 3000);
</script>
</body>
</html>
)rawliteral";

const char MIZUMA_LED_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>LED Alis - Mizuma</title>
<style>
  * { box-sizing: border-box; }
  body { margin:0; font-family: -apple-system, Roboto, Arial, sans-serif; background:#111; color:#eee; }

  .freeze { position: sticky; top: 0; z-index: 10; background:#111; border-bottom:1px solid #262626; }
  .backbar { display:flex; align-items:center; gap:10px; padding:10px 12px; background:#1a1a1a; }
  .backbar a { color:#aaa; text-decoration:none; font-size:14px; }

  .tabbar { display:flex; overflow-x:auto; background:#151515; }
  .tabbar button { flex:1 0 auto; padding:11px 10px; background:none; border:none; color:#999; font-size:12.5px; font-weight:600; white-space:nowrap; border-bottom:3px solid transparent; }
  .tabbar button.active { color:#fff; border-bottom-color:#3ddc84; }

  /* ==== Preview grid sesuai sketsa: 2 baris (kanan/kiri) + tombol sejajar + Kanan+Kiri menyamping ==== */
  .preview-grid { display:grid; grid-template-columns: 1fr 62px 78px; grid-template-rows: 34px 34px; gap:5px; padding:10px 12px; }
  .pv-bar { border-radius:6px; background:#000; display:flex; align-items:center; padding:0 3px; gap:1px; overflow:hidden; }
  .pv-bar .seg { flex:1; height:20px; background:#2a2a2a; border-radius:1px; }
  .pv-bar.kanan { grid-column:1; grid-row:1; }
  .pv-bar.kiri  { grid-column:1; grid-row:2; }
  .side-btn { grid-column:2; background:#1e1e1e; border:1px solid #333; color:#999; border-radius:6px; font-size:11px; font-weight:600; }
  .side-btn.kanan-btn { grid-row:1; }
  .side-btn.kiri-btn  { grid-row:2; }
  .side-btn.active { background:#2a5; color:#fff; border-color:#2a5; }
  .side-btn-both { grid-column:3; grid-row:1 / span 2; background:#1e1e1e; border:1px solid #333; color:#999; border-radius:6px; font-size:11px; font-weight:600; }
  .side-btn-both.active { background:#2a5; color:#fff; border-color:#2a5; }

  .body-layout { display:flex; min-height: 60vh; }
  .subnav { width:78px; flex-shrink:0; background:#161616; border-right:1px solid #232323; }
  .subnav button { display:block; width:100%; padding:16px 4px; background:none; border:none; color:#888; font-size:11px; text-align:center; border-left:3px solid transparent; }
  .subnav button.active { color:#fff; border-left-color:#3ddc84; background:#1e1e1e; }
  .subnav .icon { display:block; font-size:20px; margin-bottom:4px; }

  .content { flex:1; padding:16px; min-width:0; }
  .placeholder { color:#777; font-size:13px; line-height:1.6; background:#1a1a1a; border:1px dashed #333; border-radius:10px; padding:20px; text-align:center; margin-bottom:14px; }
  .dummy-block { background:#1a1a1a; border-radius:10px; padding:16px; margin-bottom:12px; color:#555; font-size:12px; text-align:center; }

  %BOTTOMNAV_CSS%
</style>
</head>
<body>

<div class="freeze">
  <div class="backbar"><a href="/app">&larr; Kembali ke Menu</a></div>
  <div class="tabbar" id="tabbar">
    <button data-tab="welcoming" class="active">Welcoming</button>
    <button data-tab="riding">Riding</button>
    <button data-tab="sein">Sein</button>
    <button data-tab="rem">Rem</button>
    <button data-tab="hazard">Hazard</button>
  </div>
  <div class="preview-grid" id="previewGrid">
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

  <div class="content" id="content">
    <div class="placeholder" id="placeholderText">
      Tab: <b>Welcoming</b> — Sisi: <b>Kanan</b> — Sub-tab: <b>Pola Warna</b><br><br>
      (Kontrol sebenarnya menyusul di Fase 6-8)
    </div>
    <!-- Konten dummy di bawah ini SEMENTARA, cuma untuk uji scroll freeze pane -->
    <div class="dummy-block">[placeholder isi kontrol — blok 1]</div>
    <div class="dummy-block">[placeholder isi kontrol — blok 2]</div>
    <div class="dummy-block">[placeholder isi kontrol — blok 3]</div>
    <div class="dummy-block">[placeholder isi kontrol — blok 4]</div>
    <div class="dummy-block">[placeholder isi kontrol — blok 5]</div>
    <div class="dummy-block">[placeholder isi kontrol — blok 6]</div>
    <div class="dummy-block">[placeholder isi kontrol — blok 7]</div>
    <div class="dummy-block">[placeholder isi kontrol — blok 8]</div>
  </div>
</div>

%BOTTOMNAV_HTML%

<script>
let activeTab = 'welcoming';
let activeSide = 'kanan';
let activeSub = 'warna';

function buildPreviewBar(elId) {
  const el = document.getElementById(elId);
  el.innerHTML = '';
  for (let i = 0; i < 48; i++) {
    const s = document.createElement('div');
    s.className = 'seg';
    el.appendChild(s);
  }
}
buildPreviewBar('pvKanan');
buildPreviewBar('pvKiri');

function updatePlaceholder() {
  const labels = { warna:'Pola Warna', efek:'Efek', simpan:'Simpan' };
  const sideLabels = { kanan:'Kanan', kiri:'Kiri', both:'Kanan + Kiri' };
  const tabLabels = { welcoming:'Welcoming', riding:'Riding', sein:'Sein', rem:'Rem', hazard:'Hazard' };
  document.getElementById('placeholderText').innerHTML =
    'Tab: <b>' + tabLabels[activeTab] + '</b> — Sisi: <b>' + sideLabels[activeSide] +
    '</b> — Sub-tab: <b>' + labels[activeSub] + '</b><br><br>(Kontrol sebenarnya menyusul di Fase 6-8)';
}

document.getElementById('tabbar').addEventListener('click', (e) => {
  if (e.target.tagName !== 'BUTTON') return;
  document.querySelectorAll('#tabbar button').forEach(b => b.classList.remove('active'));
  e.target.classList.add('active');
  activeTab = e.target.dataset.tab;
  updatePlaceholder();
});

document.getElementById('previewGrid').addEventListener('click', (e) => {
  const btn = e.target.closest('button');
  if (!btn) return;
  document.querySelectorAll('.side-btn, .side-btn-both').forEach(b => b.classList.remove('active'));
  btn.classList.add('active');
  activeSide = btn.dataset.side;
  updatePlaceholder();
});

document.getElementById('subnav').addEventListener('click', (e) => {
  const btn = e.target.closest('button');
  if (!btn) return;
  document.querySelectorAll('#subnav button').forEach(b => b.classList.remove('active'));
  btn.classList.add('active');
  activeSub = btn.dataset.sub;
  updatePlaceholder();
});
</script>
</body>
</html>
)rawliteral";

class MizumaSmartSystem : public Usermod {
  private:
    String vehicleName  = "";
    String vehicleBrand = "";
    uint16_t vehicleYear = 0;
    String vehiclePlate = "";

    struct ReminderItem {
      unsigned long lastServiceEpoch = 0;
      uint16_t intervalDays = 0;
    };
    ReminderItem oliMesin, oliRem, oliGardan, cvt, filter;

    uint8_t presetWelcomingKanan = 1, presetWelcomingKiri = 2;
    uint8_t presetRidingKanan    = 3, presetRidingKiri    = 4;
    uint8_t presetSeinKanan      = 5, presetSeinKiri      = 6;
    uint8_t presetRemKanan       = 7, presetRemKiri       = 8;
    uint8_t presetHazardKanan    = 9, presetHazardKiri    = 10;

    // Helper: sisipkan komponen bottom nav ke template halaman
    String renderPage(const char* pageTemplate, const char* activeHome, const char* activeLed) {
      String html = FPSTR(pageTemplate);
      String navHtml = FPSTR(MIZUMA_BOTTOMNAV_HTML);
      navHtml.replace("__ACTIVE_HOME__", activeHome);
      navHtml.replace("__ACTIVE_LED__", activeLed);
      html.replace("%BOTTOMNAV_CSS%", FPSTR(MIZUMA_BOTTOMNAV_CSS));
      html.replace("%BOTTOMNAV_HTML%", navHtml);
      return html;
    }

  public:
    void setup() override {
      apBehavior = AP_BEHAVIOR_ALWAYS;

      DEBUG_PRINTLN(F("[Mizuma] Usermod utama siap"));

      server.on("/app", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/html", renderPage(MIZUMA_SHELL_HTML, "active", ""));
      });

      server.on("/led", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/html", renderPage(MIZUMA_LED_HTML, "", "active"));
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
