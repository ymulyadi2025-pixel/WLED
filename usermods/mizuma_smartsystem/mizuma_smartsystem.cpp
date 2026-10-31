String staIP=staOn?WiFi.localIP().toString():String("");
String ssid=staOn?WiFi.SSID():String("Mizuma Smart System");
String json="{\"ap\":";json+=apOn?"true":"false";
json+=",\"sta\":";json+=staOn?"true":"false";
json+=",\"staIP\":\"";json+=staIP;
json+="\",\"ssid\":\"";json+=jsonEscape(ssid);
json+="\"}";req->send(200,"application/json",json);});
server.on("/mizuma/debug",HTTP_GET,[this](AsyncWebServerRequest *req){
char buf[288];snprintf(buf,sizeof(buf),"{\"heap\":%u,\"minHeap\":%u,\"maxAlloc\":%u,\"setupMs\":%u,\"welcomeMs\":%u,\"nowMs\":%u,\"showFxResolved\":%u,\"maxShowMs\":%u}",
(unsigned)ESP.getFreeHeap(),(unsigned)ESP.getMinFreeHeap(),(unsigned)ESP.getMaxAllocHeap(),(unsigned)tSetupMs,(unsigned)tWelcomeMs,(unsigned)millis(),(unsigned)showFxResolved,(unsigned)MZ_SHOW_MAX_DURATION_MS);
req->send(200,"application/json",buf);});
server.on("/mizuma/shows",HTTP_GET,[this](AsyncWebServerRequest *req){
String out;out.reserve(1800);out+="[";for(uint8_t i=0;i<MZ_SHOW_COUNT;i++){if(i)out+=",";out+='"';out+=jsonEscape(String(MZ_SHOWS[i].name));out+='"';}out+="]";req->send(200,"application/json",out);});
server.on("/mizuma/show-durations",HTTP_GET,[this](AsyncWebServerRequest *req){
String out;out.reserve(500);out+="[";for(uint8_t i=0;i<MZ_SHOW_COUNT;i++){if(i)out+=",";out+=String(MZ_SHOWS[i].durationMs);}out+="]";req->send(200,"application/json",out);});
server.on("/mizuma/show",HTTP_GET,[this](AsyncWebServerRequest *req){handleShow(req);});
server.on("/mizuma/show",HTTP_POST,[this](AsyncWebServerRequest *req){handleShow(req);});
server.on("/mizuma/show/palette",HTTP_GET,[this](AsyncWebServerRequest *req){handleShowPalette(req);});
server.on("/mizuma/show/palette",HTTP_POST,[this](AsyncWebServerRequest *req){handleShowPalette(req);});
server.on("/mizuma/show/stop",HTTP_GET,[this](AsyncWebServerRequest *req){handleShowStop(req);});
server.on("/mizuma/show/stop",HTTP_POST,[this](AsyncWebServerRequest *req){handleShowStop(req);});
server.on("/mizuma/preset",HTTP_GET,[this](AsyncWebServerRequest *req){handlePreset(req);});
server.on("/mizuma/preset",HTTP_POST,[this](AsyncWebServerRequest *req){handlePreset(req);});
server.on("/mizuma/apply",HTTP_GET,[this](AsyncWebServerRequest *req){handleApply(req);});
server.on("/mizuma/apply",HTTP_POST,[this](AsyncWebServerRequest *req){handleApply(req);});
server.on("/mizuma/side-swap",HTTP_GET,[this](AsyncWebServerRequest *req){String json=String("{\"enabled\":")+(sideSwap?"true":"false")+"}";req->send(200,"application/json",json);});
server.on("/mizuma/side-swap",HTTP_POST,[this](AsyncWebServerRequest *req){if(!req->hasArg("enabled")){req->send(400,"application/json","{\"ok\":false}");return;}stopShowRunners("both");sideSwap=(req->arg("enabled")=="1"||req->arg("enabled")=="true");queueSave();String json=String("{\"ok\":true,\"enabled\":")+(sideSwap?"true":"false")+"}";req->send(200,"application/json",json);});
server.on("/mizuma/show/reverse",HTTP_GET,[this](AsyncWebServerRequest *req){handleShowReverse(req,true);});
server.on("/mizuma/show/reverse",HTTP_POST,[this](AsyncWebServerRequest *req){handleShowReverse(req,false);});
server.on("/mizuma/bri",HTTP_GET,[this](AsyncWebServerRequest *req){handleBri(req);});
server.on("/mizuma/bri",HTTP_POST,[this](AsyncWebServerRequest *req){handleBri(req);});
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
  tickShowEngine();
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
top["showReverse"]=showReverse;}

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
return true;}

uint16_t getId() override {return USERMOD_ID_MIZUMA_SYSTEM;}
};
static MizumaSmartSystem mizuma_smartsystem;
REGISTER_USERMOD(mizuma_smartsystem);
// ================= AKHIR FILE =================
