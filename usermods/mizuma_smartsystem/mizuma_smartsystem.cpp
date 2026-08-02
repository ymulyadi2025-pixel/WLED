#include "wled.h"

// ===== Shell UI utama (Fase 4) — halaman menu custom, bukan wizard bawaan WLED =====
const char MIZUMA_SHELL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Mizuma SmartSystem</title>
<style>
  body { margin:0; font-family: -apple-system, Roboto, Arial, sans-serif; background:#111; color:#eee; }
  header { padding:20px; text-align:center; background:#1a1a1a; }
  header h1 { margin:0; font-size:20px; }
  .grid { display:grid; grid-template-columns: 1fr 1fr; gap:12px; padding:16px; }
  .card { background:#1e1e1e; border-radius:12px; padding:20px 12px; text-align:center; text-decoration:none; color:#eee; display:block; }
  .card .icon { font-size:28px; margin-bottom:8px; }
  .card .label { font-size:14px; }
</style>
</head>
<body>
<header><h1>Mizuma SmartSystem</h1></header>
<div class="grid">
  <a class="card" href="#"><div class="icon">&#128161;</div><div class="label">LED Alis</div></a>
  <a class="card" href="#"><div class="icon">&#128267;</div><div class="label">Kondisi Aki</div></a>
  <a class="card" href="#"><div class="icon">&#128736;</div><div class="label">Reminder Servis</div></a>
  <a class="card" href="#"><div class="icon">&#128274;</div><div class="label">Anti-Theft</div></a>
  <a class="card" href="#"><div class="icon">&#128205;</div><div class="label">Lokasi GPS</div></a>
  <a class="card" href="/settings/um"><div class="icon">&#127961;</div><div class="label">Data Motor</div></a>
  <a class="card" href="/settings/wifi"><div class="icon">&#128246;</div><div class="label">Jaringan</div></a>
</div>
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
      DEBUG_PRINTLN(F("[Mizuma] Usermod utama siap"));

      server.on("/app", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", MIZUMA_SHELL_HTML);
      });
    }

    void loop() override {
      // logic reminder/status ditambah di fase-fase berikutnya
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
