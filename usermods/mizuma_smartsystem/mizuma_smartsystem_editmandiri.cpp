// =====================================================================================================================================
// MIZUMA SMARTSYSTEM — ENGINE (R6.3 dipisah: engine di file ini, UI di mizuma_ui.h)
// File ini HANYA berisi logika: show engine, slot preset, antrean perintah web->loop(), boot welcoming, config, dan API HTTP.
// Tampilan web (CSS/HTML/JS + halaman /led /app /pengaturan ...) ada di mizuma_ui.h dan dipasang lewat SATU baris: mizumaUiRegister().
// Build tanpa UI (API saja):  -D MZ_NO_UI=1
//
// KONTRAK API (stabil — UI mana pun cukup memakai ini). side = both | kanan | kiri
//   GET       /mizuma/status          -> {ap, sta, staIP, ssid}
//   GET       /mizuma/debug           -> {heap, minHeap, maxAlloc, setupMs, welcomeMs, nowMs, showFxResolved, maxShowMs}
//   GET       /mizuma/shows           -> ["nama show", ...]            GET /mizuma/show-durations -> [ms, ...]
//   GET|POST  /mizuma/show            ?index=N&side=S                  jalankan show (preview)
//   GET|POST  /mizuma/show/stop       ?side=S
//   GET|POST  /mizuma/show/palette    ?pal=0..255&side=S               pilih palette layer warna kanan/kiri
//   GET|POST  /mizuma/show/reverse    (GET: status)  ?enabled=1|0      arah global
//   GET|POST  /mizuma/apply           ?mode=welcoming|riding|sein|rem|hazard&side=S
//   GET|POST  /mizuma/preset          ?slot=<welcomingKanan|...|hazardKiri>&fx&pal&r&g&b&r1..b2&sx&ix&c1..c3&o1..o3&bri&isShow&showIndex
//   GET       /mizuma/presets         -> semua 10 slot
//   GET|POST  /mizuma/side-swap       (GET: status)  ?enabled=1|0
//   GET|POST  /mizuma/riding/mode     ?mode=manual|semi-auto|auto|playlist
//   GET|POST  /mizuma/riding/reverse  per-mode / per-side direction
//   GET|POST  /mizuma/riding/mirror   ?enabled=1|0                  mirror Semi-auto
//   POST      /mizuma/riding/context  ?active=1|0                   current UI owns direction controls
//   GET|POST  /mizuma/riding/colors   mode=manual|semi-auto|auto&side=kanan|kiri&palette=0|1&pal&r&g&b (warna tersimpan per mode)
//   GET /mizuma/riding/effects -> snapshot efek per mode; POST /mizuma/riding/effect?mode&side&fx&... menyimpan snapshot
//   POST /mizuma/comp/preview  mode&mirror&join&(kanan_|kiri_)show,idx,fx,sx,ix,c1..c3,o1..o3,rev,pm,pn,r,g,b -> pratinjau tanpa menyimpan (kembali otomatis 20 dtk tanpa sinyal)
//   POST /mizuma/comp/play     parameter sama; simpan atomik komposisi mode itu dan jadikan mode Riding aktif
//   POST /mizuma/comp/end      akhiri pratinjau, lampu kembali ke mode Riding tersimpan
//   GET|POST  /mizuma/bri             ?v=1..255                        brightness permanen
//   GET|POST  /mizuma/skins           metadata skin custom perangkat (maks 30, termasuk titik warna)
//   POST      /mizuma/skins/delete?id=<id>
//   GET|POST  /mizuma/colors          enam slot warna solid custom (disimpan di filesystem perangkat)
//   --- Playlist Riding (maks 4 playlist x 12 slot; berjalan di engine, tetap jalan saat HP terputus) ---
//   GET   /mizuma/playlists          -> {use, lists:[{n, c}]}           use = indeks playlist yang dipakai Riding (-1 = Single Effect)
//   GET   /mizuma/pl/list?i=N        -> {name, it:[{d(0.1dtk), mode, R:{..,rv}, L:{..,rv}}]}
//   POST  /mizuma/pl/save            body JSON {list,name,it:[{mode,R,L,...}]} simpan snapshot tiap slot
//   POST  /mizuma/pl/delete?list=N
//   POST  /mizuma/pl/use?list=N      N=-1 -> kembali ke snapshot Single Effect Manual (preset lama menjadi fallback)
//   POST  /mizuma/pl/play?list=N&slot=J&hold=0|1   putar / pratinjau satu slot
//   POST  /mizuma/pl/pause?on=1|0    jeda / lanjut        POST /mizuma/pl/stop   hentikan pratinjau, pulihkan Riding
//   GET   /mizuma/pl/state           -> {use, run, hold, list, slot, el}
//   Slot preset riding (ridingKanan/Kiri) kini boleh berisi show Mizuma (isShow=1&showIndex=N); show diulang terus.
// =====================================================================================================================================
#include "wled.h"
#include "mizuma_shows.h"     // 55 rangkaian animasi multi-stage Mizuma
#ifndef MZ_NO_UI
#include "mizuma_ui.h"        // tampilan web (terpisah dari engine)
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"  // antrean perintah web -> loop()
extern void serializeConfigToFS();

// WLED 16.0.1 defines these preset helpers in presets.cpp but does not
// publish their prototypes through wled.h. They have external linkage.
extern bool getPresetName(byte index, String& name);
extern void handlePresets();


