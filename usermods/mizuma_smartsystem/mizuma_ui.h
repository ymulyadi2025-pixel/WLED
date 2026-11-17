# Let's write a python script to generate the updated mizuma_ui.h
# We will incorporate the UI/UX from Mizuma_Prototipe_UI_v19.html, 
# remove live preview (canvas animations / tick function),
# include welcoming shows into the effect selection (accessible in Riding and Welcoming),
# and ensure full functional synchronization with mizuma_smartsystem.cpp APIs.

ui_h_content = """// =====================================================================================================================================
// MIZUMA UI  (mizuma_ui.h)  — v19 UI/UX Design without Live Preview, with Welcoming Effects in Riding, fully synced with Engine.
// =====================================================================================================================================
#pragma once
#include "wled.h"
#include <functional>

struct MizumaVehicleInfo {
  String name;
  String brand;
  String plate;
  uint16_t year = 0;
};

inline void mizumaUiRegister(AsyncWebServer &server, std::function<MizumaVehicleInfo()> getVehicle) {
  server.on("/app", HTTP_GET, [getVehicle](AsyncWebServerRequest *req){
    MizumaVehicleInfo v = getVehicle();
    String html = FPSTR(MIZUMA_MAIN_HTML);
    html.replace("%VEHICLE_NAME%", v.name.length() ? v.name : String("Mizuma Smart System"));
    html.replace("%VEHICLE_BRAND%", v.brand.length() ? v.brand : String("Custom"));
    html.replace("%VEHICLE_YEAR%", v.year ? String(v.year) : String("2026"));
    html.replace("%VEHICLE_PLATE%", v.plate.length() ? v.plate : String("Z 2026 MZ"));
    req->send(200, "text/html", html);
  });
  server.on("/welcoming", HTTP_GET, [getVehicle](AsyncWebServerRequest *req){
    MizumaVehicleInfo v = getVehicle();
    String html = FPSTR(MIZUMA_MAIN_HTML);
    html.replace("%VEHICLE_NAME%", v.name.length() ? v.name : String("Mizuma Smart System"));
    html.replace("%VEHICLE_BRAND%", v.brand.length() ? v.brand : String("Custom"));
    html.replace("%VEHICLE_YEAR%", v.year ? String(v.year) : String("2026"));
    html.replace("%VEHICLE_PLATE%", v.plate.length() ? v.plate : String("Z 2026 MZ"));
    req->send(200, "text/html", html);
  });
  server.on("/led", HTTP_GET, [getVehicle](AsyncWebServerRequest *req){
    MizumaVehicleInfo v = getVehicle();
    String html = FPSTR(MIZUMA_MAIN_HTML);
    html.replace("%VEHICLE_NAME%", v.name.length() ? v.name : String("Mizuma Smart System"));
    html.replace("%VEHICLE_BRAND%", v.brand.length() ? v.brand : String("Custom"));
    html.replace("%VEHICLE_YEAR%", v.year ? String(v.year) : String("2026"));
    html.replace("%VEHICLE_PLATE%", v.plate.length() ? v.plate : String("Z 2026 MZ"));
    req->send(200, "text/html", html);
  });
  server.on("/warna", HTTP_GET, [getVehicle](AsyncWebServerRequest *req){
    MizumaVehicleInfo v = getVehicle();
    String html = FPSTR(MIZUMA_MAIN_HTML);
    html.replace("%VEHICLE_NAME%", v.name.length() ? v.name : String("Mizuma Smart System"));
    html.replace("%VEHICLE_BRAND%", v.brand.length() ? v.brand : String("Custom"));
    html.replace("%VEHICLE_YEAR%", v.year ? String(v.year) : String("2026"));
    html.replace("%VEHICLE_PLATE%", v.plate.length() ? v.plate : String("Z 2026 MZ"));
    req->send(200, "text/html", html);
  });
  server.on("/sein-rem", HTTP_GET, [getVehicle](AsyncWebServerRequest *req){
    MizumaVehicleInfo v = getVehicle();
    String html = FPSTR(MIZUMA_MAIN_HTML);
    html.replace("%VEHICLE_NAME%", v.name.length() ? v.name : String("Mizuma Smart System"));
    html.replace("%VEHICLE_BRAND%", v.brand.length() ? v.brand : String("Custom"));
    html.replace("%VEHICLE_YEAR%", v.year ? String(v.year) : String("2026"));
    html.replace("%VEHICLE_PLATE%", v.plate.length() ? v.plate : String("Z 2026 MZ"));
    req->send(200, "text/html", html);
  });
  server.on("/pengaturan", HTTP_GET, [getVehicle](AsyncWebServerRequest *req){
    MizumaVehicleInfo v = getVehicle();
    String html = FPSTR(MIZUMA_MAIN_HTML);
    html.replace("%VEHICLE_NAME%", v.name.length() ? v.name : String("Mizuma Smart System"));
    html.replace("%VEHICLE_BRAND%", v.brand.length() ? v.brand : String("Custom"));
    html.replace("%VEHICLE_YEAR%", v.year ? String(v.year) : String("2026"));
    html.replace("%VEHICLE_PLATE%", v.plate.length() ? v.plate : String("Z 2026 MZ"));
    req->send(200, "text/html", html);
  });
}

const char MIZUMA_MAIN_HTML[] PROGMEM = R"rawliteral(<!doctype html>
<html lang="id"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
<title>Mizuma — Prototipe UI v19</title>
<style>
:root{--bg:#0a0a0a;--s1:#151515;--s2:#1c1c1c;--s3:#242424;--bd:#262626;--bd2:#333;--tx:#f2f2f2;--tx2:#9a9a9a;--tx3:#666;--ac:#f5a524;--acd:rgba(245,165,36,.14);--ok:#3ddc84;--bad:#e05555;--nav:62px;color-scheme:dark;box-sizing:border-box;padding-top:env(safe-area-inset-top,0px)}
*{box-sizing:border-box;-webkit-tap-highlight-color:transparent;scrollbar-width:none}*::-webkit-scrollbar{display:none}
html,body{height:100%;margin:0;overflow:hidden}
body{display:flex;flex-direction:column;background:var(--bg);color:var(--tx);font:14px/1.4 -apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Arial,sans-serif;padding-bottom:calc(var(--nav) + env(safe-area-inset-bottom,0px))}
button{font:inherit;color:inherit;background:none;border:0;cursor:pointer}
.hd{display:flex;align-items:center;justify-content:space-between;padding:10px 16px;background:rgba(10,10,10,.9);border-bottom:1px solid var(--bd);flex:none}
.br{display:flex;align-items:center;gap:10px}.logo{width:34px;height:34px;border-radius:10px;background:linear-gradient(135deg,var(--ac),#c46a10);display:flex;align-items:center;justify-content:center;font-weight:800;color:#0a0a0a}
.b1{font-weight:800;letter-spacing:2px;font-size:14px}.b2{font-size:9px;color:var(--ac);letter-spacing:1.5px}
.volt{display:flex;align-items:center;gap:6px;font-size:12px;font-weight:700;background:var(--s2);padding:5px 10px;border-radius:20px;border:1px solid var(--bd)}.vd{width:7px;height:7px;border-radius:50%;background:var(--ok)}
.frz{flex:none;border-bottom:1px solid var(--bd);background:var(--bg)}
.tabbar{display:flex;background:var(--s1);border-bottom:1px solid var(--bd)}.tabbar button{flex:1;padding:8px 6px;color:var(--tx2);font-size:12px;font-weight:600;border-bottom:3px solid transparent}.tabbar .on{color:var(--tx);border-bottom-color:var(--ac)}
.i3{display:flex;justify-content:space-between;gap:8px;padding:2px 12px 8px;font-size:10px;color:var(--tx2)}.i3 b{color:var(--tx)}
.scr{flex:1;min-height:0;overflow-y:auto;padding:12px}
.foot{flex:none;display:flex;gap:8px;padding:10px 12px;background:var(--s1);border-top:1px solid var(--bd)}
.card{background:var(--s1);border:1px solid var(--bd);border-radius:14px;margin-bottom:10px;overflow:hidden}.cp{padding:14px}
.ah{display:flex;align-items:center;gap:10px;padding:12px 14px}.at{font-size:10px;color:var(--tx2);text-transform:uppercase;letter-spacing:.5px;font-weight:700}.as{font-size:14px;font-weight:700;margin-top:2px;display:flex;align-items:center;gap:8px}
.ah svg{margin-left:auto;width:18px;height:18px;stroke:var(--tx2);fill:none;stroke-width:2;transition:.2s}.open .ah svg{transform:rotate(180deg)}.ab{padding:0 14px 14px;border-top:1px solid var(--bd);padding-top:12px}
.sw{width:34px;height:10px;border-radius:5px;flex:none;display:inline-block}
.btn{display:flex;align-items:center;justify-content:center;gap:6px;flex:1;padding:11px;border-radius:12px;background:var(--s2);border:1px solid var(--bd2);font-size:13px;font-weight:700}.btn.p{background:var(--ac);border-color:var(--ac);color:#0a0a0a}.btn.sm{flex:none;padding:7px 12px;font-size:12px}
.chip{padding:7px 13px;border-radius:20px;background:var(--s2);border:1px solid var(--bd);color:var(--tx2);font-size:12px;font-weight:600;display:inline-block;margin:0 6px 6px 0}.chip.on{background:var(--acd);border-color:var(--ac);color:var(--ac)}
.srch{width:100%;padding:9px 12px;background:var(--s2);border:1px solid var(--bd);border-radius:10px;color:var(--tx);font-size:13px;margin-bottom:8px}
.fxg{display:grid;grid-template-columns:1fr 1fr;gap:8px}.fxi{position:relative;background:var(--s2);border:1px solid var(--bd);border-radius:10px;padding:10px 8px;font-size:12px}.fxi.on{border-color:var(--ac);background:var(--acd)}.fxi b{display:block;padding-right:18px}.sm2{font-size:10px;color:var(--tx3)}
.star{position:absolute;top:4px;right:6px;font-size:18px;color:var(--s3);padding:0}.star.on{color:var(--ac)}
.pr{margin-bottom:12px}.pl{display:flex;justify-content:space-between;font-size:11px;color:var(--tx2);margin-bottom:5px}input[type=range]{width:100%;accent-color:var(--ac)}
.wh{display:block;margin:0 auto 10px;touch-action:none;border-radius:50%}.qc{display:flex;flex-wrap:wrap;gap:8px;margin:8px 0}.qc i{width:28px;height:28px;border-radius:50%;border:2px solid rgba(255,255,255,.2)}
.pg{display:grid;grid-template-columns:1fr 1fr;gap:7px}.pw{position:relative;display:flex;align-items:center;gap:6px;background:var(--s2);border:1px solid var(--bd);border-radius:11px;padding:8px 6px 12px;overflow:hidden}.pw.on{border-color:var(--ac)}
.rd{width:12px;height:12px;border-radius:50%;border:2px solid var(--bd2);flex:none}.pw.on .rd{border-color:var(--ac);background:var(--ac)}.pn{flex:1;text-align:center;font-size:11px;font-weight:600}.ps{position:absolute;left:0;right:0;bottom:0;height:4px}
.gb{position:relative;height:30px;border-radius:8px;margin:14px 0}.stp{position:absolute;top:-6px;width:20px;height:42px;margin-left:-10px;border-radius:6px;border:3px solid #fff;box-shadow:0 0 0 1px #000}.stp.on{border-color:var(--ac);top:-9px;height:48px}
input[type=text],select,input[type=color]{background:var(--s2);border:1px solid var(--bd2);color:var(--tx);border-radius:9px;padding:8px;font:inherit}input[type=color]{width:44px;height:38px;padding:2px}
.row{display:flex;align-items:center;gap:8px}.sp{margin-left:auto}.m{color:var(--tx3);font-size:11px}
.bdg{font-size:10px;padding:2px 8px;border-radius:12px;background:var(--s3);color:var(--tx2)}.bdg.ok{background:rgba(61,220,132,.14);color:var(--ok)}
.big{font-size:34px;font-weight:800}.bar{height:8px;border-radius:4px;background:linear-gradient(90deg,#e05555 0 25%,#e8a33d 25% 45%,#3ddc84 45% 80%,#e8a33d 80%);position:relative;margin:14px 0 6px}.bar i{position:absolute;top:-4px;width:4px;height:16px;background:#fff;border-radius:2px}
nav{position:fixed;left:0;right:0;bottom:0;display:flex;background:rgba(18,18,18,.94);border-top:1px solid var(--bd);padding-bottom:env(safe-area-inset-bottom,0px);z-index:40}
nav button{flex:1;display:flex;flex-direction:column;align-items:center;gap:3px;padding:7px 2px 8px;color:var(--tx3);font-size:10px;font-weight:600}nav .on{color:var(--ac)}
nav span{padding:3px 16px;border-radius:16px;display:flex}nav .on span{background:var(--acd)}nav svg{width:19px;height:19px;stroke:currentColor;fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round}
#sheet{position:fixed;inset:0;background:rgba(0,0,0,.6);display:none;z-index:70}#sheet.on{display:flex}#sh{background:var(--s1);border:1px solid var(--bd2);width:100%;max-width:520px;margin:auto auto 0;border-radius:18px 18px 0 0;padding:16px;max-height:86vh;overflow:auto}
#toast{position:fixed;left:50%;bottom:calc(var(--nav) + 90px);transform:translateX(-50%);background:var(--s2);border:1px solid var(--bd2);color:var(--ok);padding:8px 16px;border-radius:20px;font-size:12px;font-weight:700;opacity:0;transition:.25s;pointer-events:none;z-index:80}#toast.on{opacity:1}
.sl{display:flex;align-items:center;gap:8px;padding:8px 0;border-bottom:1px solid var(--bd)}.sl b{flex:1;font-size:13px}.ib{width:30px;height:30px;border-radius:8px;background:var(--s2);border:1px solid var(--bd2);font-size:14px}
.sg{display:flex;gap:6px;padding:4px 12px 10px}.sg button{flex:1;display:flex;align-items:center;justify-content:center;gap:6px;padding:7px;border-radius:20px;background:var(--s2);border:1px solid var(--bd);color:var(--tx3);font-size:11px;font-weight:700}.sg i{font-style:normal;width:18px;height:18px;border-radius:50%;background:var(--s3);display:flex;align-items:center;justify-content:center;font-size:10px}.sg .on{border-color:var(--ac);color:var(--tx);background:var(--acd)}.sg .on i{background:var(--ac);color:#0a0a0a}.sg .done i{background:var(--ok);color:#0a0a0a}
.crumb{display:flex;align-items:center;gap:8px;padding:8px 12px;font-size:12px;color:var(--tx2);border-bottom:1px solid var(--bd)}.crumb button{width:30px;height:30px;border-radius:8px;background:var(--s2);border:1px solid var(--bd2);font-size:16px}
.cc{display:block;width:100%;text-align:left;padding:18px 16px;margin-bottom:10px;background:var(--s1);border:1px solid var(--bd);border-radius:14px}.cc b{font-size:16px}.cc:active{border-color:var(--ac)}
.sr{display:flex;justify-content:space-between;align-items:center;gap:10px;padding:9px 0;border-bottom:1px solid var(--bd);font-size:13px}.sr:last-of-type{border:0}.sr span:first-child{color:var(--tx2)}
.mt{display:flex;background:var(--s2);border:1px solid var(--bd);border-radius:20px;padding:3px}.mt button{padding:6px 14px;border-radius:16px;font-size:12px;font-weight:700;color:var(--tx2)}.mt .on{background:var(--ac);color:#0a0a0a}.fxg.one{grid-template-columns:1fr}.tabbar button .m{display:block}
.vt{display:flex;flex-direction:column;gap:6px;width:82px;flex:none}.vt button{padding:10px 6px;border-radius:10px;background:var(--s2);border:1px solid var(--bd);font-size:11px;font-weight:700;color:var(--tx2);text-align:left}.vt .on{border-color:var(--ac);color:var(--ac);background:var(--acd)}.mt{width:100%}.row .btn{flex:none}.foot{flex-direction:column;gap:10px}.foot .btn{flex:none;width:100%}
.pb{width:100%}.pt{font-size:11px;font-weight:800;color:var(--ac);margin-bottom:2px}.pg2{display:grid;grid-template-columns:repeat(auto-fit,minmax(120px,1fr));gap:4px 16px}.pg2 label{font-size:11px;color:var(--tx2)}.pg2 input{width:100%;margin:0;height:22px}
.fr{position:relative;display:flex;align-items:center;gap:7px;width:100%;text-align:left;background:var(--s1);border:1px solid var(--bd);border-radius:12px;padding:8px 8px 8px 12px;margin-bottom:6px;overflow:hidden}.fr.on{border-color:var(--ac);background:var(--acd)}.fr.fv{box-shadow:inset 3px 0 0 var(--ac)}.fr>b{flex:1;font-size:13px}.fr.add{border-style:dashed;color:var(--tx2)}
.rp{flex:none;padding:6px 10px;border-radius:14px;border:1px solid var(--bd2);font-size:11px;font-weight:700;color:var(--tx2);white-space:nowrap}.rp.on{background:var(--ac);border-color:var(--ac);color:#0a0a0a}
.star{flex:none;font-size:25px;line-height:1;color:var(--tx3);padding:0 2px}.star.on{color:var(--ac);text-shadow:0 0 10px var(--ac)}
.fr .ib{flex:none;width:28px;height:28px;padding:0;font-size:13px}.no{flex:none;width:24px;height:24px;border-radius:50%;background:var(--s3);display:flex;align-items:center;justify-content:center;font-size:11px;font-weight:800}.fr.on .no{background:var(--ac);color:#0a0a0a}
.mini{flex:none;width:34px;height:8px;border-radius:4px}.pgb{position:absolute;left:0;bottom:0;height:3px;background:var(--ac)}.stp{touch-action:none}
.mt button{flex:1;padding:9px}.tabbar button.fn{font-family:"Racing Sans One",Impact,"Arial Black",sans-serif;font-style:italic;letter-spacing:1.5px;text-transform:uppercase;font-size:14px;color:var(--ac)}
.dr{display:flex;gap:10px;overflow-x:auto;padding:4px 2px 8px}.di{flex:none;display:flex;flex-direction:column;align-items:center;gap:4px;font-size:10px;color:var(--tx2)}.di i{width:38px;height:38px;border-radius:50%;border:2px solid var(--bd2)}.di.on i{border-color:var(--ac);box-shadow:0 0 0 2px var(--ac)}
.b2{display:none}.b1{letter-spacing:1px}.hd{gap:8px}.ri{flex:1;min-width:0;border-left:1px solid var(--bd2);padding-left:10px;font-size:10px;line-height:1.3}.ri b{display:block;font-size:12px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}.ri span{display:flex;align-items:center;gap:5px;color:var(--tx2);white-space:nowrap;overflow:hidden}.ri i{flex:none;width:18px;height:6px;border-radius:3px}
.fr .star{position:static}.fbar{padding:6px 12px 8px}
.bl{display:flex;align-items:center;gap:8px;padding:6px 12px 8px;overflow-x:auto}.ride-mode{display:flex;align-items:center;gap:6px;padding:6px 12px 8px;white-space:nowrap}.ride-mode .hint{font-size:11px;color:var(--tx2);flex:1;min-width:0}.ride-mode .bu{padding:5px 8px;font-size:11px;gap:5px}.ride-mode .bu i{width:9px;height:9px;flex:none}.ride-mode .ib{flex:none}.bu{flex:none;display:flex;align-items:center;gap:7px;font-size:12px;font-weight:700;color:var(--tx2);padding:5px 11px 5px 8px;border-radius:16px;border:1px solid var(--bd)}.bu i{width:12px;height:12px;border-radius:50%;border:2px solid var(--bd2)}.bu.on{color:var(--tx);border-color:var(--ac)}.bu.on i{border-color:var(--ac);background:var(--ac)}
.fld{display:flex;align-items:center;gap:10px;background:var(--s2);border:1px solid var(--bd);border-radius:12px;padding:11px 12px;margin-bottom:8px}.fld b{flex:1}.fld svg{width:18px;height:18px;stroke:var(--tx2);fill:none;stroke-width:2}.fxl{max-height:30vh;overflow:auto;margin-bottom:6px}.cbody{max-height:34vh;overflow:auto;margin-bottom:8px}.sls{max-height:42vh;overflow:auto}.card.open .ah{padding-bottom:8px}.ah .rp{margin-left:8px}
.pkr{display:flex;flex-direction:column;align-items:center;gap:10px}.pkr.c{flex-direction:row;align-items:flex-start}.pkr .wh{width:min(230px,100%);height:auto;margin:0;border-radius:50%}.pkr.c .wh{width:42%}
.pk{flex:1;width:100%;display:flex;flex-direction:column;gap:7px}.pk label{font-size:11px;color:var(--tx2)}.pk input{width:100%;margin:2px 0 0;height:14px;-webkit-appearance:none;appearance:none;border-radius:7px;background:var(--g,#333)}
.pk input::-webkit-slider-thumb{-webkit-appearance:none;width:20px;height:20px;border-radius:50%;background:#fff;border:2px solid #000}.pk input::-moz-range-thumb{width:18px;height:18px;border-radius:50%;background:#fff;border:2px solid #000}
.qc i{cursor:pointer}.bu.big{width:100%;border:0;border-radius:14px;padding:13px 14px}.stp{width:24px;margin-left:-12px}.gb{height:34px}
.gbr{display:flex;align-items:center;gap:10px;padding:6px 16px;background:var(--s1);border-bottom:1px solid var(--bd);flex:none;font-size:13px;color:var(--ac)}.gbr input{flex:1;height:18px}.gbr b{min-width:36px;text-align:right;color:var(--tx);font-size:12px}.gbr .sv{min-width:74px;font-size:10px;color:var(--ok);opacity:0;transition:.3s}.gbr .sv.on{opacity:1}
.pkr.c{gap:12px;align-items:center}.pkr.c .wh{width:min(40%,25vh)}.pk{gap:3px}.pk label{font-size:10px}.pk input{height:12px}
.qs{display:grid;grid-template-columns:repeat(8,1fr);gap:7px;margin:10px 0 4px}.qs i{aspect-ratio:1;border-radius:50%;border:2px solid rgba(255,255,255,.22);display:flex;align-items:center;justify-content:center;font-style:normal;font-size:14px;touch-action:manipulation;user-select:none;-webkit-user-select:none;-webkit-touch-callout:none;cursor:pointer}
.cb{font-style:normal;font-size:9px;color:var(--ac);border:1px solid var(--ac);border-radius:8px;padding:1px 6px;margin-left:6px;white-space:nowrap}.pw.cu{box-shadow:inset 3px 0 0 var(--ac)}.fr .no{margin-right:2px}
#sh.fix{height:min(580px,84vh);display:flex;flex-direction:column;overflow:hidden}.pbody{flex:1;min-height:0;display:flex;flex-direction:column;gap:8px;overflow:hidden}.pbody>*{flex:none}.pbody .fld{height:52px;margin:0}.pbody .grow{flex:1;min-height:0;overflow:auto}.pbody .pfix{height:92px;overflow:auto}.pbody .sp1{flex:1}

.fxd{margin:-1px 0 9px 34px;padding:10px;background:rgba(245,165,36,.07);border:1px solid rgba(245,165,36,.35);border-radius:0 0 11px 11px}
.fxd .row{gap:9px}.fxd-thumb{width:56px;height:30px;flex:none;border:1px solid rgba(255,255,255,.25);border-radius:8px;box-shadow:inset 0 0 0 1px rgba(0,0,0,.12)}
.fxd-info{flex:1;min-width:0}.fxd-info b{display:block;font-size:12px}.fxd-info small{display:block;color:var(--tx2);font-size:10px;margin-top:2px}
.fxd .rp{background:var(--acd);border-color:var(--ac);color:var(--ac)}
.fxc-preview{display:flex;align-items:center;gap:10px;padding:9px 10px;background:var(--s2);border:1px solid var(--bd);border-radius:11px}
.fxc-preview>div{flex:1;min-width:0}.fxc-preview small{display:block;color:var(--tx2);font-size:10px}.fxc-preview b{display:block;font-size:13px}
.fxc-chip{width:66px;height:32px;flex:none;border:1px solid rgba(255,255,255,.25);border-radius:8px}
.fxc-skins{display:flex;flex-direction:column;gap:7px}.fxc-skins .pg{margin-bottom:4px}
.fxcolor{display:flex;align-items:center;gap:5px;flex:none;padding:5px 7px;border:1px solid var(--ac);border-radius:14px;background:var(--acd);color:var(--ac);font-size:10px;font-weight:700;white-space:nowrap}.fxcolor i{display:block;width:24px;height:15px;border:1px solid rgba(255,255,255,.35);border-radius:4px}.fxride{padding:5px 7px;font-size:10px}.fr .star{font-size:17px;padding:2px 3px}
.ie{margin:4px 0}.ie .fld{height:52px;margin:0 0 8px}.ie .fld.on{border-color:var(--ac)}.fld.on svg{transform:rotate(180deg)}.ied{color:var(--ac);font-size:14px;flex:none}
.iep{height:min(420px,54vh);display:flex;flex-direction:column;gap:8px;margin:0 0 10px;padding:10px;background:rgba(245,165,36,.05);border:1px solid rgba(245,165,36,.3);border-radius:12px}.iep .grow{flex:1;min-height:0;overflow:auto}.iep .pfix{flex:none;height:92px;overflow:auto}.iep>.btn,.iep>.bl,.iep>.fxc-preview{flex:none}.iep>.bl{padding:0}
.sed{margin:-1px 0 9px 8px;padding:10px;background:rgba(245,165,36,.07);border:1px solid rgba(245,165,36,.35);border-radius:0 0 11px 11px}.sed>.row{margin-bottom:8px}.card.ed{border-color:var(--ac)}.card.ed .sls{max-height:none;overflow:visible}

:root{--tx3:#8c8c8c;--nav:56px}
.b1{display:none}#hr{display:flex;align-items:center;gap:8px;flex:none}.volt{min-height:40px}
.ri{font-size:11px}.ri .svs{font-style:normal;font-weight:700}.svs.ok{color:var(--ok)}.svs.sv{color:var(--ac)}.svs.bad{color:#ff8a8a}
.ban{display:flex;align-items:center;gap:10px;padding:8px 14px;background:rgba(224,85,85,.16);color:#ffc1c1;font-size:12px;flex:none}
.pvl{display:flex;justify-content:space-between;align-items:center;padding:0 12px 6px;font-size:12px;color:var(--tx2)}.pvl b{color:var(--tx)}
.m,.sm2{font-size:12px}.at,.bdg,.cb,.pl,.pt{font-size:11px}.pk label,.pg2 label{font-size:12px}nav button{font-size:10px}
.ib,.fr .ib,.crumb button{width:44px;height:44px;font-size:16px}
.rp,.fxride{min-height:38px;padding:8px 12px;font-size:12px}
.star,.fr .star{font-size:24px;min-width:40px;min-height:40px}
.btn.sm,.bu,.chip{min-height:40px}
.pg2 input{height:32px}.pk input{height:28px;background:var(--g,#333) center/100% 12px no-repeat}
.pk input::-webkit-slider-thumb{width:26px;height:26px}.pk input::-moz-range-thumb{width:24px;height:24px}
.iep{position:fixed;inset:0 0 calc(var(--nav) + env(safe-area-inset-bottom,0px)) 0;height:auto;z-index:60;background:var(--bg);border:0;border-radius:0;margin:0;padding:calc(10px + env(safe-area-inset-top,0px)) 12px 12px}
.dh{display:flex;align-items:center;gap:8px;flex:none;font-weight:800;font-size:15px}
.drag{flex:none;width:40px;height:44px;display:flex;align-items:center;justify-content:center;color:var(--tx2);touch-action:none;font-size:22px;cursor:grab}
#toast{display:flex;gap:12px;align-items:center}#toast.act{pointer-events:auto}#toast button{color:var(--ac);font-weight:800;padding:8px 4px}
.emp{padding:18px;text-align:center;color:var(--tx2);font-size:13px;border:1px dashed var(--bd2);border-radius:12px;line-height:1.5}

.rad{flex:none;width:44px;height:44px;display:flex;align-items:center;justify-content:center}.rad i{width:22px;height:22px;border-radius:50%;border:2px solid var(--bd2)}.rad.on i{border-color:var(--ac);background:radial-gradient(var(--ac) 42%,transparent 48%)}
.fxcolor{min-height:38px;padding:6px 9px}.sbar{height:56px;border-radius:12px;margin:10px 0}.sdl{max-height:40vh;overflow:auto}.sdl .pw{margin-bottom:7px}
.pkx .wh{width:min(230px,52vw,25vh);height:auto;margin:0 auto 6px;display:block}.pkx .pk{gap:4px}
.rg{display:flex;align-items:center;gap:8px}.rg b{flex:none;width:24px;text-align:center;border-radius:6px;font-size:12px;padding:2px 0;color:#fff}.rg input{flex:1}
.pkx .smp,.pkx .slt{display:grid;grid-template-columns:repeat(6,1fr);gap:8px;margin:8px 0 0}
.pkx .smp i,.pkx .slt i{position:relative;aspect-ratio:1.2;border-radius:12px;border:2px solid rgba(255,255,255,.25);display:flex;align-items:center;justify-content:center;font-style:normal;font-size:20px;color:#fff;cursor:pointer}
.pkx .slt i.em{border-style:dashed;background:none}
.pe{position:absolute;right:-6px;top:-8px;width:30px;height:30px;border-radius:50%;background:#000;border:1px solid #777;font-size:13px;display:flex;align-items:center;justify-content:center}
.pkx .cf{display:none;gap:10px;margin-top:8px}
.pkx.ed{position:fixed;inset:0;z-index:90;background:#000;padding:calc(16px + env(safe-area-inset-top,0px)) 18px 18px;display:flex;flex-direction:column;justify-content:center;gap:10px}.pkx.ed .smp,.pkx.ed .slt{display:none}.pkx.ed .cf{display:flex}.pkx.ed .wh{width:min(290px,72vw)}

.mrow{display:flex;align-items:center;gap:6px;padding:3px 10px;background:var(--s1);border-bottom:1px solid var(--bd)}.mrow.open{background:var(--acd)}.mrow b{display:block;font-size:13px}.mrow .rad,.mrow .ib{width:40px;height:40px}
.ah{padding:10px 8px;gap:3px}.ah .ib{width:36px;height:36px;font-size:14px;flex:none}.ah .rad{width:36px;height:36px}.ah b{display:block;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}.ah .m{white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.er{display:flex;align-items:center;gap:6px;margin-bottom:6px;padding:0}.er.fr{padding:0;border:0;background:none}.er .ib{width:36px;height:36px}.er .drag{width:28px}.lbl{font-size:11px;color:var(--tx2);margin-bottom:4px}.lbl span{line-height:1.1}
.fb{flex:1;min-width:0;display:flex;align-items:center;gap:4px;height:40px;padding:0 8px;background:var(--s2);border:1px solid var(--bd2);border-radius:9px;font-size:13px;text-align:left}.fb b{flex:1;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}.fb svg{width:14px;height:14px;stroke:var(--tx2);fill:none;stroke-width:2;flex:none}
.swb{flex:none;width:40px;height:40px;border-radius:9px;border:2px solid rgba(255,255,255,.3)}
.dur,.pk input.dur{flex:none;width:60px;height:40px;text-align:center;padding:0;background:var(--s2);border:1px solid var(--bd2);color:var(--tx);border-radius:9px;font:inherit;-webkit-appearance:textfield;appearance:textfield}
.wrow{display:grid;grid-template-columns:1fr auto 1fr;align-items:end;gap:6px}.wl{justify-self:start}.wr{justify-self:end}
.rgbx{display:flex;flex-direction:column;gap:3px;padding:4px;background:var(--s2);border:1px solid var(--bd2);border-radius:9px}.rgbx span{display:flex;gap:3px}.rgbx b{width:22px;font-size:12px;text-align:center;color:#fff;border-radius:4px}.rgbx em{min-width:38px;font-style:normal;font-size:13px;text-align:center;background:rgba(255,255,255,.09);border-radius:4px;color:#fff}
#pop{position:fixed;inset:0;z-index:100;background:rgba(0,0,0,.6);display:none;align-items:flex-end;justify-content:center}#pop.on{display:flex}#pop>div{background:var(--s1);border:1px solid var(--bd2);width:100%;max-width:520px;border-radius:18px 18px 0 0;padding:16px 16px calc(16px + env(safe-area-inset-bottom,0px))}

.mrow{padding:0 8px;min-height:42px;gap:4px}.mrow .rad,.mrow .ib{width:36px;height:36px}.mrow.bt{margin-top:8px;border:1px solid var(--bd);border-radius:12px}
.mt{flex:1;min-width:0;display:flex;align-items:center;gap:8px;cursor:pointer}.mt b{flex:none;font-size:13px}.mt .mini{display:block;flex:none;height:18px;border-radius:5px}.mt span{font-size:12px;color:var(--tx2);overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.wbox{position:relative;width:min(320px,84vw,38vh);margin:0 auto 6px}.pkx .wh,.pkx.ed .wh{width:100%;height:auto;margin:0;display:block}.pkx.ed .wbox{width:min(300px,76vw)}
.wbox .rgbx{position:absolute;left:0;bottom:0;z-index:2;padding:3px;gap:2px;border-radius:7px}.wbox .rgbx span{gap:2px}.wbox .rgbx b{width:16px;font-size:10px;border-radius:3px}.wbox .rgbx em{min-width:28px;font-size:11px}
.wbox .rnd{position:absolute;right:0;bottom:0;z-index:2;width:46px;height:46px;border-radius:50%;background:var(--s2);border:1px solid var(--bd2)}
.pkx .pk>div:first-child{text-align:center}
#pkv::-webkit-slider-thumb{background:rgba(255,255,255,.3);border:2px solid rgba(255,255,255,.95);box-shadow:0 1px 4px rgba(0,0,0,.45)}#pkv::-moz-range-thumb{background:rgba(255,255,255,.3);border:2px solid rgba(255,255,255,.95)}
.fbtn{flex:1 1 0;width:auto;min-height:52px;display:flex;flex-direction:column;align-items:center;justify-content:center;line-height:1.2;padding:6px 8px}.fbtn small{font-size:10px;font-weight:500;opacity:.8;max-width:100%;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}.fbtn.sk{background:transparent;border:1.5px solid var(--ac);color:var(--ac);font-weight:800}
.sls.g4{display:grid;grid-template-columns:repeat(4,1fr);gap:6px;max-height:none;overflow:visible}.sls.g4>.m{grid-column:1/-1}
.sc{position:relative;overflow:hidden;display:flex;flex-direction:column;gap:3px;padding:5px;background:var(--s2);border:1px solid var(--bd);border-radius:10px;cursor:pointer;min-width:0}.sc.on{border-color:var(--ac)}.sc .sw{height:20px;border-radius:6px}.sc b{font-size:11px;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}.sc .m{font-size:11px}.sc .pgb{position:absolute;left:0;bottom:0;height:3px;background:var(--ac)}
#pop.dlg{align-items:center;padding:20px}#pop.dlg>div{border-radius:16px}

.blk{background:var(--s1);border:1px solid var(--bd);border-radius:14px;padding:8px 10px 10px;margin-bottom:10px}.blk.on{border-color:var(--ac)}.blk .mrow{background:none;border:0;padding:0;margin-bottom:6px;min-height:40px}
.wbox{width:min(320px,84vw,36vh);margin:0 auto}.pkx .wh,.pkx.ed .wh{border-radius:0}
.ctl{display:grid;grid-template-columns:auto 1fr auto;gap:12px;align-items:center;margin-top:6px}.ctl .pk{min-width:0}
.ctl .rgbx{padding:3px;gap:2px;border-radius:7px}.ctl .rgbx span{gap:2px}.ctl .rgbx b{width:16px;font-size:10px;border-radius:3px}.ctl .rgbx em{min-width:28px;font-size:11px}
.ctl .rnd{position:static;width:46px;height:46px;border-radius:50%;background:var(--s2);border:1px solid var(--bd2)}
.ah .drag{width:26px;height:36px;font-size:18px}
.sc{min-height:62px}.sc.em{align-items:center;justify-content:center;border-style:dashed;background:none;font-size:24px;color:var(--tx2)}
.sc .sw{position:relative;height:28px}.sn{position:absolute;left:4px;top:4px;font-size:11px;font-weight:800;color:#fff;text-shadow:0 0 3px #000,0 0 2px #000}
.se{position:absolute;right:-4px;top:-4px;width:26px;height:26px;border-radius:50%;background:rgba(0,0,0,.6);color:#fff;font-size:11px;display:flex;align-items:center;justify-content:center;border:1px solid rgba(255,255,255,.45)}
.sc .tg{font-size:10px;font-weight:700;color:var(--ac);line-height:1.2;margin-top:2px}

.sbk{display:grid;grid-template-columns:1fr 104px 1fr;gap:8px;align-items:stretch;padding:8px 12px;background:var(--s1);border-bottom:1px solid var(--bd)}
.sbn{display:flex;flex-direction:column;gap:4px;align-items:stretch;padding:5px 6px;border:1px solid var(--bd);border-radius:10px;background:var(--s2);color:var(--tx2);font-size:12px;font-weight:700;text-align:center;min-width:0}
.sbk .sbn span{line-height:20px}
.sbk.act .sbn.on{border-color:var(--ac);background:var(--acd);color:var(--ac)}
.sbm{display:flex;flex-direction:column;justify-content:space-between;gap:4px}
.sbm .chip{min-height:0;height:26px;margin:0;padding:0 6px;font-size:12px;white-space:nowrap;text-align:center}
.sbm .sbn.A{flex:1;justify-content:center;padding:0;min-height:0}
.pb{padding:2px 12px 4px}.pg2{gap:0 16px}.pg2 input{height:24px}.pg2 label{font-size:11px}
.gal{display:grid;grid-template-columns:1fr 1fr;gap:12px}
.tile{position:relative;display:flex;flex-direction:column;gap:6px;padding:8px;background:var(--s1);border:1px solid var(--bd);border-radius:14px;cursor:pointer;min-width:0;text-align:left}
.tile.on{border-color:var(--ac)}
.tile .th{display:flex;gap:2px;height:70px;border-radius:9px;overflow:hidden;background:#000}.tile .th i{flex:1;min-width:0}
.tile .th.em2{align-items:center;justify-content:center;color:var(--tx3);font-size:12px;border:1px dashed var(--bd2);background:none}
.tile .tbg{position:absolute;top:14px;left:14px;font-size:10px;font-weight:800;color:var(--ok);background:rgba(0,0,0,.65);padding:2px 7px;border-radius:10px}
.tile .tn{display:flex;flex-direction:column;gap:1px;min-width:0}.tile .tn b{font-size:14px;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.tile .tb{display:flex;gap:6px}.tile .tb .ib{flex:1;min-width:0;min-height:36px;padding:0}
.tile.add{align-items:center;justify-content:center;border-style:dashed;background:none;min-height:150px;color:var(--tx2)}.tile.add b{font-size:30px;font-weight:400}
.fh{display:flex;gap:8px;align-items:center;margin-bottom:8px}.fh2{display:flex;gap:8px;align-items:center;margin-bottom:10px}
.sls.g2{display:grid;grid-template-columns:1fr 1fr;gap:8px;max-height:none;overflow:visible}
.sls.g2 .sc{min-height:92px;padding:6px 8px 6px}.sls.g2 .sc .sw{height:34px}.sls.g2 .sc b{font-size:13px}.sls.g2 .sc .m{font-size:12px}.sls.g2 .sc .tg{font-size:10px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}.sls.g2 .sc.em{min-height:92px}
.sls.g2 .sc .gh{width:36px;height:34px;top:6px;right:8px}
.sls.g2 .sc .se{width:32px;height:32px}
#sheet.sxe{background:rgba(0,0,0,.7)}#sheet.sxe #sh{max-width:none;width:100%;height:calc(100dvh - 14px);max-height:none;margin:auto auto 0;border-radius:16px 16px 0 0;display:flex;flex-direction:column;overflow:hidden;padding:12px;overscroll-behavior:contain}
#sheet.sxe.sev #sh{display:block;overflow:auto}
.se3{flex:1;min-height:0;display:grid;grid-template-columns:2fr 1fr;gap:8px;margin-bottom:8px}
.se3 .seb{display:flex;flex-direction:column;min-height:0;margin:0;padding:8px}
.se3 .cols2{flex:1;min-height:0;display:grid;grid-template-columns:1fr 1fr;gap:6px}.se3 .cols1{flex:1;min-height:0;display:flex}
.se3 .sec{display:flex;flex-direction:column;min-height:0;flex:1;min-width:0}.se3 .fsc{flex:1;height:auto;min-height:0}
.se3 .sopt{flex:none;min-height:80px;align-items:flex-start;margin-bottom:6px}.se3 .sopt.v{flex-direction:column;align-items:stretch}.se3 .sopt .tgl{flex:1;padding:0 4px;font-size:11px;white-space:nowrap}.se3 .sopt.v .tgl{flex:none}
.se3 .fi{font-size:12px;padding:0 6px;height:32px;line-height:32px}
.se3 .skb{flex-direction:column;align-items:stretch;gap:3px;min-height:44px;padding:4px;margin-top:6px;flex:none}.se3 .skb i{width:100%;height:14px}.se3 .skb span{font-size:11px;text-align:center}
.se3 .sect{font-size:11px}
#sheet.sxe .seb.dur{flex:none;margin-bottom:8px;padding:6px 10px}#sheet.sxe .seb.dur label{white-space:nowrap;border:0;background:none;padding:0;margin:0;flex:1;min-height:0;display:block}#sheet.sxe .seb.dur input{flex:none;width:110px;min-height:40px}#sheet.sxe .cpf{flex:none;position:static;margin:0;padding:0;border:0;background:none}
#sheet.sxe.sev .cpf{position:sticky;bottom:-12px;margin:12px -12px -12px;padding:12px;background:var(--s1);border-top:1px solid var(--bd)}
.seh{display:flex;align-items:center;justify-content:space-between;margin-bottom:10px;font-size:16px}
.seb{background:var(--s2);border:1px solid var(--bd);border-radius:14px;padding:10px;margin-bottom:10px}
.sbt{font-size:12px;font-weight:800;color:var(--ac);margin-bottom:8px;text-transform:uppercase;letter-spacing:.04em}
.sopt{display:flex;gap:6px;align-items:center;margin-bottom:10px;flex-wrap:wrap}
.tgl{flex:none;min-height:36px;padding:0 10px;border-radius:10px;border:1px solid var(--bd2);background:var(--s3);color:var(--tx);font-size:12px;font-weight:700}.tgl.on{background:var(--ac);border-color:var(--ac);color:#000}.tgl:disabled{opacity:.4}
.seg2{display:flex;flex:1;min-width:150px;border:1px solid var(--bd2);border-radius:10px;overflow:hidden}.seg2 button{flex:1;min-height:36px;background:var(--s3);color:var(--tx2);font-size:12px;font-weight:700}.seg2 button.on{background:var(--acd);color:var(--ac)}
.cols2{display:grid;grid-template-columns:1fr 1fr;gap:8px}.cols1{display:grid;grid-template-columns:1fr}
.sec{min-width:0}.sect{font-size:12px;font-weight:700;color:var(--tx2);margin-bottom:4px;text-align:center}
.fsc{height:150px;overflow-y:auto;overscroll-behavior:contain;scroll-snap-type:y proximity;background:var(--s1);border:1px solid var(--bd);border-radius:10px;padding:2px}
.fi{height:34px;line-height:34px;padding:0 10px;border-radius:8px;font-size:13px;font-weight:600;scroll-snap-align:center;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;cursor:pointer}.fi.on{background:var(--acd);color:var(--ac);outline:1px solid var(--ac)}
.skb{width:100%;margin-top:6px;display:flex;align-items:center;gap:8px;min-height:40px;padding:4px 8px;border-radius:10px;border:1px solid var(--bd2);background:var(--s3);color:var(--tx);font-size:12px;font-weight:600;min-width:0}.skb i{flex:none;width:44px;height:18px;border-radius:5px}.skb span{overflow:hidden;text-overflow:ellipsis;white-space:nowrap}.skb:disabled{opacity:.5}
.seb.dur{display:flex;align-items:center;justify-content:space-between;gap:10px}.seb.dur label{font-size:13px;font-weight:700}.seb.dur input{width:96px;text-align:center}
.sdseg{display:flex;gap:6px;align-items:center;padding:0 12px 8px}.sdseg span{font-size:12px;font-weight:700;color:var(--tx2);margin-right:2px}
.sdseg button{flex:1;min-height:38px;border-radius:10px;border:1px solid var(--bd2);background:var(--s2);color:var(--tx);font-weight:600;font-size:13px}
.sdseg button.on{background:var(--ac);border-color:var(--ac);color:#000}
.lnk{background:none;border:0;color:var(--ac);font-weight:700;padding:0 2px;text-decoration:underline}
.sc .gh{position:absolute;top:5px;right:5px;width:32px;height:28px;display:flex;align-items:center;justify-content:center;font-style:normal;font-size:16px;color:#fff;background:rgba(0,0,0,.5);border-radius:0 6px 6px 0;touch-action:none;cursor:grab}
.foot .row .fbtn,.foot .fbtn{flex:1 1 0;width:auto;min-width:0}
.blk,.blk.on{border:0}
.sc .sw{display:block;width:100%;height:28px}
.sc{padding-bottom:5px;min-height:76px;touch-action:pan-y;user-select:none;-webkit-user-select:none;-webkit-touch-callout:none}
.sc .se{position:absolute;right:3px;bottom:3px;top:auto;width:26px;height:26px;min-width:0;min-height:0;padding:0;font-size:11px;background:var(--s3);border:1px solid var(--bd2)}
.sc .m{padding:0;white-space:nowrap;font-size:11px;align-self:flex-start;margin-top:4px}
.sc.drg{z-index:20;transition:none;box-shadow:0 10px 24px rgba(0,0,0,.65);border-color:var(--ac);overflow:visible}
.sc.tgt{outline:2px dashed var(--ac);outline-offset:-2px}
.cpf{position:sticky;bottom:-16px;z-index:5;background:var(--s1);margin:12px -16px -16px;padding:12px 16px calc(16px + env(safe-area-inset-bottom,0px));border-top:1px solid var(--bd)}.row.cpf .btn{flex:1}
</style><link rel="stylesheet" href="https://fonts.googleapis.com/css2?family=Racing+Sans+One&display=swap"></head><body>
<div class="hd"><div class="br"><div class="logo">M</div><div><div class="b1">MIZUMA</div><div class="b2">PERFORMANCE</div></div></div><div class="ri" id="ri"></div><div id="hr"></div></div><div id="ban"></div>
<div class="frz" id="frz"></div><div class="scr" id="scr"></div><div class="foot" id="foot"></div>
<nav id="nav"></nav><div id="sheet" onclick="if(event.target===this)closeSheet()"><div id="sh"></div></div><div id="toast"></div><div id="pop" onclick="if(event.target===this)popOut()"></div>
<script>
const $=s=>(s.startsWith('#pk')&&document.querySelector('#sheet.on '+s))||document.querySelector(s),LB=['','Menyambung · 1 segmen','Terpisah · 2 segmen','Lapisan · 3 segmen'];
const PRM={Scan:['Kecepatan','Lebar'],Chase:['Kecepatan','Jumlah titik'],Sweep:['Kecepatan'],Running:['Kecepatan','Lebar'],Breathe:['Kecepatan'],Twinkle:['Kecepatan','Jumlah kilau'],'Fire Flicker':['Kecepatan','Intensitas api'],Rainbow:['Kecepatan','Ukuran pelangi'],Aurora:['Kecepatan','Intensitas'],'Comet Duo':['Kecepatan','Panjang ekor']};
let WELCOMING_FX_NAMES=[];
let FX=[];
fetch('/mizuma/shows').then(r=>r.json()).then(shows=>{
  WELCOMING_FX_NAMES = shows || [];
  // Include welcoming effects into FX catalog as well
  const welFx = WELCOMING_FX_NAMES.map(name => ({n: name, l: 1, p: []}));
  const baseFx = [['Scan',1],['Chase',1],['Sweep',1],['Running',1],['Breathe',2],['Twinkle',2],['Fire Flicker',2],['Rainbow',1],['Aurora',3],['Comet Duo',3]].map(x=>({n:x[0],l:x[1],p:PRM[x[0]]||[]}));
  FX = baseFx.concat(welFx).sort((a,b)=>a.n.localeCompare(b.n));
  if(typeof R==='function') R();
}).catch(()=>{
  FX=[['Scan',1],['Chase',1],['Sweep',1],['Running',1],['Breathe',2],['Twinkle',2],['Fire Flicker',2],['Rainbow',1],['Aurora',3],['Comet Duo',3]].map(x=>({n:x[0],l:x[1],p:PRM[x[0]]||[]})).sort((a,b)=>a.n.localeCompare(b.n));
});
const fi=n=>FX.findIndex(f=>f.n===n);
const PAL=[['Default',['#5500ab','#e5001b','#ab7700']],['Party',['#5500ab','#84007c','#b5004b','#e5001b','#ab7700']],['Cloud',['#0000ff','#00008b','#87ceeb','#ffffff']],['Lava',['#000000','#800000','#ff4500','#ffd700','#ffffff']],['Ocean',['#00205b','#0077b6','#90e0ef']],['Forest',['#0b3d0b','#2e8b2e','#a6d96a']],['Rainbow',['#ff0000','#ffff00','#00ff00','#00ffff','#0000ff','#ff00ff']],['Sunset',['#2b1055','#d1495b','#ffb703']],['Pacifica',['#00468c','#0e9aa7','#98d9c2']],['Aurora',['#001a33','#00ff87','#7a00ff']]].map(x=>({n:x[0],c:x[1]}));
const S={tab:'dash',mode:'simple',kind:'single',tb:'fx',q:'',q2:'',sub:'one',pv:{},open:null,cs:'one',sel:0,wsel:0,wdur:7,fav:new Set([0,1]),fc:{},fshow:false,src:'pl',lf:0,ll:0,ppo:null,pi:null,si:-1,pp:null,dr:null,tp:null,fo:false,col:{h:35,s:100},tmp:50,pal:1,ed:0,sp:0,sx:128,ix:128,
cu:[{n:'Sunset Rider',st:[[0,'#ff5e3a'],[100,'#ffcc33']]}],lists:[{n:'Malam Kota',it:[{f:0,d:20},{f:1,d:20},{f:2,d:30}]},{n:'Santai',it:[{f:3,d:30}]}],
act:{t:'list',i:0},lay:'auto',rev:false,pw:true,bri:180,v:12.6,pk:null};
const IC={col:'<path d="M12 3s6 6.5 6 11a6 6 0 0 1-12 0c0-4.5 6-11 6-11z"/>',dash:'<rect x="3" y="3" width="7" height="9" rx="1"/><rect x="14" y="3" width="7" height="5" rx="1"/><rect x="14" y="12" width="7" height="9" rx="1"/><rect x="3" y="16" width="7" height="5" rx="1"/>',
wel:'<path d="M12 3l2.4 5.2 5.6.7-4.1 3.9 1 5.6L12 15.6 7.1 18.4l1-5.6L4 8.9l5.6-.7z"/>',rid:'<path d="M9 18h6M10 21h4M12 3a6 6 0 0 0-3.6 10.8c.7.6 1.1 1.4 1.3 2.2h4.6c.2-.8.6-1.6 1.3-2.2A6 6 0 0 0 12 3z"/>',
rem:'<path d="M3 12h13M11 6l6 6-6 6M20 6v12"/>',set:'<circle cx="12" cy="12" r="3"/><path d="M12 2v3M12 19v3M2 12h3M19 12h3M5 5l2 2M17 17l2 2M19 5l-2 2M7 17l-2 2"/>'};
const TABS=[['dash','Dashboard'],['wel','Welcoming'],['rid','Riding'],['col','Warna'],['rem','Rem & Sein'],['set','Pengaturan']];
const hex=h=>[1,3,5].map(i=>parseInt(h.substr(i,2),16)),css=r=>'rgb('+r+')',grad=c=>'linear-gradient(90deg,'+c.join(',')+')';
function hsl(h,s,l){s/=100;l/=100;const k=n=>(n+h/30)%12,a=s*Math.min(l,1-l),f=n=>l-a*Math.max(-1,Math.min(k(n)-3,Math.min(9-k(n),1)));return[f(0),f(8),f(4)].map(x=>Math.round(x*255));}
const one=()=>css(hsv(S.col.h,S.col.s,S.col.v??100));
const cuG=c=>'linear-gradient(90deg,'+[...c.st].sort((a,b)=>a[0]-b[0]).map(s=>s[1]+' '+s[0]+'%').join(',')+')';
const curG=()=>S.cs==='one'?grad([one(),one()]):S.cs==='pat'?grad(PAL[S.pal].c):cuG(S.cu[S.ed]);
const curN=()=>S.cs==='one'?'Warna Solid':S.cs==='pat'?PAL[S.pal].n:S.cu[S.ed].n;
function cols(sd){if(S.tab==='rid'&&S.tb==='fx'){const k=fxSel(sd);return S.fc[k]?fcCols(S.fc[k]):PAL[0].c.map(hex);}if(S.fc[S.lf])return fcCols(S.fc[S.lf]);const q0=tpOn()&&S.lists[S.tp.i].it[S.tp.j],q=q0&&slotCfg(q0,sd);if(q&&q.c)return sc(q.c);if(S.cs==='one')return[hsv(S.col.h,S.col.s,S.col.v??100)];if(S.cs==='pat')return PAL[S.pal].c.map(hex);return[...S.cu[S.ed].st].sort((a,b)=>a[0]-b[0]).map(s=>hex(s[1]));}
function at(c,p){if(c.length<2)return c[0];const x=p*(c.length-1),i=Math.min(Math.floor(x),c.length-2),f=x-i;return c[i].map((v,j)=>v+(c[i+1][j]-v)*f);}
let TP;const fxI=sd=>FX[S.lf]||FX[0],lay=sd=>S.lay==='auto'||S.tab==='wel'?fxI(sd).l:+S.lay;
let TT,UN;function toast(m,undo){const t=$('#toast');UN=undo;t.innerHTML='<span>'+m+'</span>'+(undo?'<button onclick="doUndo()">Urungkan</button>':'');t.className='on'+(undo?' act':'');clearTimeout(TT);TT=setTimeout(()=>t.className='',undo?5000:1700);}
function doUndo(){const u=UN;UN=null;$('#toast').className='';if(u){u();commit('Dikembalikan');}}
function openSheet(h,fx){$('#sh').innerHTML=h;$('#sh').className=fx?'fix':'';$('#sheet').className='on';}function closeSheet(){$('#sheet').className='';wheel();}
const act=()=>S.act.t==='fx'?FX[S.act.i].n:S.lists[S.act.i].n,CH='<svg viewBox="0 0 24 24"><path d="M6 9l6 6 6-6"/></svg>';
function R(){rs();$('#frz').innerHTML=frz();$('#scr').innerHTML=({dash:vDash,wel:vWel,rid:vRid,rem:vSoon,set:vSoon})[S.tab]();$('#foot').innerHTML=foot();$('#foot').style.display=$('#foot').innerHTML?'flex':'none';
$('#nav').innerHTML=TABS.map(t=>'<button class="'+((S.tab==='rid'&&S.tb==='col'?'col':S.tab)===t[0]?'on':'')+'" onclick="go(\''+t[0]+'\')"><span><svg viewBox="0 0 24 24">'+IC[t[0]]+'</svg></span>'+t[1]+'</button>').join('');wheel();}
function go(t){if(t==='col'){S.tab='rid';S.tb='col';S.sub='one';S.q='';S.ie=null;R();$('#scr').scrollTop=0;return;}S.tab=t;S.tb='fx';S.q='';S.ie=null;R();$('#scr').scrollTop=0;}
S.fside='all';S.fs={L:null,R:null};S.as={L:null,R:null};
const SDN={all:'Semua',L:'Kiri',R:'Kanan'};
const fxSel=sd=>sd&&S.fs[sd]!=null?S.fs[sd]:S.sel,selOf=()=>S.fside==='all'?S.sel:fxSel(S.fside),actOf=sd=>S.act.t==='fx'?(sd&&S.as[sd]!=null?S.as[sd]:S.act.i):null;
const slotCfg=(x,sd)=>sd&&sd!=='all'&&x.man&&x[sd]?{f:x[sd].f,c:x[sd].c||x.c}:x;
const slotBg=x=>{if(!x.man||JSON.stringify(x.L.c)===JSON.stringify(x.R.c))return cg(x.man?x.L.c:(x.c||DC()));const l=slotCfg(x,'L'),r=slotCfg(x,'R');return cg(l.c||DC())+' top/100% 50% no-repeat,'+cg(r.c||DC())+' bottom/100% 50% no-repeat';};
const tpOn=()=>S.tp&&((S.tab==='rid'&&S.tb==='pl')||S.se);
function goTb(k){S.sd=false;if(k==='col'&&S.tb!=='col')S.sub='one';S.tb=k;S.ie=null;R();}
function setSub(k){S.sub=k;if(k==='one')S.cs='one';else if(S.cs==='one')S.cs='pat';R();}
function acc(id,t,s,b){const o=S.open===id;return'<div class="card '+(o?'open':'')+'"><div class="ah" onclick="S.open='+(o?'null':"'"+id+"'")+';R()"><div><div class="at">'+t+'</div><div class="as">'+s+'</div></div>'+CH+'</div>'+(o?'<div class="ab">'+b+'</div>':'')+'</div>';}
const rng=(k,l,mn,mx,cb)=>'<div class="pr"><div class="pl"><span>'+l+'</span><span id="v_'+k+'">'+S[k]+'</span></div><input type="range" min="'+mn+'" max="'+mx+'" value="'+S[k]+'" oninput="S.'+k+'=+this.value;$(\'#v_'+k+'\').textContent=S.'+k+(cb||'')+'"></div>';
function fq(v){v=v.toLowerCase();document.querySelectorAll('[data-n]').forEach(e=>e.style.display=e.dataset.n.indexOf(v)<0?'none':'');}
function rPick(i){if(S.fside==='all'){S.sel=i;S.fs={L:null,R:null};}else S.fs[S.fside]=i;const c=S.fc[i];if(c){S.cs=c.cs;S.col={...c.col};S.pal=c.pal;S.ed=c.ed}R();}function wPick(i){S.wsel=i;R();}function fqx(v){S.q=v;fq(v);}
function colorBlock(){return'<div class="blk'+(S.cs==='one'?' on':'')+'">'+modeRow('one')+pkHTML(1)+'</div><div class="blk'+(S.cs!=='one'?' on':'')+'">'+modeRow('skin')+skinList()+'</div>';}
function skinList(){const cu=[...S.cu.keys()].reverse().map(i=>'<div class="pw cu'+(S.cs==='cus'&&S.ed===i?' on':'')+'" style="margin-bottom:7px" onclick="S.cs=\'cus\';S.ed='+i+';R()"><span class="rd"></span><span class="pn" style="text-align:left">'+S.cu[i].n+'<em class="cb">✦ Custom</em></span><span class="ps" style="background:'+cuG(S.cu[i])+'"></span><button class="ib" style="width:36px;height:36px" aria-label="Edit skin" onclick="event.stopPropagation();cpEdit('+i+')">✎</button><button class="ib" style="width:36px;height:36px;color:var(--bad)" aria-label="Hapus skin" onclick="event.stopPropagation();cpDel('+i+')">🗑</button></div>').join('');
return(cu||'<div class="emp" style="margin:8px 0">Belum ada skin custom. Ketuk “＋ Tambah Skin” untuk membuat.</div>')+'<div class="pg">'+PAL.map((p,i)=>'<div class="pw'+(S.cs==='pat'&&S.pal===i?' on':'')+'" onclick="S.cs=\'pat\';S.pal='+i+';R()"><span class="rd"></span><span class="pn">'+p.n+'</span><span class="ps" style="background:'+grad(p.c)+'"></span></div>').join('')+'</div>';}
const pr=i=>S.pv[i]||(S.pv[i]=(FX[i]?FX[i].p:[]).map(()=>128));
function vDash(){const l=S.v<11.8?'Aki lemah':S.v>13.2?'Sedang mengisi':'Normal';return'<div class="card cp"><div class="at" style="color:var(--ac)">Selamat datang</div><div style="font-size:18px;font-weight:800">Riding · '+act()+'</div></div><div class="card cp"><div class="at">Status Sistem</div><div class="row"><button class="btn sm" onclick="S.pw=!S.pw;R()">'+(S.pw?'Matikan':'Nyalakan')+'</button><span class="m" style="flex:1">Kecerahan diatur lewat tombol ☀ di atas</span></div></div><div class="card cp"><div class="at">Voltmeter (data tiruan)</div><div class="big" id="vv">'+S.v.toFixed(1)+' V</div><div class="bar"><i id="vi" style="left:'+((S.v-10)/5*100)+'%"></i></div><b id="vs">'+l+'</b></div>';}
function body(){const w=S.tab==='wel';if(S.tb==='col')return colorBlock();
if(S.tb==='dur'&&w)return'<div class="card cp"><div class="pr"><div class="pl"><span>Durasi sebelum pindah ke Riding (detik)</span><span id="v_wd">'+S.wdur+'</span></div><input type="range" min="2" max="15" value="'+S.wdur+'" oninput="S.wdur=+this.value;$(\'#v_wd\').textContent=this.value" onchange="R()"></div></div>';
if(S.tb==='pl'&&!w)return player();return fxList(w?'w':'r',!w);}
const vWel=()=>body();
function vSoon(){return'<div class="card cp"><div style="font-weight:800">'+(S.tab==='rem'?'Rem & Sein':'Pengaturan')+'</div><p class="m">Dirancang setelah alur Dashboard, Welcoming, dan Riding disetujui.</p></div>';}
function editList(i){S.tp=null;const p=S.lists[i];openSheet('<div class="row" style="margin-bottom:8px"><input class="srch" style="margin:0" type="text" maxlength="20" value="'+p.n+'" onchange="S.lists['+i+'].n=this.value;R()"><button class="btn sm" onclick="delPl('+i+');closeSheet();R()">Hapus</button></div>'+
(p.it.length?p.it.map((x,j)=>'<div class="sl"><span class="m">'+(j+1)+'</span><b onclick="pickFx('+i+','+j+')" style="cursor:pointer">'+FX[x.f].n+' <span class="m">· ganti</span></b><select onchange="S.lists['+i+'].it['+j+'].d=+this.value">'+[10,20,30,60].map(d=>'<option '+(x.d===d?'selected':'')+' value="'+d+'">'+d+' dtk</option>').join('')+'</select><button class="ib" onclick="mv('+i+','+j+',-1)">↑</button><button class="ib" onclick="mv('+i+','+j+',1)">↓</button><button class="ib" style="color:var(--bad)" onclick="S.lists['+i+'].it.splice('+j+',1);editList('+i+');R()">×</button></div>').join(''):'<div class="m" style="padding:10px 0">Playlist kosong.</div>')+
'<div class="row" style="margin-top:12px"><button class="btn" onclick="pickFx('+i+',-1)">+ Tambah efek ('+p.it.length+'/10)</button><button class="btn p" onclick="closeSheet();R()">Selesai</button></div>');}
function mv(i,j,d){const a=S.lists[i].it,k=j+d;if(k<0||k>=a.length)return;[a[j],a[k]]=[a[k],a[j]];editList(i);}
setInterval(()=>{S.v=Math.max(10.5,Math.min(14.4,S.v+(Math.random()-.5)*.1));const hv=$('#hv');if(hv&&S.net)hv.textContent=S.v.toFixed(1)+' V';const v=$('#vv');if(v){v.textContent=S.v.toFixed(1)+' V';$('#vi').style.left=((S.v-10)/5*100)+'%';$('#vs').textContent=S.v<11.8?'Aki lemah':S.v>13.2?'Sedang mengisi':'Normal';}},1500);
function hdr(){return'<button class="volt" onclick="netDemo()" aria-label="Status koneksi. Ketuk untuk simulasi terputus"><span class="vd" style="background:'+(S.net?'var(--ok)':'var(--bad)')+'"></span><span id="hv">'+(S.net?S.v.toFixed(1)+' V':'Terputus')+'</span></button><button class="ib" style="border-radius:50%" aria-label="Kecerahan LED" onclick="briOpen()">☀</button>';}
function vRid(){return body();}
function saveCol(){S.fc[S.sel]={cs:S.cs,col:{...S.col},pal:S.pal,ed:S.ed};R();commit('Warna '+FX[S.sel].n+' tersimpan');}
function favCol(){S.fc[S.sel]={cs:S.cs,col:{...S.col},pal:S.pal,ed:S.ed};S.fav.add(S.sel);R();commit(FX[S.sel].n+' masuk favorit');}
function setRid(i){const sd=S.fside;if(sd==='all'){S.act={t:'fx',i};S.as={L:null,R:null};S.lf=i;}else{if(S.act.t!=='fx')S.act={t:'fx',i:S.lf};S.as[sd]=i;}S.src='fx';R();commit(FX[i].n+(sd==='all'?'':' di LED '+SDN[sd].toLowerCase())+' sekarang menyala');}
function setSide(sd){S.fside=sd;R();}
function editFx(i){rPick(i);S.tb='fx';R();}
function tbs(a){return'<div class="tabbar">'+a.map(x=>'<button class="'+(x[3]?'on':'')+''+'" onclick="'+x[2]+'">'+x[0]+'</button>').join('')+'</div>';}
function pbar(i){const f=FX[i],v=pr(i);return f&&f.p&&f.p.length?'<div class="pb"><div class="pg2">'+f.p.map((n,k)=>'<label>'+n+'<input type="range" min="0" max="255" value="'+(v[k]||128)+'" oninput="S.pv['+i+']['+k+']=+this.value;'+(k?'':'S.sx=+this.value')+'"></label>').join('')+'</div></div>':'';}
function foot(){const w=S.tab==='wel',r=S.tab==='rid',sv='<button class="btn p" onclick="commit(\'Welcoming tersimpan\')">Simpan welcoming</button>';
if(S.tb==='fx'&&(w||r)){const i=w?S.wsel:S.sel;return(FX[i]&&FX[i].p.length?pbar(i):'')+(w?sv:'');}
if(r&&S.tb==='col'){const d=colDirty();return'<div class="row" style="width:100%;gap:8px"><button class="btn p fbtn" style="'+(d?'':'opacity:.55')+'" onclick="saveCol()">'+(d?'Simpan '+(S.cs==='one'?'warna':'skin'):(S.cs==='one'?'Warna':'Skin')+' tersimpan ✓')+'<small>'+FX[S.sel].n+'</small></button><button class="btn fbtn sk" onclick="cpEdit(-1)">＋ Skin Custom<small>Tambah baru</small></button></div>';}return w?sv:'';}
function tf(i){S.fav.has(i)?S.fav.delete(i):S.fav.add(i);R();}
function cpSave(){const d=S.dr,n=d.c.n.trim();d.c.n=n||'Skin Custom '+((d.i<0?S.cu.length:d.i)+1);if(d.i<0){S.cu.push(d.c);S.ed=S.cu.length-1;}else{S.cu[d.i]=d.c;S.ed=d.i;}S.cs='cus';S.sub='skin';closeSheet();R();commit('Skin tersimpan');}
const SV='<svg viewBox="0 0 24 24" width="18" height="18" fill="none" stroke="currentColor" stroke-width="2" stroke-linejoin="round"><path d="M5 3h11l3 3v15H5z"/><path d="M8 3v6h8V3M8 21v-7h8v7"/></svg>';
const srt=c=>[...c.st].sort((a,b)=>a[0]-b[0]).map(s=>hex(s[1]));
const sc=c=>c.k==='p'?PAL[c.i].c.map(hex):c.k==='c'&&S.cu[c.i]?srt(S.cu[c.i]):[hsv(c.h,c.s??100,c.v??100)];
const cg=c=>c.k==='p'?grad(PAL[c.i].c):c.k==='c'&&S.cu[c.i]?cuG(S.cu[c.i]):css(hsv(c.h,c.s??100,c.v??100));
const toFc=c=>c.k==='p'?{cs:'pat',pal:c.i,col:{h:35,s:100},ed:0}:c.k==='c'?{cs:'cus',pal:S.pal,col:{h:35,s:100},ed:c.i}:{cs:'one',pal:S.pal,col:{h:c.h,s:c.s??100,v:c.v??100},ed:0};
const fromFc=f=>f.cs==='pat'?{k:'p',i:f.pal,h:35,s:100}:f.cs==='cus'?{k:'c',i:f.ed,h:35,s:100}:{k:'o',h:f.col.h,s:f.col.s,v:f.col.v};
const fcCols=f=>f.cs==='pat'?PAL[f.pal].c.map(hex):f.cs==='cus'&&S.cu[f.ed]?srt(S.cu[f.ed]):[hsv(f.col.h,f.col.s,f.col.v??100)];
function riInfo0(){const a=S.act;if(a.t==='list'){const l=S.lists[a.i];return l?'<b>'+l.n+'</b><span>Playlist · '+l.it.length+' slot</span>':'';}
const c0=fxActiveColor(a.i),c=c0.k==='p'?['Skin · '+PAL[c0.i].n,grad(PAL[c0.i].c)]:c0.k==='c'&&S.cu[c0.i]?['Skin Custom · '+S.cu[c0.i].n,cuG(S.cu[c0.i])]:['Solid',css(hsv(c0.h,c0.s,c0.v??100))];
return'<b>'+FX[a.i].n+'</b><span>Single · <i style="background:'+c[1]+'"></i>'+c[0]+'</span>';}
function sideBlock(){const r=S.tab==='rid',fx=r&&S.tb==='fx',on=k=>fx&&S.fside===k?' on':'',T=fx?'button':'div',oc=k=>fx?' onclick="setSide(\''+k+'\')"':'';
return'<div class="sbk'+(fx?' act':'')+'" role="group" aria-label="LED kiri dan kanan"><'+T+' class="sbn L'+on('L')+'"'+oc('L')+' aria-label="LED kiri"><span>Kiri</span></'+T+'><div class="sbm">'+(fx?'<button class="chip'+(S.fo?' on':'')+'" onclick="S.fo=!S.fo;R()">★ Favorit</button><button class="sbn A'+on('all')+'"'+oc('all')+'>Semua</button>':'')+'</div><'+T+' class="sbn R'+on('R')+'"'+oc('R')+' aria-label="LED kanan"><span>Kanan</span></'+T+'></div>';}
function frz(){if(S.tab==='dash'||S.tab==='rem'||S.tab==='set')return'';const r=S.tab==='rid',t=(n,k)=>[n,'',"goTb('"+k+"')",S.tb===k];
return(r&&S.tb==='col'?'':tbs(r?[t('Single Effect','fx'),t('Playlist Animasi','pl')]:[t('Single Effect','fx'),t('Durasi','dur')]))+sideBlock();}
function fxList(cx,star){const sel=cx==='w'?S.wsel:selOf();let h='',n=0;
FX.forEach((f,i)=>{if(star&&S.fo&&!S.fav.has(i))return;n++;const on=sel===i,fv=S.fav.has(i),ra=S.fside==='all'?(S.act.t==='fx'&&S.act.i===i&&S.as.L==null&&S.as.R==null):actOf(S.fside)===i,hid=S.q&&f.n.toLowerCase().indexOf(S.q.toLowerCase())<0;
const actions=star&&on?'<button class="fxcolor" aria-label="Warna" onclick="event.stopPropagation();fxColorOpen('+i+')"><i style="background:'+cg(fxActiveColor(i))+'"></i><span>Warna</span></button><button class="rp fxride'+(ra?' on':'')+'" onclick="event.stopPropagation();setRid('+i+')">'+(ra?'✓ Dipakai':'Pakai')+(S.fside==='all'?'':' · '+SDN[S.fside])+'</button><button class="star'+(fv?' on':'')+'" aria-label="Favorit" onclick="event.stopPropagation();tf('+i+')">'+(fv?'★':'☆')+'</button>':'';
h+='<div class="fr'+(on?' on':'')+'" data-n="'+f.n.toLowerCase()+'" style="'+(hid?'display:none':'')+'" onclick="'+cx+'Pick('+i+')"><span class="no">'+n+'</span><b>'+f.n+'</b>'+actions+'</div>'+'';
});return h;}
function fxDefaultColor(){return {k:'p',i:0,h:35,s:100};}
function fxActiveColor(i){return S.fc[i]?fromFc(S.fc[i]):fxDefaultColor();}
function fxColorName(c){return c.k==='o'?'Warna Solid':c.k==='p'?'Skin · '+PAL[c.i].n:'Skin Custom · '+(S.cu[c.i]?S.cu[c.i].n:'');}
function fxColorOpen(i){const c=fxActiveColor(i);S.ec={i,c:{...c},mode:c.k==='o'?'one':'skin'};fxColorRender();}
function fxColorMode(mode){const d=S.ec;if(mode==='one'&&d.c.k!=='o'){const a=rgb2hsv(...sc(d.c)[0]);d.c={k:'o',h:a[0],s:a[1],v:a[2]};}d.mode=mode;fxColorRender();}
function fxColorPick(k,i){const d=S.ec;d.c=k==='p'?{k:'p',i,h:35,s:100}: {k:'c',i,h:35,s:100};fxColorRender();}
function fxColorRender(){const d=S.ec,c=d.c,idx=d.i,preview=cg(c),name=fxColorName(c);
let editor='';
if(d.mode==='one')editor=pkHTML(1);
else{
 const custom=S.cu.map((q,i)=>'<div class="pw'+(c.k==='c'&&c.i===i?' on':'')+' cu" onclick="fxColorPick(\'c\','+i+')"><span class="rd"></span><span class="pn">'+q.n+'<em class="cb">✦ Custom</em></span><span class="ps" style="background:'+cuG(q)+'"></span></div>').join('');
 editor='<div class="fxc-skins"><div class="at">Skin Custom</div>'+(custom||'<div class="m">Belum ada Skin Custom.</div>')+'<div class="at" style="margin-top:5px">Skin bawaan</div><div class="pg">'+PAL.map((q,i)=>'<div class="pw'+(c.k==='p'&&c.i===i?' on':'')+'" onclick="fxColorPick(\'p\','+i+')"><span class="rd"></span><span class="pn">'+q.n+'</span><span class="ps" style="background:'+grad(q.c)+'"></span></div>').join('')+'</div><div class="m">Untuk membuat atau mengelola Skin Custom, buka tab Warna / Skin.</div></div>';
}
const html='<div style="font-weight:800;margin-bottom:9px">Ubah warna · '+FX[idx].n+'</div><div class="pbody"><div class="fxc-preview"><div><small>Warna yang akan diterapkan</small><b id="fxc-name">'+name+'</b></div><span class="fxc-chip" id="fxc-chip" style="background:'+preview+'"></span></div><div class="bl" style="padding:0"><button class="bu'+(d.mode==='one'?' on':'')+'" onclick="fxColorMode(\'one\')"><i></i>Warna Solid</button><button class="bu'+(d.mode==='skin'?' on':'')+'" onclick="fxColorMode(\'skin\')"><i></i>Skin Efek</button></div><div class="grow">'+editor+'</div></div><div class="row" style="margin-top:12px;flex:none"><button class="btn" onclick="S.ec=null;closeSheet()">Batal</button><button class="btn p" onclick="fxColorApply()">Simpan warna</button></div>';
openSheet(html,true);
if(d.mode==='one')pkBind(c,()=>{const e=$('#fxc-chip'),n=$('#fxc-name');if(e)e.style.background=cg(c);if(n)n.textContent=fxColorName(c);});
}
function fxColorApply(){const d=S.ec;if(!d)return;const c=d.c;S.fc[d.i]=toFc(c);S.sel=d.i;S.ec=null;closeSheet();R();commit('Warna '+FX[d.i].n+' tersimpan');}

const rx=r=>'#'+r.map(v=>Math.round(v).toString(16).padStart(2,'0')).join(''),QC=['#ff0000','#00ff00','#0000ff','#ffffff','#ffff00','#ff00ff'];
let QS=['#00ff00',null,null,null,null,null];try{const q=JSON.parse(localStorage.getItem('mzQs2')||'null');if(Array.isArray(q)&&q.length===6)QS=q;}catch(e){}
const qsSave=()=>{try{localStorage.setItem('mzQs2',JSON.stringify(QS));}catch(e){}};
function pkRand(){pkSet(Math.random()*360,60+Math.random()*40,70+Math.random()*30);}
function hsv(h,s,v){s/=100;v/=100;const f=n=>{const k=(n+h/60)%6;return v-v*s*Math.max(0,Math.min(k,4-k,1))};return[f(5),f(3),f(1)].map(x=>Math.round(x*255));}
function rgb2hsv(r,g,b){r/=255;g/=255;b/=255;const m=Math.max(r,g,b),d=m-Math.min(r,g,b);let h=0;if(d){h=m===r?((g-b)/d)%6:m===g?(b-r)/d+2:(r-g)/d+4;h=(h*60+360)%360;}return[h,m?d/m*100:0,m*100];}
function k2rgb(k){const t=k/100,c=x=>Math.max(0,Math.min(255,Math.round(x)));return[c(t<=66?255:329.7*Math.pow(t-60,-.1332)),c(t<=66?99.47*Math.log(t)-161.12:288.12*Math.pow(t-60,-.0755)),c(t>=66?255:t<=19?0:138.52*Math.log(t-10)-305.04)];}
let PK,WI;
function pkHTML(c){return'<div class="pkx" id="pkx"><div class="wbox"><canvas id="pkw" class="wh" width="360" height="360"></canvas></div><div class="ctl"><button class="rgbx" id="pkrgbb" aria-label="Warna RGB" onclick="pkRgbOpen()">'+[['r','#e5332b'],['g','#1fa84a'],['b','#2f6bff']].map(k=>'<span><b style="background:'+k[1]+'">'+k[0].toUpperCase()+'</b><em id="pk'+k[0]+'v"></em></span>').join('')+'</button><div class="pk"><div style="font-size:13px;font-weight:700">Kecerahan: <span id="pkvl"></span></div><input id="pkv" type="range" min="0" max="100" aria-label="Kecerahan"></div><button class="ib rnd" id="pkrand" aria-label="Acak">🎲</button></div><div class="smp" id="pks"></div><div class="slt" id="pkq"></div><div class="cf"><button class="btn" onclick="pkCancel()">Batal</button><button class="btn p" id="pkcl" onclick="pkConfirm()">Simpan</button></div></div>';}
function pkRgbOpen(){const L={r:'#e5332b',g:'#1fa84a',b:'#2f6bff'};$('#pop').innerHTML='<div><div style="font-weight:800;margin-bottom:10px">Warna manual</div><div class="row" style="margin-bottom:10px"><i id="pkpv" style="width:56px;height:40px;border-radius:9px;border:1px solid rgba(255,255,255,.3)"></i><span class="m">Hex</span><input id="pkh" class="srch" maxlength="7" autocapitalize="off" spellcheck="false" style="margin:0;flex:1" placeholder="#rrggbb"></div><div class="pk">'+['r','g','b'].map(k=>'<label class="rg"><b style="background:'+L[k]+'">'+k.toUpperCase()+'</b><input id="pk'+k+'" type="range" min="0" max="255"><input id="pk'+k+'n" class="dur" type="number" inputmode="numeric" min="0" max="255"></label>').join('')+'</div><button class="btn p" style="width:100%;margin-top:12px" onclick="pkRgbClose()">Selesai</button></div>';$('#pop').className='on';S.popKind='other';
['r','g','b'].forEach(k=>{$('#pk'+k).oninput=()=>{const a=rgb2hsv(...['r','g','b'].map(x=>+$('#pk'+x).value));pkSet(a[0],a[1],a[2],'rgb');};$('#pk'+k+'n').oninput=e=>{const v=Math.max(0,Math.min(255,Math.round(+e.target.value)||0));$('#pk'+k).value=v;$('#pk'+k).oninput();};});
$('#pkh').oninput=e=>{let v=e.target.value.trim();if(/^[0-9a-f]{6}$/i.test(v))v='#'+v;if(/^#[0-9a-f]{6}$/i.test(v))pkHex(v);};pkUpd();}
function pkRgbClose(){const back=S.popKind==='other'&&S.ie&&S.ie.pop&&!S.ie.open;$('#pop').className='';S.popKind=null;if(back)slotPopShow();}
function popOut(){S.popKind==='slot'?slotPopClose():pkRgbClose();}
function pkSlots(){const q=$('#pkq');if(!q)return;q.innerHTML=QS.map((x,k)=>x?'<i data-k="'+k+'" style="background:'+x+'"><b class="pe" data-e="'+k+'">✎</b></i>':'<i data-k="'+k+'" class="em">+</i>').join('');}
function pkSlotEdit(k){const o=PK.o;PK.sl=k;PK.pv=[o.h,o.s,o.v];if(QS[k])pkHex(QS[k]);$('#pkx').classList.add('ed');$('#pkcl').textContent='Simpan slot '+(k+1);}
function pkExit(){PK.sl=null;$('#pkx').classList.remove('ed');}
function pkCancel(){const p=PK.pv;pkExit();pkSet(p[0],p[1],p[2]);}
function pkConfirm(){const o=PK.o,k=PK.sl;QS[k]=rx(hsv(o.h,o.s,o.v));qsSave();pkExit();pkSlots();commit('Slot warna '+(k+1)+' tersimpan');}
function pkUpd(skip){const o=PK.o,r=hsv(o.h,o.s,o.v),w=$('#pkw');if(!w)return;const vl=$('#pkvl');if(vl)vl.textContent=Math.round(o.v);const g=w.getContext('2d'),n=w.width,R=n/2-12;
if(!WI){WI=g.createImageData(n,n);for(let y=0;y<n;y++)for(let x=0;x<n;x++){const dx=x-n/2,dy=y-n/2,d=Math.hypot(dx,dy);if(d>R+.5)continue;WI.data.set([...hsv((Math.atan2(dy,dx)*180/Math.PI+360)%360,Math.min(100,d/R*100),100),Math.round(255*Math.min(1,R-d+.5))],(y*n+x)*4);}}
g.putImageData(WI,0,0);const a=o.h*Math.PI/180,d=o.s/100*R;g.beginPath();g.arc(n/2+Math.cos(a)*d,n/2+Math.sin(a)*d,11,0,7);g.fillStyle=rx(r);g.fill();g.lineWidth=3;g.strokeStyle='#fff';g.stroke();
const G=(id,v,bg)=>{const e=$('#'+id);if(!e)return;if(v!=null)e.value=v;e.style.setProperty('--g',bg);};
G('pkv',o.v,'linear-gradient(90deg,#000,'+rx(hsv(o.h,o.s,100))+')');G('pkk',null,'linear-gradient(90deg,#ff8a12,#ffe4b0,#fff,#cfdcff,#9bbcff)');
['r','g','b'].forEach((k,i)=>{const lo=[...r],hi=[...r];lo[i]=0;hi[i]=255;G('pk'+k,skip==='rgb'?null:r[i],'linear-gradient(90deg,'+rx(lo)+','+rx(hi)+')');});
['r','g','b'].forEach((k,i)=>{const t=$('#pk'+k+'v');if(t)t.textContent=r[i];const n=$('#pk'+k+'n');if(n&&document.activeElement!==n)n.value=r[i];});const pp=$('#pkpv');if(pp)pp.style.background=rx(r);
if(skip!=='hex'){const e=$('#pkh');if(e)e.value=rx(r);}}
function pkSet(h,s,v,skip){const o=PK.o;o.h=h;o.s=s;o.v=v;pkUpd(skip);if(PK.cb)PK.cb();}
function pkHex(x){const a=rgb2hsv(...hex(x));pkSet(a[0],a[1],a[2]);}
function pkBind(o,cb){PK={o,cb};o.v=o.v??100;pkUpd();const w=$('#pkw'),mv=e=>{const b=w.getBoundingClientRect(),dx=e.clientX-b.left-b.width/2,dy=e.clientY-b.top-b.height/2;pkSet((Math.atan2(dy,dx)*180/Math.PI+360)%360,Math.min(100,Math.hypot(dx,dy)/((b.width/2)*(1-24/w.width))*100),o.v);};
w.onpointerdown=e=>{w.setPointerCapture(e.pointerId);mv(e);w.onpointermove=mv;};w.onpointerup=()=>{w.onpointermove=null;};
$('#pkv').oninput=e=>pkSet(o.h,o.s,+e.target.value);if($('#pkk'))$('#pkk').oninput=e=>{const a=rgb2hsv(...k2rgb(+e.target.value));pkSet(a[0],a[1],o.v);};
if($('#pkh'))$('#pkh').oninput=e=>{if(/^#[0-9a-f]{6}$/i.test(e.target.value))pkHex(e.target.value);};
$('#pks').innerHTML=QC.map(x=>'<i style="background:'+x+'" data-c="'+x+'"></i>').join('');pkSlots();
$('#pks').onclick=e=>{const t=e.target.closest('i');if(t)pkHex(t.dataset.c);};
$('#pkq').onclick=e=>{const t=e.target.closest('i');if(!t)return;const k=+t.dataset.k;if(e.target.closest('[data-e]')||!QS[k])pkSlotEdit(k);else pkHex(QS[k]);};
$('#pkrand').onclick=pkRand;}

function wheel(){if(!$('#scr #pkw')||document.querySelector('#sheet.on'))return;if(S.ie&&S.ie.open==='col'&&S.ie.mode==='one'&&S.tab==='rid'&&S.tb==='pl')pkBind(S.ie.c,()=>{const ch=$('#iech'),n=$('#iecn');if(ch)ch.style.background=cg(S.ie.c);if(n)n.textContent=ckn(S.ie.c);});else pkBind(S.col,()=>{S.cs='one';blkSync();$('#foot').innerHTML=foot();});}
function ask(t,fn){S.ak=fn;openSheet('<div style="font-weight:800;margin-bottom:12px">'+t+'</div><div class="row"><button class="btn" onclick="closeSheet()">Batal</button><button class="btn p" style="background:var(--bad);border-color:var(--bad);color:#fff" onclick="closeSheet();S.ak()">Hapus</button></div>');}
function cusEd(){return S.cu.map((c,i)=>'<div class="pw'+(S.cs==='cus'&&S.ed===i?' on':'')+'" style="margin-bottom:7px" onclick="S.cs=\'cus\';S.ed='+i+';R()"><span class="rd"></span><span class="pn" style="text-align:left">'+c.n+'</span><button class="ib" aria-label="Edit" onclick="event.stopPropagation();cpEdit('+i+')">✎</button><button class="ib" aria-label="Hapus" style="color:var(--bad)" onclick="event.stopPropagation();cpDel('+i+')">🗑</button><span class="ps" style="background:'+cuG(c)+'"></span></div>').join('')||'<div class="m" style="padding:8px 0">Belum ada pola warna custom.</div>';}
function cpEdit(i){if(i<0&&S.cu.length>=30)return toast('Maksimal 30 skin custom.');S.dr={i,c:i<0?{n:'',st:[[0,'#ff0000'],[100,'#0000ff']]}:JSON.parse(JSON.stringify(S.cu[i]))};S.sp=0;cpR();}
function cpR(){const c=S.dr.c;openSheet('<div style="font-weight:800;margin-bottom:8px">'+(S.dr.i<0?'Skin efek baru':'Edit skin efek')+'</div><input class="srch" type="text" maxlength="24" placeholder="Isi nama" value="'+c.n+'" oninput="S.dr.c.n=this.value"><div class="gb" id="gbar" style="background:'+cuG(c)+'">'+c.st.map((s,j)=>'<i class="stp'+(j===S.sp?' on':'')+'" style="left:'+s[0]+'%;background:'+s[1]+'"></i>').join('')+'</div><div class="m" style="margin-bottom:8px">Ketuk titik untuk memilih warna, geser untuk posisi.</div><div class="row" style="margin-bottom:10px;flex-wrap:wrap"><button class="btn sm" onclick="if(S.dr.c.st.length<8){S.dr.c.st.push([50,\'#ffffff\']);S.sp=S.dr.c.st.length-1;cpR()}">+ Titik</button><button class="btn sm" onclick="if(S.dr.c.st.length>2){S.dr.c.st.splice(S.sp,1);S.sp=0;cpR()}">Hapus</button><button class="btn sm" onclick="cpRand()">🎲 Acak</button></div>'+pkHTML(1)+'<div class="row cpf"><button class="btn" onclick="closeSheet()">Batal</button><button class="btn p" onclick="cpSave()">Simpan</button></div>');cpBind();}
function cpU(){const c=S.dr.c;$('#gbar').style.background=cuG(c);document.querySelectorAll('#gbar .stp').forEach((e,j)=>{e.style.left=c.st[j][0]+'%';e.style.background=c.st[j][1];});}
function cpRand(){const n=S.dr.c.st.length,b=Math.random()*360,m=Math.random()<.5;S.dr.c.st.forEach((s,k)=>{s[1]=rx(hsv(m?(b+k*360/n+Math.random()*30)%360:Math.random()*360,65+Math.random()*35,70+Math.random()*30));});cpR();}
function cpBind(){const bar=$('#gbar'),sel=j=>{S.sp=j;bar.querySelectorAll('.stp').forEach((x,k)=>x.classList.toggle('on',k===j));const a=rgb2hsv(...hex(S.dr.c.st[j][1]));S.dpk={h:a[0],s:a[1],v:a[2]};pkBind(S.dpk,()=>{S.dr.c.st[S.sp][1]=rx(hsv(S.dpk.h,S.dpk.s,S.dpk.v));cpU();});};let dg=false;
bar.onpointerdown=e=>{const b=bar.getBoundingClientRect(),x=(e.clientX-b.left)/b.width*100;let j=-1,m=1e9;S.dr.c.st.forEach((s,k)=>{const d=Math.abs(s[0]-x);if(d<m){m=d;j=k;}});if(j<0||m*b.width/100>28)return;e.preventDefault();bar.setPointerCapture(e.pointerId);dg=true;if(S.sp!==j)sel(j);};
bar.onpointermove=e=>{if(!dg)return;const b=bar.getBoundingClientRect();S.dr.c.st[S.sp][0]=Math.round(Math.max(0,Math.min(100,(e.clientX-b.left)/b.width*100)));cpU();};bar.onpointerup=bar.onpointercancel=()=>{dg=false;};sel(S.sp);}
function cpDel0(i){const c=S.cu[i],se=[S.ed,S.cs];S.cu.splice(i,1);S.ed=0;if(!S.cu.length&&S.cs==='cus')S.cs='pat';R();commit('Skin “'+c.n+'” dihapus',()=>{S.cu.splice(i,0,c);S.ed=se[0];S.cs=se[1];R();});}
const curC=()=>S.cs==='pat'?{k:'p',i:S.pal,h:35,s:100}:S.cs==='cus'?{k:'c',i:S.ed,h:35,s:100}:{k:'o',h:S.col.h,s:S.col.s,v:S.col.v};
function setSrc(k){if(S.src===k)return;S.src=k;S.pi=null;S.tp=null;S.ie=null;S.act=k==='fx'?{t:'fx',i:S.lf}:S.lists.length?{t:'list',i:Math.min(S.ll,S.lists.length-1)}:S.act;R();}
function player(){return S.pi!=null&&S.lists[S.pi]?plFolder(S.pi):plGallery();}
const DC=()=>({k:'p',i:1,h:35,s:100});
const favFirst=()=>FX.map((f,k)=>k).sort((a,b)=>(S.fav.has(b)-S.fav.has(a))||(a-b));
function ieCur(){const e=S.ie;if(e.t==='single')return{f:S.lf,c:fxActiveColor(S.lf)};const x=S.lists[e.i].it[e.j],g=slotCfg(x,e.sd);return{f:g.f,c:g.c||DC()};}
function ieR(){const a=$('#iel'),b=$('#ieg'),la=a?a.scrollTop:0,lb=b?b.scrollTop:0;R();const a2=$('#iel'),b2=$('#ieg');if(a2)a2.scrollTop=la;if(b2)b2.scrollTop=lb;}
function ieStart(){S.ie={t:'single',open:null,h:S.lf,c:{...fxActiveColor(S.lf)},mode:'skin'};R();}
function ieEnd(){S.ie=null;R();}
function ieTog(k){const e=S.ie;if(!e)return;if(e.open===k){e.open=null;return R();}const u=ieCur();e.open=k;e.h=u.f;e.c={...u.c};e.mode=u.c.k==='o'?'one':'skin';R();}
function ieHi(k){S.ie.h=k;ieR();}
function ieColMode(m){const e=S.ie;if(m==='one'&&e.c.k!=='o'){const a=rgb2hsv(...sc(e.c)[0]);e.c={k:'o',h:a[0],s:a[1],v:a[2]};}e.mode=m;ieR();}
function ieColPick(k,i){S.ie.c={k,i,h:35,s:100};ieR();}
function ieSetFx(){const e=S.ie;if(e.t==='single'){S.lf=e.h;if(S.act.t==='fx')S.act={t:'fx',i:e.h};}else{const x=S.lists[e.i].it[e.j];if(!e.sd||e.sd==='all')x.f=e.h;else x[e.sd]={f:e.h,c:(x[e.sd]&&x[e.sd].c)||x.c};}e.open=null;R();commit(FX[e.h].n+' dipilih untuk slot');if(S.ie&&S.ie.pop)slotPopShow();}
function ieSetCol(){const e=S.ie;if(e.t==='single')S.fc[S.lf]=toFc(e.c);else{const x=S.lists[e.i].it[e.j];if(!e.sd||e.sd==='all')x.c={...e.c};else x[e.sd]={f:x[e.sd]?x[e.sd].f:x.f,c:{...e.c}};}e.open=null;R();commit('Warna slot tersimpan');if(S.ie&&S.ie.pop)slotPopShow();}
function ieParams(h){const f=FX[h];return f&&f.p&&f.p.length?'<div class="pt">Parameter · '+f.n+'</div><div class="pg2">'+f.p.map((n,k)=>'<label>'+n+'<input type="range" min="0" max="255" value="'+(pr(h)[k]||128)+'" oninput="S.pv['+h+']['+k+']=+this.value'+(k?'':';S.sx=+this.value')+'"></label>').join('')+'</div>':'<div class="m">Efek ini tidak punya parameter.</div>';}
function ieField(k,l,v,o){return'<div class="fld'+(o?' on':'')+'" onclick="ieTog(\''+k+'\')"><span class="m">'+l+'</span>'+v+'<span class="ied">✎</span>'+CH+'</div>';}
function ieFxPanel(f,e){return'<div class="iep"><div class="dh"><button class="ib" aria-label="Kembali" onclick="ieBack()">‹</button>Pilih efek</div><div class="grow" id="iel">'+favFirst().map(k=>'<div class="fr'+(k===e.h?' on':'')+(S.fav.has(k)?' fv':'')+'" onclick="ieHi('+k+')"><b>'+(S.fav.has(k)?'★ ':'')+FX[k].n+(k===f?' ✓':'')+'</b></div>').join('')+'</div><div class="pfix">'+ieParams(e.h)+'</div><button class="btn p" onclick="ieSetFx()">Pilih “'+FX[e.h].n+'”</button></div>';}
function skinPick(c){return[...S.cu.keys()].reverse().map(i=>'<div class="pw cu'+(c.k==='c'&&c.i===i?' on':'')+'" style="margin-bottom:7px" onclick="ieColPick(\'c\','+i+')"><span class="rd"></span><span class="pn" style="text-align:left">'+S.cu[i].n+'<em class="cb">✦ Custom</em></span><span class="ps" style="background:'+cuG(S.cu[i])+'"></span></div>').join('')+'<div class="pg">'+PAL.map((q,i)=>'<div class="pw'+(c.k==='p'&&c.i===i?' on':'')+'" onclick="ieColPick(\'p\','+i+')"><span class="rd"></span><span class="pn">'+q.n+'</span><span class="ps" style="background:'+grad(q.c)+'"></span></div>').join('')+'</div>';}
function ieColPanel(e){const c=e.c;return'<div class="iep"><div class="dh"><button class="ib" aria-label="Kembali" onclick="ieBack()">‹</button>Pilih warna</div><div class="fxc-preview"><div><small>Warna</small><b id="iecn">'+ckn(c)+'</b></div><span class="fxc-chip" id="iech" style="background:'+cg(c)+'"></span></div><div class="bl">'+[['one','Warna Solid'],['skin','Skin Efek']].map(z=>'<button class="bu'+(e.mode===z[0]?' on':'')+'" onclick="ieColMode(\''+z[0]+'\')"><i></i>'+z[1]+'</button>').join('')+'</div><div class="grow" id="ieg">'+(e.mode==='one'?pkHTML(1):skinPick(c))+'</div><button class="btn p" onclick="ieSetCol()">Pilih warna</button></div>';}
function ieFields(f,c,e){return ieField('fx','Efek','<b>'+FX[f].n+'</b>',e.open==='fx')+(e.open==='fx'?ieFxPanel(f,e):'')+ieField('col','Warna','<i class="mini" style="width:44px;background:'+cg(c)+'"></i><b>'+ckn(c)+'</b>',e.open==='col')+(e.open==='col'?ieColPanel(e):'');}
function singleBody(){const i=S.lf,c=fxActiveColor(i);
return'<div class="fr on"><b>'+FX[i].n+'</b><span class="mini" style="width:44px;background:'+cg(c)+'"></span></div><div class="m" style="margin:6px 0 10px">Efek ini menyala saat mode Riding.</div><div class="row"><button class="btn" onclick="S.sel='+i+';goTb(\'fx\')">Ganti efek</button><button class="btn" onclick="rPick('+i+');goTb(\'col\')">Ubah warna</button></div>';}
let SCD=null,SCB=0;
const scTM=e=>{if(SCD&&SCD.on)e.preventDefault();};
function scDown(ev,i,k){if(ev.target.closest('.se')||(ev.pointerType==='mouse'&&ev.button!==0))return;const el=ev.currentTarget,st={i,k,el,x0:ev.clientX,y0:ev.clientY,on:false,to:k,id:ev.pointerId,touch:ev.pointerType!=='mouse'};SCD=st;
if(ev.target.closest('.gh'))scStart(st);else if(st.touch)st.t=setTimeout(()=>scStart(st),240);
el.onpointermove=e=>scMove(e);el.onpointerup=el.onpointercancel=e=>scEnd(e);}
function scStart(st){if(SCD!==st||st.on)return;st.on=true;try{st.el.setPointerCapture(st.id);}catch(e){}st.el.classList.add('drg');if(navigator.vibrate)navigator.vibrate(12);
st.rects=[...st.el.parentNode.querySelectorAll('[data-sc]')].map(e=>({k:+e.dataset.sc,e,r:e.getBoundingClientRect()}));document.addEventListener('touchmove',scTM,{passive:false});}
function scMove(e){const st=SCD;if(!st)return;const dx=e.clientX-st.x0,dy=e.clientY-st.y0;
if(!st.on){if(Math.hypot(dx,dy)>(st.touch?10:6)){if(st.touch){clearTimeout(st.t);SCD=null;st.el.onpointermove=st.el.onpointerup=st.el.onpointercancel=null;}else scStart(st);}if(!st.on)return;}
st.el.style.transform='translate('+dx+'px,'+dy+'px) scale(1.06)';
for(const q of st.rects){const r=q.r;if(e.clientX>=r.left&&e.clientX<=r.right&&e.clientY>=r.top&&e.clientY<=r.bottom){st.to=q.k;break;}}
st.rects.forEach(q=>q.e.classList.toggle('tgt',q.k===st.to&&st.to!==st.k));}
function scEnd(e){const st=SCD;if(!st)return;clearTimeout(st.t);document.removeEventListener('touchmove',scTM);st.el.onpointermove=st.el.onpointerup=st.el.onpointercancel=null;
if(!st.on){SCD=null;return;}SCB=Date.now();SCD=null;if(e.type==='pointercancel'||st.to===st.k)R();else slotReorder(st.i,st.k,st.to);}
function slotReorder(i,from,to,quiet){const a=S.lists[i].it,x=a.splice(from,1)[0];a.splice(to,0,x);
const m=q=>q===from?to:(from<to&&q>from&&q<=to)?q-1:(from>to&&q>=to&&q<from)?q+1:q;
if(S.pi===i&&S.si>=0)S.si=m(S.si);if(S.tp&&S.tp.i===i)S.tp.j=m(S.tp.j);if(S.ie&&S.ie.t==='pl'&&S.ie.i===i&&S.ie.j>=0)S.ie.j=m(S.ie.j);
R();if(!quiet)commit('Urutan slot diubah',()=>slotReorder(i,to,from,1));}

const tot=p=>Math.round(p.it.reduce((z,x)=>z+x.d,0)*10)/10;
function plOpen(i){S.pi=i;S.si=-1;S.tp=S.tp&&S.tp.i===i?S.tp:null;R();$('#scr').scrollTop=0;}
function plBack(){S.pi=null;S.si=-1;R();}
function thumb(p){return p.it.length?'<div class="th">'+p.it.map(x=>'<i style="background:'+slotBg(x)+'"></i>').join('')+'</div>':'<div class="th em2">Kosong</div>';}
function plGallery(){const E='event.stopPropagation();',ib=(f,l,x)=>'<button class="ib" '+(x||'')+' onclick="'+E+f+'">'+l+'</button>';
let h='<div class="gal">';
S.lists.forEach((p,i)=>{const a=S.act.t==='list'&&S.act.i===i,run=a&&S.tp&&S.tp.i===i&&!S.tp.pa;
h+='<div class="tile'+(a?' on':'')+'" data-pl onclick="plOpen('+i+')" role="button" aria-label="Playlist '+p.n+'">'+thumb(p)+(a?'<span class="tbg">● Menyala</span>':'')+'<div class="tn"><b>'+p.n+'</b><span class="m">'+p.it.length+'/12 slot · '+fd(tot(p))+' dtk</span></div><div class="tb">'+ib('plPlay('+i+')',run?'⏸':'▶','style="color:var(--ac)"')+ib('plStop('+i+')','■','')+ib('delPl('+i+')','🗑','style="color:var(--bad)"')+'</div></div>';});
if(S.lists.length<4)h+='<button class="tile add" onclick="newPl()"><b>＋</b><span class="m">Playlist baru</span><span class="m">'+S.lists.length+'/4</span></button>';
return h+'</div>';}
function slotTitle(x){return x.man&&x.L.f!==x.R.f?FX[x.L.f].n+' · '+FX[x.R.f].n:FX[x.man?x.L.f:x.f].n;}
function plFolder(i){const p=S.lists[i],t=S.tp&&S.tp.i===i?S.tp:null,E='event.stopPropagation();',a=S.act.t==='list'&&S.act.i===i,run=a&&t&&!t.pa;
let h='<div class="fh"><button class="ib" onclick="plBack()">‹</button><input class="srch" style="margin:0;flex:1;min-width:0" type="text" maxlength="20" value="'+p.n+'" oninput="S.lists['+i+'].n=this.value"></div><div class="fh2"><span class="m" style="flex:1">'+p.it.length+'/12 slot · '+fd(tot(p))+' dtk</span><button class="ib" style="color:var(--ac)" onclick="plPlay('+i+')">'+(run?'⏸':'▶')+'</button><button class="ib" onclick="plStop('+i+')">■</button></div><div class="sls g2">';
for(let k=0;k<12;k++){const x=p.it[k];if(!x){h+='<button class="sc em" onclick="slotAdd('+i+')">+</button>';continue;}
h+='<div class="sc'+(S.si===k?' on':'')+'" data-sc="'+k+'" onpointerdown="scDown(event,'+i+','+k+')" oncontextmenu="return false" onclick="if(Date.now()-SCB<400)return;slotSel('+i+','+k+')"><span class="sw" style="background:'+slotBg(x)+'"><span class="sn">'+(k+1)+'</span></span><i class="gh">⠿</i><b>'+slotTitle(x)+'</b>'+(x.rv||x.ex?'<span class="tg">'+(x.rv?'⟲ Reverse':'')+(x.rv&&x.ex?' · ':'')+(x.ex?'✦ '+FX[x.ex.f].n:'')+'</span>':'')+'<span class="m">'+fd(x.d)+' dtk</span><button class="se" onclick="'+E+'seOpen('+i+','+k+')">✎</button>'+(t&&t.j===k?'<i class="pgb" id="pgb" style="width:'+(t.el/x.d*100)+'%"></i>':'')+'</div>';}
return h+'</div>'+(p.it.length>1?'<div class="m" style="margin-top:8px">Geser ikon ⠿ untuk ubah urutan.</div>':'');}

const seX=()=>S.lists[S.se.i].it[S.se.j];
function seOpen(i,j){const x0=S.lists[i].it[j];if(!x0.man){x0.L={f:x0.f,c:{...(x0.c||DC())}};x0.R={f:x0.f,c:{...(x0.c||DC())}};x0.man=true;}S.pi=i;S.si=j;S.se={i,j,v:'main',dirty:false};S.tp={i,j,el:0,pa:true};R();seR();}
function seClose(){const d=S.se&&S.se.dirty;S.se=null;S.tp=null;closeSheet();R();if(d)commit('Slot tersimpan');}
function seCfg(k){const x=seX();return k==='X'?x.ex:x.man?x[k]:x;}
function seCol(k){return(k==='X'?(seX().ex||{}).c:seCfg(k).c)||DC();}
function seList(k){const x=seX(),cur=k==='X'?(x.ex?x.ex.f:-1):seCfg(k).f;
return'<div class="fsc" id="fsc_'+k+'">'+(k==='X'?'<div class="fi'+(cur<0?' on':'')+'" onclick="seFx(\'X\',-1)">Tidak ada</div>':'')+favFirst().map(f=>'<div class="fi'+(f===cur?' on':'')+'" onclick="seFx(\''+k+'\','+f+')">'+(S.fav.has(f)?'★ ':'')+FX[f].n+'</div>').join('')+'</div>';}
function seColBtn(k){const dis=k==='X'&&!seX().ex,c=seCol(k);return'<button class="skb" '+(dis?'disabled':'')+' onclick="seColOpen(\''+k+'\')"><i style="background:'+(dis?'var(--s3)':cg(c))+'"></i><span>'+(dis?'Pilih efek dulu':ckn(c))+'</span></button>';}
function seColumn(k,t){return'<div class="sec"><div class="sect">'+t+'</div>'+seList(k)+seColBtn(k)+'</div>';}
const tg=(on,onc,l,dis)=>''+'<button class="tgl'+(on?' on':'')+'" '+(dis?'disabled ':'')+'onclick="'+onc+'">'+l+'</button>';
function seMain(){const x=seX(),e=S.se;
return'<div class="seh"><b>Edit slot '+(e.j+1)+'</b><button class="ib" onclick="seClose()">✕</button></div>'+
'<div class="se3"><div class="seb"><div class="sbt">Main Effect</div><div class="sopt">'+tg(x.rv,'seRv()','⟲ Reverse')+tg(0,'seSwap()','⇄ Tukar')+'</div><div class="cols2">'+seColumn('L','Kiri')+seColumn('R','Kanan')+'</div></div>'+
'<div class="seb"><div class="sbt">Additional</div><div class="sopt v">'+tg(x.ex&&x.ex.mir,'seExOpt(\'mir\')','◧ Mirror',!x.ex)+tg(x.ex&&x.ex.rv,'seExOpt(\'rv\')','⟲ Reverse',!x.ex)+'</div><div class="cols1">'+seColumn('X','Tambahan')+'</div></div></div>'+
'<div class="seb dur"><label for="sedur">Durasi (detik)</label><input id="sedur" type="number" inputmode="decimal" min="0.1" max="999" step="0.5" value="'+fd(x.d)+'" onchange="seDur(this)"></div>'+
'<div class="row cpf"><button class="btn" style="color:var(--bad)" onclick="slotDel('+e.i+','+e.j+')">🗑 Hapus</button><button class="btn p" onclick="seClose()">Selesai</button></div>';}
function seColView(){const e=S.se,c=e.c,nm={L:'Kiri',R:'Kanan',X:'Efek tambahan'}[e.tg];
return'<div class="seh"><button class="ib" onclick="seColBack()">‹</button><b style="flex:1;margin-left:8px">Warna · '+nm+'</b></div><div class="fxc-preview"><div><small>Warna</small><b id="iecn">'+ckn(c)+'</b></div><span class="fxc-chip" id="iech" style="background:'+cg(c)+'"></span></div><div class="bl">'+[['one','Warna Solid'],['skin','Skin Efek']].map(z=>'<button class="bu'+(e.mode===z[0]?' on':'')+'" onclick="seColMode(\''+z[0]+'\')"><i></i>'+z[1]+'</button>').join('')+'</div><div id="ieg">'+(e.mode==='one'?pkHTML(1):seSkins(c))+'</div><div class="row cpf"><button class="btn" onclick="seColBack()">Batal</button><button class="btn p" onclick="seColSet()">Pilih warna</button></div>';}
function seSkins(c){return[...S.cu.keys()].reverse().map(i=>'<div class="pw cu'+(c.k==='c'&&c.i===i?' on':'')+'" style="margin-bottom:7px" onclick="seColPick(\'c\','+i+')"><span class="rd"></span><span class="pn" style="text-align:left">'+S.cu[i].n+'<em class="cb">✦ Custom</em></span><span class="ps" style="background:'+cuG(S.cu[i])+'"></span></div>').join('')+'<div class="pg">'+PAL.map((q,i)=>'<div class="pw'+(c.k==='p'&&c.i===i?' on':'')+'" onclick="seColPick(\'p\','+i+')"><span class="rd"></span><span class="pn">'+q.n+'</span><span class="ps" style="background:'+grad(q.c)+'"></span></div>').join('')+'</div>';}
function seR(){const e=S.se;if(!e)return;const sh=$('#sh'),keep={};sh.querySelectorAll('.fsc').forEach(a=>keep[a.id]=a.scrollTop);const top=sh.scrollTop;
openSheet(e.v==='col'?seColView():seMain());$('#sheet').classList.add('sxe');$('#sheet').classList.toggle('sev',e.v==='col');
if(e.v==='main'){sh.querySelectorAll('.fsc').forEach(a=>{if(keep[a.id]!=null)a.scrollTop=keep[a.id];else{const o=a.querySelector('.fi.on');if(o)a.scrollTop=Math.max(0,o.offsetTop-a.offsetTop-50);}});sh.scrollTop=top;}
else if(e.mode==='one'){const o={h:e.c.h??35,s:e.c.s??100,v:e.c.v??100};e.pk=o;pkBind(o,()=>{e.c={k:'o',h:o.h,s:o.s,v:o.v};const ch=$('#iech'),n=$('#iecn');if(ch)ch.style.background=cg(e.c);if(n)n.textContent=ckn(e.c);});}}
function seTouch(){S.se.dirty=true;R();seR();}
function seFx(k,f){const x=seX();if(k==='X'){x.ex=f<0?null:{f,c:x.ex?x.ex.c:{k:'p',i:2,h:35,s:100},mir:x.ex?x.ex.mir:false,rv:x.ex?x.ex.rv:false};}
else if(!x.man)x.f=f;else{x[k]={f,c:x[k].c};}seTouch();}
function seRv(){const x=seX();x.rv=!x.rv;seTouch();}
function seSwap(){const x=seX();if(!x.man)return;[x.L,x.R]=[x.R,x.L];seTouch();}
function seExOpt(k){const x=seX();if(!x.ex)return;x.ex[k]=!x.ex[k];seTouch();}
function seDur(el){const v=Math.max(.1,Math.min(999,Math.round((+el.value||5)*10)/10));seX().d=v;el.value=fd(v);S.se.dirty=true;R();}
function seColOpen(k){const e=S.se;if(k==='X'&&!seX().ex)return;e.v='col';e.tg=k;e.c={...seCol(k)};e.mode=e.c.k==='o'?'one':'skin';seR();}
function seColBack(){const e=S.se;e.v='main';seR();}
function seColMode(m){const e=S.se;if(m==='one'&&e.c.k!=='o'){const a=rgb2hsv(...sc(e.c)[0]);e.c={k:'o',h:a[0],s:a[1],v:a[2]};}e.mode=m;seR();}
function seColPick(k,i){const e=S.se;e.c={k,i,h:35,s:100};const b=$('#ieg').scrollTop;seR();$('#ieg').scrollTop=b;}
function seColSet(){const e=S.se,x=seX(),k=e.tg,c={...e.c};if(k==='X')x.ex.c=c;else if(x.man)x[k].c=c;else x.c=c;e.v='main';seTouch();}
function slotSel(i,j){S.si=j;if(S.tp&&S.tp.i===i){S.tp.j=j;S.tp.el=0;}else S.tp={i,j,el:0,pa:true};R();}
function usePl(i){S.act={t:'list',i};S.ll=i;S.src='pl';R();commit('Playlist dipakai untuk Riding');}
function useSingle(){S.act={t:'fx',i:S.lf};S.src='fx';R();commit(FX[S.lf].n+' dipakai untuk Riding');}
function modeRow(k){const one=k==='one',a=one===(S.cs==='one'),cs=S.cs==='cus'&&S.cu[S.ed],g=one?css(hsv(S.col.h,S.col.s,S.col.v??100)):cs?cuG(cs):grad(PAL[S.pal].c),n=one?rx(hsv(S.col.h,S.col.s,S.col.v??100)):cs?cs.n:PAL[S.pal].n;
return'<div class="mrow"><button class="rad'+(a?' on':'')+'" onclick="setMode(\''+k+'\')"><i></i></button><div class="mt" onclick="setMode(\''+k+'\')"><b>'+(one?'Warna Solid':'Skin Efek')+'</b><i class="mini" style="width:36px;background:'+g+'"></i><span>'+n+'</span></div></div>';}
function blkSync(){const b=[...document.querySelectorAll('.blk')];if(b.length<2)return;const one=S.cs==='one';b[0].classList.toggle('on',one);b[1].classList.toggle('on',!one);b.forEach((x,i)=>{const r=x.querySelector('.rad');if(r)r.classList.toggle('on',(i===0)===one);});}
function setMode(k){S.sub=k;if(k==='one')S.cs='one';else if(S.cs==='one')S.cs='pat';R();}
function openMode(k){S.sub=k;R();}
function modeSingle(){const i=S.lf,c=fxActiveColor(i),a=S.act.t==='fx',e=S.ie&&S.ie.t==='single'?S.ie:null;
let h='<div class="card'+(e?' ed':'')+'" style="margin-bottom:8px"><div class="ah"><button class="rad'+(a?' on':'')+'" onclick="useSingle()"><i></i></button><div style="flex:1;min-width:0"><b>Single · '+FX[i].n+'</b><div class="m row" style="gap:6px"><span class="mini" style="background:'+cg(c)+'"></span>'+fxColorName(c)+'</div></div>'+(e?'<button class="rp on" onclick="ieEnd()">✓ Selesai</button>':'<button class="rp" onclick="ieStart()">✎ Edit</button>')+'</div>';
if(e)h+='<div class="ab"><div class="er lbl"><span style="flex:1">Efek</span><span style="width:40px;text-align:center">Warna</span></div><div class="er"><button class="fb" onclick="ieTog(\'fx\')"><b>'+FX[i].n+'</b>'+CH+'</button><button class="swb" style="background:'+cg(c)+'" onclick="ieTog(\'col\')"></button></div></div>'+(e.open==='fx'?ieFxPanel(i,e):e.open==='col'?ieColPanel(e):'');
return h+'</div>';}
function confirmDel(t,m,fn){S.ak=fn;$('#pop').innerHTML='<div><div style="font-weight:800;font-size:16px;margin-bottom:6px">'+t+'</div><div class="m" style="margin-bottom:16px;font-size:13px">'+m+'</div><div class="row"><button class="btn" onclick="pkRgbClose()">Batal</button><button class="btn p" style="background:var(--bad);border-color:var(--bad);color:#fff" onclick="pkRgbClose();S.ak()">Hapus</button></div></div>';$('#pop').className='on dlg';S.popKind='other';}
function slotDel(i,j){confirmDel('Hapus efek ini?','“'+FX[S.lists[i].it[j].f].n+'” akan dihapus dari slot '+(j+1)+'.',()=>slotDel0(i,j));}
function delPl(i){confirmDel('Hapus playlist?','“'+S.lists[i].n+'” beserta '+S.lists[i].it.length+' slot akan dihapus.',()=>delPl0(i));}
function cpDel(i){confirmDel('Hapus skin?','Skin “'+S.cu[i].n+'” akan dihapus.',()=>cpDel0(i));}
function slotOpen(i,j,k){const e=S.ie;e.j=j;e.open=null;S.tp={i,j,el:0,pa:true};ieTog(k);}
function slotDur(i,j,el){const v=Math.max(.1,Math.min(999,Math.round((+el.value||.1)*10)/10));S.lists[i].it[j].d=v;el.value=v;commit('Durasi diubah');}
function plTog(i){S.pi=S.pi===i?null:i;S.si=-1;if(S.tp&&S.tp.i!==S.pi)S.tp=null;if(S.ie&&S.ie.t==='pl'&&S.ie.i!==S.pi)S.ie=null;R();}
function plEdit(i){S.pi=i;S.si=-1;S.ie={t:'pl',i,j:-1,open:null,h:0,c:null,mode:'skin'};R();}
function plEnd(){S.ie=null;R();}
function slotEx(i,j){const e=S.ie;e.j=e.j===j?-1:j;e.open=null;if(e.j>=0)S.tp={i,j,el:0,pa:true};R();}
function slotAdd(i){const l=S.lists[i];if(l.it.length>=12)return toast('Playlist penuh (12 slot)');l.it.push({f:0,d:5,c:DC()});S.pi=i;seOpen(i,l.it.length-1);}
function slotEdit(i,j){return seOpen(i,j);}
function slotEdit0(i,j){S.pi=i;S.si=j;S.ie={t:'pl',i,j,open:null,h:0,c:null,mode:'skin',pop:true,sd:'all'};S.tp={i,j,el:0,pa:true};R();slotPopShow();}
function ieBack(){const e=S.ie;if(e)e.open=null;R();if(e&&e.pop&&e.t==='pl')slotPopShow();}
const fd=d=>Number.isInteger(d)?d:(+d).toFixed(1);
function slotPopShow(){const e=S.ie;if(!e||e.t!=='pl')return;const i=e.i,j=e.j,x=S.lists[i]&&S.lists[i].it[j];if(!x)return slotPopClose();S.popKind='slot';const n=S.lists[i].it.length,cf=slotCfg(x,e.sd),sdv=e.sd||'all';
$('#pop').innerHTML='<div><div class="row" style="margin-bottom:12px"><b style="flex:1;font-size:15px">Edit slot '+(j+1)+'</b><button class="ib" onclick="slotPopClose()">✕</button></div><div class="sdseg" style="padding:0 0 10px"><span>LED</span>'+['all','L','R'].map(k=>'<button class="'+(sdv===k?'on':'')+'" onclick="slotSide(\''+k+'\')">'+SDN[k]+(k!=='all'&&x[k]?' ✓':'')+'</button>').join('')+'</div><div class="er lbl"><span style="flex:1">Efek</span><span style="width:40px;text-align:center">Warna</span><span style="width:72px;text-align:center">Durasi (dtk)</span></div><div class="er"><button class="fb" onclick="slotPopTo(\'fx\')"><b>'+FX[cf.f].n+'</b>'+CH+'</button><button class="swb" style="background:'+cg(cf.c||DC())+'" onclick="slotPopTo(\'col\')"></button><input class="dur" style="width:72px" type="number" inputmode="decimal" step="0.1" min="0.1" max="999" value="'+x.d+'" onchange="slotDur('+i+','+j+',this)"></div>'+(sdv!=='all'?'<div class="m" style="margin-top:8px">'+(x[sdv]?'LED '+SDN[sdv].toLowerCase()+' pakai pengaturan sendiri. <button class="lnk" onclick="slotSideReset()">Ikut Semua</button>':'LED '+SDN[sdv].toLowerCase()+' ikut Semua.')+'</div>':'')+'<div class="er" style="margin-top:12px"><span style="flex:1">Efek tambahan</span><select class="dur" style="width:150px;text-align:left" onchange="slotEx(this.value)"><option value="">Tidak ada</option>'+favFirst().map(k=>'<option value="'+k+'"'+(x.ex===k?' selected':'')+'>'+FX[k].n+'</option>').join('')+'</select></div><div class="er" style="margin-top:12px"><button class="btn" style="flex:1;'+(j?'':'opacity:.4')+'" onclick="slotShift(-1)">◀ Majukan</button><button class="btn" style="flex:1;'+(j<n-1?'':'opacity:.4')+'" onclick="slotShift(1)">Mundurkan ▶</button></div><div class="row" style="margin-top:12px"><button class="btn" style="flex:1;color:var(--bad)" onclick="slotDel('+i+','+j+')">🗑 Hapus</button><button class="btn p" style="flex:1" onclick="slotPopClose()">Selesai</button></div></div>';$('#pop').className='on';}
function slotSide(sd){const e=S.ie;if(!e)return;e.sd=sd;slotPopShow();}
function slotSideReset(){const e=S.ie,x=S.lists[e.i].it[e.j];delete x[e.sd];R();slotPopShow();commit('LED '+SDN[e.sd].toLowerCase()+' ikut Semua');}
function slotEx(v){const e=S.ie,x=S.lists[e.i].it[e.j];x.ex=v===''?null:+v;R();slotPopShow();commit(v===''?'Efek tambahan dihapus':'Efek tambahan: '+FX[+v].n);}
function slotPopClose(){S.ie=null;S.popKind=null;$('#pop').className='';R();}
function slotPopTo(k){$('#pop').className='';S.popKind=null;ieTog(k);}
function slotShift(d){const e=S.ie,a=S.lists[e.i].it,j=e.j,k=j+d;if(k<0||k>=a.length)return;[a[j],a[k]]=[a[k],a[j]];e.j=k;S.si=k;S.tp={i:e.i,j:k,el:0,pa:true};R();slotPopShow();commit('Urutan diubah');}
function plView(){const E='event.stopPropagation();',ib=(f,l,x)=>'<button class="ib" '+(x||'')+' onclick="'+E+f+'">'+l+'</button>';
return'<div class="m" style="margin:0 2px 8px">Pilih mode untuk Riding.</div>'+modeSingle()+S.lists.map((p,i)=>{const o=S.pi===i,a=S.act.t==='list'&&S.act.i===i,t=S.tp&&S.tp.i===i?S.tp:null,e=S.ie&&S.ie.t==='pl'&&S.ie.i===i?S.ie:null,
pp=ib('plPlay('+i+')',t&&!t.pa?'⏸':'▶','style="color:var(--ac)"')+ib('plStop()','■','')+ib('delPl('+i+')','🗑','style="color:var(--bad)"'),tot=Math.round(p.it.reduce((z,x)=>z+x.d,0)*10)/10;
let h='<div data-pl class="card'+(o?' open':'')+'" style="margin-bottom:8px"><div class="ah"><button class="rad'+(a?' on':'')+'" onclick="'+E+'usePl('+i+')"><i></i></button><div style="flex:1;min-width:0;cursor:pointer" onclick="plTog('+i+')"><b>'+p.n+'</b><div class="m">'+p.it.length+'/12 slot · '+fd(tot)+' dtk</div></div>'+pp+ib('delPl('+i+')','🗑','style="color:var(--bad)"')+'<button class="ib" onclick="plTog('+i+')">'+(o?'▴':'▾')+'</button></div>';
if(o){h+='<div class="ab"><div class="row" style="margin-bottom:8px"><span class="m">Nama</span><input class="srch" style="margin:0;flex:1;min-width:0" type="text" maxlength="20" value="'+p.n+'" oninput="S.lists['+i+'].n=this.value"></div><div class="sls g4">';
for(let k=0;k<12;k++){const x=p.it[k];if(!x){h+='<button class="sc em" onclick="slotAdd('+i+')">+</button>';continue;}
h+='<div class="sc'+(S.si===k?' on':'')+'" data-sc="'+k+'" onpointerdown="scDown(event,'+i+','+k+')" oncontextmenu="return false" onclick="if(Date.now()-SCB<400)return;slotSel('+i+','+k+')"><span class="sw" style="background:'+slotBg(x)+'"><span class="sn">'+(k+1)+'</span></span><i class="gh">⠿</i><b>'+FX[x.f].n+'</b>'+(x.L||x.R||x.ex!=null?'<span class="tg">'+(x.L?'Ki':'')+(x.L&&x.R?'·':'')+(x.R?'Ka':'')+((x.L||x.R)&&x.ex!=null?' ':'')+(x.ex!=null?'✦':'')+'</span>':'')+'<span class="m">'+fd(x.d)+' dtk</span><button class="se" onclick="'+E+'slotEdit('+i+','+k+')">✎</button>'+(t&&t.j===k?'<i class="pgb" id="pgb" style="width:'+(t.el/x.d*100)+'%"></i>':'')+'</div>';}
h+='</div>'+(p.it.length>1?'<div class="m" style="margin-top:8px">Geser ikon ⠿ untuk mengubah urutan.</div>':'')+(e&&e.j>=0&&e.open==='fx'?ieFxPanel(p.it[e.j].f,e):e&&e.j>=0&&e.open==='col'?ieColPanel(e):'')+'</div>';}
return h+'</div>';}).join('')+(S.lists.length?'':'<div class="emp" style="margin-bottom:10px">Belum ada playlist.</div>')+(S.lists.length<4?'<button class="btn" style="width:100%" onclick="newPl()">+ Playlist baru</button>':'<div class="m">Maksimal 4 playlist.</div>');}
function plPlay(i){const l=S.lists[i];if(!l.it.length)return toast('Playlist kosong');if(S.tp&&S.tp.i===i&&S.act.t==='list'&&S.act.i===i)S.tp.pa=!S.tp.pa;else{S.tp={i,j:0,el:0,pa:false};S.act={t:'list',i};S.ll=i;S.src='pl';commit('Playlist “'+l.n+'” diputar');}R();}
function plStop(i){if(S.act.t==='list'&&S.act.i===i){S.tp=null;S.act={t:'fx',i:S.lf};S.src='fx';commit('Playlist dihentikan');}else if(S.tp&&S.tp.i===i)S.tp=null;R();}
function newPl(){if(S.lists.length>=4)return toast('Maksimal 4 playlist');S.lists.push({n:'Playlist '+(S.lists.length+1),it:[]});S.pi=S.lists.length-1;S.si=-1;R();}
function delPl0(i){const p=S.lists[i],sa=JSON.stringify(S.act),sr=S.src;S.lists.splice(i,1);S.tp=null;S.ll=0;if(S.act.t==='list')S.act=S.lists.length?{t:'list',i:0}:{t:'fx',i:S.lf};S.src=S.act.t==='fx'?'fx':'pl';S.pi=null;S.ie=null;closeSheet();R();commit('Playlist dihapus',()=>{S.lists.splice(i,0,p);S.act=JSON.parse(sa);S.src=sr;R();});}
function pickFx(i,j){ppOpen('slot',i,j);}
const CK={o:'Warna Solid',p:'Skin Efek',c:'Skin Efek'};
const ckn=c=>c.k==='o'?'Warna Solid':'Skin · '+(c.k==='p'?PAL[c.i].n:S.cu[c.i]?S.cu[c.i].n:'');
function ppOpen(ctx,a,b){S.ppo=null;if(ctx==='slot'){const l=S.lists[a];if(b<0&&l.it.length>=10)return toast('Playlist penuh');const x=b>=0?l.it[b]:null;S.pp={ctx,i:a,j:b,f:x?x.f:0,d:x?x.d:20,c:x&&x.c?{...x.c}:{k:'p',i:1,h:35,s:100},t:b<0?'Tambah ke slot '+(l.it.length+1):'Edit slot '+(b+1)};}
else{const fc=S.fc[a];S.pp={ctx,o:a,f:a,c:fc?fromFc(fc):curC(),t:'Ganti efek'};}ppR();}
function ppT(k){S.ppo=S.ppo===k?null:k;S.pp.h=S.pp.f;ppR();}
function ppk(k){const c=S.pp.c;S.pp.c=k==='o'?{k,h:c.h??35,s:c.s??100,v:c.v??100}:(c.k==='p'||c.k==='c')?c:{k:'p',i:1,h:35,s:100};ppR();}
function ppc(k,i){S.pp.c={k,i,h:35,s:100};S.ppo=null;ppR();}
function ppR(){const p=S.pp,o=S.ppo,h=p.h??p.f,c=p.c,fl=(k,l,m)=>'<div class="fld" onclick="ppT(\''+k+'\')"><span class="m">'+l+'</span>'+m+CH+'</div>',
pw=(k,i,g,n,on,cu)=>'<div class="pw'+(on?' on':'')+(cu?' cu':'')+'" onclick="ppc(\''+k+'\','+i+')"><span class="rd"></span><span class="pn">'+n+(cu?'<em class="cb">✦ Custom</em>':'')+'</span><span class="ps" style="background:'+g+'"></span></div>';
let x='<div style="font-weight:800;margin-bottom:8px;flex:none">'+p.t+'</div><div class="pbody">'+fl('fx','Efek','<b>'+FX[p.f].n+'</b>');
if(o==='fx'){const ord=FX.map((f,k)=>k).sort((a,b)=>(S.fav.has(b)-S.fav.has(a))||(a-b));
x+='<div class="grow">'+ord.map(k=>'<div class="fr'+(k===h?' on':'')+(S.fav.has(k)?' fv':'')+'" onclick="S.pp.h='+k+';ppR()"><b>'+(S.fav.has(k)?'★ ':'')+FX[k].n+(k===p.f?' ✓':'')+'</b><button class="rp" onclick="event.stopPropagation();S.pp.f='+k+';S.ppo=null;ppR()">Tetapkan</button></div>').join('')+'</div><div class="pfix">'+(FX[h]&&FX[h].p.length?'<div class="pt">Parameter · '+FX[h].n+'</div><div class="pg2">'+FX[h].p.map((n,k)=>'<label>'+n+'<input type="range" min="0" max="255" value="'+(pr(h)[k]||128)+'" oninput="S.pv['+h+']['+k+']=+this.value"></label>').join('')+'</div>':'<div class="m">Tidak ada parameter.</div>')+'</div>';}
x+=fl('col','Warna','<i class="mini" id="ppsw" style="width:44px;background:'+cg(c)+'"></i><b>'+ckn(c)+'</b>');
if(o==='col'){x+='<div class="bl" style="padding:0">'+[['o','Warna Solid'],['s','Skin Efek']].map(z=>'<button class="bu'+((z[0]==='o')===(c.k==='o')?' on':'')+'" onclick="ppk(\''+z[0]+'\')"><i></i>'+z[1]+'</button>').join('')+'</div><div class="grow">';
if(c.k==='o')x+=pkHTML(1)+'<button class="btn p" style="width:100%;margin-top:8px" onclick="S.ppo=null;ppR()">Tetapkan</button>';
else x+=[...S.cu.keys()].reverse().map(i=>pw('c',i,cuG(S.cu[i]),S.cu[i].n,c.k==='c'&&c.i===i,1)).join('')+'<div class="pg">'+PAL.map((q,i)=>pw('p',i,grad(q.c),q.n,c.k==='p'&&c.i===i)).join('')+'</div>';x+='</div>';}
if(!o)x+='<div class="sp1"></div>';
if(p.ctx==='slot')x+='<div class="row"><span class="m">Durasi</span><select onchange="S.pp.d=+this.value">'+[10,20,30,60].map(d=>'<option'+(d===p.d?' selected':'')+' value="'+d+'">'+d+' dtk</option>').join('')+'</select></div>';
x+='</div><div class="row" style="margin-top:12px;flex:none"><button class="btn" onclick="closeSheet()">Batal</button><button class="btn p" onclick="ppSave()">'+(p.ctx==='slot'?'Simpan':'Terapkan')+'</button></div>';
openSheet(x,1);
if(o==='col'&&c.k==='o')pkBind(c,()=>{const e=$('#ppsw');if(e)e.style.background=cg(c);});}
function ppSave(){const p=S.pp;if(p.ctx==='slot'){const l=S.lists[p.i],x={f:p.f,d:p.d,c:p.c};p.j<0?l.it.push(x):l.it[p.j]=x;}else{S.fc[p.f]=toFc(p.c);S.act={t:'fx',i:p.f};S.lf=p.f;S.src='fx';}closeSheet();R();toast('Tersimpan');}
setInterval(()=>{const p=S.tp;if(!p||p.pa)return;const l=S.lists[p.i],x=l&&l.it[p.j];if(!x){S.tp=null;return R();}p.el+=.1;if(p.el>=x.d){p.el=0;p.j=(p.j+1)%l.it.length;if(!(SCD&&SCD.on))R();}const b=$('#pgb');if(b)b.style.width=(p.el/x.d*100)+'%';},100);
let BT;try{const b=+localStorage.getItem('mzBri');if(b>=1&&b<=255)S.bri=b;}catch(e){}
S.net=true;S.sv='ok';
const colDirty=()=>{const c=S.fc[S.sel];return !c||JSON.stringify([c.cs,c.col,c.pal,c.ed])!==JSON.stringify([S.cs,S.col,S.pal,S.ed]);};
function pvl(){return'';}
function riInfo(){const st=!S.net?['Terputus','bad']:S.sv==='saving'?['Menyimpan…','sv']:S.sv==='fail'?['Gagal','bad']:['✓ Tersimpan','ok'];return'<span style="gap:6px"><em style="font-style:normal;color:var(--ac);font-weight:700">Aktif</em><em class="svs '+st[1]+'">'+st[0]+'</em></span>'+riInfo0();}
function banner(){if(!S.net)return'<div class="ban"><span style="flex:1">Terputus.</span><button class="btn sm" onclick="retry()">Sambung</button></div>';return'';}
function rs(){$('#ri').innerHTML=riInfo();$('#hr').innerHTML=hdr();$('#ban').innerHTML=banner();}
function commit(m,u){if(!S.net){S.sv='fail';rs();return toast('Gagal menyimpan.');}S.sv='saving';rs();toast(m,u);setTimeout(()=>{if(S.sv==='saving'){S.sv='ok';rs();}},700);}
function retry(){S.net=true;S.sv='ok';rs();toast('Tersambung');}
function netDemo(){S.net=!S.net;if(S.net&&S.sv==='fail')S.sv='ok';rs();}
function briOpen(){openSheet('<div style="font-weight:800;margin-bottom:12px">Kecerahan LED</div><div class="row"><span>☀</span><input id="gbi" type="range" min="1" max="255" value="'+S.bri+'" style="flex:1;height:36px;accent-color:var(--ac)"><b id="gbv" style="min-width:46px;text-align:right">'+Math.round(S.bri/2.55)+'%</b></div><button class="btn p" style="width:100%;margin-top:14px" onclick="closeSheet()">Tutup</button>');
$('#gbi').oninput=e=>{S.bri=+e.target.value;$('#gbv').textContent=Math.round(S.bri/2.55)+'%';clearTimeout(BT);BT=setTimeout(()=>{try{localStorage.setItem('mzBri',S.bri);}catch(x){}commit('Kecerahan tersimpan');},500);};}
R();
</script></body></html>
)rawliteral";
"""

with open("mizuma_ui.h", "w", encoding="utf-8") as f:
    f.write(ui_h_content)

print("Successfully generated mizuma_ui.h")
