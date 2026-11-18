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
//   GET|POST  /mizuma/show/palette    ?pal=0..255&side=S               0 = palette asli resep show
//   GET|POST  /mizuma/show/reverse    (GET: status)  ?enabled=1|0      arah global
//   GET|POST  /mizuma/apply           ?mode=welcoming|riding|sein|rem|hazard&side=S
//   GET|POST  /mizuma/preset          ?slot=<welcomingKanan|...|hazardKiri>&fx&pal&r&g&b&r1..b2&sx&ix&c1..c3&o1..o3&bri&isShow&showIndex
//   GET       /mizuma/presets         -> semua 10 slot
//   GET|POST  /mizuma/side-swap       (GET: status)  ?enabled=1|0
//   GET|POST  /mizuma/bri             ?v=1..255                        brightness permanen
//   --- Playlist Riding (maks 4 playlist x 12 slot; berjalan di engine, tetap jalan saat HP terputus) ---
//   GET   /mizuma/playlists          -> {use, lists:[{n, c}]}           use = indeks playlist yang dipakai Riding (-1 = Single Effect)
//   GET   /mizuma/pl/list?i=N        -> {name, it:[{d(0.1dtk), rv, R:{..}, L:{..}}]}
//   POST  /mizuma/pl/save            body JSON {list,name,it:[...]}     simpan satu playlist
//   POST  /mizuma/pl/delete?list=N
//   POST  /mizuma/pl/use?list=N      N=-1 -> kembali ke preset Single Effect (ridingKanan/ridingKiri)
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
    struct ShowRunner { bool active=false; bool hasCurrentShow=false; bool logicalRight=true; uint8_t showIndex=0; uint8_t stageIndex=0; uint8_t currentStageIndex=255; uint8_t userPalette=0; uint8_t expectedMode=255; uint32_t startedAt=0; bool loop=false; };
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
    enum MzCmdType : uint8_t { MZ_CMD_NONE=0, MZ_CMD_SHOW_START, MZ_CMD_SHOW_STOP, MZ_CMD_SHOW_PALETTE, MZ_CMD_DIRECTION, MZ_CMD_APPLY, MZ_CMD_SIDE_SWAP, MZ_CMD_PL_PLAY, MZ_CMD_PL_PAUSE, MZ_CMD_PL_USE, MZ_CMD_PL_STOP };
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
stopPlaylist();
if(mode=="welcoming")return applyWelcomeModeSide(side);
stopShowRunners("both");
int base=modeBase(mode);if(base<0)return false;
if(side!="both"&&side!="kanan"&&side!="kiri")return false;
if(base==2&&ridingPl>=0&&ridingPl<PL_MAX&&pls[ridingPl].n>0)return startPlaylist(ridingPl,0,false);
if(side=="both")applyTwoSlots(base,base+1,transition100ms);
else if(side=="kanan")applySingleSlot(base);
else applySingleSlot(base+1);
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
  r.loop=false;r.active=true;r.hasCurrentShow=true;r.logicalRight=logicalRight;r.showIndex=showIndex;r.stageIndex=0;r.currentStageIndex=255;r.expectedMode=255;
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