// =====================================================================================================================================
// BLOK 6 — FINAL v3 (konsolidasi + FIX: brightness permanen via /mizuma/bri)
// =====================================================================================================================================
    #ifndef USERMOD_ID_MIZUMA_SYSTEM
    #define USERMOD_ID_MIZUMA_SYSTEM 0x9001
    #endif
    class MizumaSmartSystem : public Usermod {
    private:
    String vehicleName = "";
    String vehicleBrand = "";
    uint16_t vehicleYear = 0;
    String vehiclePlate = "";
    struct ReminderItem { unsigned long lastServiceEpoch = 0; uint16_t intervalDays = 0; };
    ReminderItem oliMesin, oliRem, oliGardan, cvt, filter;
    struct PresetSlot {
    bool valid = false;
    bool isShow = false;
    uint8_t showIndex = 0;
    uint8_t fx = 0; uint8_t pal = 0;
    uint8_t r = 255, g = 255, b = 255;
    uint8_t r1 = 0, g1 = 0, b1 = 0;
    uint8_t r2 = 0, g2 = 0, b2 = 0;
    uint8_t sx = 128, ix = 128;
    uint8_t c1 = 128, c2 = 128, c3 = 128;
    uint8_t o1 = 0, o2 = 0, o3 = 0;
    uint8_t bri = 180;
    };
    PresetSlot pslots[10];
    static const uint16_t NATIVE_WELCOME_FALLBACK_MS = 7000; // hanya untuk preset boot WLED native, bukan show Mizuma
    int bootStage = 0;
    uint32_t bootT = 0;
    uint32_t tSetupMs = 0, tWelcomeMs = 0;   // diagnostik waktu boot (lihat /mizuma/debug)
    uint8_t  showFxResolved = 0;              // jumlah nama efek WLED yang berhasil dipetakan
    uint8_t  showModeIds[MZ_FXN_COUNT];       // indeks aktual efek WLED, di-resolve saat boot
    struct ShowRunner { bool active=false; bool hasCurrentShow=false; bool logicalRight=true; uint8_t showIndex=0; uint8_t stageIndex=0; uint8_t currentStageIndex=255; uint8_t expectedMode=255; uint32_t startedAt=0; bool loop=false; bool reverseOverride=false; bool wholeStrip=false; };
    ShowRunner showRunner[2];                  // 0=Kanan, 1=Kiri (logis; dipetakan melalui sideSwap)
    bool bootDone = false;
    bool bootWelcomePending = false;
    uint32_t bootWelcomeAt = 0;
    bool usingNativeWelcomePreset = false;
    bool sideSwap = false; // true: pemetaan logis Kanan/Kiri ke segmen fisik ditukar
    bool showReverse = false; // arah gerak global semua tab; stage Mizuma tetap mempertahankan flip resep via XOR
    bool ridingReverse[3][2] = {}; // arah tambahan per mode Riding dan sisi logis (Kanan, Kiri)
    bool ridingMirror = false; // opsi mirror WLED pada segmen efek Riding
    enum RidingMode : uint8_t { RIDING_MANUAL=0, RIDING_SEMI_AUTO=1, RIDING_AUTO=2, RIDING_PLAYLIST=3 };
    static const uint8_t SEG_COLOR_RIGHT=0, SEG_COLOR_LEFT=1, SEG_FX_RIGHT=2, SEG_FX_LEFT=3, SEG_FX_WHOLE=4;
    struct RidingColor { bool palette=true; uint8_t pal=2; uint8_t r=255,g=255,b=255; };
    // Warna tersimpan terpisah per mode: [0]=Manual, [1]=Semi Auto, [2]=Auto.
    // Auto memakai satu warna bersama yang direplikasi ke kedua sisi.
    RidingColor ridingColors[3][2];
    struct RidingEffectConfig {
      bool valid=false; bool isShow=false; uint8_t showIndex=0; uint8_t fx=0;
      uint8_t sx=128,ix=128,c1=128,c2=128,c3=128,o1=0,o2=0,o3=0;
    };
    // Snapshot efek per mode: Manual [kanan,kiri], Semi Auto [shared], Auto [Signature shared].
    RidingEffectConfig ridingEffects[3][2];
    // Sambung kiri-kanan (khusus Semi Auto): satu warna/skin membentang di seluruh strip.
    bool ridingColorJoin = false;
    // Rentang indeks LED tiap bus, diisi ensureRidingSegments(); dipakai melebarkan dan memulihkan segmen warna.
    uint16_t ridingRightStart=0,ridingRightStop=0,ridingLeftStart=0,ridingLeftStop=0,ridingWholeStop=0;
    // ===== DRAFT / PRATINJAU COMPOSER =====
    // Pratinjau meminjam array Riding di RAM (tidak pernah disimpan). Cadangan draftBk* = data tersimpan.
    // Selama draftActive, penyimpanan config dan GET /mizuma/riding/* membaca cadangan, bukan draft.
    struct CompSide { uint8_t flags; /* bit0 show, bit1 reverse, bit2 palette */ uint8_t showIndex,fx,sx,ix,c1,c2,c3,o /* bit0..2 = o1..o3 */,pal,r,g,b; };
    static const uint32_t MZ_DRAFT_TIMEOUT_MS = 20000; // tanpa sinyal dari UI selama 20 dtk -> lampu kembali ke data tersimpan
    bool draftActive=false;
    volatile uint32_t draftBeatAt=0;
    RidingEffectConfig draftBkEffects[3][2];
    RidingColor draftBkColors[3][2];
    bool draftBkReverse[3][2]={};
    bool draftBkMirror=false;
    bool draftBkJoin=false;
    RidingMode ridingMode = RIDING_MANUAL;
    RidingMode savedRidingMode = RIDING_MANUAL;
    bool ridingUiActive = false;
    bool ridingSegmentsReady = false;

    /* ===== ANTREAN PERINTAH WEB -> loop() =====
       Handler HTTP berjalan di task async_tcp, sedangkan strip.service() berjalan di task loop.
       FreeRTOS dapat menyela strip.service() di tengah frame, sehingga handler TIDAK BOLEH mengubah Segment,
       showRunner[], atau memanggil deallocateData() (WLED: reset/dealokasi tidak boleh terjadi saat efek berjalan).
       Aturan: handler hanya memvalidasi argumen lalu menitipkan perintah; processCmdQueue() di loop() adalah
       satu-satunya tempat yang mengeksekusi perubahan. loop() berjalan sebelum strip.service() pada task yang sama. */
    enum MzCmdType : uint8_t { MZ_CMD_NONE=0, MZ_CMD_SHOW_START, MZ_CMD_SHOW_STOP, MZ_CMD_SHOW_PALETTE, MZ_CMD_DIRECTION, MZ_CMD_APPLY, MZ_CMD_SIDE_SWAP, MZ_CMD_PL_PLAY, MZ_CMD_PL_PAUSE, MZ_CMD_PL_USE, MZ_CMD_PL_STOP, MZ_CMD_RIDING_MODE, MZ_CMD_RIDING_COLOR, MZ_CMD_RIDING_EFFECT, MZ_CMD_RIDING_DIRECTION, MZ_CMD_RIDING_MIRROR, MZ_CMD_RIDING_CONTEXT, MZ_CMD_COMP_PREVIEW, MZ_CMD_COMP_PLAY, MZ_CMD_COMP_END };
    struct MzCmd { uint8_t type; uint8_t side; uint8_t value; uint8_t flag; uint8_t r,g,b; char mode[10]; uint8_t fx,showIndex,sx,ix,c1,c2,c3,o1,o2,o3; CompSide cs[2]; uint8_t mirror; uint8_t join; };
    static const uint8_t MZ_CMD_QUEUE_LEN = 12;
    QueueHandle_t cmdQueue = nullptr;
    static uint8_t mzSideCode(const String &s){ if(s=="both")return 0; if(s=="kanan")return 1; if(s=="kiri")return 2; return 255; }
    static const char* mzSideName(uint8_t c){ return c==1?"kanan":(c==2?"kiri":"both"); }
    bool enqueueCmd(const MzCmd &c){ return cmdQueue && xQueueSend(cmdQueue,&c,0)==pdTRUE; }
    void sendBusy(AsyncWebServerRequest *req){ req->send(503,"application/json","{\"ok\":false,\"error\":\"busy\"}"); }

    /* Brightness Mizuma disimpan untuk fallback Welcome dan kontrol manual.
       Jika memakai native WLED Boot Preset, brightness mengikuti isi preset WLED. */
    uint8_t savedBri = 180;
    bool cfgSavePending = false;
    unsigned long cfgSaveT = 0;
    void queueSave(){cfgSavePending=true;cfgSaveT=millis();}
    
    const char* slotKey(int i){
    switch(i){
    case 0: return "welcomingKanan"; case 1: return "welcomingKiri";
    case 2: return "ridingKanan";    case 3: return "ridingKiri";
    case 4: return "seinKanan";      case 5: return "seinKiri";
    case 6: return "remKanan";       case 7: return "remKiri";
    case 8: return "hazardKanan";    default: return "hazardKiri";}}
    int slotIdx(const String &k){for(int i=0;i<10;i++)if(k==slotKey(i))return i;return -1;}
    bool isRestrictedSlot(int i){return i>=4&&i<=9;}
    PresetSlot defaultSlot(int idx){
    PresetSlot p;p.valid=true;p.fx=0;p.pal=0;
    if(idx<=1){p.isShow=true;p.showIndex=0;p.r=255;p.g=165;p.b=0;p.r1=255;p.g1=255;p.b1=255;p.r2=255;p.g2=26;p.b2=0;}
    else {p.r1=0;p.g1=0;p.b1=0;p.r2=0;p.g2=0;p.b2=0;
      if(idx==4||idx==5||idx==8||idx==9){p.r=255;p.g=165;p.b=0;}
      else if(idx==6||idx==7){p.r=255;p.g=0;p.b=0;}
      else{p.r=255;p.g=255;p.b=255;}}
    p.sx=128;p.ix=128;p.c1=128;p.c2=128;p.c3=128;p.o1=0;p.o2=0;p.o3=0;p.bri=180;
    return p;}
    void normalizeRestrictedSlot(PresetSlot &p,int idx){
    if(!isRestrictedSlot(idx))return;
    p.pal=0;p.r1=0;p.g1=0;p.b1=0;p.r2=0;p.g2=0;p.b2=0;
    if(p.r==0&&p.g==0&&p.b==0){if(idx==6||idx==7){p.r=255;p.g=0;p.b=0;}else{p.r=255;p.g=165;p.b=0;}}}
    PresetSlot getEffectiveSlot(int idx){
    if(idx<0||idx>9)idx=2;
    PresetSlot p=pslots[idx].valid?pslots[idx]:defaultSlot(idx);
    normalizeRestrictedSlot(p,idx);
    return p;}
    uint8_t argU8(AsyncWebServerRequest *req,const char *key,uint8_t def){
    if(!req->hasArg(key))return def;
    return (uint8_t)constrain(req->arg(key).toInt(),0,255);}
    
    void addSegmentFromSlot(JsonArray seg,uint8_t segId,uint8_t slotIdx,bool useRidingColor=false){
    PresetSlot p=getEffectiveSlot(slotIdx);
    const bool logicalRight=(slotIdx%2)==0;
        const RidingColor &ridingColor=ridingColorFor(ridingMode,logicalRight);
    JsonObject color=seg.createNestedObject();
    color["id"]=physicalColorSegForLogicalSide(logicalRight);color["on"]=true;
        color["fx"]=FX_MODE_PALETTE;color["pal"]=useRidingColor?(ridingColor.palette?ridingColor.pal:2):p.pal;color["sx"]=211;color["ix"]=77;
    color["c1"]=77;color["c2"]=0;color["c3"]=0;color["o1"]=0;color["o2"]=0;color["o3"]=0;
    JsonArray colorCol=color.createNestedArray("col");
        JsonArray cc0=colorCol.createNestedArray();cc0.add(useRidingColor?ridingColor.r:p.r);cc0.add(useRidingColor?ridingColor.g:p.g);cc0.add(useRidingColor?ridingColor.b:p.b);
        JsonArray cc1=colorCol.createNestedArray();cc1.add(useRidingColor?0:p.r1);cc1.add(useRidingColor?0:p.g1);cc1.add(useRidingColor?0:p.b1);
        JsonArray cc2=colorCol.createNestedArray();cc2.add(useRidingColor?0:p.r2);cc2.add(useRidingColor?0:p.g2);cc2.add(useRidingColor?0:p.b2);

    JsonObject effect=seg.createNestedObject();
    effect["id"]=segId;effect["on"]=true;effect["fx"]=p.fx;effect["pal"]=0;
    effect["rev"]=showReverse;effect["sx"]=p.sx;effect["ix"]=p.ix;
    effect["c1"]=p.c1;effect["c2"]=p.c2;effect["c3"]=p.c3;
    effect["o1"]=p.o1;effect["o2"]=p.o2;effect["o3"]=p.o3;
    JsonArray effectCol=effect.createNestedArray("col");
    for(uint8_t i=0;i<3;i++){JsonArray c=effectCol.createNestedArray();const uint8_t v=i==0?255:0;c.add(v);c.add(v);c.add(v);}
}
uint8_t physicalColorSegForLogicalSide(bool rightSide){return (rightSide != sideSwap)?SEG_COLOR_RIGHT:SEG_COLOR_LEFT;}
uint8_t physicalSegForLogicalSide(bool rightSide){return (rightSide != sideSwap)?SEG_FX_RIGHT:SEG_FX_LEFT;}
// Sambung kiri-kanan aktif hanya saat mode Semi Auto sedang dirender (bukan playlist, bukan mode lain).
bool ridingJoinActive(){return ridingColorJoin&&ridingMode==RIDING_SEMI_AUTO;}
// Bus GPIO 0 = segmen warna "kanan" (id 0), bus GPIO 1 = segmen warna "kiri" (id 1). Jika bus GPIO 1 dikonfigurasi reversed
// di pengaturan LED dan GPIO 0 berindeks lebih dulu, seluruh strip menjadi satu garis kontinu: ujung luar GPIO 0 -> tengah -> ujung luar GPIO 1.
// Arah alami (indeks naik) = dari sisi GPIO 0 ke sisi GPIO 1. Agar palet terbaca kiri ke kanan, balik bila GPIO 0 berindeks lebih dulu, dan balik lagi jika sideSwap aktif.
bool ridingJoinReverse(){return (ridingRightStart<ridingLeftStart)!=sideSwap;}
// Geometri dan arah yang seharusnya untuk segmen warna fisik segId pada keadaan sekarang. joinOff=true: segmen harus mati.
void colorSegTarget(uint8_t segId,bool &joinOff,uint16_t &start,uint16_t &stop,bool &reverse){
  joinOff=false;reverse=false;
  if(ridingJoinActive()){
    if(segId==SEG_COLOR_RIGHT){start=0;stop=ridingWholeStop;reverse=ridingJoinReverse();}
    else{joinOff=true;start=ridingLeftStart;stop=ridingLeftStop;}
    return;
  }
  if(segId==SEG_COLOR_RIGHT){start=ridingRightStart;stop=ridingRightStop;}
  else{start=ridingLeftStart;stop=ridingLeftStop;}
}
void applyRidingColorLayer(bool rightSide){
  if(!ridingSegmentsReady)return;
  const uint8_t segId=physicalColorSegForLogicalSide(rightSide);
  Segment &seg=strip.getSegment(segId);
  bool joinOff=false,wantReverse=false;uint16_t wantStart=0,wantStop=0;
  colorSegTarget(segId,joinOff,wantStart,wantStop,wantReverse);
  if(seg.start!=wantStart||seg.stop!=wantStop){seg.setGeometry(wantStart,wantStop);seg.markForReset();}
  if(seg.reverse!=wantReverse){seg.setOption(1,wantReverse);seg.markForReset();}
  if(joinOff){seg.on=false;return;}
  // Sambung aktif: satu warna bersama (slot kanan Semi Auto) untuk seluruh strip.
  const RidingColor &color=ridingJoinActive()?ridingColors[1][0]:ridingColorFor(ridingMode,rightSide);
  seg.setMode(FX_MODE_PALETTE,true);
  seg.setPalette(color.palette?color.pal:2);
  seg.blendMode=0;
  seg.speed=211;seg.intensity=77;seg.custom1=77;seg.custom2=0;seg.custom3=0;
  seg.check1=false;seg.check2=false;seg.check3=false;
  seg.setColor(0,RGBW32(color.r,color.g,color.b,0));
  seg.setColor(1,0);seg.setColor(2,0);seg.on=true;
}
void enforceRidingColorLayers(){
  if(!ridingSegmentsReady)return;
  bool changed=false;
  for(uint8_t side=0;side<2;side++){
    const bool rightSide=side==0;
    const uint8_t segId=physicalColorSegForLogicalSide(rightSide);
    Segment &seg=strip.getSegment(segId);
    bool joinOff=false,wantReverse=false;uint16_t wantStart=0,wantStop=0;
    colorSegTarget(segId,joinOff,wantStart,wantStop,wantReverse);
    if(joinOff){if(seg.on){seg.on=false;changed=true;}continue;}
    if(seg.start!=wantStart||seg.stop!=wantStop||seg.reverse!=wantReverse){
      applyRidingColorLayer(rightSide);
      changed=true;
      continue;
    }
    if(seg.check1||seg.check2||seg.check3){
      applyRidingColorLayer(rightSide);
      changed=true;
      continue;
    }
    if(seg.mode!=FX_MODE_PALETTE){seg.setMode(FX_MODE_PALETTE,false);changed=true;}
    if(seg.blendMode!=0){seg.blendMode=0;changed=true;}
    if(seg.speed!=211){seg.speed=211;changed=true;}
    if(seg.intensity!=77){seg.intensity=77;changed=true;}
    if(seg.custom1!=77){seg.custom1=77;changed=true;}
    if(seg.custom2!=0){seg.custom2=0;changed=true;}
    if(seg.custom3!=0){seg.custom3=0;changed=true;}
    if(!seg.on){seg.on=true;changed=true;}
  }
  if(changed)strip.trigger();
}
void enforceRidingEffectLayers(){
  if(!ridingSegmentsReady)return;
  bool changed=false;
  for(uint8_t id=SEG_FX_RIGHT;id<=SEG_FX_WHOLE;id++){
    Segment &seg=strip.getSegment(id);
    if(seg.blendMode!=6){seg.blendMode=6;changed=true;}
    if(seg.palette!=0){seg.setPalette(0);changed=true;}
    for(uint8_t color=0;color<3;color++){
      const uint32_t target=color==0?WHITE:0;
      if(seg.colors[color]!=target){seg.setColor(color,target);changed=true;}
    }
  }
  if(changed)strip.trigger();
}
bool busRangeForPin(uint8_t pin,uint16_t &start,uint16_t &stop){
  bool found=false;
  for(size_t i=0;i<BusManager::getNumBusses();i++){
    const Bus *bus=BusManager::getBus(i);
    if(!bus||!bus->isOk()||!bus->isDigital())continue;
    uint8_t pins[OUTPUT_MAX_PINS]={255,255,255,255,255};
    const size_t count=bus->getPins(pins);
    bool matches=false;
    for(size_t j=0;j<count&&j<OUTPUT_MAX_PINS;j++)if(pins[j]==pin)matches=true;
    if(!matches)continue;
    if(!bus->hasRGB())return false;
    if(found)return false;
    const uint32_t end=(uint32_t)bus->getStart()+bus->getLength();
    if(bus->getLength()==0||end>strip.getLengthTotal())return false;
    start=bus->getStart();stop=(uint16_t)end;found=true;
  }
  return found&&stop>start;
}
bool ensureRidingSegments(){
  if(ridingSegmentsReady)return true;
  uint16_t rightStart=0,rightStop=0,leftStart=0,leftStop=0;
  if(!busRangeForPin(0,rightStart,rightStop)||!busRangeForPin(1,leftStart,leftStop))return false;
  if(rightStart<leftStop&&leftStart<rightStop)return false;
  const uint16_t wholeStop=strip.getLengthTotal();
  if(wholeStop==0||wholeStop>65535)return false;
  ridingRightStart=rightStart;ridingRightStop=rightStop;ridingLeftStart=leftStart;ridingLeftStop=leftStop;ridingWholeStop=wholeStop;

  strip.resetSegments();
  strip.getSegment(0).setGeometry(rightStart,rightStop);
  strip.appendSegment(leftStart,leftStop);
  strip.appendSegment(rightStart,rightStop);
  strip.appendSegment(leftStart,leftStop);
  strip.appendSegment(0,wholeStop);
  if(strip.getSegmentsNum()!=5)return false;
  for(uint8_t id=0;id<5;id++)strip.getSegment(id).refreshLightCapabilities();

  Segment &rightColor=strip.getSegment(SEG_COLOR_RIGHT);
  Segment &leftColor=strip.getSegment(SEG_COLOR_LEFT);
  Segment &rightFx=strip.getSegment(SEG_FX_RIGHT);
  Segment &leftFx=strip.getSegment(SEG_FX_LEFT);
  Segment &wholeFx=strip.getSegment(SEG_FX_WHOLE);
  rightColor.setName("warna kanan");leftColor.setName("warna kiri");
  rightFx.setName("efek kanan");leftFx.setName("efek kiri");wholeFx.setName("efek sambung");
  for(Segment *seg:{&rightColor,&leftColor}){
    seg->setMode(FX_MODE_PALETTE,true);seg->setPalette(2);seg->blendMode=0;
    seg->speed=211;seg->intensity=77;seg->custom1=77;seg->custom2=0;seg->custom3=0;
    seg->check1=false;seg->check2=false;seg->check3=false;
  }
  for(Segment *seg:{&rightFx,&leftFx,&wholeFx}){
    seg->setColor(0,WHITE);seg->setColor(1,0);seg->setColor(2,0);
    seg->setMode(FX_MODE_STATIC,true);seg->setPalette(0);seg->blendMode=6;
  }
  strip.setMainSegmentId(SEG_COLOR_RIGHT);
  ridingSegmentsReady=true;
  applyRidingColorLayer(true);applyRidingColorLayer(false);
  return true;
}
bool ridingSegmentMirror(RidingMode mode,uint8_t id){
  // Mirror hanya berlaku pada efek linked Semi Auto. Untuk slot playlist, gunakan
  // snapshot slot (bit 0 PlSlot::pad), bukan setelan Semi Auto yang sedang aktif.
  if(mode!=RIDING_SEMI_AUTO||id!=SEG_FX_WHOLE)return false;
  if(plRun.active&&plRun.list<PL_MAX&&plRun.slot<pls[plRun.list].n)
    return (pls[plRun.list].it[plRun.slot].pad&0x01U)!=0;
  return ridingMirror;
}
void applyRidingMirrorOptions(RidingMode mode){
  bool changed=false;
  for(uint8_t id=SEG_FX_RIGHT;id<=SEG_FX_WHOLE;id++){
    Segment &effect=strip.getSegment(id);
    const bool mirror=ridingSegmentMirror(mode,id);
    // WLED's MIRROR segment option is option bit 3; color-only segments are excluded.
    if(effect.getOption(3)!=mirror){effect.setOption(3,mirror);effect.markForReset();changed=true;}
  }
  if(changed){strip.trigger();stateUpdated(CALL_MODE_DIRECT_CHANGE);}
}
void applyRidingLayerMode(RidingMode mode){
  if(!ensureRidingSegments())return;
  ridingMode=mode;
  applyRidingColorLayer(true);applyRidingColorLayer(false);
  for(uint8_t id=SEG_FX_RIGHT;id<=SEG_FX_WHOLE;id++){
    Segment &effect=strip.getSegment(id);effect.blendMode=6;effect.setPalette(0);
    effect.setColor(0,WHITE);effect.setColor(1,0);effect.setColor(2,0);
  }
  strip.getSegment(SEG_COLOR_RIGHT).on=true;
  strip.getSegment(SEG_COLOR_LEFT).on=true;
  const bool linkedEffect=mode==RIDING_SEMI_AUTO;
  strip.getSegment(SEG_FX_RIGHT).on=!linkedEffect;
  strip.getSegment(SEG_FX_LEFT).on=!linkedEffect;
  strip.getSegment(SEG_FX_WHOLE).on=linkedEffect;
  applyRidingMirrorOptions(mode);
  strip.trigger();
}
static uint8_t ridingModeIndex(RidingMode mode){
  return mode==RIDING_SEMI_AUTO?1:(mode==RIDING_AUTO?2:0);
}
RidingColor &ridingColorFor(RidingMode mode,bool logicalRight){
  uint8_t mi=(mode==RIDING_PLAYLIST)?ridingModeIndex(savedRidingMode):ridingModeIndex(mode);
  if(mi>2)mi=0;
  return ridingColors[mi][mi==2?0:(logicalRight?0:1)];
}
const RidingColor &ridingColorFor(RidingMode mode,bool logicalRight) const {
  uint8_t mi=(mode==RIDING_PLAYLIST)?ridingModeIndex(savedRidingMode):ridingModeIndex(mode);
  if(mi>2)mi=0;
  return ridingColors[mi][mi==2?0:(logicalRight?0:1)];
}
bool ridingSideReverse(RidingMode mode,bool logicalRight){
  // Auto memakai arah internal Signature Effect; nilai reverse warisan tidak boleh mengubahnya.
  if(mode==RIDING_PLAYLIST||mode==RIDING_AUTO)return false;
  const uint8_t modeIdx=ridingModeIndex(mode);
  return ridingReverse[modeIdx][logicalRight?0:1];
}
bool ridingWholeReverse(){
  // Fold any previously saved per-side settings into the single linked-segment direction.
  return ridingReverse[1][0]^ridingReverse[1][1];
}
void execRidingColor(uint8_t side,bool palette,uint8_t pal,uint8_t r,uint8_t g,uint8_t b,uint8_t modeIdx){
  if(side<1||side>2||modeIdx>2)return;
  const bool rightSide=side==1;
  RidingColor &color=ridingColors[modeIdx][modeIdx==2?0:(rightSide?0:1)];
  color.palette=palette;color.pal=palette?pal:2;color.r=r;color.g=g;color.b=b;
  // Auto adalah satu konfigurasi warna bersama, bukan dua warna sisi.
  if(modeIdx==2)ridingColors[2][1]=ridingColors[2][0];
  const uint8_t activeIdx=ridingMode==RIDING_PLAYLIST?ridingModeIndex(savedRidingMode):ridingModeIndex(ridingMode);
  if(!plRun.active&&activeIdx==modeIdx){applyRidingColorLayer(true);applyRidingColorLayer(false);strip.trigger();stateUpdated(CALL_MODE_DIRECT_CHANGE);}
  queueSave();
}
void applySingleSlot(uint8_t slotIdx,bool useRidingColor=false){
bool rightSide=(slotIdx%2)==0;uint8_t segId=physicalSegForLogicalSide(rightSide);
StaticJsonDocument<1024> doc;JsonObject root=doc.to<JsonObject>();
root["on"]=true;if(bri==0)root["bri"]=savedBri;
JsonArray seg=root.createNestedArray("seg");
addSegmentFromSlot(seg,segId,slotIdx,useRidingColor);
deserializeState(root);}
void applyTwoSlots(uint8_t rightSlot,uint8_t leftSlot,uint16_t transition100ms=0,bool useRidingColor=false){
StaticJsonDocument<2048> doc;JsonObject root=doc.to<JsonObject>();
root["on"]=true;root["transition"]=transition100ms;if(bri==0)root["bri"]=savedBri;
JsonArray seg=root.createNestedArray("seg");
addSegmentFromSlot(seg,physicalSegForLogicalSide(true),rightSlot,useRidingColor);
addSegmentFromSlot(seg,physicalSegForLogicalSide(false),leftSlot,useRidingColor);
deserializeState(root);}
void applyWholeSlot(uint8_t slotIdx,uint16_t transition100ms=0){
  PresetSlot p=getEffectiveSlot(slotIdx);
  StaticJsonDocument<1024> doc;JsonObject root=doc.to<JsonObject>();
  root["on"]=true;root["transition"]=transition100ms;if(bri==0)root["bri"]=savedBri;
  JsonArray seg=root.createNestedArray("seg");JsonObject effect=seg.createNestedObject();
  effect["id"]=SEG_FX_WHOLE;effect["on"]=true;effect["fx"]=p.fx;effect["pal"]=0;
  effect["rev"]=ridingWholeReverse();effect["sx"]=p.sx;effect["ix"]=p.ix;
  effect["c1"]=p.c1;effect["c2"]=p.c2;effect["c3"]=p.c3;
  effect["o1"]=p.o1;effect["o2"]=p.o2;effect["o3"]=p.o3;
  JsonArray colors=effect.createNestedArray("col");
  for(uint8_t i=0;i<3;i++){JsonArray color=colors.createNestedArray();const uint8_t value=i==0?255:0;color.add(value);color.add(value);color.add(value);}
  deserializeState(root);
}
int modeBase(const String &mode){
if(mode=="welcoming")return 0;if(mode=="riding")return 2;if(mode=="sein")return 4;
if(mode=="rem")return 6;if(mode=="hazard")return 8;return -1;}
bool applyModeSide(const String &mode,const String &side,uint16_t transition100ms=0){
stopPlaylist();
if(mode=="welcoming"){ridingUiActive=false;applyRidingLayerMode(RIDING_MANUAL);return applyWelcomeModeSide(side);}
stopShowRunners("both");
int base=modeBase(mode);if(base<0)return false;
if(side!="both"&&side!="kanan"&&side!="kiri")return false;
ridingUiActive=(base==2);
if(base==2&&ridingMode==RIDING_PLAYLIST&&ridingPl>=0&&ridingPl<PL_MAX&&pls[ridingPl].n>0)return startPlaylist(ridingPl,0,false);
if(base==2&&ridingMode==RIDING_PLAYLIST){ridingMode=RIDING_MANUAL;savedRidingMode=RIDING_MANUAL;queueSave();}
applyRidingLayerMode(base==2?ridingMode:RIDING_MANUAL);
if(base==2&&ridingMode==RIDING_SEMI_AUTO){
  const uint8_t slot=pslots[base].valid?base:base+1;
  applyWholeSlot(slot,transition100ms);
  PresetSlot p=getEffectiveSlot(slot);
  if(p.isShow&&p.showIndex<MZ_SHOW_COUNT){startShowRunner(true,p.showIndex,false,true);showRunner[0].loop=true;tickShowEngine();}
  return true;
}
if(side=="both")applyTwoSlots(base,base+1,transition100ms,base==2);
else if(side=="kanan")applySingleSlot(base,base==2);
else applySingleSlot(base+1,base==2);
if(base==2)startSlotShows(side=="both"||side=="kanan",side=="both"||side=="kiri");
return true;}
// Slot Riding boleh berisi show Mizuma: jalankan dan ulang terus (loop).
void startSlotShows(bool useR,bool useL){
for(uint8_t k=0;k<2;k++){
if(k==0&&!useR)continue;if(k==1&&!useL)continue;
PresetSlot p=getEffectiveSlot(2+k);
if(p.isShow&&p.showIndex<MZ_SHOW_COUNT){startShowRunner(k==0,p.showIndex);showRunner[k].loop=true;}}
tickShowEngine();}

// ---------------- MIZUMA SHOW ENGINE (WLED 16.0.1) ----------------
char lowerAscii(char c){return (c>='A'&&c<='Z')?(char)(c+('a'-'A')):c;}
int findWledModeByNameExact(const char *wanted){
  if(!wanted||!*wanted)return -1;
  const size_t wantedLen=strlen(wanted);
  for(uint16_t id=0;id<strip.getModeCount();id++){
    const char *meta=strip.getModeData(id);
    if(!meta)continue;
    size_t j=0;
    for(;j<wantedLen;j++){
      char m=(char)pgm_read_byte(meta+j);
      if(m=='\0'||m=='@'||lowerAscii(m)!=lowerAscii(wanted[j]))break;
    }
    if(j==wantedLen){char tail=(char)pgm_read_byte(meta+j);if(tail=='@'||tail=='\0')return (int)id;}
  }
  return -1;
}
int findWledModeByName(const char *wanted){
  int id=findWledModeByNameExact(wanted);
  if(id>=0||!wanted)return id;
  // WLED can replace the legacy effect with its Particle-System equivalent
  // when PARTICLESYSTEM1D is enabled. Keep show recipe labels stable.
  if(strcmp(wanted,"Fireworks 1D")==0)return findWledModeByNameExact("PS Fireworks 1D");
  if(strcmp(wanted,"PS Starburst")==0)return findWledModeByNameExact("Starburst");
  if(strcmp(wanted,"Fire 2012")==0)return findWledModeByNameExact("PS Fire 1D");
  return -1;
}
uint8_t resolveShowEffects(){
  uint8_t found=0;
  for(uint8_t i=0;i<MZ_FXN_COUNT;i++){
    int id=findWledModeByName(MZ_FXN[i]);
    showModeIds[i]=(id>=0&&id<255)?(uint8_t)id:255;
    if(id>=0){found++;}
    else{DEBUG_PRINTF_P(PSTR("[Mizuma] Efek show tidak ditemukan di WLED: %s\n"),MZ_FXN[i]);}
  }
  return found;
}
// ISOLASI TRIK INTERNAL WLED — Tetrix tanpa jeda fade-out 2 detik.
// Bergantung pada detail internal efek mode_tetrix di WLED 16.0.1 (field Segment: data, reset, step, call, aux0, aux1)
// dan TIDAK memakai API publik, sehingga update WLED dapat mengubah perilakunya tanpa error kompilasi.
// Hanya aman dipanggil dari loop() (task yang sama dengan strip.service()); semua pemanggilnya lewat tickShowEngine().
// Setelah setiap update WLED: uji visual show yang memakai flag 2 (Tetrix/TBJ). Bila perilakunya berubah,
// matikan tanpa mengubah kode lain dengan build flag:  -D MZ_TETRIX_INSTANT_START=0  (Tetrix kembali memakai fade-out bawaan WLED).
#ifndef MZ_TETRIX_INSTANT_START
#define MZ_TETRIX_INSTANT_START 1
#endif
void mzTetrixInstantStart(Segment &seg){
#if MZ_TETRIX_INSTANT_START
  seg.deallocateData();seg.reset=false;seg.step=0;seg.call=1;seg.aux0=0;seg.aux1=0;
  seg.fill(seg.colors[1]);
#else
  (void)seg;
#endif
}

void applyShowStage(const MzStage &st,bool logicalRight,bool wholeStrip=false){
  if(st.fx>=MZ_FXN_COUNT)return;
  const uint8_t modeId=showModeIds[st.fx];
  if(modeId==255)return; // FX tidak ada pada build WLED ini: lewati tahap secara aman
  const uint8_t segId=wholeStrip?SEG_FX_WHOLE:physicalSegForLogicalSide(logicalRight);
  if(segId>=strip.getSegmentsNum())return;
  Segment &seg=strip.getSegment(segId);
  if(!seg.isActive())return;

  const uint16_t previousTransition=strip.getTransition();
  const uint16_t stageTransition=(uint16_t)st.tt*10U; // tt disimpan dalam satuan 10 ms
  strip.setTransition(stageTransition);

  if(seg.mode!=modeId)seg.setMode(modeId,true); // muat default parameter efek WLED
  else if(st.t==0)seg.markForReset();
  seg.blendMode=6;seg.setPalette(0);
  seg.setColor(0,WHITE);seg.setColor(1,0);seg.setColor(2,0);
  const bool mirror=ridingUiActive&&ridingSegmentMirror(ridingMode,segId);
  if(seg.getOption(3)!=mirror){seg.setOption(3,mirror);seg.markForReset();}
  if(st.sx>=0)seg.speed=(uint8_t)constrain(st.sx,0,255);
  if(st.ix>=0)seg.intensity=(uint8_t)constrain(st.ix,0,255);
  if(st.c1>=0)seg.custom1=(uint8_t)constrain(st.c1,0,255);
  if(st.c2>=0)seg.custom2=(uint8_t)constrain(st.c2,0,255);
  if(st.c3>=0)seg.custom3=(uint8_t)constrain(st.c3,0,31); // WLED stores Custom 3 in 5 bits
  if(st.o>=0)seg.check1=(st.o!=0);
  const bool globalReverse=ridingUiActive?(ridingMode==RIDING_PLAYLIST&&showReverse):showReverse;
  const bool sideReverse=ridingUiActive
    ?((wholeStrip&&ridingMode==RIDING_SEMI_AUTO)?ridingWholeReverse():ridingSideReverse(ridingMode,logicalRight))
    :false;
  const bool stageReverse=(((st.flags&1U)!=0) ^ globalReverse ^ showRunner[logicalRight?0:1].reverseOverride ^ sideReverse);
  // WLED menyimpan reverse sebagai option bit 1 (REVERSE = 0x0002).
  // Gunakan setter resmi agar stateChanged WLED ikut diperbarui, lalu reset runtime FX.
  if(seg.reverse!=stageReverse){
    seg.setOption(1,stageReverse);
    seg.markForReset();
  }
  seg.on=true;
  showRunner[logicalRight?0:1].expectedMode=modeId; // dipakai untuk mendeteksi perubahan efek dari luar (UI WLED / preset)

  if(st.flags&2U)mzTetrixInstantStart(seg); // lihat catatan isolasi di atas applyShowStage()
  if(st.tb>=0)strip.timebase=(unsigned long)(uint16_t)st.tb-(unsigned long)millis();
  strip.trigger();
  stateUpdated(CALL_MODE_DIRECT_CHANGE);
  strip.setTransition(previousTransition); // restore WLED's global/default transition time
}

void startShowRunner(bool logicalRight,uint8_t showIndex,bool reverseOverride=false,bool wholeStrip=false){
  if(showIndex>=MZ_SHOW_COUNT)return;
  ShowRunner &r=showRunner[logicalRight?0:1];
  r.loop=false;r.active=true;r.hasCurrentShow=true;r.logicalRight=logicalRight;r.showIndex=showIndex;r.stageIndex=0;r.currentStageIndex=255;r.expectedMode=255;r.reverseOverride=reverseOverride;r.wholeStrip=wholeStrip;
  r.startedAt=millis();
}
void addStoredRidingEffect(JsonArray seg,uint8_t segId,const RidingEffectConfig &e,bool reverse){
  JsonObject effect=seg.createNestedObject();effect["id"]=segId;effect["on"]=true;effect["fx"]=e.isShow?0:e.fx;effect["pal"]=0;effect["rev"]=reverse;
  effect["sx"]=e.sx;effect["ix"]=e.ix;effect["c1"]=e.c1;effect["c2"]=e.c2;effect["c3"]=e.c3;effect["o1"]=e.o1;effect["o2"]=e.o2;effect["o3"]=e.o3;
  JsonArray colors=effect.createNestedArray("col");for(uint8_t i=0;i<3;i++){JsonArray c=colors.createNestedArray();const uint8_t v=i==0?255:0;c.add(v);c.add(v);c.add(v);}
}
void applyStoredRidingMode(RidingMode mode,uint16_t transition100ms=4){
  if(mode==RIDING_PLAYLIST){applyModeSide("riding","both",transition100ms);return;}
  const uint8_t mi=ridingModeIndex(mode);
  // Mode baru yang belum pernah disimpan harus memiliki konfigurasi awal miliknya
  // sendiri. Jangan jatuh ke preset Manual: Auto wajib Signature Show, sedangkan
  // Semi Auto wajib efek WLED bersama. Default ini langsung tersimpan di ESP32.
  if(mode==RIDING_SEMI_AUTO&&(!ridingEffects[mi][0].valid||ridingEffects[mi][0].isShow)){
    RidingEffectConfig initial;initial.valid=true;initial.isShow=false;initial.fx=0;
    ridingEffects[mi][0]=initial;ridingEffects[mi][1]=initial;queueSave();
  }else if(mode==RIDING_AUTO&&(!ridingEffects[mi][0].valid||!ridingEffects[mi][0].isShow)){
    RidingEffectConfig initial;initial.valid=true;initial.isShow=true;initial.showIndex=0;
    ridingEffects[mi][0]=initial;ridingEffects[mi][1]=initial;queueSave();
  }
  const bool hasRight=ridingEffects[mi][0].valid,hasLeft=ridingEffects[mi][1].valid;
  if(!hasRight&&!(mode==RIDING_MANUAL&&hasLeft)){applyModeSide("riding","both",transition100ms);return;}
  stopPlaylist();stopShowRunners("both");ridingUiActive=true;applyRidingLayerMode(mode);
  StaticJsonDocument<2048> doc;JsonObject root=doc.to<JsonObject>();root["on"]=true;root["transition"]=transition100ms;if(bri==0)root["bri"]=savedBri;
  JsonArray seg=root.createNestedArray("seg");
  if(mode==RIDING_MANUAL){
    // Setiap sisi pulih dari snapshot baru masing-masing. Sisi yang belum pernah
    // disimpan tetap memakai preset Riding lama sebagai fallback, bukan efek sisi lain.
    for(uint8_t k=0;k<2;k++){
      const bool logicalRight=(k==0);
      const RidingEffectConfig &saved=ridingEffects[0][k];
      if(saved.valid){
        addStoredRidingEffect(seg,physicalSegForLogicalSide(logicalRight),saved,ridingReverse[0][k]);
      }else{
        const PresetSlot legacy=getEffectiveSlot(2+k);
        RidingEffectConfig fallback;fallback.valid=true;fallback.isShow=legacy.isShow;fallback.showIndex=legacy.showIndex;fallback.fx=legacy.fx;
        fallback.sx=legacy.sx;fallback.ix=legacy.ix;fallback.c1=legacy.c1;fallback.c2=legacy.c2;fallback.c3=legacy.c3;
        fallback.o1=legacy.o1;fallback.o2=legacy.o2;fallback.o3=legacy.o3;
        addStoredRidingEffect(seg,physicalSegForLogicalSide(logicalRight),fallback,ridingReverse[0][k]);
      }
    }
  }else if(mode==RIDING_SEMI_AUTO){addStoredRidingEffect(seg,SEG_FX_WHOLE,ridingEffects[1][0],ridingWholeReverse());}
  else{
    addStoredRidingEffect(seg,physicalSegForLogicalSide(true),ridingEffects[2][0],false);
    addStoredRidingEffect(seg,physicalSegForLogicalSide(false),ridingEffects[2][0],false);
  }
  deserializeState(root);
  if(mode==RIDING_MANUAL){
    for(uint8_t k=0;k<2;k++){
      const RidingEffectConfig &saved=ridingEffects[0][k];
      const PresetSlot legacy=getEffectiveSlot(2+k);
      const bool isShow=saved.valid?saved.isShow:legacy.isShow;
      const uint8_t showIndex=saved.valid?saved.showIndex:legacy.showIndex;
      if(isShow&&showIndex<MZ_SHOW_COUNT){startShowRunner(k==0,showIndex);showRunner[k].loop=true;}
    }
  }else if(mode==RIDING_SEMI_AUTO){const RidingEffectConfig &e=ridingEffects[1][0];if(e.isShow&&e.showIndex<MZ_SHOW_COUNT){startShowRunner(true,e.showIndex,false,true);showRunner[0].loop=true;}}
  else{const RidingEffectConfig &e=ridingEffects[2][0];if(e.isShow&&e.showIndex<MZ_SHOW_COUNT){startShowRunner(true,e.showIndex);startShowRunner(false,e.showIndex);showRunner[0].loop=true;showRunner[1].loop=true;}}
  tickShowEngine();
}

void stopShowRunners(const String &side){
  if(side=="both"||side=="kanan"){showRunner[0].active=false;showRunner[0].hasCurrentShow=false;showRunner[0].currentStageIndex=255;showRunner[0].expectedMode=255;}
  if(side=="both"||side=="kiri"){showRunner[1].active=false;showRunner[1].hasCurrentShow=false;showRunner[1].currentStageIndex=255;showRunner[1].expectedMode=255;}
  if(side!="both"&&showRunner[0].wholeStrip){showRunner[0].active=false;showRunner[0].hasCurrentShow=false;showRunner[0].currentStageIndex=255;showRunner[0].expectedMode=255;showRunner[0].wholeStrip=false;}
}
// Jika efek pada segmen berbeda dari yang terakhir dipasang show, berarti pengguna mengganti efek lewat UI WLED,
// preset, atau API lain. Hentikan show agar pilihan itu tidak ditimpa sampai show selesai.
void checkExternalOverride(ShowRunner &r){
  if(!r.hasCurrentShow||r.expectedMode==255)return;
  const uint8_t segId=r.wholeStrip?SEG_FX_WHOLE:physicalSegForLogicalSide(r.logicalRight);
  if(segId>=strip.getSegmentsNum())return;
  Segment &seg=strip.getSegment(segId);
  if(!seg.isActive()||seg.mode==r.expectedMode)return;
  r.active=false;r.hasCurrentShow=false;r.currentStageIndex=255;r.expectedMode=255;
}
void tickShowEngine(){
  const uint32_t now=millis();
  for(uint8_t rIdx=0;rIdx<2;rIdx++){
    ShowRunner &r=showRunner[rIdx];checkExternalOverride(r);if(!r.active||r.showIndex>=MZ_SHOW_COUNT)continue;
    const MzShow &show=MZ_SHOWS[r.showIndex];const uint32_t elapsed=now-r.startedAt;
    while(r.stageIndex<show.n&&elapsed>=show.st[r.stageIndex].t){
      const MzStage &st=show.st[r.stageIndex];
      if(r.wholeStrip||st.side==0||st.side==(r.logicalRight?1:2)){
        applyShowStage(st,r.logicalRight,r.wholeStrip);
        r.currentStageIndex=r.stageIndex;
      }
      r.stageIndex++;
    }
    if(elapsed>=show.durationMs){
      if(r.loop){r.stageIndex=0;r.currentStageIndex=255;r.startedAt=now;} // Riding: show diulang terus
      else r.active=false; // selesai setelah cue terakhir + waktu penutup show
    }
  }
}
bool startShowPreview(int index,const String &side){
  if(index<0||index>=MZ_SHOW_COUNT)return false;
  if(side!="both"&&side!="kanan"&&side!="kiri")return false;
  stopPlaylist();
  stopShowRunners(side); // jangan biarkan runner preview sebelumnya hidup pada sisi lain
  if(bri==0){bri=savedBri?savedBri:180;stateUpdated(CALL_MODE_DIRECT_CHANGE);}
  if(ridingUiActive&&ridingMode==RIDING_SEMI_AUTO&&side=="both"){
    startShowRunner(true,(uint8_t)index,false,true);
  }else if(side=="both"){
    startShowRunner(true,(uint8_t)index);startShowRunner(false,(uint8_t)index);
  }else if(side=="kanan")startShowRunner(true,(uint8_t)index);
  else if(side=="kiri")startShowRunner(false,(uint8_t)index);
  else return false;
  tickShowEngine();
  return true;
}
void addShowBaseSegment(JsonArray seg,uint8_t segId,uint8_t slotIdx){
  // Seed segment colors/settings first; the show engine changes the FX stage immediately after this JSON is applied.
  addSegmentFromSlot(seg,segId,slotIdx);
}
bool applyWelcomeModeSide(const String &side){
  if(side!="both"&&side!="kanan"&&side!="kiri")return false;
  stopShowRunners("both");
  StaticJsonDocument<2048> doc;JsonObject root=doc.to<JsonObject>();root["on"]=true;
  if(bri==0)root["bri"]=savedBri;
  JsonArray seg=root.createNestedArray("seg");
  PresetSlot rightSlot,leftSlot;bool useRight=(side=="both"||side=="kanan"),useLeft=(side=="both"||side=="kiri");
  if(useRight){rightSlot=getEffectiveSlot(0);addShowBaseSegment(seg,physicalSegForLogicalSide(true),0);}
  if(useLeft){leftSlot=getEffectiveSlot(1);addShowBaseSegment(seg,physicalSegForLogicalSide(false),1);}
  deserializeState(root);
  if(useRight&&rightSlot.isShow)startShowRunner(true,rightSlot.showIndex);
  if(useLeft&&leftSlot.isShow)startShowRunner(false,leftSlot.showIndex);
  tickShowEngine();
  return true;
}
void handleShow(AsyncWebServerRequest *req){
  if(!req->hasArg("index")){req->send(400,"application/json","{\"ok\":false,\"error\":\"index required\"}");return;}
  const int index=req->arg("index").toInt();
  const uint8_t sc=mzSideCode(req->hasArg("side")?req->arg("side"):String("both"));
  if(index<0||index>=MZ_SHOW_COUNT||sc==255){req->send(400,"application/json","{\"ok\":false}");return;}
  MzCmd c={};c.type=MZ_CMD_SHOW_START;c.side=sc;c.value=(uint8_t)index;
  if(!enqueueCmd(c)){sendBusy(req);return;}
  req->send(200,"application/json","{\"ok\":true}");
}
// Dieksekusi HANYA dari processCmdQueue() (task loop).
void execShowPalette(uint8_t pal,const String &side){
  const uint16_t previousTransition=strip.getTransition();
  strip.setTransition(220); // 220 ms saat pengguna mengganti palette
  for(uint8_t logical=0;logical<2;logical++){
    const bool rightSide=(logical==0);
    if(side!="both" && side!=(rightSide?String("kanan"):String("kiri")))continue;
    RidingColor &color=ridingColorFor(ridingMode,rightSide);
    color.palette=true;color.pal=pal;
    applyRidingColorLayer(rightSide);
  }
  strip.trigger();
  stateUpdated(CALL_MODE_DIRECT_CHANGE);
  queueSave();
  strip.setTransition(previousTransition);
}
void handleShowPalette(AsyncWebServerRequest *req){
  if(!req->hasArg("pal")){req->send(400,"application/json","{\"ok\":false,\"error\":\"pal required\"}");return;}
  const int requested=req->arg("pal").toInt();
  if(requested<0||requested>255){req->send(400,"application/json","{\"ok\":false,\"error\":\"invalid palette\"}");return;}
  const uint8_t sc=mzSideCode(req->hasArg("side")?req->arg("side"):String("both"));
  if(sc==255){req->send(400,"application/json","{\"ok\":false}");return;}
  MzCmd c={};c.type=MZ_CMD_SHOW_PALETTE;c.side=sc;c.value=(uint8_t)requested;
  if(!enqueueCmd(c)){sendBusy(req);return;}
  req->send(200,"application/json","{\"ok\":true}");
}
void handleShowStop(AsyncWebServerRequest *req){
  const uint8_t sc=mzSideCode(req->hasArg("side")?req->arg("side"):String("both"));
  if(sc==255){req->send(400,"application/json","{\"ok\":false}");return;}
  MzCmd c={};c.type=MZ_CMD_SHOW_STOP;c.side=sc;
  if(!enqueueCmd(c)){sendBusy(req);return;}
  req->send(200,"application/json","{\"ok\":true}");
}
uint8_t applyCurrentShowDirection(){
  // Arah global dan arah per sisi digabungkan pada tiap segmen aktif.
  // Reset runtime memastikan efek yang menyimpan state internal membaca arah baru.
  if(ridingMode==RIDING_SEMI_AUTO){
    Segment &seg=strip.getSegment(SEG_FX_WHOLE);
    if(!seg.isActive())return 0;
    bool targetReverse=ridingWholeReverse();
    ShowRunner &r=showRunner[0];
    if(r.hasCurrentShow&&r.showIndex<MZ_SHOW_COUNT&&r.currentStageIndex<MZ_SHOWS[r.showIndex].n){
      const MzStage &st=MZ_SHOWS[r.showIndex].st[r.currentStageIndex];
      targetReverse=((st.flags&1U)!=0)^r.reverseOverride^ridingWholeReverse();
    }
    if(seg.reverse!=targetReverse){
      seg.setOption(1,targetReverse);
      seg.markForReset();
      strip.trigger();stateUpdated(CALL_MODE_DIRECT_CHANGE);
    }
    return 1;
  }
  uint8_t activeSegments=0;
  bool changed=false;
  for(uint8_t logical=0;logical<2;logical++){
    const bool logicalRight=(logical==0);
    const uint8_t segId=physicalSegForLogicalSide(logicalRight);
    if(segId>=strip.getSegmentsNum())continue;
    Segment &seg=strip.getSegment(segId);
    if(!seg.isActive())continue;
    activeSegments++;

    const bool globalReverse=ridingUiActive?(ridingMode==RIDING_PLAYLIST&&showReverse):showReverse;
    const bool sideReverse=ridingUiActive?ridingSideReverse(ridingMode,logicalRight):false;
    bool targetReverse=globalReverse ^ sideReverse;
    if(ridingMode==RIDING_PLAYLIST&&plRun.active&&plRun.list<PL_MAX&&plRun.slot<pls[plRun.list].n){
      targetReverse^=pls[plRun.list].it[plRun.slot].s[logical].rv!=0;
    }
    ShowRunner &r=showRunner[logical];
    if(r.hasCurrentShow && r.showIndex<MZ_SHOW_COUNT && r.currentStageIndex<MZ_SHOWS[r.showIndex].n){
      const MzStage &st=MZ_SHOWS[r.showIndex].st[r.currentStageIndex];
      targetReverse=(((st.flags&1U)!=0) ^ globalReverse ^ r.reverseOverride ^ sideReverse);
    }
    if(seg.reverse!=targetReverse){
      // Bit 1 adalah opsi REVERSE resmi pada Segment WLED.
      seg.setOption(1,targetReverse);
      seg.markForReset();
      changed=true;
    }
  }
  if(changed){strip.trigger();stateUpdated(CALL_MODE_DIRECT_CHANGE);}
  return activeSegments;
}
// Hanya MEMBACA status segmen (aman); perubahan nyata dilakukan applyCurrentShowDirection() di loop().
uint8_t countActiveSegments(){
  if(ridingMode==RIDING_SEMI_AUTO)return strip.getSegment(SEG_FX_WHOLE).isActive()?1:0;
  uint8_t n=0;
  for(uint8_t logical=0;logical<2;logical++){
    const uint8_t segId=physicalSegForLogicalSide(logical==0);
    if(segId>=strip.getSegmentsNum())continue;
    if(strip.getSegment(segId).isActive())n++;
  }
  return n;
}
void handleShowReverse(AsyncWebServerRequest *req,bool isGet){
  if(isGet){
    String json=String("{\"enabled\":")+(showReverse?"true":"false")+"}";
    req->send(200,"application/json",json);return;
  }
  if(!req->hasArg("enabled")){req->send(400,"application/json","{\"ok\":false}");return;}
  const bool previousReverse=showReverse;
  const String value=req->arg("enabled");
  const bool wanted=(value=="1"||value=="true"||value=="on");
  const uint8_t activeSegments=countActiveSegments();
  if(activeSegments==0){
    String json=String("{\"ok\":false,\"enabled\":")+(previousReverse?"true":"false")+String(",\"activeSegments\":0,\"error\":\"no_active_segment\"}");
    req->send(409,"application/json",json);
    return;
  }
  showReverse=wanted;
  MzCmd c={};c.type=MZ_CMD_DIRECTION;
  if(!enqueueCmd(c)){showReverse=previousReverse;sendBusy(req);return;}
  queueSave();
  String json=String("{\"ok\":true,\"enabled\":")+(showReverse?"true":"false")+String(",\"activeSegments\":")+String(activeSegments)+"}";
  req->send(200,"application/json",json);
}

String jsonEscape(const String &s){
String o;o.reserve(s.length()+4);
for(size_t k=0;k<s.length();k++){char c=s[k];
if(c=='"'||c=='\\'){o+='\\';o+=c;}
else if((uint8_t)c<0x20){o+=' ';}
else o+=c;}
return o;}

void handlePreset(AsyncWebServerRequest *req){
String slotName=req->arg("slot");
int i=slotIdx(slotName);
if(i<0){req->send(400,"application/json","{\"ok\":false}");return;}
PresetSlot p;p.valid=true;
p.isShow=(i<=3&&req->hasArg("isShow")&&(req->arg("isShow")=="1"||req->arg("isShow")=="true"));
p.showIndex=(i<=3)?(uint8_t)constrain(req->arg("showIndex").toInt(),0,MZ_SHOW_COUNT-1):0;
p.fx=p.isShow?0:argU8(req,"fx",0);p.pal=argU8(req,"pal",0);
p.r=argU8(req,"r",255);p.g=argU8(req,"g",255);p.b=argU8(req,"b",255);
p.r1=argU8(req,"r1",0);p.g1=argU8(req,"g1",0);p.b1=argU8(req,"b1",0);
p.r2=argU8(req,"r2",0);p.g2=argU8(req,"g2",0);p.b2=argU8(req,"b2",0);
p.sx=argU8(req,"sx",128);p.ix=argU8(req,"ix",128);
p.c1=argU8(req,"c1",128);p.c2=argU8(req,"c2",128);p.c3=argU8(req,"c3",128);
p.o1=req->hasArg("o1")?(req->arg("o1").toInt()?1:0):0;
p.o2=req->hasArg("o2")?(req->arg("o2").toInt()?1:0):0;
p.o3=req->hasArg("o3")?(req->arg("o3").toInt()?1:0):0;
p.bri=argU8(req,"bri",180);if(p.bri==0)p.bri=180;
if(isRestrictedSlot(i)){p.pal=0;p.r1=0;p.g1=0;p.b1=0;p.r2=0;p.g2=0;p.b2=0;
if(p.r==0&&p.g==0&&p.b==0){if(i==6||i==7){p.r=255;p.g=0;p.b=0;}else{p.r=255;p.g=165;p.b=0;}}}
pslots[i]=p;
queueSave();
req->send(200,"application/json","{\"ok\":true}");}

void handleApply(AsyncWebServerRequest *req){
if(!req->hasArg("mode")||!req->hasArg("side")){req->send(400,"application/json","{\"ok\":false}");return;}
const String mode=req->arg("mode");const uint8_t sc=mzSideCode(req->arg("side"));
const bool modeOk=(mode=="welcoming")||(modeBase(mode)>=0);
if(!modeOk||sc==255||mode.length()>=sizeof(((MzCmd*)0)->mode)){req->send(200,"application/json","{\"ok\":false}");return;}
MzCmd c={};c.type=MZ_CMD_APPLY;c.side=sc;strncpy(c.mode,mode.c_str(),sizeof(c.mode)-1);
if(!enqueueCmd(c)){sendBusy(req);return;}
req->send(200,"application/json","{\"ok\":true}");}
void handleSideSwap(AsyncWebServerRequest *req){
if(!req->hasArg("enabled")){req->send(400,"application/json","{\"ok\":false}");return;}
const bool en=(req->arg("enabled")=="1"||req->arg("enabled")=="true");
MzCmd c={};c.type=MZ_CMD_SIDE_SWAP;c.flag=en?1:0;
if(!enqueueCmd(c)){sendBusy(req);return;}
String json=String("{\"ok\":true,\"enabled\":")+(en?"true":"false")+"}";
req->send(200,"application/json",json);}
void handleRidingReverse(AsyncWebServerRequest *req,bool isGet){
  if(isGet){
    String json="{\"ok\":true,\"manual\":{\"kanan\":";
    json+=(draftActive?draftBkReverse:ridingReverse)[0][0]?"true":"false";json+=",\"kiri\":";json+=(draftActive?draftBkReverse:ridingReverse)[0][1]?"true":"false";
    json+="},\"semi-auto\":{\"kanan\":";json+=(draftActive?draftBkReverse:ridingReverse)[1][0]?"true":"false";json+=",\"kiri\":";json+=(draftActive?draftBkReverse:ridingReverse)[1][1]?"true":"false";
    json+="},\"auto\":{\"kanan\":";json+=(draftActive?draftBkReverse:ridingReverse)[2][0]?"true":"false";json+=",\"kiri\":";json+=(draftActive?draftBkReverse:ridingReverse)[2][1]?"true":"false";json+="}}";
    req->send(200,"application/json",json);return;
  }
  if(!req->hasArg("mode")||!req->hasArg("side")||!req->hasArg("enabled")){
    req->send(400,"application/json","{\"ok\":false,\"error\":\"mode, side, and enabled required\"}");return;
  }
  const String mode=req->arg("mode"),side=req->arg("side");
  uint8_t modeIdx=255;
  if(mode=="manual")modeIdx=0;else if(mode=="semi-auto")modeIdx=1;else if(mode=="auto")modeIdx=2;
  const uint8_t sideCode=mzSideCode(side);
  if(modeIdx==255||(sideCode!=1&&sideCode!=2)){req->send(400,"application/json","{\"ok\":false,\"error\":\"invalid mode or side\"}");return;}
  const bool en=req->arg("enabled")=="1"||req->arg("enabled")=="true";
  MzCmd c={};c.type=MZ_CMD_RIDING_DIRECTION;c.value=modeIdx;c.side=sideCode;c.flag=en?1:0;
  if(!enqueueCmd(c)){sendBusy(req);return;}
  req->send(200,"application/json","{\"ok\":true}");
}
void handleRidingMirror(AsyncWebServerRequest *req,bool isGet){
  if(isGet){String json=String("{\"ok\":true,\"enabled\":")+((draftActive?draftBkMirror:ridingMirror)?"true":"false")+"}";req->send(200,"application/json",json);return;}
  if(!req->hasArg("enabled")){req->send(400,"application/json","{\"ok\":false,\"error\":\"enabled required\"}");return;}
  const bool en=req->arg("enabled")=="1"||req->arg("enabled")=="true";
  MzCmd c={};c.type=MZ_CMD_RIDING_MIRROR;c.flag=en?1:0;
  if(!enqueueCmd(c)){sendBusy(req);return;}
  req->send(200,"application/json","{\"ok\":true}");
}
void handleRidingContext(AsyncWebServerRequest *req){
  if(!req->hasArg("active")){req->send(400,"application/json","{\"ok\":false,\"error\":\"active required\"}");return;}
  const bool active=req->arg("active")=="1"||req->arg("active")=="true";
  MzCmd c={};c.type=MZ_CMD_RIDING_CONTEXT;c.flag=active?1:0;
  if(!enqueueCmd(c)){sendBusy(req);return;}
  req->send(200,"application/json","{\"ok\":true}");
}
void handleRidingMode(AsyncWebServerRequest *req,bool isGet){
  if(isGet){
    const RidingMode reportedMode=savedRidingMode;
    const char *name=reportedMode==RIDING_SEMI_AUTO?"semi-auto":reportedMode==RIDING_AUTO?"auto":reportedMode==RIDING_PLAYLIST?"playlist":"manual";
    String json=String("{\"ok\":true,\"ready\":")+(ridingSegmentsReady?"true":"false")+",\"mode\":\""+name+"\"}";
    req->send(200,"application/json",json);return;
  }
  if(!req->hasArg("mode")){req->send(400,"application/json","{\"ok\":false,\"error\":\"mode required\"}");return;}
  const String mode=req->arg("mode");
  uint8_t value=255;
  if(mode=="manual")value=RIDING_MANUAL;
  else if(mode=="semi-auto")value=RIDING_SEMI_AUTO;
  else if(mode=="auto")value=RIDING_AUTO;
  else if(mode=="playlist")value=RIDING_PLAYLIST;
  if(value==255){req->send(400,"application/json","{\"ok\":false,\"error\":\"invalid mode\"}");return;}
  if(!ridingSegmentsReady){req->send(503,"application/json","{\"ok\":false,\"error\":\"GPIO 0/1 LED buses not ready\"}");return;}
  MzCmd c={};c.type=MZ_CMD_RIDING_MODE;c.value=value;
  if(!enqueueCmd(c)){sendBusy(req);return;}
  req->send(200,"application/json","{\"ok\":true}");
}
void appendRidingColorJson(String &json,const RidingColor &c){
  json+="{\"palette\":";json+=c.palette?"true":"false";
  json+=",\"pal\":"+String(c.pal)+",\"rgb\":["+String(c.r)+","+String(c.g)+","+String(c.b)+"]}";
}
void handleRidingColors(AsyncWebServerRequest *req,bool isGet){
  if(isGet){
    String json="{\"ok\":true,\"manual\":{\"right\":";
    appendRidingColorJson(json,(draftActive?draftBkColors:ridingColors)[0][0]);json+=",\"left\":";appendRidingColorJson(json,(draftActive?draftBkColors:ridingColors)[0][1]);
    json+="},\"semi-auto\":{\"right\":";appendRidingColorJson(json,(draftActive?draftBkColors:ridingColors)[1][0]);json+=",\"left\":";appendRidingColorJson(json,(draftActive?draftBkColors:ridingColors)[1][1]);
    json+="},\"auto\":{\"right\":";appendRidingColorJson(json,(draftActive?draftBkColors:ridingColors)[2][0]);json+=",\"left\":";appendRidingColorJson(json,(draftActive?draftBkColors:ridingColors)[2][0]);
    // Alias kanan/kiri dipertahankan untuk UI lama; sekarang menunjuk konfigurasi Manual.
    json+="},\"right\":";appendRidingColorJson(json,(draftActive?draftBkColors:ridingColors)[0][0]);json+=",\"left\":";appendRidingColorJson(json,(draftActive?draftBkColors:ridingColors)[0][1]);json+="}";
    req->send(200,"application/json",json);return;
  }
  if(!req->hasArg("side")||!req->hasArg("palette")||!req->hasArg("pal")||!req->hasArg("r")||!req->hasArg("g")||!req->hasArg("b")){
    req->send(400,"application/json","{\"ok\":false,\"error\":\"side, palette, pal and rgb required\"}");return;
  }
  const uint8_t side=mzSideCode(req->arg("side"));
  if(side<1||side>2){req->send(400,"application/json","{\"ok\":false,\"error\":\"invalid side\"}");return;}
  const String mode=req->hasArg("mode")?req->arg("mode"):String("manual");
  const uint8_t modeIdx=mode=="manual"?0:(mode=="semi-auto"?1:(mode=="auto"?2:255));
  if(modeIdx>2){req->send(400,"application/json","{\"ok\":false,\"error\":\"invalid mode\"}");return;}
  const String palette=req->arg("palette");
  if(palette!="0"&&palette!="1"){req->send(400,"application/json","{\"ok\":false,\"error\":\"invalid palette mode\"}");return;}
  MzCmd c={};c.type=MZ_CMD_RIDING_COLOR;c.side=side;c.flag=(palette=="1");c.value=(uint8_t)constrain(req->arg("pal").toInt(),0,255);
  c.r=(uint8_t)constrain(req->arg("r").toInt(),0,255);c.g=(uint8_t)constrain(req->arg("g").toInt(),0,255);c.b=(uint8_t)constrain(req->arg("b").toInt(),0,255);
  strlcpy(c.mode,mode.c_str(),sizeof(c.mode));
  if(!enqueueCmd(c)){sendBusy(req);return;}
  req->send(200,"application/json","{\"ok\":true}");
}

void appendRidingEffectJson(String &json,const RidingEffectConfig &e){
  json+="{\"valid\":";json+=e.valid?"true":"false";
  json+=",\"isShow\":";json+=e.isShow?"true":"false";
  json+=",\"showIndex\":"+String(e.showIndex)+",\"fx\":"+String(e.fx);
  json+=",\"sx\":"+String(e.sx)+",\"ix\":"+String(e.ix)+",\"c1\":"+String(e.c1)+",\"c2\":"+String(e.c2)+",\"c3\":"+String(e.c3);
  json+=",\"o1\":"+String(e.o1)+",\"o2\":"+String(e.o2)+",\"o3\":"+String(e.o3)+"}";
}
void handleRidingEffects(AsyncWebServerRequest *req,bool isGet){
  if(isGet){
    String json="{\"ok\":true,\"manual\":{\"right\":";appendRidingEffectJson(json,(draftActive?draftBkEffects:ridingEffects)[0][0]);json+=",\"left\":";appendRidingEffectJson(json,(draftActive?draftBkEffects:ridingEffects)[0][1]);
    json+="},\"semi-auto\":{\"shared\":";appendRidingEffectJson(json,(draftActive?draftBkEffects:ridingEffects)[1][0]);
    json+="},\"auto\":{\"shared\":";appendRidingEffectJson(json,(draftActive?draftBkEffects:ridingEffects)[2][0]);json+="}}";
    req->send(200,"application/json",json);return;
  }
  if(!req->hasArg("mode")||!req->hasArg("side")||!req->hasArg("fx")){req->send(400,"application/json","{\"ok\":false,\"error\":\"mode, side and fx required\"}");return;}
  const String mode=req->arg("mode"),side=req->arg("side");
  const uint8_t mi=mode=="manual"?0:(mode=="semi-auto"?1:(mode=="auto"?2:255));
  const uint8_t si=side=="kanan"?0:(side=="kiri"?1:255);
  if(mi>2||si>1){req->send(400,"application/json","{\"ok\":false,\"error\":\"invalid mode or side\"}");return;}
  RidingEffectConfig next;next.valid=true;next.isShow=(req->arg("isShow")=="1"||req->arg("isShow")=="true");
  next.showIndex=(uint8_t)constrain(req->arg("showIndex").toInt(),0,MZ_SHOW_COUNT-1);
  next.fx=(uint8_t)constrain(req->arg("fx").toInt(),0,255);
  next.sx=(uint8_t)constrain(req->arg("sx").toInt(),0,255);next.ix=(uint8_t)constrain(req->arg("ix").toInt(),0,255);
  next.c1=(uint8_t)constrain(req->arg("c1").toInt(),0,255);next.c2=(uint8_t)constrain(req->arg("c2").toInt(),0,255);next.c3=(uint8_t)constrain(req->arg("c3").toInt(),0,255);
  next.o1=req->arg("o1").toInt()?1:0;next.o2=req->arg("o2").toInt()?1:0;next.o3=req->arg("o3").toInt()?1:0;
  if(next.isShow)next.fx=0;
  if(mi==2&&!next.isShow){req->send(400,"application/json","{\"ok\":false,\"error\":\"auto_requires_signature\"}");return;}
  if(mi==1&&next.isShow){req->send(400,"application/json","{\"ok\":false,\"error\":\"semi_auto_requires_wled_effect\"}");return;}
  // Snapshot konfigurasi juga dimasukkan ke antrean loop agar tidak berbenturan
  // dengan serialisasi config atau proses LED pada task yang berbeda.
  MzCmd c={};c.type=MZ_CMD_RIDING_EFFECT;c.value=mi;c.side=si;c.flag=next.isShow?1:0;
  c.fx=next.fx;c.showIndex=next.showIndex;c.sx=next.sx;c.ix=next.ix;
  c.c1=next.c1;c.c2=next.c2;c.c3=next.c3;c.o1=next.o1;c.o2=next.o2;c.o3=next.o3;
  if(!enqueueCmd(c)){sendBusy(req);return;}
  req->send(200,"application/json","{\"ok\":true}");
}

// ===== COMPOSER: PRATINJAU / PLAY / DRAFT =====
// Pratinjau tidak menambah jalur render baru: draft ditulis ke array Riding di RAM lalu dirender oleh
// applyStoredRidingMode() yang sama dengan mode tersimpan. Data tersimpan dicadangkan di draftBk* dan dipulihkan saat draft berakhir.
void compRestoreBackups(){
  for(uint8_t m=0;m<3;m++)for(uint8_t k=0;k<2;k++){
    ridingEffects[m][k]=draftBkEffects[m][k];ridingColors[m][k]=draftBkColors[m][k];ridingReverse[m][k]=draftBkReverse[m][k];
  }
  ridingMirror=draftBkMirror;ridingColorJoin=draftBkJoin;draftActive=false;
}
void compDraftBegin(){
  if(draftActive)return;
  for(uint8_t m=0;m<3;m++)for(uint8_t k=0;k<2;k++){
    draftBkEffects[m][k]=ridingEffects[m][k];draftBkColors[m][k]=ridingColors[m][k];draftBkReverse[m][k]=ridingReverse[m][k];
  }
  draftBkMirror=ridingMirror;draftBkJoin=ridingColorJoin;draftActive=true;
}
// reapply=true: lampu dikembalikan ke mode Riding tersimpan (playlist dimulai dari slot pertama).
void compDraftEnd(bool reapply){
  if(!draftActive)return;
  compRestoreBackups();
  if(!reapply)return;
  if(savedRidingMode==RIDING_PLAYLIST&&ridingPl>=0){ridingUiActive=true;applyRidingLayerMode(RIDING_PLAYLIST);applyModeSide("riding","both");}
  else applyStoredRidingMode(savedRidingMode,4);
}
// Menulis satu komposisi ke array Riding untuk mode c.value (0 Manual, 1 Semi Auto, 2 Auto).
void compLoadDraft(const MzCmd &c){
  const uint8_t m=c.value;if(m>2)return;
  for(uint8_t k=0;k<2;k++){
    const CompSide &s=c.cs[k];
    if(m==0||k==0){
      RidingEffectConfig e;e.valid=true;e.isShow=(s.flags&1U)!=0;e.showIndex=s.showIndex;e.fx=e.isShow?0:s.fx;
      e.sx=s.sx;e.ix=s.ix;e.c1=s.c1;e.c2=s.c2;e.c3=s.c3;e.o1=s.o&1U;e.o2=(s.o>>1)&1U;e.o3=(s.o>>2)&1U;
      if(m==0)ridingEffects[0][k]=e;else{ridingEffects[m][0]=e;ridingEffects[m][1]=e;}
    }
    if(m==0||m==1||k==0){
      RidingColor col;col.palette=(s.flags&4U)!=0;col.pal=col.palette?s.pal:2;col.r=s.r;col.g=s.g;col.b=s.b;
      ridingColors[m][m==2?0:k]=col;
    }
    // Semi Auto memakai satu arah bersama (slot kanan); Auto memakai arah internal Signature Show.
    if(m==0)ridingReverse[0][k]=(s.flags&2U)!=0;
    else if(m==1)ridingReverse[1][k]=(k==0)&&((s.flags&2U)!=0);
  }
  if(m==1){ridingMirror=(c.mirror!=0);ridingColorJoin=(c.join!=0);} // mirror global hanya berlaku di Semi Auto (mirror per sisi: tahap berikutnya)
}
static bool compFlag(AsyncWebServerRequest *req,const String &k){return req->hasArg(k)&&(req->arg(k)=="1"||req->arg(k)=="true");}
static uint8_t compU8(AsyncWebServerRequest *req,const String &k,uint8_t def){return req->hasArg(k)?(uint8_t)constrain(req->arg(k).toInt(),0,255):def;}
// Parameter: mode=manual|semi-auto|auto, mirror, join (Semi Auto), lalu per sisi dengan awalan kanan_ / kiri_:
// show, idx, fx, sx, ix, c1..c3, o1..o3, rev, pm (palette 0|1), pn, r, g, b.
// Manual: kedua sisi wajib. Semi Auto: efek dari kanan_, warna dari kedua sisi. Auto: show dan warna dari kanan_.
bool compParse(AsyncWebServerRequest *req,MzCmd &c,const char *&err){
  const String mode=req->hasArg("mode")?req->arg("mode"):String("");
  const uint8_t mi=(mode=="manual")?0:((mode=="semi-auto")?1:((mode=="auto")?2:255));
  if(mi>2){err="mode";return false;}
  c.value=mi;c.mirror=compFlag(req,"mirror")?1:0;c.join=(mi==1&&compFlag(req,"join"))?1:0;
  const char *pre[2]={"kanan_","kiri_"};
  for(uint8_t k=0;k<2;k++){
    const String p=pre[k];CompSide &s=c.cs[k];
    const bool needEffect=(mi==0)||(k==0);
    const bool needColor=(mi!=2)||(k==0);
    const bool show=compFlag(req,p+"show");
    if(needEffect){
      if(!show&&!req->hasArg(p+"fx")){err="effect";return false;}
      if(mi==1&&show){err="semi_needs_fx";return false;}
      if(mi==2&&!show){err="auto_needs_show";return false;}
    }
    if(needColor&&(!req->hasArg(p+"r")||!req->hasArg(p+"g")||!req->hasArg(p+"b"))){err="color";return false;}
    s.flags=(show?1:0)|(compFlag(req,p+"rev")?2:0)|(compFlag(req,p+"pm")?4:0);
    s.showIndex=compU8(req,p+"idx",0);
    if(show&&s.showIndex>=MZ_SHOW_COUNT){err="show_index";return false;}
    s.fx=show?0:compU8(req,p+"fx",0);
    s.sx=compU8(req,p+"sx",128);s.ix=compU8(req,p+"ix",128);
    s.c1=compU8(req,p+"c1",128);s.c2=compU8(req,p+"c2",128);s.c3=compU8(req,p+"c3",128);
    s.o=(compFlag(req,p+"o1")?1:0)|(compFlag(req,p+"o2")?2:0)|(compFlag(req,p+"o3")?4:0);
    s.pal=compU8(req,p+"pn",2);s.r=compU8(req,p+"r",255);s.g=compU8(req,p+"g",255);s.b=compU8(req,p+"b",255);
  }
  return true;
}
void compEnqueue(AsyncWebServerRequest *req,uint8_t type,bool withBody){
  if(!ridingSegmentsReady){sendBusy(req);return;}
  MzCmd c={};c.type=type;const char *err="";
  if(withBody&&!compParse(req,c,err)){req->send(400,"application/json",String("{\"ok\":false,\"error\":\"")+err+"\"}");return;}
  if(withBody&&type==MZ_CMD_COMP_PREVIEW)draftBeatAt=millis();
  if(!enqueueCmd(c)){sendBusy(req);return;}
  req->send(200,"application/json",type==MZ_CMD_COMP_PREVIEW?"{\"ok\":true,\"timeoutMs\":20000}":"{\"ok\":true}");
}
void handleCompPreview(AsyncWebServerRequest *req){compEnqueue(req,MZ_CMD_COMP_PREVIEW,true);}
void handleCompPlay(AsyncWebServerRequest *req){compEnqueue(req,MZ_CMD_COMP_PLAY,true);}
void handleCompEnd(AsyncWebServerRequest *req){compEnqueue(req,MZ_CMD_COMP_END,false);}

// ===== EKSEKUTOR ANTREAN: satu-satunya jalur yang mengubah Segment/showRunner dari permintaan web =====
void processCmdQueue(){
  if(!cmdQueue)return;
  MzCmd c;
  for(uint8_t budget=MZ_CMD_QUEUE_LEN;budget>0&&xQueueReceive(cmdQueue,&c,0)==pdTRUE;budget--){
    // Draft Composer berakhir bila perintah lain mengambil alih lampu. CONTEXT active=0 (halaman ditutup) mengembalikan lampu ke data tersimpan.
    if(draftActive){
      if(c.type==MZ_CMD_RIDING_CONTEXT){if(c.flag==0)compDraftEnd(true);}
      else if(c.type!=MZ_CMD_COMP_PREVIEW&&c.type!=MZ_CMD_COMP_PLAY&&c.type!=MZ_CMD_COMP_END)compDraftEnd(false);
    }
    const String side=String(mzSideName(c.side));
    switch(c.type){
      case MZ_CMD_SHOW_START:   startShowPreview((int)c.value,side);break;
      case MZ_CMD_SHOW_STOP:    stopShowRunners(side);break;
      case MZ_CMD_SHOW_PALETTE: execShowPalette(c.value,side);break;
      case MZ_CMD_DIRECTION:    applyCurrentShowDirection();break;
      case MZ_CMD_APPLY:        applyModeSide(String(c.mode),side);break;
      case MZ_CMD_SIDE_SWAP: {
        const bool playlistActive=plRun.active;
        const uint8_t playlistList=plRun.list,playlistSlot=plRun.slot;
        const bool playlistHold=plRun.hold;
        stopShowRunners("both");sideSwap=(c.flag!=0);
        if(playlistActive&&playlistList<PL_MAX&&playlistSlot<pls[playlistList].n){
          // Re-render slot aktif dengan pemetaan fisik baru tanpa mengubah indeks,
          // durasi, status pause, atau snapshot playlist.
          applyPlSlot(playlistList,playlistSlot,playlistHold||pls[playlistList].n==1);
        }else if(savedRidingMode==RIDING_PLAYLIST){
          applyRidingLayerMode(RIDING_PLAYLIST);
          if(ridingPl>=0)applyModeSide("riding","both");
        }else applyStoredRidingMode(savedRidingMode,4);
        queueSave();break;
      }
      case MZ_CMD_PL_PLAY:      startPlaylist((int)c.value,(int)(c.flag&0x7F),(c.flag&0x80)!=0);break;
      case MZ_CMD_PL_PAUSE:     execPlPause(c.flag!=0);break;
      case MZ_CMD_PL_USE:
        ridingUiActive=true;
        if(c.flag!=0){ridingPl=-1;savedRidingMode=RIDING_MANUAL;queueSave();applyStoredRidingMode(RIDING_MANUAL,4);}
        else{ridingPl=(int8_t)c.value;savedRidingMode=RIDING_PLAYLIST;queueSave();applyRidingLayerMode(RIDING_PLAYLIST);applyModeSide("riding","both");}
        break;
      case MZ_CMD_PL_STOP:
        ridingUiActive=true;ridingPl=-1;savedRidingMode=RIDING_MANUAL;queueSave();applyStoredRidingMode(RIDING_MANUAL,4);break;
      case MZ_CMD_RIDING_MODE:
        ridingUiActive=true;savedRidingMode=(RidingMode)c.value;ridingPl=-1;stopPlaylist();stopShowRunners("both");
        applyStoredRidingMode(savedRidingMode,4);queueSave();break;
      case MZ_CMD_RIDING_COLOR: execRidingColor(c.side,c.flag!=0,c.value,c.r,c.g,c.b,(uint8_t)(c.mode[0]=='s'?1:(c.mode[0]=='a'?2:0)));break;
      case MZ_CMD_RIDING_EFFECT: {
        if(c.value<=2&&c.side<=1){
          RidingEffectConfig next;next.valid=true;next.isShow=c.flag!=0;next.fx=c.fx;next.showIndex=c.showIndex;
          next.sx=c.sx;next.ix=c.ix;next.c1=c.c1;next.c2=c.c2;next.c3=c.c3;next.o1=c.o1;next.o2=c.o2;next.o3=c.o3;
          ridingEffects[c.value][c.value==0?c.side:0]=next;
          if(c.value!=0)ridingEffects[c.value][1]=next;
          queueSave();
        }
        break;
      }
      case MZ_CMD_RIDING_DIRECTION:
        if(c.value<3&&(c.side==1||c.side==2)){
          ridingUiActive=true;
          ridingReverse[c.value][c.side==1?0:1]=(c.flag!=0);
          if(c.value==1&&c.side==1)ridingReverse[1][1]=false;
          if(ridingMode!=RIDING_PLAYLIST&&ridingModeIndex(ridingMode)==c.value)applyCurrentShowDirection();
          queueSave();
        }
        break;
      case MZ_CMD_RIDING_MIRROR:
        ridingUiActive=true;
        ridingMirror=(c.flag!=0);
        applyRidingMirrorOptions(ridingMode);
        queueSave();
        break;
      case MZ_CMD_RIDING_CONTEXT:
        ridingUiActive=(c.flag!=0);
        applyCurrentShowDirection();
        break;
      case MZ_CMD_COMP_PREVIEW:
        compDraftBegin();compLoadDraft(c);draftBeatAt=millis();
        applyStoredRidingMode((RidingMode)c.value,2);
        break;
      case MZ_CMD_COMP_PLAY:
        // Simpan atomik: pulihkan data tersimpan (membuang draft mode lain), tulis komposisi mode ini, jadikan mode Riding aktif.
        if(draftActive)compRestoreBackups();
        compLoadDraft(c);
        ridingUiActive=true;savedRidingMode=(RidingMode)c.value;ridingPl=-1;
        applyStoredRidingMode(savedRidingMode,4);
        queueSave();
        break;
      case MZ_CMD_COMP_END:
        compDraftEnd(true);
        break;
      default: break;
    }
  }
}


/* FIX BUG 1: handler brightness permanen */
void handleBri(AsyncWebServerRequest *req){
if(!req->hasArg("v")){req->send(400,"application/json","{\"ok\":false}");return;}
savedBri=(uint8_t)constrain(req->arg("v").toInt(),1,255);
bri = savedBri;
stateUpdated(CALL_MODE_DIRECT_CHANGE);
queueSave();
req->send(200,"application/json","{\"ok\":true}");}


/* ===== PLAYLIST RIDING =====
   Maks 4 playlist x 12 slot. Jadwal slot berjalan di sini (loop task), bukan di browser, sehingga tetap berputar
   saat HP terputus. Tiap slot menyimpan mode dan snapshot konfigurasi sesuai Manual/Semi Auto/Auto, termasuk warna, parameter, dan arah yang relevan + durasi (0.1 dtk). Disimpan di file /mz_pl.bin (bukan di cfg.json agar config tetap kecil). */
static const uint8_t PL_MAX=4, PL_SLOTS=12;
struct PlSide{uint8_t isShow;uint8_t showIndex;uint8_t fx;uint8_t pal;uint8_t col[9];uint8_t sx,ix,c1,c2,c3,o1,o2,o3,rv;};
// mode: 0=Manual (dua efek), 1=Semi Auto (satu efek + dua warna), 2=Auto (Signature Effect + satu warna).
struct PlSlot{uint16_t d;uint8_t mode;uint8_t pad;PlSide s[2];};   // pad bit 0=Mirror Semi Auto; s[0]=Kanan/shared, s[1]=Kiri (logis)
struct PlList{char name[21];uint8_t n;PlSlot it[PL_SLOTS];};
// Layout format lama MZP v3 (tanpa mode) dan v2 (reverse global) untuk migrasi.
struct PlSlotV3{uint16_t d;uint8_t pad[2];PlSide s[2];};
struct PlListV3{char name[21];uint8_t n;PlSlotV3 it[PL_SLOTS];};
static_assert(sizeof(PlSlot)==sizeof(PlSlotV3),"MZP v3 slot layout must remain readable");
static_assert(sizeof(PlList)==sizeof(PlListV3),"MZP v3 playlist layout must remain readable");
struct PlSideV2{uint8_t isShow;uint8_t showIndex;uint8_t fx;uint8_t pal;uint8_t col[9];uint8_t sx,ix,c1,c2,c3,o1,o2,o3;};
struct PlSlotV2{uint16_t d;uint8_t rv;uint8_t pad;PlSideV2 s[2];};
struct PlListV2{char name[21];uint8_t n;PlSlotV2 it[PL_SLOTS];};
PlList pls[PL_MAX];
static const uint8_t SKIN_MAX=30, SKIN_STOPS_MAX=8;
struct SkinStopMeta{uint8_t pos;char color[8];};
struct SkinMeta{char id[25];char name[25];uint8_t count;int8_t slot;SkinStopMeta stops[SKIN_STOPS_MAX];};
SkinMeta skinMeta[SKIN_MAX];uint8_t skinCount=0;
static const uint8_t SOLID_COLOR_SLOTS=6;
char solidColorSlots[SOLID_COLOR_SLOTS][8]={};bool solidColorSlotsInitialized=false;
int8_t ridingPl=-1;                 // -1: Riding memakai preset Single Effect
volatile bool plBusy=false;         // true saat HTTP sedang menulis pls[]; runner menunggu
bool plSavePending=false;uint32_t plSaveT=0;
struct PlRun{bool active=false;bool hold=false;uint8_t list=0;uint8_t slot=0;uint32_t startedAt=0;uint32_t heldAt=0;};
PlRun plRun;

void plSaveFile(){File f=WLED_FS.open(F("/mz_pl.bin"),"w");if(!f)return;const uint8_t h[4]={'M','Z','P',4};f.write(h,4);f.write((const uint8_t*)pls,sizeof(pls));f.close();}
void plLoadFile(){
  File f=WLED_FS.open(F("/mz_pl.bin"),"r");if(!f)return;
  uint8_t h[4]={0,0,0,0};
  if(f.read(h,4)==4&&h[0]=='M'&&h[1]=='Z'&&h[2]=='P'){
    if(h[3]==4&&f.size()==(4+sizeof(pls))){f.read((uint8_t*)pls,sizeof(pls));}
    else if(h[3]==3&&f.size()==(4+sizeof(PlListV3)*PL_MAX)){
      PlListV3 old[PL_MAX];
      if(f.read((uint8_t*)old,sizeof(old))==sizeof(old)){
        for(uint8_t i=0;i<PL_MAX;i++){
          strlcpy(pls[i].name,old[i].name,sizeof(pls[i].name));pls[i].n=old[i].n;
          for(uint8_t j=0;j<PL_SLOTS;j++){PlSlot &dst=pls[i].it[j];const PlSlotV3 &src=old[i].it[j];dst.d=src.d;dst.mode=0;dst.pad=0;memcpy(dst.s,src.s,sizeof(dst.s));}
        }
        plSavePending=true;plSaveT=millis();
      }
    }
    else if(h[3]==2&&f.size()==(4+sizeof(PlListV2)*PL_MAX)){
      PlListV2 old[PL_MAX];
      if(f.read((uint8_t*)old,sizeof(old))==sizeof(old)){
        for(uint8_t i=0;i<PL_MAX;i++){
          strlcpy(pls[i].name,old[i].name,sizeof(pls[i].name));pls[i].n=old[i].n;
          for(uint8_t j=0;j<PL_SLOTS;j++){
            PlSlot &dst=pls[i].it[j];const PlSlotV2 &src=old[i].it[j];dst.d=src.d;dst.mode=0;dst.pad=0;
            for(uint8_t k=0;k<2;k++){
              const PlSideV2 &a=src.s[k];PlSide &b=dst.s[k];
              b.isShow=a.isShow;b.showIndex=a.showIndex;b.fx=a.fx;b.pal=a.pal;memcpy(b.col,a.col,sizeof(b.col));
              b.sx=a.sx;b.ix=a.ix;b.c1=a.c1;b.c2=a.c2;b.c3=a.c3;b.o1=a.o1;b.o2=a.o2;b.o3=a.o3;b.rv=src.rv;
            }
          }
        }
        plSavePending=true;plSaveT=millis();
      }
    }
  }
  f.close();
  for(uint8_t i=0;i<PL_MAX;i++){pls[i].name[20]=0;if(pls[i].n>PL_SLOTS)pls[i].n=0;for(uint8_t j=0;j<pls[i].n;j++)if(pls[i].it[j].mode>2)pls[i].it[j].mode=0;}
}
void plQueueSave(){plSavePending=true;plSaveT=millis();}
uint32_t plSlotMs(const PlSlot &sl){uint32_t ms=(uint32_t)sl.d*100U;if(ms<100U)ms=100U;return ms;}

void stopPlaylist(){plRun.active=false;plRun.hold=false;}

// Dieksekusi hanya dari task loop().
void applyPlSlot(uint8_t li,uint8_t si,bool loopShows){
  if(li>=PL_MAX||si>=pls[li].n)return;
  const PlSlot sl=pls[li].it[si];
  const RidingMode mode=(RidingMode)constrain((int)sl.mode,0,2);
  stopShowRunners("both");
  applyRidingLayerMode(mode); // topologi segmen mengikuti mode yang disimpan di slot
  StaticJsonDocument<2048> doc;JsonObject root=doc.to<JsonObject>();
  root["on"]=true;root["transition"]=3;if(bri==0)root["bri"]=savedBri;
  JsonArray seg=root.createNestedArray("seg");
  // Lapisan warna selalu dua sisi. Pada Auto, warna kanan menjadi satu-satunya warna bersama.
  for(uint8_t k=0;k<2;k++){
    const PlSide &p=(mode==RIDING_AUTO)?sl.s[0]:sl.s[k];
    JsonObject color=seg.createNestedObject();
    color["id"]=physicalColorSegForLogicalSide(k==0);color["on"]=true;color["fx"]=FX_MODE_PALETTE;color["pal"]=p.pal;
    color["sx"]=211;color["ix"]=77;color["c1"]=77;color["c2"]=0;color["c3"]=0;color["o1"]=0;color["o2"]=0;color["o3"]=0;
    JsonArray colorCol=color.createNestedArray("col");
    for(uint8_t c=0;c<3;c++){JsonArray a=colorCol.createNestedArray();a.add(p.col[c*3]);a.add(p.col[c*3+1]);a.add(p.col[c*3+2]);}
  }
  auto addEffect=[&](uint8_t segId,const PlSide &p,bool reverse){
    JsonObject effect=seg.createNestedObject();effect["id"]=segId;effect["on"]=true;
    effect["fx"]=p.isShow?0:p.fx;effect["pal"]=0;effect["rev"]=reverse;
    effect["sx"]=p.sx;effect["ix"]=p.ix;effect["c1"]=p.c1;effect["c2"]=p.c2;effect["c3"]=p.c3;
    effect["o1"]=p.o1;effect["o2"]=p.o2;effect["o3"]=p.o3;
    JsonArray effectCol=effect.createNestedArray("col");
    for(uint8_t c=0;c<3;c++){JsonArray a=effectCol.createNestedArray();const uint8_t v=c==0?255:0;a.add(v);a.add(v);a.add(v);}
  };
  if(mode==RIDING_MANUAL){
    addEffect(physicalSegForLogicalSide(true),sl.s[0],sl.s[0].rv!=0);
    addEffect(physicalSegForLogicalSide(false),sl.s[1],sl.s[1].rv!=0);
  }else if(mode==RIDING_SEMI_AUTO){
    addEffect(SEG_FX_WHOLE,sl.s[0],sl.s[0].rv!=0);
  }else{
    addEffect(physicalSegForLogicalSide(true),sl.s[0],false);
    addEffect(physicalSegForLogicalSide(false),sl.s[0],false);
  }
  deserializeState(root);
  if(mode==RIDING_MANUAL){
    for(uint8_t k=0;k<2;k++)if(sl.s[k].isShow&&sl.s[k].showIndex<MZ_SHOW_COUNT){startShowRunner(k==0,sl.s[k].showIndex,sl.s[k].rv!=0);showRunner[k].loop=loopShows;}
  }else if(mode==RIDING_SEMI_AUTO){
    if(sl.s[0].isShow&&sl.s[0].showIndex<MZ_SHOW_COUNT){startShowRunner(true,sl.s[0].showIndex,sl.s[0].rv!=0,true);showRunner[0].loop=loopShows;}
  }else if(sl.s[0].isShow&&sl.s[0].showIndex<MZ_SHOW_COUNT){
    startShowRunner(true,sl.s[0].showIndex,false);startShowRunner(false,sl.s[0].showIndex,false);
    showRunner[0].loop=loopShows;showRunner[1].loop=loopShows;
  }
  tickShowEngine();
}
bool startPlaylist(int li,int si,bool hold){
  if(li<0||li>=PL_MAX||pls[li].n==0)return false;
  ridingUiActive=true;
  applyRidingLayerMode(RIDING_PLAYLIST);
  if(si<0||si>=pls[li].n)si=0;
  plRun.active=true;plRun.hold=hold;plRun.list=(uint8_t)li;plRun.slot=(uint8_t)si;plRun.startedAt=millis();
  applyPlSlot((uint8_t)li,(uint8_t)si,hold||pls[li].n==1);
  return true;
}
void execPlPause(bool on){
  if(!plRun.active)return;
  if(on&&!plRun.hold){plRun.hold=true;plRun.heldAt=millis();}
  else if(!on&&plRun.hold){plRun.hold=false;plRun.startedAt=millis()-(plRun.heldAt-plRun.startedAt);}
}
void tickPlaylist(){
  if(!plRun.active||plBusy)return;
  const PlList &L=pls[plRun.list];
  if(L.n==0){plRun.active=false;return;}
  if(plRun.hold||L.n==1)return;
  if(plRun.slot>=L.n)plRun.slot=0;
  const uint32_t now=millis();
  if((uint32_t)(now-plRun.startedAt)>=plSlotMs(L.it[plRun.slot])){
    plRun.slot=(uint8_t)((plRun.slot+1)%L.n);plRun.startedAt=now;applyPlSlot(plRun.list,plRun.slot,false);
  }
}
void plParseSide(JsonObject o,PlSide &p){
  memset(&p,0,sizeof(p));
  if(o.isNull()){p.col[0]=255;p.col[1]=165;return;}
  p.isShow=((int)(o["show"]|0))?1:0;
  p.showIndex=(uint8_t)constrain((int)(o["si"]|0),0,MZ_SHOW_COUNT-1);
  p.fx=(uint8_t)constrain((int)(o["fx"]|0),0,255);p.pal=(uint8_t)constrain((int)(o["pal"]|0),0,255);
  JsonArray col=o["col"];
  for(uint8_t c=0;c<3;c++){JsonArray ca=col[c];for(uint8_t k=0;k<3;k++){int v=ca.isNull()?0:(int)(ca[k]|0);p.col[c*3+k]=(uint8_t)constrain(v,0,255);}}
  p.sx=(uint8_t)constrain((int)(o["sx"]|128),0,255);p.ix=(uint8_t)constrain((int)(o["ix"]|128),0,255);
  p.c1=(uint8_t)constrain((int)(o["c1"]|128),0,255);p.c2=(uint8_t)constrain((int)(o["c2"]|128),0,255);p.c3=(uint8_t)constrain((int)(o["c3"]|128),0,255);
  p.o1=((int)(o["o1"]|0))?1:0;p.o2=((int)(o["o2"]|0))?1:0;p.o3=((int)(o["o3"]|0))?1:0;
  p.rv=((int)(o["rv"]|0))?1:0;
}
void plSideJson(String &out,const PlSide &p){
  char b[256];
  snprintf(b,sizeof(b),"{\"show\":%u,\"si\":%u,\"fx\":%u,\"pal\":%u,\"col\":[[%u,%u,%u],[%u,%u,%u],[%u,%u,%u]],\"sx\":%u,\"ix\":%u,\"c1\":%u,\"c2\":%u,\"c3\":%u,\"o1\":%u,\"o2\":%u,\"o3\":%u,\"rv\":%u}",
    (unsigned)p.isShow,(unsigned)p.showIndex,(unsigned)p.fx,(unsigned)p.pal,
    (unsigned)p.col[0],(unsigned)p.col[1],(unsigned)p.col[2],(unsigned)p.col[3],(unsigned)p.col[4],(unsigned)p.col[5],(unsigned)p.col[6],(unsigned)p.col[7],(unsigned)p.col[8],
    (unsigned)p.sx,(unsigned)p.ix,(unsigned)p.c1,(unsigned)p.c2,(unsigned)p.c3,(unsigned)p.o1,(unsigned)p.o2,(unsigned)p.o3,(unsigned)p.rv);
  out+=b;
}
bool saveSkinFile(){
  File f=WLED_FS.open(F("/mz_skins.bin"),"w");if(!f)return false;
  const uint8_t h[5]={'M','Z','S',1,skinCount};
  const bool ok=f.write(h,sizeof(h))==sizeof(h)&&f.write((const uint8_t*)skinMeta,(size_t)skinCount*sizeof(SkinMeta))==(size_t)skinCount*sizeof(SkinMeta);
  f.close();return ok;
}
void loadSkinFile(){
  File f=WLED_FS.open(F("/mz_skins.bin"),"r");if(!f)return;
  uint8_t h[5]={0,0,0,0,0};
  if(f.read(h,sizeof(h))==sizeof(h)&&h[0]=='M'&&h[1]=='Z'&&h[2]=='S'&&h[3]==1&&h[4]<=SKIN_MAX&&f.size()==sizeof(h)+(size_t)h[4]*sizeof(SkinMeta)){
    skinCount=h[4];if(skinCount)f.read((uint8_t*)skinMeta,(size_t)skinCount*sizeof(SkinMeta));
    for(uint8_t i=0;i<skinCount;i++){skinMeta[i].id[sizeof(skinMeta[i].id)-1]=0;skinMeta[i].name[sizeof(skinMeta[i].name)-1]=0;if(skinMeta[i].count>SKIN_STOPS_MAX)skinMeta[i].count=0;}
  }
  f.close();
}
void handleSkinList(AsyncWebServerRequest *req){
  String out;out.reserve(128+(size_t)skinCount*330);out+='[';
  for(uint8_t i=0;i<skinCount;i++){
    if(i)out+=',';
    const SkinMeta &k=skinMeta[i];
    out+="{\"id\":\"";out+=jsonEscape(String(k.id));out+="\",\"n\":\"";out+=jsonEscape(String(k.name));out+="\",\"slot\":";out+=String((int)k.slot);out+=",\"st\":[";
    for(uint8_t j=0;j<k.count;j++){if(j)out+=',';out+='[';out+=String((unsigned)k.stops[j].pos);out+=",\"";out+=k.stops[j].color;out+="\"]";}
    out+="]}";
  }
  out+=']';req->send(200,"application/json",out);
}
bool skinHexColor(const char *s){
  if(!s||strlen(s)!=7||s[0]!='#')return false;
  for(uint8_t i=1;i<7;i++){const char c=s[i];if(!((c>='0'&&c<='9')||(c>='a'&&c<='f')||(c>='A'&&c<='F')))return false;}
  return true;
}
bool saveSolidColorFile(const char slots[][8]){
  File f=WLED_FS.open(F("/mz_colors.bin"),"w");if(!f)return false;
  const uint8_t h[4]={'M','Z','C',1};
  const bool ok=f.write(h,sizeof(h))==sizeof(h)&&f.write((const uint8_t*)slots,SOLID_COLOR_SLOTS*8U)==SOLID_COLOR_SLOTS*8U;
  f.close();return ok;
}
void loadSolidColorFile(){
  File f=WLED_FS.open(F("/mz_colors.bin"),"r");if(!f)return;
  uint8_t h[4]={0,0,0,0};char slots[SOLID_COLOR_SLOTS][8]={};
  if(f.read(h,sizeof(h))==sizeof(h)&&h[0]=='M'&&h[1]=='Z'&&h[2]=='C'&&h[3]==1&&f.size()==sizeof(h)+SOLID_COLOR_SLOTS*8U&&f.read((uint8_t*)slots,sizeof(slots))==sizeof(slots)){
    bool valid=true;
    for(uint8_t i=0;i<SOLID_COLOR_SLOTS;i++){
      slots[i][7]=0;
      if(slots[i][0]&&!skinHexColor(slots[i]))valid=false;
    }
    if(valid){memcpy(solidColorSlots,slots,sizeof(slots));solidColorSlotsInitialized=true;}
  }
  f.close();
}
void handleSolidColors(AsyncWebServerRequest *req,bool get){
  if(get){
    String out="{\"initialized\":";out+=solidColorSlotsInitialized?"true":"false";out+=",\"slots\":[";
    for(uint8_t i=0;i<SOLID_COLOR_SLOTS;i++){if(i)out+=',';if(solidColorSlots[i][0]){out+='"';out+=solidColorSlots[i];out+='"';}else out+="null";}
    out+="]}";req->send(200,"application/json",out);return;
  }
  char *buf=(char*)req->_tempObject;
  if(!buf){req->send(400,"application/json","{\"ok\":false,\"error\":\"no_body\"}");return;}
  DynamicJsonDocument doc(512);
  if(deserializeJson(doc,buf)!=DeserializationError::Ok){req->send(400,"application/json","{\"ok\":false,\"error\":\"json\"}");return;}
  JsonArray slots=doc["slots"];
  if(slots.size()!=SOLID_COLOR_SLOTS){req->send(400,"application/json","{\"ok\":false,\"error\":\"slots\"}");return;}
  char next[SOLID_COLOR_SLOTS][8]={};
  for(uint8_t i=0;i<SOLID_COLOR_SLOTS;i++){
    JsonVariant v=slots[i];
    if(v.isNull())continue;
    if(!v.is<const char*>()){req->send(400,"application/json","{\"ok\":false,\"error\":\"color\"}");return;}
    const char *color=v.as<const char*>();
    if(!skinHexColor(color)){req->send(400,"application/json","{\"ok\":false,\"error\":\"color\"}");return;}
    strlcpy(next[i],color,sizeof(next[i]));
  }
  char old[SOLID_COLOR_SLOTS][8];memcpy(old,solidColorSlots,sizeof(old));const bool oldInitialized=solidColorSlotsInitialized;
  memcpy(solidColorSlots,next,sizeof(next));solidColorSlotsInitialized=true;
  if(!saveSolidColorFile(solidColorSlots)){
    memcpy(solidColorSlots,old,sizeof(old));solidColorSlotsInitialized=oldInitialized;
    req->send(500,"application/json","{\"ok\":false,\"error\":\"storage\"}");return;
  }
  req->send(200,"application/json","{\"ok\":true}");
}
void handleSkinSave(AsyncWebServerRequest *req){
  char *buf=(char*)req->_tempObject;
  if(!buf){req->send(400,"application/json","{\"ok\":false,\"error\":\"no_body\"}");return;}
  DynamicJsonDocument doc(2048);
  if(deserializeJson(doc,buf)!=DeserializationError::Ok){req->send(400,"application/json","{\"ok\":false,\"error\":\"json\"}");return;}
  JsonObject o=doc.as<JsonObject>();String id=o["id"]|"";String name=o["n"]|"";JsonArray st=o["st"];
  if(id.length()==0||id.length()>=sizeof(skinMeta[0].id)||name.length()==0||name.length()>=sizeof(skinMeta[0].name)||st.size()<2||st.size()>SKIN_STOPS_MAX){
    req->send(400,"application/json","{\"ok\":false,\"error\":\"fields\"}");return;
  }
  for(size_t i=0;i<id.length();i++){const char c=id[i];if(!((c>='0'&&c<='9')||(c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='_'||c=='-')){req->send(400,"application/json","{\"ok\":false,\"error\":\"id\"}");return;}}
  SkinMeta next={};strlcpy(next.id,id.c_str(),sizeof(next.id));strlcpy(next.name,name.c_str(),sizeof(next.name));
  next.count=(uint8_t)st.size();next.slot=(int8_t)constrain((int)(o["slot"]|-1),-1,SKIN_MAX-1);
  for(uint8_t i=0;i<next.count;i++){
    JsonArray stop=st[i];if(stop.size()!=2){req->send(400,"application/json","{\"ok\":false,\"error\":\"stop\"}");return;}
    const int pos=stop[0]|-1;const char *color=stop[1]|"";
    if(pos<0||pos>100||!skinHexColor(color)){req->send(400,"application/json","{\"ok\":false,\"error\":\"stop\"}");return;}
    next.stops[i].pos=(uint8_t)pos;strlcpy(next.stops[i].color,color,sizeof(next.stops[i].color));
  }
  int idx=-1;for(uint8_t i=0;i<skinCount;i++)if(id==skinMeta[i].id){idx=i;break;}
  if(idx<0&&skinCount>=SKIN_MAX){req->send(409,"application/json","{\"ok\":false,\"error\":\"full\"}");return;}
  SkinMeta old={};if(idx>=0)old=skinMeta[idx];else idx=skinCount++;
  skinMeta[idx]=next;
  if(!saveSkinFile()){
    if(idx+1==skinCount&&old.id[0]==0)skinCount--;else skinMeta[idx]=old;
    req->send(500,"application/json","{\"ok\":false,\"error\":\"storage\"}");return;
  }
  req->send(200,"application/json","{\"ok\":true}");
}
void handleSkinDelete(AsyncWebServerRequest *req){
  if(!req->hasArg("id")){req->send(400,"application/json","{\"ok\":false,\"error\":\"id\"}");return;}
  const String id=req->arg("id");uint8_t idx=0;while(idx<skinCount&&id!=skinMeta[idx].id)idx++;
  if(idx>=skinCount){req->send(404,"application/json","{\"ok\":false,\"error\":\"not_found\"}");return;}
  const uint8_t oldCount=skinCount;SkinMeta removed=skinMeta[idx];
  for(uint8_t i=idx;i+1<skinCount;i++)skinMeta[i]=skinMeta[i+1];
  skinCount--;
  if(!saveSkinFile()){
    for(uint8_t i=skinCount;i>idx;i--)skinMeta[i]=skinMeta[i-1];
    skinMeta[idx]=removed;skinCount=oldCount;
    req->send(500,"application/json","{\"ok\":false,\"error\":\"storage\"}");return;
  }
  memset(&skinMeta[skinCount],0,sizeof(SkinMeta));
  req->send(200,"application/json","{\"ok\":true}");
}
void handlePlSave(AsyncWebServerRequest *req){
  char *buf=(char*)req->_tempObject;
  if(!buf){req->send(400,"application/json","{\"ok\":false,\"error\":\"no_body\"}");return;}
  DynamicJsonDocument doc(14336);
  if(deserializeJson(doc,buf)!=DeserializationError::Ok){req->send(400,"application/json","{\"ok\":false,\"error\":\"json\"}");return;}
  JsonObject o=doc.as<JsonObject>();
  const int li=o["list"]|-1;
  if(li<0||li>=PL_MAX){req->send(400,"application/json","{\"ok\":false,\"error\":\"list\"}");return;}
  JsonArray it=o["it"];
  if(it.isNull()||it.size()>PL_SLOTS){req->send(400,"application/json","{\"ok\":false,\"error\":\"slot_limit\"}");return;}

  // Parse into a temporary object first. An invalid slot must never partially overwrite
  // a playlist that is already saved or currently running.
  PlList nextList;memset(&nextList,0,sizeof(nextList));
  strlcpy(nextList.name,(const char*)(o["name"]|"Playlist"),sizeof(nextList.name));
  uint8_t n=0;
  for(JsonObject x:it){
    if(n>=PL_SLOTS)break;
    const String mode=x["mode"]|"manual";
    if(mode!="manual"&&mode!="semi-auto"&&mode!="auto"){
      req->send(400,"application/json","{\"ok\":false,\"error\":\"invalid_slot_mode\"}");return;
    }
    JsonObject r=x["R"],l=x["L"];
    if(r.isNull()||l.isNull()){
      req->send(400,"application/json","{\"ok\":false,\"error\":\"slot_sides_required\"}");return;
    }
    PlSlot &sl=nextList.it[n];
    const int d=x["d"]|50;sl.d=(uint16_t)constrain(d,1,9999);
    sl.mode=mode=="semi-auto"?1:(mode=="auto"?2:0);
    sl.pad=(sl.mode==1&&((bool)(x["mirror"]|false)))?0x01U:0U;
    const uint8_t legacyRv=((int)(x["rv"]|0))?1:0;
    plParseSide(r,sl.s[0]);plParseSide(l,sl.s[1]);
    if(!r.containsKey("rv"))sl.s[0].rv=legacyRv;
    if(!l.containsKey("rv"))sl.s[1].rv=legacyRv;

    if(sl.mode==2){
      if(!sl.s[0].isShow){
        req->send(400,"application/json","{\"ok\":false,\"error\":\"auto_requires_signature\"}");return;
      }
      // Auto has one Signature Effect and one shared color configuration.
      sl.s[1]=sl.s[0];
    }else if(sl.mode==1){
      if(sl.s[0].isShow){
        req->send(400,"application/json","{\"ok\":false,\"error\":\"semi_auto_requires_wled_effect\"}");return;
      }
      // Semi Auto has one shared WLED effect/direction, but retains two color layers.
      PlSide &left=sl.s[1];const PlSide &shared=sl.s[0];
      left.isShow=shared.isShow;left.showIndex=shared.showIndex;left.fx=shared.fx;
      left.sx=shared.sx;left.ix=shared.ix;left.c1=shared.c1;left.c2=shared.c2;left.c3=shared.c3;
      left.o1=shared.o1;left.o2=shared.o2;left.o3=shared.o3;left.rv=shared.rv;
    }
    n++;
  }
  nextList.n=n;
  plBusy=true;
  pls[li]=nextList;
  plBusy=false;
  if(plRun.active&&plRun.list==li&&plRun.slot>=n)plRun.slot=0;
  plQueueSave();
  req->send(200,"application/json","{\"ok\":true}");
}
void handlePlList(AsyncWebServerRequest *req){
  const int li=req->hasArg("i")?req->arg("i").toInt():-1;
  if(li<0||li>=PL_MAX){req->send(400,"application/json","{\"ok\":false}");return;}
  const PlList &L=pls[li];
  String out;out.reserve(160+(size_t)L.n*470);
  out+="{\"name\":\"";out+=jsonEscape(String(L.name));out+="\",\"it\":[";
  for(uint8_t k=0;k<L.n;k++){
    if(k)out+=',';
    char b[88];const char *mode=L.it[k].mode==1?"semi-auto":(L.it[k].mode==2?"auto":"manual");snprintf(b,sizeof(b),"{\"d\":%u,\"mode\":\"%s\",\"mirror\":%s,\"R\":",(unsigned)L.it[k].d,mode,(L.it[k].mode==1&&(L.it[k].pad&0x01U))?"true":"false");out+=b;
    plSideJson(out,L.it[k].s[0]);out+=",\"L\":";plSideJson(out,L.it[k].s[1]);out+="}";
  }
  out+="]}";
  req->send(200,"application/json",out);
}
void handlePlSummary(AsyncWebServerRequest *req){
  String out;out.reserve(220);
  out+="{\"use\":";out+=String((int)ridingPl);out+=",\"lists\":[";
  for(uint8_t i=0;i<PL_MAX;i++){
    if(i)out+=',';
    out+="{\"n\":\"";out+=jsonEscape(String(pls[i].name));out+="\",\"c\":";out+=String((int)pls[i].n);out+="}";
  }
  out+="]}";
  req->send(200,"application/json",out);
}
void handlePlState(AsyncWebServerRequest *req){
  char b[140];
  const uint32_t el=plRun.active?(plRun.hold?(plRun.heldAt-plRun.startedAt):(uint32_t)(millis()-plRun.startedAt)):0;
  snprintf(b,sizeof(b),"{\"use\":%d,\"run\":%s,\"hold\":%s,\"list\":%u,\"slot\":%u,\"el\":%u}",
    (int)ridingPl,plRun.active?"true":"false",plRun.hold?"true":"false",(unsigned)plRun.list,(unsigned)plRun.slot,(unsigned)el);
  req->send(200,"application/json",b);
}
void handlePlDelete(AsyncWebServerRequest *req){
  const int li=req->hasArg("list")?req->arg("list").toInt():-1;
  if(li<0||li>=PL_MAX){req->send(400,"application/json","{\"ok\":false}");return;}
  plBusy=true;
  for(int k=li;k<PL_MAX-1;k++)pls[k]=pls[k+1];
  memset(&pls[PL_MAX-1],0,sizeof(PlList));
  plBusy=false;
  const bool wasUsed=(ridingPl==li);
  if(ridingPl==li)ridingPl=-1;else if(ridingPl>li)ridingPl--;
  if(plRun.active){if(plRun.list==li)plRun.active=false;else if(plRun.list>li)plRun.list--;}
  plQueueSave();queueSave();
  if(wasUsed){MzCmd c={};c.type=MZ_CMD_PL_STOP;enqueueCmd(c);}
  req->send(200,"application/json","{\"ok\":true}");
}
void handlePlUse(AsyncWebServerRequest *req){
  if(!req->hasArg("list")){req->send(400,"application/json","{\"ok\":false}");return;}
  const int li=req->arg("list").toInt();
  if(li>=PL_MAX||(li>=0&&pls[li].n==0)){req->send(409,"application/json","{\"ok\":false,\"error\":\"empty\"}");return;}
  MzCmd c={};c.type=MZ_CMD_PL_USE;c.value=(uint8_t)(li<0?0:li);c.flag=(li<0)?1:0;
  if(!enqueueCmd(c)){sendBusy(req);return;}
  req->send(200,"application/json","{\"ok\":true}");
}
void handlePlPlay(AsyncWebServerRequest *req){
  if(!req->hasArg("list")){req->send(400,"application/json","{\"ok\":false}");return;}
  const int li=req->arg("list").toInt();
  const int si=req->hasArg("slot")?req->arg("slot").toInt():0;
  const bool hold=req->hasArg("hold")&&req->arg("hold").toInt()!=0;
  if(li<0||li>=PL_MAX||pls[li].n==0||si<0||si>=PL_SLOTS){req->send(409,"application/json","{\"ok\":false,\"error\":\"empty\"}");return;}
  MzCmd c={};c.type=MZ_CMD_PL_PLAY;c.value=(uint8_t)li;c.flag=(uint8_t)si|(hold?0x80:0);
  if(!enqueueCmd(c)){sendBusy(req);return;}
  req->send(200,"application/json","{\"ok\":true}");
}
void handlePlPause(AsyncWebServerRequest *req){
  const bool on=!req->hasArg("on")||req->arg("on").toInt()!=0;
  MzCmd c={};c.type=MZ_CMD_PL_PAUSE;c.flag=on?1:0;
  if(!enqueueCmd(c)){sendBusy(req);return;}
  req->send(200,"application/json","{\"ok\":true}");
}
void handlePlStop(AsyncWebServerRequest *req){
  MzCmd c={};c.type=MZ_CMD_PL_STOP;
  if(!enqueueCmd(c)){sendBusy(req);return;}
  req->send(200,"application/json","{\"ok\":true}");
}

/*
 * Startup R6:
 * - beginStrip() WLED sudah mengantrekan bootPreset melalui applyPreset().
 * - Jika Boot Preset native bernama "Mizuma Welcoming", konsumsi antrean itu
 *   di sini memakai handler preset bawaan WLED, lalu render sebelum Wi-Fi.
 * - Jika belum ada / namanya berbeda, gunakan slot Welcoming Mizuma sebagai
 *   fallback. Boot preset lain tidak dibiarkan menimpa fallback.
 * - Tidak ada ketergantungan Wi-Fi/HTTP dan tidak mengubah sistem palette.
 */
void startWelcomeSequence(){
  // Jadwalkan welcome dari loop tanpa blokir startup Wi-Fi/WLED.
  usingNativeWelcomePreset = false;

  if (bootPreset > 0) {
   // Diagnostik tertunda untuk memastikan pesan muncul setelah USB Serial siap.
  static bool diagnosticPrinted = false;

  if (!diagnosticPrinted && millis() >= 5000) {
    diagnosticPrinted = true;

    DEBUG_PRINTF_P(
      PSTR("[Mizuma] Efek WLED: %u/%u\n"),
      (unsigned)showFxResolved,
      (unsigned)MZ_FXN_COUNT
    );

    DEBUG_PRINTLN(F("[Mizuma] Usermod loop aktif"));
  }
  
    String nativeName;
    const bool presetReadable = getPresetName(bootPreset, nativeName);

    // beginStrip() telah memanggil applyPreset(bootPreset, CALL_MODE_INIT).
    // handlePresets() menyelesaikan state tersebut sekarang, sebelum Wi-Fi.
    handlePresets();

    if (presetReadable && nativeName == "Mizuma Welcoming") {
      usingNativeWelcomePreset = true;
      DEBUG_PRINTLN(F("[Mizuma] Native WLED Welcome preset loaded."));
    } else {
      DEBUG_PRINTLN(F("[Mizuma] Native boot preset bukan 'Mizuma Welcoming'; memakai fallback slot Mizuma."));
    }
  }

  if (!usingNativeWelcomePreset) {
    // beginStrip() mengawali WLED dengan bri=0 pada mode tertentu.
    // Pulihkan brightness Mizuma hanya untuk fallback; native preset memiliki
    // brightness sendiri sehingga tidak ditimpa.
    bri = savedBri ? savedBri : 180;
    applyModeSide("welcoming", "both");
  }

  // Hanya jadwalkan frame awal; render diteruskan oleh strip.service() normal WLED.
  strip.trigger();
  bootStage = 1;
  bootDone = false;
  bootT = millis(); // awal durasi dihitung sesudah render frame Welcome dijadwalkan
  tWelcomeMs = bootT;
}


public:
void setup() override {
plLoadFile();
loadSkinFile();
loadSolidColorFile();
if(ridingPl>=PL_MAX||(ridingPl>=0&&pls[ridingPl].n==0))ridingPl=-1;
cmdQueue=xQueueCreate(MZ_CMD_QUEUE_LEN,sizeof(MzCmd)); // harus ada sebelum route HTTP didaftarkan
tSetupMs=millis();
showFxResolved=resolveShowEffects();
DEBUG_PRINTF_P(PSTR("[Mizuma] %u/%u efek WLED siap untuk 55 show\n"),(unsigned)showFxResolved,(unsigned)MZ_FXN_COUNT);
if(showFxResolved<MZ_FXN_COUNT)DEBUG_PRINTLN(F("[Mizuma] PERINGATAN: beberapa tahap akan dilewati karena nama FX tidak ditemukan pada build ini."));
apBehavior=AP_BEHAVIOR_ALWAYS;
bootWelcomePending=true;
bootWelcomeAt=millis();
DEBUG_PRINTLN(F("[Mizuma] Usermod utama siap"));
#ifndef MZ_NO_UI
// UI hanya diberi cara membaca data kendaraan; selebihnya UI berbicara ke engine lewat API HTTP di bawah.
mizumaUiRegister(server,[this](){MizumaVehicleInfo v;v.name=vehicleName;v.brand=vehicleBrand;v.plate=vehiclePlate;v.year=vehicleYear;return v;});
#endif
server.on("/mizuma/status",HTTP_GET,[this](AsyncWebServerRequest *req){
  if(draftActive)draftBeatAt=millis(); // polling status = sinyal hidup dari UI selama pratinjau
  const uint8_t wifiMode = WiFi.getMode();
  const bool apOn = ((wifiMode & WIFI_AP) != 0);
  const bool staOn = (WiFi.status() == WL_CONNECTED);
  const String staIP = staOn ? WiFi.localIP().toString() : String("");
  const String ssid = staOn ? WiFi.SSID() : String("Mizuma Smart System");

  // Susun JSON tanpa literal escape quote agar aman saat file dipindahkan/disalin.
  String json = "{";
  json += char(34); json += "ap"; json += char(34); json += ':';
  json += (apOn ? "true" : "false");
  json += ',';
  json += char(34); json += "sta"; json += char(34); json += ':';
  json += (staOn ? "true" : "false");
  json += ',';
  json += char(34); json += "staIP"; json += char(34); json += ':';
  json += char(34); json += jsonEscape(staIP); json += char(34);
  json += ',';
  json += char(34); json += "ssid"; json += char(34); json += ':';
  json += char(34); json += jsonEscape(ssid); json += char(34);
  json += '}';

  req->send(200, "application/json", json);
});
server.on("/mizuma/debug",HTTP_GET,[this](AsyncWebServerRequest *req){
char buf[448];snprintf(buf,sizeof(buf),"{\"heap\":%u,\"minHeap\":%u,\"maxAlloc\":%u,\"setupMs\":%u,\"welcomeMs\":%u,\"nowMs\":%u,\"showFxResolved\":%u,\"maxShowMs\":%u,\"wifiMode\":%u,\"wifiStatus\":%d,\"apClients\":%u,\"bootWelcomePending\":%s,\"bootDone\":%s,\"resetReason\":%d}",
(unsigned)ESP.getFreeHeap(),(unsigned)ESP.getMinFreeHeap(),(unsigned)ESP.getMaxAllocHeap(),(unsigned)tSetupMs,(unsigned)tWelcomeMs,(unsigned)millis(),(unsigned)showFxResolved,(unsigned)MZ_SHOW_MAX_DURATION_MS,(unsigned)WiFi.getMode(),(int)WiFi.status(),(unsigned)WiFi.softAPgetStationNum(),bootWelcomePending?"true":"false",bootDone?"true":"false",(int)esp_reset_reason());
req->send(200,"application/json",buf);});
server.on("/mizuma/shows",HTTP_GET,[this](AsyncWebServerRequest *req){
String out;out.reserve(1800);out+="[";for(uint8_t i=0;i<MZ_SHOW_COUNT;i++){if(i)out+=",";out+='"';out+=jsonEscape(String(MZ_SHOWS[i].name));out+='"';}out+="]";req->send(200,"application/json",out);});
server.on("/mizuma/show-durations",HTTP_GET,[this](AsyncWebServerRequest *req){
String out;out.reserve(500);out+="[";for(uint8_t i=0;i<MZ_SHOW_COUNT;i++){if(i)out+=",";out+=String(MZ_SHOWS[i].durationMs);}out+="]";req->send(200,"application/json",out);});
// PENTING: ESPAsyncWebServer mencocokkan URL berdasarkan awalan "/path/". Rute "/mizuma/show" juga menangkap
// "/mizuma/show/reverse|palette|stop" bila terdaftar lebih dulu. Karena itu rute turunan HARUS didaftarkan sebelum "/mizuma/show".
server.on("/mizuma/show/reverse",HTTP_GET,[this](AsyncWebServerRequest *req){handleShowReverse(req,true);});
server.on("/mizuma/show/reverse",HTTP_POST,[this](AsyncWebServerRequest *req){handleShowReverse(req,false);});
server.on("/mizuma/show/palette",HTTP_GET,[this](AsyncWebServerRequest *req){handleShowPalette(req);});
server.on("/mizuma/show/palette",HTTP_POST,[this](AsyncWebServerRequest *req){handleShowPalette(req);});
server.on("/mizuma/show/stop",HTTP_GET,[this](AsyncWebServerRequest *req){handleShowStop(req);});
server.on("/mizuma/show/stop",HTTP_POST,[this](AsyncWebServerRequest *req){handleShowStop(req);});
server.on("/mizuma/show",HTTP_GET,[this](AsyncWebServerRequest *req){handleShow(req);});
server.on("/mizuma/show",HTTP_POST,[this](AsyncWebServerRequest *req){handleShow(req);});
server.on("/mizuma/preset",HTTP_GET,[this](AsyncWebServerRequest *req){handlePreset(req);});
server.on("/mizuma/preset",HTTP_POST,[this](AsyncWebServerRequest *req){handlePreset(req);});
server.on("/mizuma/apply",HTTP_GET,[this](AsyncWebServerRequest *req){handleApply(req);});
server.on("/mizuma/apply",HTTP_POST,[this](AsyncWebServerRequest *req){handleApply(req);});
server.on("/mizuma/side-swap",HTTP_GET,[this](AsyncWebServerRequest *req){String json=String("{\"enabled\":")+(sideSwap?"true":"false")+"}";req->send(200,"application/json",json);});
server.on("/mizuma/side-swap",HTTP_POST,[this](AsyncWebServerRequest *req){handleSideSwap(req);});
server.on("/mizuma/riding/mode",HTTP_GET,[this](AsyncWebServerRequest *req){handleRidingMode(req,true);});
server.on("/mizuma/riding/mode",HTTP_POST,[this](AsyncWebServerRequest *req){handleRidingMode(req,false);});
server.on("/mizuma/riding/reverse",HTTP_GET,[this](AsyncWebServerRequest *req){handleRidingReverse(req,true);});
server.on("/mizuma/riding/reverse",HTTP_POST,[this](AsyncWebServerRequest *req){handleRidingReverse(req,false);});
server.on("/mizuma/riding/mirror",HTTP_GET,[this](AsyncWebServerRequest *req){handleRidingMirror(req,true);});
server.on("/mizuma/riding/mirror",HTTP_POST,[this](AsyncWebServerRequest *req){handleRidingMirror(req,false);});
server.on("/mizuma/riding/context",HTTP_POST,[this](AsyncWebServerRequest *req){handleRidingContext(req);});
server.on("/mizuma/riding/colors",HTTP_GET,[this](AsyncWebServerRequest *req){handleRidingColors(req,true);});
server.on("/mizuma/riding/colors",HTTP_POST,[this](AsyncWebServerRequest *req){handleRidingColors(req,false);});
server.on("/mizuma/riding/effects",HTTP_GET,[this](AsyncWebServerRequest *req){handleRidingEffects(req,true);});
server.on("/mizuma/riding/effect",HTTP_POST,[this](AsyncWebServerRequest *req){handleRidingEffects(req,false);});
server.on("/mizuma/comp/preview",HTTP_POST,[this](AsyncWebServerRequest *req){handleCompPreview(req);});
server.on("/mizuma/comp/play",HTTP_POST,[this](AsyncWebServerRequest *req){handleCompPlay(req);});
server.on("/mizuma/comp/end",HTTP_POST,[this](AsyncWebServerRequest *req){handleCompEnd(req);});
server.on("/mizuma/bri",HTTP_GET,[this](AsyncWebServerRequest *req){handleBri(req);});
server.on("/mizuma/bri",HTTP_POST,[this](AsyncWebServerRequest *req){handleBri(req);});
server.on("/mizuma/playlists",HTTP_GET,[this](AsyncWebServerRequest *req){handlePlSummary(req);});
server.on("/mizuma/pl/list",HTTP_GET,[this](AsyncWebServerRequest *req){handlePlList(req);});
server.on("/mizuma/pl/state",HTTP_GET,[this](AsyncWebServerRequest *req){handlePlState(req);});
server.on("/mizuma/skins/delete",HTTP_POST,[this](AsyncWebServerRequest *req){handleSkinDelete(req);});
server.on("/mizuma/colors",HTTP_GET,[this](AsyncWebServerRequest *req){handleSolidColors(req,true);});
server.on("/mizuma/colors",HTTP_POST,[this](AsyncWebServerRequest *req){handleSolidColors(req,false);},nullptr,
  [](AsyncWebServerRequest *req,uint8_t *data,size_t len,size_t index,size_t total){
    if(total==0||total>512)return;
    if(index==0){req->_tempObject=malloc(total+1);if(req->_tempObject)((char*)req->_tempObject)[0]=0;}
    char *b=(char*)req->_tempObject;if(!b)return;
    memcpy(b+index,data,len);
    if(index+len==total)b[total]=0;
  });
server.on("/mizuma/skins",HTTP_GET,[this](AsyncWebServerRequest *req){handleSkinList(req);});
server.on("/mizuma/skins",HTTP_POST,[this](AsyncWebServerRequest *req){handleSkinSave(req);},nullptr,
  [](AsyncWebServerRequest *req,uint8_t *data,size_t len,size_t index,size_t total){
    if(total==0||total>2048)return;
    if(index==0){req->_tempObject=malloc(total+1);if(req->_tempObject)((char*)req->_tempObject)[0]=0;}
    char *b=(char*)req->_tempObject;if(!b)return;
    memcpy(b+index,data,len);
    if(index+len==total)b[total]=0;
  });
server.on("/mizuma/pl/delete",HTTP_POST,[this](AsyncWebServerRequest *req){handlePlDelete(req);});
server.on("/mizuma/pl/use",HTTP_POST,[this](AsyncWebServerRequest *req){handlePlUse(req);});
server.on("/mizuma/pl/play",HTTP_POST,[this](AsyncWebServerRequest *req){handlePlPlay(req);});
server.on("/mizuma/pl/pause",HTTP_POST,[this](AsyncWebServerRequest *req){handlePlPause(req);});
server.on("/mizuma/pl/stop",HTTP_POST,[this](AsyncWebServerRequest *req){handlePlStop(req);});
// Simpan playlist: body JSON dikumpulkan manual (dibebaskan otomatis oleh server via free() saat request selesai).
server.on("/mizuma/pl/save",HTTP_POST,
  [this](AsyncWebServerRequest *req){handlePlSave(req);},
  nullptr,
  [](AsyncWebServerRequest *req,uint8_t *data,size_t len,size_t index,size_t total){
    if(total==0||total>12000)return;
    if(index==0){req->_tempObject=malloc(total+1);if(req->_tempObject)((char*)req->_tempObject)[0]=0;}
    char *b=(char*)req->_tempObject;if(!b)return;
    memcpy(b+index,data,len);
    if(index+len==total)b[total]=0;
  });
server.on("/mizuma/presets",HTTP_GET,[this](AsyncWebServerRequest *req){
String out;out.reserve(2600);out+="{";
for(int i=0;i<10;i++){PresetSlot p=getEffectiveSlot(i);char buf[340];
snprintf(buf,sizeof(buf),"%s\"%s\":{\"valid\":%s,\"fx\":%u,\"pal\":%u,\"isShow\":%s,\"showIndex\":%u,\"col\":[[%u,%u,%u],[%u,%u,%u],[%u,%u,%u]],\"sx\":%u,\"ix\":%u,\"c1\":%u,\"c2\":%u,\"c3\":%u,\"o1\":%u,\"o2\":%u,\"o3\":%u,\"bri\":%u}",
i?",":"",slotKey(i),pslots[i].valid?"true":"false",(unsigned)p.fx,(unsigned)p.pal,p.isShow?"true":"false",(unsigned)p.showIndex,
(unsigned)p.r,(unsigned)p.g,(unsigned)p.b,(unsigned)p.r1,(unsigned)p.g1,(unsigned)p.b1,(unsigned)p.r2,(unsigned)p.g2,(unsigned)p.b2,
(unsigned)p.sx,(unsigned)p.ix,(unsigned)p.c1,(unsigned)p.c2,(unsigned)p.c3,(unsigned)p.o1,(unsigned)p.o2,(unsigned)p.o3,(unsigned)p.bri);
out+=buf;}
out+="}";req->send(200,"application/json",out);});}

void loop() override {
  if(!ridingSegmentsReady && ensureRidingSegments())applyRidingLayerMode(savedRidingMode);
  processCmdQueue(); // satu-satunya tempat perubahan Segment dari web dijalankan (task yang sama dengan strip.service())
  if(draftActive&&(uint32_t)(millis()-draftBeatAt)>MZ_DRAFT_TIMEOUT_MS)compDraftEnd(true); // pratinjau ditinggalkan: lampu kembali ke data tersimpan
  enforceRidingColorLayers();
  enforceRidingEffectLayers();
  tickPlaylist();
  tickShowEngine();
  if(bootWelcomePending && (uint32_t)(millis()-bootWelcomeAt)>=250U){
    if(!ridingSegmentsReady)return;
    bootWelcomePending=false;
    startWelcomeSequence();
  }
  if(plSavePending&&(uint32_t)(millis()-plSaveT)>1500U){plSavePending=false;plSaveFile();}
  if(cfgSavePending && (uint32_t)(millis() - cfgSaveT) > 1500U){
    cfgSavePending=false;
    serializeConfigToFS();
  }
  if(bootDone || bootStage != 1) return;

  bool welcomeFinished=false;
  if(usingNativeWelcomePreset){
    // Preset native WLED tidak memiliki metadata durasi dari generator Mizuma.
    welcomeFinished=((uint32_t)(millis()-bootT)>=NATIVE_WELCOME_FALLBACK_MS);
  }else{
    const bool hasShow=showRunner[0].hasCurrentShow||showRunner[1].hasCurrentShow;
    welcomeFinished=hasShow?(!showRunner[0].active&&!showRunner[1].active)
                           :((uint32_t)(millis()-bootT)>=NATIVE_WELCOME_FALLBACK_MS);
  }
  if(welcomeFinished){
    ridingMode=savedRidingMode;
    applyStoredRidingMode(savedRidingMode,4); // pulihkan snapshot mode tersimpan; fallback ke preset lama bila belum dimigrasikan
    bootStage = 2;
    bootDone = true;
  }
}

void addToConfig(JsonObject &root) override {
JsonObject top=root.createNestedObject("Mizuma");
JsonObject vehicle=top.createNestedObject("vehicle");
vehicle["name"]=vehicleName;vehicle["brand"]=vehicleBrand;vehicle["year"]=vehicleYear;vehicle["plate"]=vehiclePlate;
JsonObject rem=top.createNestedObject("reminder");
rem["oliMesin_last"]=oliMesin.lastServiceEpoch;rem["oliMesin_int"]=oliMesin.intervalDays;
rem["oliRem_last"]=oliRem.lastServiceEpoch;rem["oliRem_int"]=oliRem.intervalDays;
rem["oliGardan_last"]=oliGardan.lastServiceEpoch;rem["oliGardan_int"]=oliGardan.intervalDays;
rem["cvt_last"]=cvt.lastServiceEpoch;rem["cvt_int"]=cvt.intervalDays;
rem["filter_last"]=filter.lastServiceEpoch;rem["filter_int"]=filter.intervalDays;
JsonObject pm=top.createNestedObject("presets");
for(int i=0;i<10;i++){JsonObject s=pm.createNestedObject(slotKey(i));
s["valid"]=pslots[i].valid;s["isShow"]=pslots[i].isShow;s["showIndex"]=pslots[i].showIndex;s["fx"]=pslots[i].fx;s["pal"]=pslots[i].pal;
s["r"]=pslots[i].r;s["g"]=pslots[i].g;s["b"]=pslots[i].b;
s["r1"]=pslots[i].r1;s["g1"]=pslots[i].g1;s["b1"]=pslots[i].b1;
s["r2"]=pslots[i].r2;s["g2"]=pslots[i].g2;s["b2"]=pslots[i].b2;
s["sx"]=pslots[i].sx;s["ix"]=pslots[i].ix;
s["c1"]=pslots[i].c1;s["c2"]=pslots[i].c2;s["c3"]=pslots[i].c3;
s["o1"]=pslots[i].o1;s["o2"]=pslots[i].o2;s["o3"]=pslots[i].o3;
s["bri"]=pslots[i].bri;}
top["bri"]=savedBri;
top["sideSwap"]=sideSwap;
top["showReverse"]=showReverse;
JsonArray reverse=top.createNestedArray("ridingReverse");
for(uint8_t mode=0;mode<3;mode++){JsonArray sides=reverse.createNestedArray();sides.add((draftActive?draftBkReverse:ridingReverse)[mode][0]);sides.add((draftActive?draftBkReverse:ridingReverse)[mode][1]);}
top["ridingMirror"]=draftActive?draftBkMirror:ridingMirror;top["ridingColorJoin"]=draftActive?draftBkJoin:ridingColorJoin;
top["ridingMode"]=(uint8_t)savedRidingMode;
top["ridingPl"]=(int)ridingPl;
JsonArray re=top.createNestedArray("ridingEffects");for(uint8_t m=0;m<3;m++){JsonArray sides=re.createNestedArray();for(uint8_t k=0;k<2;k++){const RidingEffectConfig &e=(draftActive?draftBkEffects:ridingEffects)[m][k];JsonObject o=sides.createNestedObject();o["valid"]=e.valid;o["isShow"]=e.isShow;o["showIndex"]=e.showIndex;o["fx"]=e.fx;o["sx"]=e.sx;o["ix"]=e.ix;o["c1"]=e.c1;o["c2"]=e.c2;o["c3"]=e.c3;o["o1"]=e.o1;o["o2"]=e.o2;o["o3"]=e.o3;}}
JsonObject rc=top.createNestedObject("ridingColors");
JsonObject rr=rc.createNestedObject("right");rr["palette"]=(draftActive?draftBkColors:ridingColors)[0][0].palette;rr["pal"]=(draftActive?draftBkColors:ridingColors)[0][0].pal;rr["r"]=(draftActive?draftBkColors:ridingColors)[0][0].r;rr["g"]=(draftActive?draftBkColors:ridingColors)[0][0].g;rr["b"]=(draftActive?draftBkColors:ridingColors)[0][0].b;
JsonObject rl=rc.createNestedObject("left");rl["palette"]=(draftActive?draftBkColors:ridingColors)[0][1].palette;rl["pal"]=(draftActive?draftBkColors:ridingColors)[0][1].pal;rl["r"]=(draftActive?draftBkColors:ridingColors)[0][1].r;rl["g"]=(draftActive?draftBkColors:ridingColors)[0][1].g;rl["b"]=(draftActive?draftBkColors:ridingColors)[0][1].b;
JsonArray rmc=top.createNestedArray("ridingModeColors");
for(uint8_t m=0;m<3;m++){JsonArray sides=rmc.createNestedArray();for(uint8_t k=0;k<2;k++){const RidingColor &c=(draftActive?draftBkColors:ridingColors)[m][k];JsonObject co=sides.createNestedObject();co["palette"]=c.palette;co["pal"]=c.pal;co["r"]=c.r;co["g"]=c.g;co["b"]=c.b;}}}

bool readFromConfig(JsonObject &root) override {
JsonObject top=root["Mizuma"];if(top.isNull())return false;
JsonObject vehicle=top["vehicle"];
vehicleName=vehicle["name"]|"";vehicleBrand=vehicle["brand"]|"";vehicleYear=vehicle["year"]|0;vehiclePlate=vehicle["plate"]|"";
JsonObject rem=top["reminder"];
oliMesin.lastServiceEpoch=rem["oliMesin_last"]|0;oliMesin.intervalDays=rem["oliMesin_int"]|0;
oliRem.lastServiceEpoch=rem["oliRem_last"]|0;oliRem.intervalDays=rem["oliRem_int"]|0;
oliGardan.lastServiceEpoch=rem["oliGardan_last"]|0;oliGardan.intervalDays=rem["oliGardan_int"]|0;
cvt.lastServiceEpoch=rem["cvt_last"]|0;cvt.intervalDays=rem["cvt_int"]|0;
filter.lastServiceEpoch=rem["filter_last"]|0;filter.intervalDays=rem["filter_int"]|0;
JsonObject pm=top["presets"];
if(!pm.isNull()){for(int i=0;i<10;i++){JsonObject s=pm[slotKey(i)];if(s.isNull())continue;
pslots[i].valid=s["valid"]|false;
const bool hadShowField=s.containsKey("isShow");pslots[i].isShow=s["isShow"]|false;pslots[i].showIndex=(uint8_t)constrain((int)(s["showIndex"]|0),0,MZ_SHOW_COUNT-1);
pslots[i].fx=s["fx"]|0;pslots[i].pal=s["pal"]|0;
// Slot Welcoming lama menyimpan FX custom dengan ID runtime yang sudah tidak dipakai. Migrasikan ke MZ01.
if(i<=1&&pslots[i].valid&&!hadShowField){pslots[i].isShow=true;pslots[i].showIndex=0;pslots[i].fx=0;}
pslots[i].r=s["r"]|255;pslots[i].g=s["g"]|255;pslots[i].b=s["b"]|255;
pslots[i].r1=s["r1"]|0;pslots[i].g1=s["g1"]|0;pslots[i].b1=s["b1"]|0;
pslots[i].r2=s["r2"]|0;pslots[i].g2=s["g2"]|0;pslots[i].b2=s["b2"]|0;
pslots[i].sx=s["sx"]|128;pslots[i].ix=s["ix"]|128;
pslots[i].c1=s["c1"]|128;pslots[i].c2=s["c2"]|128;pslots[i].c3=s["c3"]|128;
pslots[i].o1=s["o1"]|0;pslots[i].o2=s["o2"]|0;pslots[i].o3=s["o3"]|0;
pslots[i].bri=s["bri"]|180;}}
savedBri=top["bri"]|180;
sideSwap=top["sideSwap"]|false;
showReverse=top["showReverse"]|false;
JsonArray reverse=top["ridingReverse"];
if(!reverse.isNull()){
  for(uint8_t mode=0;mode<3;mode++){
    JsonArray sides=reverse[mode];
    if(!sides.isNull()){ridingReverse[mode][0]=sides[0]|false;ridingReverse[mode][1]=sides[1]|false;}
  }
}
ridingMirror=top["ridingMirror"]|false;
ridingColorJoin=top["ridingColorJoin"]|false;
savedRidingMode=(RidingMode)constrain((int)(top["ridingMode"]|RIDING_MANUAL),(int)RIDING_MANUAL,(int)RIDING_PLAYLIST);
ridingMode=savedRidingMode;
JsonArray re=top["ridingEffects"];
if(!re.isNull()){for(uint8_t m=0;m<3;m++){JsonArray sides=re[m];for(uint8_t k=0;k<2;k++){JsonObject o=sides[k];if(o.isNull())continue;RidingEffectConfig &e=ridingEffects[m][k];e.valid=o["valid"]|false;e.isShow=o["isShow"]|false;e.showIndex=(uint8_t)constrain((int)(o["showIndex"]|0),0,MZ_SHOW_COUNT-1);e.fx=o["fx"]|0;e.sx=o["sx"]|128;e.ix=o["ix"]|128;e.c1=o["c1"]|128;e.c2=o["c2"]|128;e.c3=o["c3"]|128;e.o1=o["o1"]|0;e.o2=o["o2"]|0;e.o3=o["o3"]|0;}}}
if(ridingEffects[1][0].valid)ridingEffects[1][1]=ridingEffects[1][0];if(ridingEffects[2][0].valid)ridingEffects[2][1]=ridingEffects[2][0];
JsonObject rc=top["ridingColors"];
if(!rc.isNull()){
  JsonObject rr=rc["right"];if(!rr.isNull()){ridingColors[0][0].palette=rr["palette"]|true;ridingColors[0][0].pal=rr["pal"]|2;ridingColors[0][0].r=rr["r"]|255;ridingColors[0][0].g=rr["g"]|255;ridingColors[0][0].b=rr["b"]|255;}
  JsonObject rl=rc["left"];if(!rl.isNull()){ridingColors[0][1].palette=rl["palette"]|true;ridingColors[0][1].pal=rl["pal"]|2;ridingColors[0][1].r=rl["r"]|255;ridingColors[0][1].g=rl["g"]|255;ridingColors[0][1].b=rl["b"]|255;}
}
bool migratedLegacyConfig=false;
JsonArray rmc=top["ridingModeColors"];
if(!rmc.isNull()){for(uint8_t m=0;m<3;m++){JsonArray sides=rmc[m];for(uint8_t k=0;k<2;k++){JsonObject co=sides[k];if(co.isNull())continue;RidingColor &c=ridingColors[m][k];c.palette=co["palette"]|true;c.pal=co["pal"]|2;c.r=co["r"]|255;c.g=co["g"]|255;c.b=co["b"]|255;}}}
else{
  // Migrasi warna lama yang sebelumnya dipakai bersama oleh semua mode.
  ridingColors[1][0]=ridingColors[0][0];ridingColors[1][1]=ridingColors[0][1];
  ridingColors[2][0]=ridingColors[0][0];ridingColors[2][1]=ridingColors[0][1];
  migratedLegacyConfig=true;
}
ridingColors[2][1]=ridingColors[2][0];
const bool hasRidingMode=top.containsKey("ridingMode");
ridingPl=(int8_t)((int)(top["ridingPl"]|-1));
if(!hasRidingMode&&ridingPl>=0){savedRidingMode=RIDING_PLAYLIST;migratedLegacyConfig=true;}
ridingMode=savedRidingMode;
bool migratedLegacyEffects=false;
// Konversi preset Riding versi lama ke snapshot Manual tanpa mengubah preset asal.
for(uint8_t side=0;side<2;side++){
  const PresetSlot &src=pslots[2+side];RidingEffectConfig &dst=ridingEffects[0][side];
  if(!dst.valid&&src.valid){dst.valid=true;dst.isShow=src.isShow;dst.showIndex=src.showIndex;dst.fx=src.fx;
    dst.sx=src.sx;dst.ix=src.ix;dst.c1=src.c1;dst.c2=src.c2;dst.c3=src.c3;dst.o1=src.o1;dst.o2=src.o2;dst.o3=src.o3;migratedLegacyEffects=true;}
}
// Jika mode lama yang aktif Semi Auto atau Auto, bawa efek aktifnya juga ke mode terkait.
const PresetSlot &activeLegacy=pslots[2];
if(activeLegacy.valid&&savedRidingMode==RIDING_SEMI_AUTO&&!activeLegacy.isShow&&!ridingEffects[1][0].valid){
  RidingEffectConfig &dst=ridingEffects[1][0];dst.valid=true;dst.isShow=false;dst.fx=activeLegacy.fx;dst.sx=activeLegacy.sx;dst.ix=activeLegacy.ix;dst.c1=activeLegacy.c1;dst.c2=activeLegacy.c2;dst.c3=activeLegacy.c3;dst.o1=activeLegacy.o1;dst.o2=activeLegacy.o2;dst.o3=activeLegacy.o3;ridingEffects[1][1]=dst;migratedLegacyEffects=true;
}else if(activeLegacy.valid&&savedRidingMode==RIDING_AUTO&&activeLegacy.isShow&&!ridingEffects[2][0].valid){
  RidingEffectConfig &dst=ridingEffects[2][0];dst.valid=true;dst.isShow=true;dst.showIndex=activeLegacy.showIndex;dst.sx=activeLegacy.sx;dst.ix=activeLegacy.ix;dst.c1=activeLegacy.c1;dst.c2=activeLegacy.c2;dst.c3=activeLegacy.c3;dst.o1=activeLegacy.o1;dst.o2=activeLegacy.o2;dst.o3=activeLegacy.o3;ridingEffects[2][1]=dst;migratedLegacyEffects=true;
}
if(migratedLegacyEffects)migratedLegacyConfig=true;
if(migratedLegacyConfig)queueSave();
return true;}

uint16_t getId() override {return USERMOD_ID_MIZUMA_SYSTEM;}
};
static MizumaSmartSystem mizuma_smartsystem;
REGISTER_USERMOD(mizuma_smartsystem);
// ================= AKHIR FILE =================
