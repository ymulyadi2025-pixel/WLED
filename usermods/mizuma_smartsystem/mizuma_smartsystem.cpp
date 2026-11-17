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
//   GET|POST  /mizuma/show            ?index=N&side=S                  jalankan show Welcoming terpilih
//   GET|POST  /mizuma/show/stop       ?side=S
//   GET|POST  /mizuma/show/palette    ?pal=0..255&side=S               0 = palette asli resep show
//   GET|POST  /mizuma/show/reverse    (GET: status)  ?enabled=1|0      arah global
//   GET|POST  /mizuma/apply           ?mode=welcoming|riding|sein|rem|hazard&side=S
//   GET|POST  /mizuma/preset          ?slot=<welcomingKanan|...|hazardKiri>&fx&pal&r&g&b&r1..b2&sx&ix&c1..c3&o1..o3&bri&isShow&showIndex
//   GET       /mizuma/presets         -> semua 10 slot
//   GET       /mizuma/riding/playlists -> 4 playlist + status player
//   POST      /mizuma/riding/playlist/meta|clear|slot|reorder|start|pause|stop
//   GET|POST  /mizuma/side-swap       (GET: status)  ?enabled=1|0
//   GET|POST  /mizuma/bri             ?v=1..255                        brightness permanen
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

    // Playlist Riding disimpan di konfigurasi ESP32 (maks. 4 playlist x 12 slot).
    static const uint8_t MZ_RIDE_PLAYLIST_COUNT = 4;
    static const uint8_t MZ_RIDE_PLAYLIST_SLOTS = 12;
    struct RidePlaylistSlot {
      bool valid = false;
      bool isShow = false;
      uint8_t showIndex = 0;
      uint8_t fx = 0, pal = 0;
      uint8_t r = 255, g = 165, b = 0;
      uint8_t r1 = 255, g1 = 255, b1 = 255;
      uint8_t r2 = 255, g2 = 26, b2 = 0;
      uint8_t sx = 128, ix = 128;
      uint8_t c1 = 128, c2 = 128, c3 = 128;
      uint8_t o1 = 0, o2 = 0, o3 = 0;
      uint8_t bri = 180;
      uint16_t durationSec = 20;
    };
    struct RidePlaylist {
      char name[25] = {0};
      uint8_t count = 0;
      RidePlaylistSlot slots[MZ_RIDE_PLAYLIST_SLOTS];
    };
    struct RidePlaylistRunner {
      bool active = false, paused = false;
      uint8_t playlist = 0, slot = 0, side = 0;
      uint32_t slotStartedAt = 0, pausedAt = 0;
    };
    RidePlaylist ridingPlaylists[MZ_RIDE_PLAYLIST_COUNT];
    RidePlaylistRunner rideRunner;

    static const uint16_t NATIVE_WELCOME_FALLBACK_MS = 7000; // hanya untuk preset boot WLED native, bukan show Mizuma
    int bootStage = 0;
    uint32_t bootT = 0;
    uint32_t tSetupMs = 0, tWelcomeMs = 0;   // diagnostik waktu boot (lihat /mizuma/debug)
    uint8_t  showFxResolved = 0;              // jumlah nama efek WLED yang berhasil dipetakan
    uint8_t  showModeIds[MZ_FXN_COUNT];       // indeks aktual efek WLED, di-resolve saat boot
    struct ShowRunner { bool active=false; bool hasCurrentShow=false; bool logicalRight=true; uint8_t showIndex=0; uint8_t stageIndex=0; uint8_t currentStageIndex=255; uint8_t userPalette=0; uint8_t expectedMode=255; uint32_t startedAt=0; };
    ShowRunner showRunner[2];                  // 0=Kanan, 1=Kiri (logis; dipetakan melalui sideSwap)
    bool bootDone = false;
    bool usingNativeWelcomePreset = false;
    bool sideSwap = false; // true: pemetaan logis Kanan/Kiri ke segmen fisik ditukar
    bool showReverse = false; // arah gerak global semua tab; stage Mizuma tetap mempertahankan flip resep via XOR

    /* ===== ANTREAN PERINTAH WEB -> loop() =====
       Handler HTTP berjalan di task async_tcp, sedangkan strip.service() berjalan di task loop.
       FreeRTOS dapat menyela strip.service() di tengah frame, sehingga handler TIDAK BOLEH mengubah Segment,
       showRunner[], atau memanggil deallocateData() (WLED: reset/dealokasi tidak boleh terjadi saat efek berjalan).
       Aturan: handler hanya memvalidasi argumen lalu menitipkan perintah; processCmdQueue() di loop() adalah
       satu-satunya tempat yang mengeksekusi perubahan. loop() berjalan sebelum strip.service() pada task yang sama. */
    enum MzCmdType : uint8_t { MZ_CMD_NONE=0, MZ_CMD_SHOW_START, MZ_CMD_SHOW_STOP, MZ_CMD_SHOW_PALETTE, MZ_CMD_DIRECTION, MZ_CMD_APPLY, MZ_CMD_SIDE_SWAP, MZ_CMD_RIDE_PLAY_START, MZ_CMD_RIDE_PLAY_PAUSE, MZ_CMD_RIDE_PLAY_STOP };
    struct MzCmd { uint8_t type; uint8_t side; uint8_t value; uint8_t flag; char mode[10]; };
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
    
    void addSegmentFromSlot(JsonArray seg,uint8_t segId,uint8_t slotIdx){
    PresetSlot p=getEffectiveSlot(slotIdx);
    JsonObject s=seg.createNestedObject();
    s["id"]=segId;s["on"]=true;s["fx"]=p.fx;s["pal"]=p.pal;s["rev"]=showReverse;s["sx"]=p.sx;s["ix"]=p.ix;
    s["c1"]=p.c1;s["c2"]=p.c2;s["c3"]=p.c3;s["o1"]=p.o1;s["o2"]=p.o2;s["o3"]=p.o3;
JsonArray col=s.createNestedArray("col");
JsonArray c0=col.createNestedArray();c0.add(p.r);c0.add(p.g);c0.add(p.b);
JsonArray c1=col.createNestedArray();c1.add(p.r1);c1.add(p.g1);c1.add(p.b1);
JsonArray c2=col.createNestedArray();c2.add(p.r2);c2.add(p.g2);c2.add(p.b2);}
uint8_t physicalSegForLogicalSide(bool rightSide){return (rightSide != sideSwap)?0:1;}
void applySingleSlot(uint8_t slotIdx){
bool rightSide=(slotIdx%2)==0;uint8_t segId=physicalSegForLogicalSide(rightSide);
StaticJsonDocument<1024> doc;JsonObject root=doc.to<JsonObject>();
root["on"]=true;if(bri==0)root["bri"]=savedBri;
JsonArray seg=root.createNestedArray("seg");
addSegmentFromSlot(seg,segId,slotIdx);
deserializeState(root);}
void applyTwoSlots(uint8_t rightSlot,uint8_t leftSlot,uint16_t transition100ms=0){
StaticJsonDocument<2048> doc;JsonObject root=doc.to<JsonObject>();
root["on"]=true;root["transition"]=transition100ms;if(bri==0)root["bri"]=savedBri;
JsonArray seg=root.createNestedArray("seg");
addSegmentFromSlot(seg,physicalSegForLogicalSide(true),rightSlot);
addSegmentFromSlot(seg,physicalSegForLogicalSide(false),leftSlot);
deserializeState(root);}
int modeBase(const String &mode){
if(mode=="welcoming")return 0;if(mode=="riding")return 2;if(mode=="sein")return 4;
if(mode=="rem")return 6;if(mode=="hazard")return 8;return -1;}
bool applyModeSide(const String &mode,const String &side,uint16_t transition100ms=0){
rideRunner.active=false;rideRunner.paused=false;
if(mode=="welcoming")return applyWelcomeModeSide(side);
stopShowRunners("both");
int base=modeBase(mode);if(base<0)return false;
if(mode=="riding"){
  if(side!="both"&&side!="kanan"&&side!="kiri")return false;
  StaticJsonDocument<2048> doc;JsonObject root=doc.to<JsonObject>();root["on"]=true;root["transition"]=transition100ms;
  if(bri==0)root["bri"]=savedBri;
  JsonArray seg=root.createNestedArray("seg");
  const bool useRight=(side=="both"||side=="kanan"),useLeft=(side=="both"||side=="kiri");
  PresetSlot rightSlot,leftSlot;
  if(useRight){rightSlot=getEffectiveSlot(2);addSegmentFromSlot(seg,physicalSegForLogicalSide(true),2);}
  if(useLeft){leftSlot=getEffectiveSlot(3);addSegmentFromSlot(seg,physicalSegForLogicalSide(false),3);}
  deserializeState(root);
  if(useRight&&rightSlot.isShow)startShowRunner(true,rightSlot.showIndex);
  if(useLeft&&leftSlot.isShow)startShowRunner(false,leftSlot.showIndex);
  tickShowEngine();
  return true;
}
if(side=="both"){applyTwoSlots(base,base+1,transition100ms);return true;}
if(side=="kanan"){applySingleSlot(base);return true;}
if(side=="kiri"){applySingleSlot(base+1);return true;}
return false;}

