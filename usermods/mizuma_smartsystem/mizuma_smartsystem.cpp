#include "wled.h"

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
  
  /* Mode Header Box */
  .mode-card { background:#1e1e1e; border-radius:12px; padding:16px; border:1px solid #2a2a2a; margin-bottom:16px; }
  .mode-header { display:flex; justify-content:space-between; align-items:center; gap:12px; }
  .mode-info { display:flex; align-items:center; gap:12px; }
  .mode-icon { font-size:26px; line-height:1; }
  .mode-label { font-size:11px; color:#aaa; text-transform:uppercase; letter-spacing:0.5px; }
  .mode-title { font-size:13px; font-weight:bold; color:#3ddc84; margin-top:2px; }
  
  .btn-switch { padding:8px 14px; background:#2a2a2a; color:#fff; border:1px solid #444; border-radius:8px; font-size:12px; font-weight:600; cursor:pointer; transition:0.2s; white-space:nowrap; }
  .btn-switch:hover { background:#333; }
  
  /* Switch Panel Dynamic */
  .switch-panel { margin-top:14px; padding-top:14px; border-top:1px solid #2a2a2a; }
  .target-title { font-size:13px; font-weight:bold; color:#fff; margin-bottom:8px; }
  .guide-box { font-size:12px; color:#ccc; background:#141414; padding:10px 12px; border-radius:8px; margin-bottom:12px; line-height:1.5; border-left:3px solid #007acc; }
  
  .status-row { display:flex; align-items:center; gap:8px; font-size:13px; margin-bottom:12px; color:#aaa; }
  .dot { width:10px; height:10px; border-radius:50%; background:#555; display:inline-block; transition: background 0.3s; flex-shrink:0; }
  .dot.on { background:#3ddc84; box-shadow: 0 0 8px #3ddc84; }
  
  .btn-action { display:block; width:100%; text-align:center; padding:10px; background:#2a5; color:#fff; border-radius:8px; text-decoration:none; font-size:13px; font-weight:bold; transition:0.2s; }
  .btn-action:hover { background:#3b6; }
  
  /* Navigation Grid */
  .grid { display:grid; grid-template-columns: 1fr 1fr; gap:12px; }
  .card { background:#1e1e1e; border-radius:12px; padding:18px 12px; text-align:center; text-decoration:none; color:#eee; display:block; border:1px solid #252525; transition:0.2s; }
  .card:hover { border-color:#444; background:#222; }
  .card .icon { font-size:26px; margin-bottom:6px; }
  .card .label { font-size:13px; }
</style>
</head>
<body>

<header>
  <h1>Mizuma Smart System</h1>
</header>

<div class="container">
  <!-- Dynamic Mode Header -->
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

    <!-- Panel Penggantian Mode (Hidden by Default) -->
    <div class="switch-panel" id="switchPanel" style="display:none;">
      <div class="target-title" id="targetTitle">Beralih Mode</div>
      <div class="guide-box" id="guideBox">Memuat panduan...</div>
      
      <div class="status-row">
        <span class="dot" id="statusDot"></span>
        <span id="statusText">Memeriksa koneksi...</span>
      </div>
      
      <a class="btn-action" id="btnSwitchAction" style="display:none;" href="#">Beralih Mode Sekarang</a>
    </div>
  </div>

  <!-- Menu Utama -->
  <div class="grid">
    <a class="card" href="#"><div class="icon">&#128161;</div><div class="label">LED Alis</div></a>
    <a class="card" href="#"><div class="icon">&#128267;</div><div class="label">Kondisi Aki</div></a>
    <a class="card" href="#"><div class="icon">&#128736;</div><div class="label">Reminder Servis</div></a>
    <a class="card" href="#"><div class="icon">&#128274;</div><div class="label">Anti-Theft</div></a>
    <a class="card" href="#"><div class="icon">&#128205;</div><div class="label">Lokasi GPS</div></a>
    <a class="card" href="/settings/um"><div class="icon">&#127961;</div><div class="label">Data Motor</div></a>
    <a class="card" href="/settings/wifi"><div class="icon">&#128246;</div><div class="label">Jaringan</div></a>
  </div>
</div>

<script>
const isAPMode = (window.location.hostname === '4.3.2.1');

// Deklarasi Elemen
const modeIcon = document.getElementById('modeIcon');
const modeTitle = document.getElementById('modeTitle');
const targetTitle = document.getElementById('targetTitle');
const guideBox = document.getElementById('guideBox');

// Atur Tampilan Awal Berdasarkan Mode Aktif Saat Ini
if (isAPMode) {
  // Mode Aktif: Offline
  modeIcon.textContent = '📶';
  modeTitle.textContent = 'Mode Kontrol Only (Offline)';
  
  // Persiapan Target saat Klik Ganti Mode
  targetTitle.textContent = 'Pilihan: Mode Kontrol + Internet (Hotspot)';
  guideBox.innerHTML = '<b>Panduan:</b><br>1. Aktifkan Hotspot / Tethering di HP Anda.<br>2. Tunggu hingga sistem terhubung ke Hotspot HP.<br>3. Tombol beralih akan muncul otomatis saat terhubung.';
} else {
  // Mode Aktif: Online/Internet
  modeIcon.textContent = '🌐';
  modeTitle.textContent = 'Mode Kontrol + Internet (Hotspot)';
  
  // Persiapan Target saat Klik Ganti Mode
  targetTitle.textContent = 'Pilihan: Mode Kontrol Only (Offline)';
  guideBox.innerHTML = '<b>Panduan:</b><br>1. Aktifkan Wi-Fi di HP Anda.<br>2. Sambungkan HP Anda ke Wi-Fi <b>Mizuma Smart System</b>.<br>3. Tombol beralih akan muncul otomatis saat terhubung.';
}

// Toggle buka/tutup panel ganti mode
function toggleSwitchPanel() {
  const panel = document.getElementById('switchPanel');
  panel.style.display = (panel.style.display === 'none') ? 'block' : 'none';
}

// Pengecekan Status Realtime via Async Endpoint
async function refreshStatus() {
  try {
    const res = await fetch('/mizuma/status');
    const data = await res.json();

    const dot = document.getElementById('statusDot');
    const statusText = document.getElementById('statusText');
    const btnAction = document.getElementById('btnSwitchAction');

    if (isAPMode) {
      // Jika saat ini Offline, periksa apakah ESP32 sudah terhubung ke Hotspot HP (STA)
      if (data.sta && data.staIP) {
        dot.className = 'dot on';
        statusText.textContent = 'Sistem terhubung ke Hotspot HP!';
        statusText.style.color = '#3ddc84';
        btnAction.href = 'http://' + data.staIP + '/app';
        btnAction.textContent = 'Beralih ke Mode Kontrol + Internet (Hotspot)';
        btnAction.style.display = 'block';
      } else {
        dot.className = 'dot';
        statusText.textContent = 'Menunggu ESP32 terhubung ke Hotspot HP...';
        statusText.style.color = '#aaa';
        btnAction.style.display = 'none';
      }
    } else {
      // Jika saat ini Online, periksa apakah HP sudah terhubung ke Wi-Fi SoftAP ESP32
      if (data.ap) {
        dot.className = 'dot on';
        statusText.textContent = 'HP Anda terhubung ke Wi-Fi Mizuma!';
        statusText.style.color = '#3ddc84';
        btnAction.href = 'http://4.3.2.1/app';
        btnAction.textContent = 'Beralih ke Mode Kontrol Only (Offline)';
        btnAction.style.display = 'block';
      } else {
        dot.className = 'dot';
        statusText.textContent = 'Menunggu HP terhubung ke Wi-Fi Mizuma...';
        statusText.style.color = '#aaa';
        btnAction.style.display = 'none';
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

  public:
    void setup() override {
      apBehavior = AP_BEHAVIOR_ALWAYS;  // paksa tiap boot, tidak perlu setting manual

      DEBUG_PRINTLN(F("[Mizuma] Usermod utama siap"));

      server.on("/app", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", MIZUMA_SHELL_HTML);
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

    void loop() override {
      // logic reminder/status ditambah di fase berikutnya
    }

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