// ===== EKSEKUTOR ANTREAN: satu-satunya jalur yang mengubah Segment/showRunner dari permintaan web =====
void processCmdQueue(){
  if(!cmdQueue)return;
  MzCmd c;
  for(uint8_t budget=MZ_CMD_QUEUE_LEN;budget>0&&xQueueReceive(cmdQueue,&c,0)==pdTRUE;budget--){
    const String side=String(mzSideName(c.side));
    switch(c.type){
      case MZ_CMD_SHOW_START:   startShowPreview((int)c.value,side);break;
      case MZ_CMD_SHOW_STOP:    stopShowRunners(side);break;
      case MZ_CMD_SHOW_PALETTE: execShowPalette(c.value,side);break;
      case MZ_CMD_DIRECTION:    applyCurrentShowDirection();break;
      case MZ_CMD_APPLY:        applyModeSide(String(c.mode),side);break;
      case MZ_CMD_SIDE_SWAP:    stopShowRunners("both");sideSwap=(c.flag!=0);queueSave();break;
      case MZ_CMD_PL_PLAY:      startPlaylist((int)c.value,(int)(c.flag&0x7F),(c.flag&0x80)!=0);break;
      case MZ_CMD_PL_PAUSE:     execPlPause(c.flag!=0);break;
      case MZ_CMD_PL_USE:       ridingPl=(c.flag!=0)?(int8_t)-1:(int8_t)c.value;queueSave();applyModeSide("riding","both");break;
      case MZ_CMD_PL_STOP:      applyModeSide("riding","both");break;
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
   saat HP terputus. Tiap slot menyimpan konfigurasi LENGKAP untuk kedua sisi (efek atau show Mizuma, palette, 3 warna,
   parameter) + durasi (0.1 dtk) + reverse. Disimpan di file /mz_pl.bin (bukan di cfg.json agar config tetap kecil). */
static const uint8_t PL_MAX=4, PL_SLOTS=12;
struct PlSide{uint8_t isShow;uint8_t showIndex;uint8_t fx;uint8_t pal;uint8_t col[9];uint8_t sx,ix,c1,c2,c3,o1,o2,o3;};
struct PlSlot{uint16_t d;uint8_t rv;uint8_t pad;PlSide s[2];};   // s[0]=Kanan, s[1]=Kiri (logis)
struct PlList{char name[21];uint8_t n;PlSlot it[PL_SLOTS];};
PlList pls[PL_MAX];
int8_t ridingPl=-1;                 // -1: Riding memakai preset Single Effect
volatile bool plBusy=false;         // true saat HTTP sedang menulis pls[]; runner menunggu
bool plSavePending=false;uint32_t plSaveT=0;
struct PlRun{bool active=false;bool hold=false;uint8_t list=0;uint8_t slot=0;uint32_t startedAt=0;uint32_t heldAt=0;};
PlRun plRun;

void plSaveFile(){File f=WLED_FS.open(F("/mz_pl.bin"),"w");if(!f)return;const uint8_t h[4]={'M','Z','P',2};f.write(h,4);f.write((const uint8_t*)pls,sizeof(pls));f.close();}
void plLoadFile(){
  File f=WLED_FS.open(F("/mz_pl.bin"),"r");if(!f)return;
  uint8_t h[4]={0,0,0,0};
  if(f.size()==(4+sizeof(pls))&&f.read(h,4)==4&&h[0]=='M'&&h[1]=='Z'&&h[2]=='P'&&h[3]==2){f.read((uint8_t*)pls,sizeof(pls));}
  f.close();
  for(uint8_t i=0;i<PL_MAX;i++){pls[i].name[20]=0;if(pls[i].n>PL_SLOTS)pls[i].n=0;}
}
void plQueueSave(){plSavePending=true;plSaveT=millis();}
uint32_t plSlotMs(const PlSlot &sl){uint32_t ms=(uint32_t)sl.d*100U;if(ms<100U)ms=100U;return ms;}

void stopPlaylist(){plRun.active=false;plRun.hold=false;}

// Dieksekusi hanya dari task loop().
void applyPlSlot(uint8_t li,uint8_t si,bool loopShows){
  if(li>=PL_MAX||si>=pls[li].n)return;
  const PlSlot sl=pls[li].it[si];               // salinan lokal: aman terhadap edit dari task HTTP
  stopShowRunners("both");
  StaticJsonDocument<2048> doc;JsonObject root=doc.to<JsonObject>();
  root["on"]=true;root["transition"]=3;if(bri==0)root["bri"]=savedBri;
  JsonArray seg=root.createNestedArray("seg");
  for(uint8_t k=0;k<2;k++){
    const PlSide &p=sl.s[k];
    JsonObject o=seg.createNestedObject();
    o["id"]=physicalSegForLogicalSide(k==0);o["on"]=true;
    o["fx"]=p.isShow?0:p.fx;o["pal"]=p.pal;
    o["rev"]=p.isShow?showReverse:((sl.rv!=0)!=showReverse);   // stage show memakai flag arah resepnya sendiri (XOR di show engine)
    o["sx"]=p.sx;o["ix"]=p.ix;o["c1"]=p.c1;o["c2"]=p.c2;o["c3"]=p.c3;o["o1"]=p.o1;o["o2"]=p.o2;o["o3"]=p.o3;
    JsonArray col=o.createNestedArray("col");
    for(uint8_t c=0;c<3;c++){JsonArray a=col.createNestedArray();a.add(p.col[c*3]);a.add(p.col[c*3+1]);a.add(p.col[c*3+2]);}
  }
  deserializeState(root);
  for(uint8_t k=0;k<2;k++){
    if(sl.s[k].isShow&&sl.s[k].showIndex<MZ_SHOW_COUNT){startShowRunner(k==0,sl.s[k].showIndex);showRunner[k].loop=loopShows;}
  }
  tickShowEngine();
}
bool startPlaylist(int li,int si,bool hold){
  if(li<0||li>=PL_MAX||pls[li].n==0)return false;
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
}
void plSideJson(String &out,const PlSide &p){
  char b[230];
  snprintf(b,sizeof(b),"{\"show\":%u,\"si\":%u,\"fx\":%u,\"pal\":%u,\"col\":[[%u,%u,%u],[%u,%u,%u],[%u,%u,%u]],\"sx\":%u,\"ix\":%u,\"c1\":%u,\"c2\":%u,\"c3\":%u,\"o1\":%u,\"o2\":%u,\"o3\":%u}",
    (unsigned)p.isShow,(unsigned)p.showIndex,(unsigned)p.fx,(unsigned)p.pal,
    (unsigned)p.col[0],(unsigned)p.col[1],(unsigned)p.col[2],(unsigned)p.col[3],(unsigned)p.col[4],(unsigned)p.col[5],(unsigned)p.col[6],(unsigned)p.col[7],(unsigned)p.col[8],
    (unsigned)p.sx,(unsigned)p.ix,(unsigned)p.c1,(unsigned)p.c2,(unsigned)p.c3,(unsigned)p.o1,(unsigned)p.o2,(unsigned)p.o3);
  out+=b;
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
  plBusy=true;
  PlList &L=pls[li];
  strlcpy(L.name,(const char*)(o["name"]|"Playlist"),sizeof(L.name));
  uint8_t n=0;
  if(!it.isNull()){
    for(JsonObject x:it){
      if(n>=PL_SLOTS)break;
      PlSlot &sl=L.it[n];memset(&sl,0,sizeof(sl));
      const int d=x["d"]|50;sl.d=(uint16_t)constrain(d,1,9999);
      sl.rv=((int)(x["rv"]|0))?1:0;
      JsonObject r=x["R"];JsonObject l=x["L"];
      plParseSide(r,sl.s[0]);plParseSide(l,sl.s[1]);
      n++;
    }
  }
  L.n=n;
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
    char b[48];snprintf(b,sizeof(b),"{\"d\":%u,\"rv\":%u,\"R\":",(unsigned)L.it[k].d,(unsigned)L.it[k].rv);out+=b;
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
plLoadFile();
if(ridingPl>=PL_MAX||(ridingPl>=0&&pls[ridingPl].n==0))ridingPl=-1;
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
server.on("/mizuma/playlists",HTTP_GET,[this](AsyncWebServerRequest *req){handlePlSummary(req);});
server.on("/mizuma/pl/list",HTTP_GET,[this](AsyncWebServerRequest *req){handlePlList(req);});
server.on("/mizuma/pl/state",HTTP_GET,[this](AsyncWebServerRequest *req){handlePlState(req);});
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
    if(total==0||total>9000)return;
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
  processCmdQueue(); // satu-satunya tempat perubahan Segment dari web dijalankan (task yang sama dengan strip.service())
  tickPlaylist();
  tickShowEngine();
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
top["ridingPl"]=(int)ridingPl;}

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
ridingPl=(int8_t)((int)(top["ridingPl"]|-1));
return true;}

uint16_t getId() override {return USERMOD_ID_MIZUMA_SYSTEM;}
};
static MizumaSmartSystem mizuma_smartsystem;
REGISTER_USERMOD(mizuma_smartsystem);
// ================= AKHIR FILE =================