// ---------------- RIDING PLAYLIST ENGINE ----------------
void addRideSegment(JsonArray seg,uint8_t segId,const RidePlaylistSlot &p){
  JsonObject s=seg.createNestedObject();
  s["id"]=segId;s["on"]=true;s["fx"]=p.fx;s["pal"]=p.pal;s["rev"]=showReverse;
  s["sx"]=p.sx;s["ix"]=p.ix;s["c1"]=p.c1;s["c2"]=p.c2;s["c3"]=p.c3;
  s["o1"]=p.o1;s["o2"]=p.o2;s["o3"]=p.o3;
  JsonArray col=s.createNestedArray("col");
  JsonArray c0=col.createNestedArray();c0.add(p.r);c0.add(p.g);c0.add(p.b);
  JsonArray c1=col.createNestedArray();c1.add(p.r1);c1.add(p.g1);c1.add(p.b1);
  JsonArray c2=col.createNestedArray();c2.add(p.r2);c2.add(p.g2);c2.add(p.b2);
}
void applyRidePlaylistSlot(){
  if(!rideRunner.active||rideRunner.playlist>=MZ_RIDE_PLAYLIST_COUNT)return;
  RidePlaylist &pl=ridingPlaylists[rideRunner.playlist];
  if(!pl.count||rideRunner.slot>=pl.count)return;
  const RidePlaylistSlot &p=pl.slots[rideRunner.slot];
  if(!p.valid)return;
  stopShowRunners("both");
  StaticJsonDocument<2048> doc;JsonObject root=doc.to<JsonObject>();
  root["on"]=true;root["transition"]=3;
  if(bri==0)root["bri"]=savedBri;
  JsonArray seg=root.createNestedArray("seg");
  if(rideRunner.side==0){
    addRideSegment(seg,physicalSegForLogicalSide(true),p);
    addRideSegment(seg,physicalSegForLogicalSide(false),p);
  }else if(rideRunner.side==1){
    addRideSegment(seg,physicalSegForLogicalSide(true),p);
  }else{
    addRideSegment(seg,physicalSegForLogicalSide(false),p);
  }
  deserializeState(root);
  if(p.isShow){const String sideName=String(mzSideName(rideRunner.side));startShow((int)p.showIndex,sideName);if(p.pal!=0)execShowPalette(p.pal,sideName);}
}
void startRidePlaylist(uint8_t index,uint8_t side){
  if(index>=MZ_RIDE_PLAYLIST_COUNT||side>2)return;
  RidePlaylist &pl=ridingPlaylists[index];
  if(!pl.count)return;
  stopShowRunners("both");
  rideRunner.active=true;rideRunner.paused=false;rideRunner.playlist=index;rideRunner.slot=0;rideRunner.side=side;
  rideRunner.slotStartedAt=millis();rideRunner.pausedAt=0;
  applyRidePlaylistSlot();
}
void pauseRidePlaylist(bool paused){
  if(!rideRunner.active||rideRunner.paused==paused)return;
  const uint32_t now=millis();
  if(paused){rideRunner.paused=true;rideRunner.pausedAt=now;}
  else{rideRunner.slotStartedAt+=(uint32_t)(now-rideRunner.pausedAt);rideRunner.paused=false;rideRunner.pausedAt=0;}
}
void stopRidePlaylist(bool restoreRiding){
  const uint8_t side=rideRunner.side;
  rideRunner.active=false;rideRunner.paused=false;rideRunner.pausedAt=0;
  stopShowRunners("both");
  if(restoreRiding)applyModeSide("riding",String(mzSideName(side)));
}
void tickRidingPlaylist(){
  if(!rideRunner.active||rideRunner.paused||rideRunner.playlist>=MZ_RIDE_PLAYLIST_COUNT)return;
  RidePlaylist &pl=ridingPlaylists[rideRunner.playlist];
  if(!pl.count){rideRunner.active=false;return;}
  if(rideRunner.slot>=pl.count)rideRunner.slot=0;
  const uint16_t duration=(pl.slots[rideRunner.slot].durationSec<1)?1:pl.slots[rideRunner.slot].durationSec;
  const uint32_t now=millis();
  if((uint32_t)(now-rideRunner.slotStartedAt)>=(uint32_t)duration*1000UL){
    rideRunner.slot=(uint8_t)((rideRunner.slot+1U)%pl.count);
    rideRunner.slotStartedAt=now;
    applyRidePlaylistSlot();
  }
}

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
uint32_t resolveStageColor(uint32_t code,Segment &seg,uint8_t colorIndex,uint8_t userPalette,bool &keep){
  keep=false;
  if((code&0xFF000000UL)==0xFF000000UL){keep=true;return 0;}

  static const uint8_t palettePos[3]={18,128,238};
  const bool useUserPalette=(userPalette!=0);
  bool dim=false;
  uint8_t sample=palettePos[(colorIndex<3)?colorIndex:0];

  if((code&0xFF000000UL)==0xFE000000UL){
    uint8_t slot=(uint8_t)(code&0xFFU);if(slot>2)slot=0;
    dim=(code&0x100UL)!=0;
    if(!useUserPalette){
      uint32_t c=seg.colors[slot];
      if(dim)c=color_fade(c,128,true);
      return c;
    }
    sample=palettePos[slot];
  }else{
    const uint32_t rgb=code&0x00FFFFFFUL;
    // Black remains the designed background; white remains a neutral flash/highlight.
    if(rgb==0x000000UL||rgb==0xFFFFFFUL||!useUserPalette)return rgb;
  }

  uint32_t c=seg.color_from_palette(sample,false,true,0,255)&0x00FFFFFFUL;
  if(dim)c=color_fade(c,128,true)&0x00FFFFFFUL;
  return c;
}

