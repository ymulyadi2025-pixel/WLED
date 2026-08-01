#include "wled.h"
#include <NimBLEDevice.h>

// Nama BLE rahasia yang harus dibroadcast dari HP untuk trigger AP
// GANTI ini jadi unik per unit sebelum produksi massal
#define MIZUMA_UNLOCK_NAME "MZM-UNLOCK-7F3A9C"

class MizumaSmartSystem : public Usermod {
  private:
    // ===== Data kendaraan =====
    String vehicleName  = "";
    String vehicleBrand = "";
    uint16_t vehicleYear = 0;
    String vehiclePlate = "";

    // ===== Reminder servis: waktu terakhir ganti (epoch detik) + interval (hari) =====
    struct ReminderItem {
      unsigned long lastServiceEpoch = 0;
      uint16_t intervalDays = 0;
    };
    ReminderItem oliMesin, oliRem, oliGardan, cvt, filter;

    // ===== Mapping preset LED (10 slot: mode x sisi) =====
    uint8_t presetWelcomingKanan = 1, presetWelcomingKiri = 2;
    uint8_t presetRidingKanan    = 3, presetRidingKiri    = 4;
    uint8_t presetSeinKanan      = 5, presetSeinKiri      = 6;
    uint8_t presetRemKanan       = 7, presetRemKiri       = 8;
    uint8_t presetHazardKanan    = 9, presetHazardKiri    = 10;

    // ===== BLE trigger untuk paksa mode AP =====
    unsigned long lastScanAt = 0;
    unsigned long bootTime = 0;
    bool didInitialScan = false;
    const unsigned long INITIAL_SCAN_DELAY_MS = 10000;   // 10 detik setelah boot
    const unsigned long SCAN_INTERVAL_MS      = 300000;  // scan ulang tiap 5 menit
    const unsigned long SCAN_DURATION_MS      = 2000;    // durasi tiap scan

  public:
    void setup() override {
      bootTime = millis();
      NimBLEDevice::init("Mizuma");
      DEBUG_PRINTLN(F("[Mizuma] Usermod utama siap, BLE menunggu jendela scan pertama"));
    }

    void checkUnlockBeacon() {
      DEBUG_PRINTLN(F("[Mizuma] Mulai scan BLE..."));
      NimBLEScan* pScan = NimBLEDevice::getScan();
      pScan->setActiveScan(true);
      NimBLEScanResults results = pScan->getResults(SCAN_DURATION_MS, false);

      for (int i = 0; i < results.getCount(); i++) {
        const NimBLEAdvertisedDevice* dev = results.getDevice(i);
        if (dev->haveName() && dev->getName() == MIZUMA_UNLOCK_NAME) {
          DEBUG_PRINTLN(F("[Mizuma] Beacon unlock terdeteksi -> paksa mode AP"));
          WLED::instance().initAP(true);
          return;
        }
      }
      DEBUG_PRINTLN(F("[Mizuma] Tidak ada beacon unlock terdeteksi"));
    }

    void loop() override {
      unsigned long now = millis();

      if (!didInitialScan && (now - bootTime >= INITIAL_SCAN_DELAY_MS)) {
        didInitialScan = true;
        lastScanAt = now;
        checkUnlockBeacon();
      } else if (didInitialScan && (now - lastScanAt >= SCAN_INTERVAL_MS)) {
        lastScanAt = now;
        checkUnlockBeacon();
      }
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