void applyShowStageColors(const MzStage &st,Segment &seg,uint8_t userPalette){
  const bool useUserPalette=(userPalette!=0);
  if(useUserPalette)seg.beginDraw(); // rebuild palette sampling after setPalette()
  uint32_t resolved[3]={0,0,0};
  bool keep[3]={false,false,false};
  for(uint8_t c=0;c<3;c++)resolved[c]=resolveStageColor(st.col[c],seg,c,userPalette,keep[c]);
  for(uint8_t c=0;c<3;c++)if(!keep[c])seg.setColor(c,resolved[c]);
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

void applyShowStage(const MzStage &st,bool logicalRight,uint8_t userPalette){
  if(st.fx>=MZ_FXN_COUNT)return;
  const uint8_t modeId=showModeIds[st.fx];
  if(modeId==255)return; // FX tidak ada pada build WLED ini: lewati tahap secara aman
  const uint8_t segId=physicalSegForLogicalSide(logicalRight);
  if(segId>=strip.getSegmentsNum())return;
  Segment &seg=strip.getSegment(segId);
  if(!seg.isActive())return;

  // userPalette disimpan terpisah dari seg.palette: 0 = Mizuma Original,
  // sedangkan nilai nonzero = palette global pilihan pengguna.
  const uint16_t previousTransition=strip.getTransition();
  const uint16_t stageTransition=(uint16_t)st.tt*10U; // tt disimpan dalam satuan 10 ms
  strip.setTransition(stageTransition);

  if(seg.mode!=modeId)seg.setMode(modeId,true); // muat default parameter efek WLED
  else if(st.t==0)seg.markForReset();
  if(userPalette!=0){
    if(seg.palette!=userPalette)seg.setPalette(userPalette);
  }else{
    const uint8_t recipePalette=(st.pal>=0)?(uint8_t)st.pal:0;
    if(seg.palette!=recipePalette)seg.setPalette(recipePalette); // 0 = default palette efek WLED
  }
  if(st.sx>=0)seg.speed=(uint8_t)constrain(st.sx,0,255);
  if(st.ix>=0)seg.intensity=(uint8_t)constrain(st.ix,0,255);
  if(st.c1>=0)seg.custom1=(uint8_t)constrain(st.c1,0,255);
  if(st.c2>=0)seg.custom2=(uint8_t)constrain(st.c2,0,255);
  if(st.c3>=0)seg.custom3=(uint8_t)constrain(st.c3,0,31); // WLED stores Custom 3 in 5 bits
  if(st.o>=0)seg.check1=(st.o!=0);
  const bool stageReverse=(((st.flags&1U)!=0) ^ showReverse); // resep dan pilihan pengguna digabungkan
  // WLED menyimpan reverse sebagai option bit 1 (REVERSE = 0x0002).
  // Gunakan setter resmi agar stateChanged WLED ikut diperbarui, lalu reset runtime FX.
  if(seg.reverse!=stageReverse){
    seg.setOption(1,stageReverse);
    seg.markForReset();
  }
  seg.on=true;
  showRunner[logicalRight?0:1].expectedMode=modeId; // dipakai untuk mendeteksi perubahan efek dari luar (UI WLED / preset)

  applyShowStageColors(st,seg,userPalette);

  if(st.flags&2U)mzTetrixInstantStart(seg); // lihat catatan isolasi di atas applyShowStage()
  if(st.tb>=0)strip.timebase=(unsigned long)(uint16_t)st.tb-(unsigned long)millis();
  strip.trigger();
  stateUpdated(CALL_MODE_DIRECT_CHANGE);
  strip.setTransition(previousTransition); // restore WLED's global/default transition time
}

void startShowRunner(bool logicalRight,uint8_t showIndex){
  if(showIndex>=MZ_SHOW_COUNT)return;
  ShowRunner &r=showRunner[logicalRight?0:1];
  r.active=true;r.hasCurrentShow=true;r.logicalRight=logicalRight;r.showIndex=showIndex;r.stageIndex=0;r.currentStageIndex=255;r.expectedMode=255;
  const uint8_t segId=physicalSegForLogicalSide(logicalRight);
  r.userPalette=(segId<strip.getSegmentsNum())?strip.getSegment(segId).palette:0;
  r.startedAt=millis();
}
void stopShowRunners(const String &side){
  if(side=="both"||side=="kanan"){showRunner[0].active=false;showRunner[0].hasCurrentShow=false;showRunner[0].currentStageIndex=255;showRunner[0].expectedMode=255;}
  if(side=="both"||side=="kiri"){showRunner[1].active=false;showRunner[1].hasCurrentShow=false;showRunner[1].currentStageIndex=255;showRunner[1].expectedMode=255;}
}
// Jika efek pada segmen berbeda dari yang terakhir dipasang show, berarti pengguna mengganti efek lewat UI WLED,
// preset, atau API lain. Hentikan show agar pilihan itu tidak ditimpa sampai show selesai.
void checkExternalOverride(ShowRunner &r){
  if(!r.hasCurrentShow||r.expectedMode==255)return;
  const uint8_t segId=physicalSegForLogicalSide(r.logicalRight);
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
      if(st.side==0||st.side==(r.logicalRight?1:2)){
        applyShowStage(st,r.logicalRight,r.userPalette);
        r.currentStageIndex=r.stageIndex;
      }
      r.stageIndex++;
    }
    if(elapsed>=show.durationMs)r.active=false; // selesai setelah cue terakhir + waktu penutup show
  }
}
bool startShow(int index,const String &side){
  if(index<0||index>=MZ_SHOW_COUNT)return false;
  if(side!="both"&&side!="kanan"&&side!="kiri")return false;
  stopShowRunners(side); // hentikan show sebelumnya pada sisi yang dipilih
  if(bri==0){bri=savedBri?savedBri:180;stateUpdated(CALL_MODE_DIRECT_CHANGE);}
  if(side=="both"){
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
    const uint8_t segId=physicalSegForLogicalSide(rightSide);
    if(segId>=strip.getSegmentsNum())continue;
    Segment &seg=strip.getSegment(segId);
    if(!seg.isActive())continue;
    ShowRunner &runner=showRunner[logical];
    runner.userPalette=pal;
    const MzStage *currentStage=nullptr;
    if(runner.hasCurrentShow && runner.showIndex<MZ_SHOW_COUNT && runner.currentStageIndex<MZ_SHOWS[runner.showIndex].n){
      currentStage=&MZ_SHOWS[runner.showIndex].st[runner.currentStageIndex];
    }
    const uint8_t effectivePalette=(pal==0 && currentStage && currentStage->pal>=0)?(uint8_t)currentStage->pal:pal;
    seg.setPalette(effectivePalette);
    if(currentStage)applyShowStageColors(*currentStage,seg,pal);
  }
  strip.trigger();
  stateUpdated(CALL_MODE_DIRECT_CHANGE);
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
  // Arah global diterapkan ke kedua segmen aktif pada semua tab.
  // Pada show Mizuma, flag arah khusus stage digabungkan dengan pilihan global (XOR).
  // Reset runtime memastikan efek yang menyimpan state internal membaca arah baru.
  uint8_t activeSegments=0;
  bool changed=false;
  for(uint8_t logical=0;logical<2;logical++){
    const bool logicalRight=(logical==0);
    const uint8_t segId=physicalSegForLogicalSide(logicalRight);
    if(segId>=strip.getSegmentsNum())continue;
    Segment &seg=strip.getSegment(segId);
    if(!seg.isActive())continue;
    activeSegments++;

    bool targetReverse=showReverse;
    ShowRunner &r=showRunner[logical];
    if(r.hasCurrentShow && r.showIndex<MZ_SHOW_COUNT && r.currentStageIndex<MZ_SHOWS[r.showIndex].n){
      const MzStage &st=MZ_SHOWS[r.showIndex].st[r.currentStageIndex];
      targetReverse=(((st.flags&1U)!=0) ^ showReverse);
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
p.showIndex=(i<=3&&p.isShow)?(uint8_t)constrain(req->arg("showIndex").toInt(),0,MZ_SHOW_COUNT-1):0;
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

uint8_t ridePlaylistIndex(AsyncWebServerRequest *req){
  if(!req->hasArg("index"))return 255;
  const int i=req->arg("index").toInt();
  return (i>=0&&i<MZ_RIDE_PLAYLIST_COUNT)?(uint8_t)i:255;
}
void handleRidePlaylists(AsyncWebServerRequest *req){
  String out;out.reserve(12000);out+="{\"playlists\":[";
  for(uint8_t i=0;i<MZ_RIDE_PLAYLIST_COUNT;i++){
    if(i)out+=",";
    RidePlaylist &pl=ridingPlaylists[i];
    out+="{\"name\":\"";out+=jsonEscape(String(pl.name));out+="\",\"count\":";out+=String(pl.count);out+=",\"slots\":[";
    for(uint8_t j=0;j<pl.count;j++){
      if(j)out+=",";
      const RidePlaylistSlot &p=pl.slots[j];
      char b[420];
      snprintf(b,sizeof(b),"{\"fx\":%u,\"isShow\":%s,\"showIndex\":%u,\"pal\":%u,\"col\":[[%u,%u,%u],[%u,%u,%u],[%u,%u,%u]],\"sx\":%u,\"ix\":%u,\"c1\":%u,\"c2\":%u,\"c3\":%u,\"o1\":%u,\"o2\":%u,\"o3\":%u,\"bri\":%u,\"duration\":%u}",
      (unsigned)p.fx,p.isShow?"true":"false",(unsigned)p.showIndex,(unsigned)p.pal,(unsigned)p.r,(unsigned)p.g,(unsigned)p.b,(unsigned)p.r1,(unsigned)p.g1,(unsigned)p.b1,(unsigned)p.r2,(unsigned)p.g2,(unsigned)p.b2,
      (unsigned)p.sx,(unsigned)p.ix,(unsigned)p.c1,(unsigned)p.c2,(unsigned)p.c3,(unsigned)p.o1,(unsigned)p.o2,(unsigned)p.o3,(unsigned)p.bri,(unsigned)p.durationSec);
      out+=b;
    }
    out+="]}";
  }
  out+="],\"active\":";out+=(rideRunner.active?"true":"false");
  out+=",\"paused\":";out+=(rideRunner.paused?"true":"false");
  out+=",\"playlist\":";out+=String(rideRunner.playlist);
  out+=",\"slot\":";out+=String(rideRunner.slot);
  out+=",\"side\":\"";out+=mzSideName(rideRunner.side);out+="\"}";
  req->send(200,"application/json",out);
}
void handleRidePlaylistMeta(AsyncWebServerRequest *req){
  const uint8_t i=ridePlaylistIndex(req);if(i==255){req->send(400,"application/json","{\"ok\":false,\"error\":\"index\"}");return;}
  if(req->hasArg("name")){String n=req->arg("name");n.trim();if(n.length()>24)n=n.substring(0,24);if(!n.length())n="Playlist "+String(i+1);n.toCharArray(ridingPlaylists[i].name,sizeof(ridingPlaylists[i].name));}
  queueSave();req->send(200,"application/json","{\"ok\":true}");
}
void handleRidePlaylistClear(AsyncWebServerRequest *req){
  const uint8_t i=ridePlaylistIndex(req);if(i==255){req->send(400,"application/json","{\"ok\":false}");return;}
  if(rideRunner.active&&rideRunner.playlist==i){MzCmd c={};c.type=MZ_CMD_RIDE_PLAY_STOP;c.flag=1;if(!enqueueCmd(c)){sendBusy(req);return;}}
  ridingPlaylists[i]=RidePlaylist();
  String n="Playlist "+String(i+1);n.toCharArray(ridingPlaylists[i].name,sizeof(ridingPlaylists[i].name));
  queueSave();req->send(200,"application/json","{\"ok\":true}");
}
void handleRidePlaylistSlot(AsyncWebServerRequest *req){
  const uint8_t i=ridePlaylistIndex(req);if(i==255||!req->hasArg("slot")){req->send(400,"application/json","{\"ok\":false}");return;}
  RidePlaylist &pl=ridingPlaylists[i];const int at=req->arg("slot").toInt();
  if(at<0||at>=MZ_RIDE_PLAYLIST_SLOTS){req->send(400,"application/json","{\"ok\":false,\"error\":\"slot\"}");return;}
  if(req->hasArg("remove")&&req->arg("remove")=="1"){
    if(at>=pl.count){req->send(404,"application/json","{\"ok\":false}");return;}
    for(uint8_t k=(uint8_t)at;k+1<pl.count;k++)pl.slots[k]=pl.slots[k+1];
    if(pl.count)pl.count--;pl.slots[pl.count]=RidePlaylistSlot();queueSave();req->send(200,"application/json","{\"ok\":true}");return;
  }
  if(at>(int)pl.count|| (at==(int)pl.count&&pl.count>=MZ_RIDE_PLAYLIST_SLOTS)){req->send(409,"application/json","{\"ok\":false,\"error\":\"playlist_full_or_gap\"}");return;}
  RidePlaylistSlot p=(at<pl.count&&pl.slots[at].valid)?pl.slots[at]:RidePlaylistSlot();
  p.valid=true;
  if(req->hasArg("isShow"))p.isShow=(req->arg("isShow")=="1"||req->arg("isShow")=="true");
  if(p.isShow){p.showIndex=(uint8_t)constrain(req->arg("showIndex").toInt(),0,MZ_SHOW_COUNT-1);p.fx=0;}
  else{p.showIndex=0;p.fx=argU8(req,"fx",p.fx);}
  p.pal=argU8(req,"pal",p.pal);
  p.r=argU8(req,"r",p.r);p.g=argU8(req,"g",p.g);p.b=argU8(req,"b",p.b);
  p.r1=argU8(req,"r1",p.r1);p.g1=argU8(req,"g1",p.g1);p.b1=argU8(req,"b1",p.b1);
  p.r2=argU8(req,"r2",p.r2);p.g2=argU8(req,"g2",p.g2);p.b2=argU8(req,"b2",p.b2);
  p.sx=argU8(req,"sx",p.sx);p.ix=argU8(req,"ix",p.ix);
  p.c1=argU8(req,"c1",p.c1);p.c2=argU8(req,"c2",p.c2);p.c3=argU8(req,"c3",p.c3);
  p.o1=argU8(req,"o1",p.o1)?1:0;p.o2=argU8(req,"o2",p.o2)?1:0;p.o3=argU8(req,"o3",p.o3)?1:0;
  p.bri=argU8(req,"bri",p.bri);if(!p.bri)p.bri=180;
  if(req->hasArg("duration"))p.durationSec=(uint16_t)constrain(req->arg("duration").toInt(),1,600);
  pl.slots[at]=p;if(at>=pl.count)pl.count=(uint8_t)(at+1);queueSave();
  req->send(200,"application/json","{\"ok\":true}");
}
void handleRidePlaylistReorder(AsyncWebServerRequest *req){
  const uint8_t i=ridePlaylistIndex(req);if(i==255||!req->hasArg("from")||!req->hasArg("to")){req->send(400,"application/json","{\"ok\":false}");return;}
  RidePlaylist &pl=ridingPlaylists[i];const int from=req->arg("from").toInt(),to=req->arg("to").toInt();
  if(from<0||to<0||from>=pl.count||to>=pl.count){req->send(400,"application/json","{\"ok\":false}");return;}
  if(from!=to){RidePlaylistSlot tmp=pl.slots[from];if(from<to){for(int k=from;k<to;k++)pl.slots[k]=pl.slots[k+1];}else{for(int k=from;k>to;k--)pl.slots[k]=pl.slots[k-1];}pl.slots[to]=tmp;queueSave();}
  req->send(200,"application/json","{\"ok\":true}");
}
void handleRidePlaylistStart(AsyncWebServerRequest *req){
  const uint8_t i=ridePlaylistIndex(req);const uint8_t sc=mzSideCode(req->hasArg("side")?req->arg("side"):String("both"));
  if(i==255||sc==255){req->send(400,"application/json","{\"ok\":false}");return;}
  if(!ridingPlaylists[i].count){req->send(409,"application/json","{\"ok\":false,\"error\":\"empty\"}");return;}
  MzCmd c={};c.type=MZ_CMD_RIDE_PLAY_START;c.value=i;c.side=sc;if(!enqueueCmd(c)){sendBusy(req);return;}
  req->send(200,"application/json","{\"ok\":true}");
}
void handleRidePlaylistPause(AsyncWebServerRequest *req){
  if(!req->hasArg("paused")){req->send(400,"application/json","{\"ok\":false}");return;}
  MzCmd c={};c.type=MZ_CMD_RIDE_PLAY_PAUSE;c.flag=(req->arg("paused")=="1"||req->arg("paused")=="true")?1:0;
  if(!enqueueCmd(c)){sendBusy(req);return;}req->send(200,"application/json","{\"ok\":true}");
}
void handleRidePlaylistStop(AsyncWebServerRequest *req){
  const bool restore=!req->hasArg("restore")||req->arg("restore")!="0";
  MzCmd c={};c.type=MZ_CMD_RIDE_PLAY_STOP;c.flag=restore?1:0;
  if(!enqueueCmd(c)){sendBusy(req);return;}req->send(200,"application/json","{\"ok\":true}");
}

// ===== EKSEKUTOR ANTREAN: satu-satunya jalur yang mengubah Segment/showRunner dari permintaan web =====
void processCmdQueue(){
  if(!cmdQueue)return;
  MzCmd c;
  for(uint8_t budget=MZ_CMD_QUEUE_LEN;budget>0&&xQueueReceive(cmdQueue,&c,0)==pdTRUE;budget--){
    const String side=String(mzSideName(c.side));
    switch(c.type){
      case MZ_CMD_SHOW_START:
        // Pemilihan Welcoming manual menghentikan playlist Riding agar engine tidak saling menimpa.
        if(rideRunner.active)stopRidePlaylist(false);
        startShow((int)c.value,side);break;
      case MZ_CMD_SHOW_STOP:
        // Saat pengguna beralih dari animasi Welcoming ke efek WLED, hentikan playlist aktif lebih dulu.
        if(rideRunner.active)stopRidePlaylist(false);
        stopShowRunners(side);break;
      case MZ_CMD_SHOW_PALETTE: execShowPalette(c.value,side);break;
      case MZ_CMD_DIRECTION:    applyCurrentShowDirection();break;
      case MZ_CMD_APPLY:        applyModeSide(String(c.mode),side);break;
      case MZ_CMD_SIDE_SWAP:   rideRunner.active=false;rideRunner.paused=false;stopShowRunners("both");sideSwap=(c.flag!=0);queueSave();break;
      case MZ_CMD_RIDE_PLAY_START: startRidePlaylist(c.value,c.side);break;
      case MZ_CMD_RIDE_PLAY_PAUSE: pauseRidePlaylist(c.flag!=0);break;
      case MZ_CMD_RIDE_PLAY_STOP:  stopRidePlaylist(c.flag!=0);break;
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
  usingNativeWelcomePreset = false;

  if (bootPreset > 0) {
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

  // Paksa frame awal dijadwalkan sebelum WLED meneruskan startup jaringan.
  // Tunggu hanya sebentar agar transfer frame sebelumnya tidak bertabrakan.
  strip.trigger();
  const uint32_t renderWaitStart = millis();
  while (strip.isUpdating() && (uint32_t)(millis() - renderWaitStart) < 40U) {
    delay(1);
  }
  if (!strip.isUpdating()) {
    strip.service();
  }

  bootStage = 1;
  bootDone = false;
  bootT = millis(); // awal durasi dihitung sesudah render frame Welcome dijadwalkan
  tWelcomeMs = bootT;
}


public:
void setup() override {
for(uint8_t i=0;i<MZ_RIDE_PLAYLIST_COUNT;i++){
  if(!ridingPlaylists[i].name[0]){String n="Playlist "+String(i+1);n.toCharArray(ridingPlaylists[i].name,sizeof(ridingPlaylists[i].name));}
}
cmdQueue=xQueueCreate(MZ_CMD_QUEUE_LEN,sizeof(MzCmd)); // harus ada sebelum route HTTP didaftarkan
// Mulai Welcoming sebelum registrasi route HTTP dan sebelum WLED memulai Wi-Fi.
tSetupMs=millis();
showFxResolved=resolveShowEffects();
DEBUG_PRINTF_P(PSTR("[Mizuma] %u/%u efek WLED siap untuk 55 show\n"),(unsigned)showFxResolved,(unsigned)MZ_FXN_COUNT);
if(showFxResolved<MZ_FXN_COUNT)DEBUG_PRINTLN(F("[Mizuma] PERINGATAN: beberapa tahap akan dilewati karena nama FX tidak ditemukan pada build ini."));
startWelcomeSequence();
apBehavior=AP_BEHAVIOR_ALWAYS;
DEBUG_PRINTLN(F("[Mizuma] Usermod utama siap"));
#ifndef MZ_NO_UI
// UI hanya diberi cara membaca data kendaraan; selebihnya UI berbicara ke engine lewat API HTTP di bawah.
mizumaUiRegister(server,[this](){MizumaVehicleInfo v;v.name=vehicleName;v.brand=vehicleBrand;v.plate=vehiclePlate;v.year=vehicleYear;return v;});
#endif
server.on("/mizuma/status",HTTP_GET,[this](AsyncWebServerRequest *req){
  const bool apOn = (WiFi.softAPgetStationNum() > 0);
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
char buf[288];snprintf(buf,sizeof(buf),"{\"heap\":%u,\"minHeap\":%u,\"maxAlloc\":%u,\"setupMs\":%u,\"welcomeMs\":%u,\"nowMs\":%u,\"showFxResolved\":%u,\"maxShowMs\":%u}",
(unsigned)ESP.getFreeHeap(),(unsigned)ESP.getMinFreeHeap(),(unsigned)ESP.getMaxAllocHeap(),(unsigned)tSetupMs,(unsigned)tWelcomeMs,(unsigned)millis(),(unsigned)showFxResolved,(unsigned)MZ_SHOW_MAX_DURATION_MS);
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
server.on("/mizuma/bri",HTTP_GET,[this](AsyncWebServerRequest *req){handleBri(req);});
server.on("/mizuma/bri",HTTP_POST,[this](AsyncWebServerRequest *req){handleBri(req);});
server.on("/mizuma/riding/playlists",HTTP_GET,[this](AsyncWebServerRequest *req){handleRidePlaylists(req);});
server.on("/mizuma/riding/playlist/meta",HTTP_POST,[this](AsyncWebServerRequest *req){handleRidePlaylistMeta(req);});
server.on("/mizuma/riding/playlist/clear",HTTP_POST,[this](AsyncWebServerRequest *req){handleRidePlaylistClear(req);});
server.on("/mizuma/riding/playlist/slot",HTTP_POST,[this](AsyncWebServerRequest *req){handleRidePlaylistSlot(req);});
server.on("/mizuma/riding/playlist/reorder",HTTP_POST,[this](AsyncWebServerRequest *req){handleRidePlaylistReorder(req);});
server.on("/mizuma/riding/playlist/start",HTTP_POST,[this](AsyncWebServerRequest *req){handleRidePlaylistStart(req);});
server.on("/mizuma/riding/playlist/pause",HTTP_POST,[this](AsyncWebServerRequest *req){handleRidePlaylistPause(req);});
server.on("/mizuma/riding/playlist/stop",HTTP_POST,[this](AsyncWebServerRequest *req){handleRidePlaylistStop(req);});
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
  processCmdQueue(); // satu-satunya tempat perubahan Segment dari web dijalankan (task yang sama dengan strip.service())
  tickShowEngine();
  tickRidingPlaylist();
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
    applyModeSide("riding", "both", 4); // 400 ms crossfade setelah show Mizuma selesai
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
JsonArray rp=top.createNestedArray("ridingPlaylists");
for(uint8_t i=0;i<MZ_RIDE_PLAYLIST_COUNT;i++){
  RidePlaylist &pl=ridingPlaylists[i];JsonObject po=rp.createNestedObject();po["name"]=pl.name;JsonArray slots=po.createNestedArray("slots");
  for(uint8_t j=0;j<pl.count;j++){
    const RidePlaylistSlot &p=pl.slots[j];JsonObject so=slots.createNestedObject();
    so["valid"]=p.valid;so["isShow"]=p.isShow;so["showIndex"]=p.showIndex;so["fx"]=p.fx;so["pal"]=p.pal;
    so["r"]=p.r;so["g"]=p.g;so["b"]=p.b;so["r1"]=p.r1;so["g1"]=p.g1;so["b1"]=p.b1;so["r2"]=p.r2;so["g2"]=p.g2;so["b2"]=p.b2;
    so["sx"]=p.sx;so["ix"]=p.ix;so["c1"]=p.c1;so["c2"]=p.c2;so["c3"]=p.c3;so["o1"]=p.o1;so["o2"]=p.o2;so["o3"]=p.o3;so["bri"]=p.bri;so["duration"]=p.durationSec;
  }
}}

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
for(uint8_t i=0;i<MZ_RIDE_PLAYLIST_COUNT;i++){
  if(!ridingPlaylists[i].name[0]){String n="Playlist "+String(i+1);n.toCharArray(ridingPlaylists[i].name,sizeof(ridingPlaylists[i].name));}
}
JsonArray rp=top["ridingPlaylists"].as<JsonArray>();
if(!rp.isNull()){
  for(uint8_t i=0;i<MZ_RIDE_PLAYLIST_COUNT&&i<rp.size();i++){
    JsonObject po=rp[i];String n=po["name"]|"";n.trim();if(!n.length())n="Playlist "+String(i+1);if(n.length()>24)n=n.substring(0,24);n.toCharArray(ridingPlaylists[i].name,sizeof(ridingPlaylists[i].name));
    JsonArray slots=po["slots"].as<JsonArray>();ridingPlaylists[i].count=0;
    if(!slots.isNull())for(uint8_t j=0;j<MZ_RIDE_PLAYLIST_SLOTS&&j<slots.size();j++){
      JsonObject so=slots[j];RidePlaylistSlot p;p.valid=so["valid"]|true;
      p.isShow=so["isShow"]|false;p.showIndex=(uint8_t)constrain((int)(so["showIndex"]|0),0,MZ_SHOW_COUNT-1);p.fx=so["fx"]|0;p.pal=so["pal"]|0;
      p.r=so["r"]|255;p.g=so["g"]|165;p.b=so["b"]|0;p.r1=so["r1"]|255;p.g1=so["g1"]|255;p.b1=so["b1"]|255;p.r2=so["r2"]|255;p.g2=so["g2"]|26;p.b2=so["b2"]|0;
      p.sx=so["sx"]|128;p.ix=so["ix"]|128;p.c1=so["c1"]|128;p.c2=so["c2"]|128;p.c3=so["c3"]|128;p.o1=so["o1"]|0;p.o2=so["o2"]|0;p.o3=so["o3"]|0;p.bri=so["bri"]|180;p.durationSec=(uint16_t)constrain((int)(so["duration"]|20),1,600);
      if(p.valid){ridingPlaylists[i].slots[ridingPlaylists[i].count++]=p;}
    }
  }
}
return true;}

uint16_t getId() override {return USERMOD_ID_MIZUMA_SYSTEM;}
};
static MizumaSmartSystem mizuma_smartsystem;
REGISTER_USERMOD(mizuma_smartsystem);
// ================= AKHIR FILE =================
