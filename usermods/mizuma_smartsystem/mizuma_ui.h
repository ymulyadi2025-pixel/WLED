// =====================================================================================================================================
// MIZUMA UI  (mizuma_ui.h)  — HANYA tampilan web: CSS, HTML, JavaScript, dan perute halaman.
// File ini TIDAK menyentuh LED/segmen/show. Semua aksi lampu dilakukan UI lewat API engine (/mizuma/*) dan API bawaan WLED (/json/*).
// Taruh di folder yang sama dengan mizuma_smartsystem.cpp dan mizuma_shows.h.
// Hapus file ini + tambahkan  -D MZ_NO_UI=1  bila ingin build engine saja (tanpa halaman web).
// =====================================================================================================================================
#pragma once
#include "wled.h"
#include <functional>

// Data kendaraan yang ditampilkan di halaman (diisi oleh engine, UI hanya membaca).
struct MizumaVehicleInfo {
  String name;
  String brand;
  String plate;
  uint16_t year = 0;
};


// --------------------------------------------------------------------------------------------------------------------------------------
// Blok 1 — Komponen Bersama (Native-feel FINAL: tanpa getar, transisi 110ms, nav "Dashboard")
// --------------------------------------------------------------------------------------------------------------------------------------
const char MIZUMA_SHARED_CSS[] PROGMEM = R"rawliteral(
:root{
--bg:#0a0a0a; --s1:#151515; --s2:#1c1c1c; --s3:#242424;
--bd:#262626; --bd2:#333;
--tx:#f2f2f2; --tx2:#9a9a9a; --tx3:#666;
--ac:#f5a524; --ac-dim:rgba(245,165,36,.14);
--ok:#3ddc84; --ok-dim:rgba(61,220,132,.14);
--warn:#e8a33d; --warn-dim:rgba(232,163,61,.14);
--bad:#e05555; --bad-dim:rgba(224,85,85,.14);
--r-lg:16px; --r-md:12px; --r-sm:9px;
--nav-h:62px;
--ease:cubic-bezier(.22,1,.36,1);
color-scheme:dark;
}
*{box-sizing:border-box;-webkit-tap-highlight-color:transparent;scrollbar-width:none;}
*::-webkit-scrollbar{display:none;}
html{overscroll-behavior-y:none;}
body{margin:0;font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,Arial,sans-serif;background:var(--bg);color:var(--tx);padding-bottom:calc(var(--nav-h) + env(safe-area-inset-bottom));overscroll-behavior-y:none;-webkit-font-smoothing:antialiased;-webkit-text-size-adjust:100%;}
button{font:inherit;color:inherit;background:none;border:none;cursor:pointer;touch-action:manipulation;user-select:none;-webkit-user-select:none;}
a{touch-action:manipulation;user-select:none;-webkit-user-select:none;-webkit-touch-callout:none;}
svg{display:block;}
:focus-visible{outline:2px solid var(--ac);outline-offset:2px;}
a,button{transition:transform .12s var(--ease),background .2s,border-color .2s,color .2s,opacity .2s;}
a:active,button:active{transform:scale(.96);}
@keyframes mzRise{from{opacity:0;transform:translateY(12px);}to{opacity:1;transform:none;}}
@keyframes mzNav{from{transform:translateY(100%);}to{transform:none;}}
.mzheader{display:flex;align-items:center;justify-content:space-between;padding:12px 16px;padding-top:calc(12px + env(safe-area-inset-top));background:rgba(10,10,10,.85);backdrop-filter:blur(12px);-webkit-backdrop-filter:blur(12px);border-bottom:1px solid var(--bd);position:sticky;top:0;z-index:30;animation:mzRise .22s var(--ease) backwards;}
.container,.body-layout,.freeze{animation:mzRise .26s var(--ease) backwards .02s;}
.bottomnav{animation:mzNav .26s var(--ease) backwards .04s;}
.card,.tile,.stat-box{animation:mzRise .3s var(--ease) backwards;}
.mzheader .brand{display:flex;align-items:center;gap:10px;}
.mzheader .logo{width:34px;height:34px;border-radius:10px;background:linear-gradient(135deg,var(--ac),#c46a10);display:flex;align-items:center;justify-content:center;font-weight:800;color:#0a0a0a;font-size:16px;box-shadow:0 2px 12px rgba(245,165,36,.35);}
.mzheader .brandtext{line-height:1.1;}
.mzheader .b1{font-size:13px;font-weight:800;color:#fff;letter-spacing:.5px;}
.mzheader .b2{font-size:9px;color:var(--ac);letter-spacing:1.5px;}
.mzheader .status{display:flex;align-items:center;gap:8px;}
.mzheader .volt{display:flex;align-items:center;gap:6px;font-size:12px;font-weight:700;color:var(--tx);background:var(--s2);padding:5px 10px;border-radius:20px;border:1px solid var(--bd);}
.mzheader .vdot{width:6px;height:6px;border-radius:50%;background:#555;}
.mzheader .vdot.ok{background:var(--ok);box-shadow:0 0 6px var(--ok);}
.mzheader .conn{display:flex;align-items:center;justify-content:center;padding:4px;text-decoration:none;}
.mzheader .conn svg{width:16px;height:16px;stroke:var(--tx2);fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
.bottomnav{position:fixed;bottom:0;left:0;right:0;display:flex;background:rgba(18,18,18,.92);backdrop-filter:blur(14px);-webkit-backdrop-filter:blur(14px);border-top:1px solid var(--bd);z-index:40;padding-bottom:env(safe-area-inset-bottom);}
.bottomnav a{flex:1;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:3px;padding:7px 2px 8px;text-decoration:none;color:var(--tx3);font-size:10px;font-weight:600;}
.bottomnav a:active{transform:none;}
.bottomnav .bn-pill{display:flex;align-items:center;justify-content:center;padding:3px 16px;border-radius:16px;transition:background .25s var(--ease),transform .12s var(--ease);}
.bottomnav a:active .bn-pill{transform:scale(.88);}
.bottomnav .bn-pill svg{width:19px;height:19px;stroke:currentColor;fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
.bottomnav a.active{color:var(--ac);}
.bottomnav a.active .bn-pill{background:var(--ac-dim);}
.bottomnav a.disabled{opacity:.4;pointer-events:none;}
.card{background:var(--s1);border-radius:var(--r-lg);padding:14px;border:1px solid var(--bd);margin-bottom:12px;}
.card-title{font-size:12px;color:var(--tx2);text-transform:uppercase;letter-spacing:.5px;margin-bottom:10px;font-weight:700;}
.placeholder-text{color:var(--tx3);font-size:12px;text-align:center;padding:10px 0;}
.badge{font-size:10px;padding:4px 9px;border-radius:20px;font-weight:700;}
.badge.gray{background:var(--s3);color:var(--tx2);}
.badge.green{background:var(--ok-dim);color:var(--ok);}
.badge.orange{background:var(--warn-dim);color:var(--warn);}
.badge.red{background:var(--bad-dim);color:var(--bad);}
.chip{padding:8px 14px;border-radius:20px;background:var(--s2);border:1px solid var(--bd);color:var(--tx2);font-size:12px;font-weight:600;display:inline-flex;align-items:center;gap:6px;}
.chip.active{background:var(--ac-dim);border-color:var(--ac);color:var(--ac);}
.progress{height:6px;border-radius:3px;background:var(--s3);overflow:hidden;}
.progress .fill{height:100%;border-radius:3px;background:var(--ok);width:0;transition:width .9s var(--ease);}
.progress .fill.warn{background:var(--warn);}
.progress .fill.bad{background:var(--bad);}
.icobox{width:38px;height:38px;border-radius:12px;display:flex;align-items:center;justify-content:center;flex-shrink:0;}
.icobox svg{width:19px;height:19px;stroke:currentColor;fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
.icobox.ac{background:var(--ac-dim);color:var(--ac);}
.icobox.ok{background:var(--ok-dim);color:var(--ok);}
.icobox.warn{background:var(--warn-dim);color:var(--warn);}
.btn{display:flex;align-items:center;justify-content:center;gap:8px;width:100%;padding:11px;border-radius:var(--r-md);background:var(--s2);border:1px solid var(--bd2);color:var(--tx);font-size:13px;font-weight:700;text-decoration:none;margin-top:10px;}
.btn.primary{background:var(--ac);border-color:var(--ac);color:#0a0a0a;}
.btn.danger{background:#3d1414;border-color:#5a1c1c;color:#ff8080;}
)rawliteral";

const char MIZUMA_HEADER_HTML[] PROGMEM = R"rawliteral(
<div class="mzheader">
<div class="brand">
<div class="logo">M</div>
<div class="brandtext"><div class="b1">MIZUMA</div><div class="b2">PERFORMANCE</div></div>
</div>
<div class="status">
<span class="volt"><span class="vdot" id="hdrVoltDot"></span><span id="hdrVolt">-- V</span></span>
<a class="conn" id="hdrConn" href="/pengaturan" title="Status koneksi — buka Pengaturan"></a>
</div>
</div>
)rawliteral";

const char MIZUMA_HEADER_SCRIPT[] PROGMEM = R"rawliteral(
(function(){
var m=document.querySelector('meta[name="viewport"]');
if(m && m.content.indexOf('viewport-fit')<0) m.content+=', viewport-fit=cover';
if(!document.querySelector('meta[name="theme-color"]')){var t=document.createElement('meta');t.name='theme-color';t.content='#0a0a0a';document.head.appendChild(t);}
})();
var MZ_WIFI='<svg viewBox="0 0 24 24"><path d="M2 9a15 15 0 0 1 20 0"/><path d="M5 12.5a11 11 0 0 1 14 0"/><path d="M8.5 15.8a7 7 0 0 1 7 0"/><circle cx="12" cy="19" r="1.6"/></svg>';
var MZ_GLOBE='<svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="9"/><path d="M3 12h18"/><path d="M12 3c2.6 2.6 4 5.7 4 9s-1.4 6.4-4 9c-2.6-2.6-4-5.7-4-9s1.4-6.4 4-9z"/></svg>';
async function mzRefreshHeader() {
try {
const res = await fetch('/mizuma/status');
const data = await res.json();
document.getElementById('hdrConn').innerHTML = data.sta ? MZ_GLOBE : MZ_WIFI;
if (data.volt) { document.getElementById('hdrVolt').textContent = data.volt + ' V'; document.getElementById('hdrVoltDot').className = 'vdot ok'; }
} catch (e) {}
}
mzRefreshHeader();
setInterval(mzRefreshHeader, 5000);
document.querySelectorAll('.card,.tile,.stat-box').forEach(function(el,i){ el.style.animationDelay = (30 + i*30) + 'ms'; });
setTimeout(function(){ document.querySelectorAll('.progress .fill[data-w]').forEach(function(f){ f.style.width = f.getAttribute('data-w') + '%'; }); }, 200);
function mzFadeTo(url){
if(window.__mzFading) return; window.__mzFading = true;
var f=document.getElementById('mzFade');
if(!f){ f=document.createElement('div'); f.id='mzFade';
f.style.cssText='position:fixed;inset:0;background:#0a0a0a;opacity:0;pointer-events:none;transition:opacity .12s ease;z-index:999;';
document.body.appendChild(f); }
requestAnimationFrame(function(){ f.style.opacity='1'; });
setTimeout(function(){ window.location.href=url; }, 110);
}
document.addEventListener('click', function(e){
var a=e.target.closest('a'); if(!a) return;
var href=a.getAttribute('href');
if(!href || href.charAt(0)==='#' || href.indexOf('http')===0 || a.hasAttribute('data-noanim')) return;
if(e.metaKey||e.ctrlKey) return;
e.preventDefault(); mzFadeTo(href);
});
)rawliteral";

const char MIZUMA_BOTTOMNAV_HTML[] PROGMEM = R"rawliteral(
<div class="bottomnav">
<a href="/app" class="__ACTIVE_BERANDA__"><span class="bn-pill"><svg viewBox="0 0 24 24"><rect x="3" y="3" width="7" height="9" rx="1"/><rect x="14" y="3" width="7" height="5" rx="1"/><rect x="14" y="12" width="7" height="9" rx="1"/><rect x="3" y="16" width="7" height="5" rx="1"/></svg></span>Dashboard</a>
<a href="/riding?t=warna" class=""><span class="bn-pill"><svg viewBox="0 0 24 24"><path d="M12 3s6 6.5 6 11a6 6 0 0 1-12 0c0-4.5 6-11 6-11z"/></svg></span>Warna</a>
<a href="/led?t=welcoming" class=""><span class="bn-pill"><svg viewBox="0 0 24 24"><path d="M12 3l2.4 5.2 5.6.7-4.1 3.9 1 5.6L12 15.6 7.1 18.4l1-5.6L4 8.9l5.6-.7z"/></svg></span>Welcoming</a>
<a href="/riding" class=""><span class="bn-pill"><svg viewBox="0 0 24 24"><path d="M9 18h6M10 21h4M12 3a6 6 0 0 0-3.6 10.8c.7.6 1.1 1.4 1.3 2.2h4.6c.2-.8.6-1.6 1.3-2.2A6 6 0 0 0 12 3z"/></svg></span>Riding</a>
<a href="/led?t=sein" class=""><span class="bn-pill"><svg viewBox="0 0 24 24"><path d="M3 12h13M11 6l6 6-6 6M20 6v12"/></svg></span>Sein-Rem</a>
<a href="/pengaturan" class="__ACTIVE_PENGATURAN__"><span class="bn-pill"><svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="3"/><path d="M12 2v3M12 19v3M2 12h3M19 12h3M5 5l2 2M17 17l2 2M19 5l-2 2M7 17l-2 2"/></svg></span>Pengaturan</a>
</div>
)rawliteral";

// --------------------------------------------------------------------------------------------------------------------------------------
// Blok 2 — Dashboard
// --------------------------------------------------------------------------------------------------------------------------------------
const char MIZUMA_HOME_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Dashboard - Mizuma</title>
<link rel="stylesheet" href="/mizuma.css">
<style>
.container{padding:16px;max-width:520px;margin:0 auto;}
.greet-card{display:flex;justify-content:space-between;align-items:center;}
.greet-k{font-size:10px;font-weight:800;letter-spacing:1.6px;color:var(--ac);text-transform:uppercase;}
.greet-name{font-size:22px;font-weight:800;color:#fff;margin-top:2px;letter-spacing:.2px;}
.greet-sub{font-size:12px;color:var(--tx2);margin-top:3px;}
.greet-ico{width:44px;height:44px;border-radius:14px;background:var(--ac-dim);color:var(--ac);display:flex;align-items:center;justify-content:center;flex-shrink:0;}
.greet-ico svg{width:22px;height:22px;stroke:currentColor;fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
.stat-row{display:grid;grid-template-columns:1fr 1fr 1fr;gap:8px;margin-bottom:12px;}
.stat-box{background:var(--s1);border:1px solid var(--bd);border-radius:12px;padding:10px 6px;display:flex;flex-direction:column;align-items:center;gap:4px;}
.stat-box .si{width:30px;height:30px;border-radius:9px;display:flex;align-items:center;justify-content:center;}
.stat-box .si svg{width:15px;height:15px;stroke:currentColor;fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
.si.ac{background:var(--ac-dim);color:var(--ac);} .si.ok{background:var(--ok-dim);color:var(--ok);} .si.warn{background:var(--warn-dim);color:var(--warn);}
.stat-box .sv{font-size:13px;font-weight:800;color:var(--tx);}
.stat-box .sl{font-size:9px;color:var(--tx3);text-transform:uppercase;letter-spacing:.5px;}
.volt-row{display:flex;align-items:center;gap:12px;}
.volt-mid{flex:1;}
.volt-big{font-size:26px;font-weight:800;color:#fff;}
.volt-unit{font-size:13px;color:var(--tx2);}
.quick-tiles{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-bottom:12px;}
.tile{display:flex;align-items:center;gap:10px;background:var(--s1);border:1px solid var(--bd);border-radius:12px;padding:12px;text-decoration:none;}
.tile .ti{width:36px;height:36px;border-radius:11px;display:flex;align-items:center;justify-content:center;flex-shrink:0;}
.tile .ti svg{width:18px;height:18px;stroke:currentColor;fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
.tile .tl{font-size:12.5px;font-weight:700;color:var(--tx);}
.tile .ts{font-size:10px;color:var(--tx3);margin-top:1px;}
</style>
</head>
<body>
%HEADER%
<div class="container">
<div class="card greet-card">
<div>
<div class="greet-k" id="greetText">SELAMAT PAGI</div>
<div class="greet-name">%VEHICLE_NAME%</div>
<div class="greet-sub">%VEHICLE_BRAND% &bull; %VEHICLE_YEAR% &bull; %VEHICLE_PLATE%</div>
</div>
<div class="greet-ico"><svg viewBox="0 0 24 24"><path d="M3 12h4l3 8 4-16 3 8h4"/></svg></div>
</div>
<div class="stat-row">
<div class="stat-box"><div class="si ac"><svg viewBox="0 0 24 24"><path d="M4 14a8 8 0 0 1 16 0"/><path d="M12 14l4-4"/></svg></div><div class="sv">--</div><div class="sl">Odometer</div></div>
<div class="stat-box"><div class="si ok"><svg viewBox="0 0 24 24"><path d="M12 22s8-3.5 8-10V5l-8-3-8 3v7c0 6.5 8 10 8 10z"/></svg></div><div class="sv">--</div><div class="sl">Alarm</div></div>
<div class="stat-box"><div class="si warn"><svg viewBox="0 0 24 24"><path d="M9 18h6"/><path d="M10 21h4"/><path d="M12 3a6 6 0 0 0-3.6 10.8c.7.6 1.1 1.4 1.3 2.2h4.6c.2-.8.6-1.6 1.3-2.2A6 6 0 0 0 12 3z"/></svg></div><div class="sv">--</div><div class="sl">Lampu</div></div>
</div>
<div class="card">
<div class="card-title">Tegangan Aki</div>
<div class="volt-row">
<div class="icobox warn"><svg viewBox="0 0 24 24"><path d="M13 2L4 14h6l-1 8 9-12h-6l1-8z"/></svg></div>
<div class="volt-mid">
<span class="volt-big">--</span><span class="volt-unit"> V</span>
<div class="progress" style="margin-top:6px;"><div class="fill warn" data-w="0"></div></div>
</div>
<span class="badge gray">Sensor belum terpasang</span>
</div>
</div>
<div class="quick-tiles">
<a class="tile" href="/riding"><div class="ti ac"><svg viewBox="0 0 24 24"><path d="M9 18h6"/><path d="M10 21h4"/><path d="M12 3a6 6 0 0 0-3.6 10.8c.7.6 1.1 1.4 1.3 2.2h4.6c.2-.8.6-1.6 1.3-2.2A6 6 0 0 0 12 3z"/></svg></div><div><div class="tl">Kontrol Lampu</div><div class="ts">5 mode &bull; 2 sisi</div></div></a>
</div>
<div class="card">
<div class="card-title">Riwayat Kejadian</div>
<div class="placeholder-text">Belum ada riwayat</div>
</div>
</div>
%BOTTOMNAV%
<script src="/mizuma.js"></script>
<script>
const h = new Date().getHours();
let g = 'SELAMAT MALAM';
if (h >= 4 && h < 11) g = 'SELAMAT PAGI';
else if (h >= 11 && h < 15) g = 'SELAMAT SIANG';
else if (h >= 15 && h < 18) g = 'SELAMAT SORE';
document.getElementById('greetText').textContent = g;
</script>
</body>
</html>
)rawliteral";

// --------------------------------------------------------------------------------------------------------------------------------------
// Blok 3 — Pengaturan v2 (sub-tab Koneksi/Jaringan + panel Ubah Mode Koneksi + daftar Fitur)
// --------------------------------------------------------------------------------------------------------------------------------------
const char MIZUMA_SETTINGS_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Pengaturan - Mizuma</title>
<link rel="stylesheet" href="/mizuma.css">
<style>
.container { padding:16px; max-width:520px; margin:0 auto; }
.info-row { display:flex; justify-content:space-between; align-items:center; gap:10px; padding:8px 0; font-size:13px; border-bottom:1px solid var(--bd); }
.info-row:last-child { border-bottom:none; }
.info-row .k { color:var(--tx2); }
.info-row .v { color:var(--tx); font-weight:600; text-align:right; }
.chip-row { display:flex; gap:8px; margin-bottom:12px; }
.switch-panel { margin-top:12px; padding-top:12px; border-top:1px solid var(--bd); animation:mzRise .25s var(--ease) backwards; }
.target-title { font-size:13px; font-weight:700; color:var(--tx); margin-bottom:8px; }
.guide-box { font-size:12px; color:var(--tx2); background:var(--s2); padding:10px 12px; border-radius:var(--r-sm); margin-bottom:12px; line-height:1.55; border-left:3px solid var(--ac); }
.guide-box b { color:var(--tx); }
.status-row { display:flex; align-items:center; gap:8px; font-size:12px; margin-bottom:4px; color:var(--tx2); }
.dot { width:9px; height:9px; border-radius:50%; background:#555; display:inline-block; flex-shrink:0; transition:background .3s, box-shadow .3s; }
.dot.on { background:var(--ok); box-shadow:0 0 8px var(--ok); }
.danger { border-color:#4a1f1f; background:#1a1010; }
.danger .card-title { color:var(--bad); }
.feat-row{display:flex;align-items:center;gap:10px;padding:8px 0;border-bottom:1px solid var(--bd);font-size:13px;}
.feat-row:last-child{border-bottom:none;}
.feat-row .fi{width:26px;height:26px;border-radius:8px;display:flex;align-items:center;justify-content:center;flex-shrink:0;}
.fi.on{background:var(--ok-dim);color:var(--ok);} .fi.off{background:var(--s3);color:var(--tx3);}
.fi svg{width:13px;height:13px;stroke:currentColor;fill:none;stroke-width:2.5;stroke-linecap:round;stroke-linejoin:round;}
.feat-row .fn{flex:1;color:var(--tx);}
.feat-row .fs{font-size:10px;font-weight:800;}
.fs.on{color:var(--ok);} .fs.off{color:var(--tx3);}
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
<div class="card-title">Koneksi &amp; Jaringan</div>
<div class="chip-row">
<button class="chip active" data-pane="koneksi">Koneksi</button>
<button class="chip" data-pane="jaringan">Jaringan</button>
</div>
<div id="paneKoneksi">
<div class="info-row"><span class="k">Jenis Mode</span><span class="v" id="connMode">Memuat...</span></div>
<div class="info-row"><span class="k">Nama WiFi</span><span class="v" id="connSSID">--</span></div>
<button class="btn" id="btnModePanel">Ubah Mode Koneksi</button>
<div class="switch-panel" id="switchPanel" style="display:none;">
<div class="target-title" id="targetTitle">Beralih Mode</div>
<div class="guide-box" id="guideBox">Memuat panduan...</div>
<div class="status-row"><span class="dot" id="statusDot"></span><span id="statusText">Memeriksa koneksi...</span></div>
<a class="btn primary" id="btnSwitchAction" style="display:none;" href="#">Beralih Mode Sekarang</a>
</div>
</div>
<div id="paneJaringan" style="display:none;">
<div class="info-row"><span class="k">Jaringan Aktif</span><span class="v" id="netSSID">--</span></div>
<div class="info-row"><span class="k">Alamat IP</span><span class="v" id="netIP">--</span></div>
<a class="btn" href="/settings/wifi">Buka Pengaturan WiFi &amp; Network</a>
</div>
</div>
<div class="card">
<div class="card-title">Perangkat &amp; Firmware</div>
<div class="info-row"><span class="k">Firmware</span><span class="v">MIZUMA WLED 16.0.1</span></div>
<div class="card-title" style="margin-top:12px;">Fitur</div>
<div class="feat-row"><span class="fi on"><svg viewBox="0 0 24 24"><path d="M20 6L9 17l-5-5"/></svg></span><span class="fn">LED Alis Pintar</span><span class="fs on">Aktif</span></div>
<div class="feat-row"><span class="fi off"><svg viewBox="0 0 24 24"><path d="M5 12h14"/></svg></span><span class="fn">Kesehatan Aki (Voltase)</span><span class="fs off">Fase 11</span></div>
<div class="feat-row"><span class="fi off"><svg viewBox="0 0 24 24"><path d="M5 12h14"/></svg></span><span class="fn">GPS Tracker</span><span class="fs off">Fase 13</span></div>
</div>
<div class="card danger">
<div class="card-title">Reset Pabrik</div>
<div class="placeholder-text" style="color:#c99;">Menghapus semua konfigurasi dan mengembalikan ke pengaturan awal</div>
<a class="btn danger" href="/settings/sec">Buka Halaman Reset</a>
</div>
</div>
%BOTTOMNAV%
<script src="/mizuma.js"></script>
<script>
var isAPMode = (window.location.hostname === '4.3.2.1');
document.querySelectorAll('.chip-row .chip').forEach(function(ch){
ch.addEventListener('click', function(){
document.querySelectorAll('.chip-row .chip').forEach(function(x){ x.classList.remove('active'); });
ch.classList.add('active');
var k = ch.getAttribute('data-pane');
document.getElementById('paneKoneksi').style.display  = (k === 'koneksi')  ? 'block' : 'none';
document.getElementById('paneJaringan').style.display = (k === 'jaringan') ? 'block' : 'none';
});
});
document.getElementById('btnModePanel').addEventListener('click', function(){
var p = document.getElementById('switchPanel');
p.style.display = (p.style.display === 'none') ? 'block' : 'none';
});
if (isAPMode) {
document.getElementById('targetTitle').textContent = 'Pilihan: Mode Kontrol + Internet (Hotspot)';
document.getElementById('guideBox').innerHTML = '<b>Panduan:</b><br>1. Aktifkan Hotspot / Tethering di HP Anda.<br>2. Tunggu hingga sistem terhubung ke Hotspot HP.<br>3. Tombol beralih muncul otomatis saat terhubung.';
} else {
document.getElementById('targetTitle').textContent = 'Pilihan: Mode Kontrol Only (Offline)';
document.getElementById('guideBox').innerHTML = '<b>Panduan:</b><br>1. Aktifkan Wi-Fi di HP Anda.<br>2. Sambungkan ke Wi-Fi <b>Mizuma Smart System</b>.<br>3. Tombol beralih muncul otomatis saat terhubung.';
}
async function refreshConn() {
try {
const res = await fetch('/mizuma/status');
const data = await res.json();
document.getElementById('connMode').textContent = data.sta ? 'Mode Internet (Hotspot HP)' : 'Mode Kontrol (WiFi Mizuma)';
document.getElementById('connSSID').textContent = data.ssid || '--';
document.getElementById('netSSID').textContent  = data.ssid || '--';
document.getElementById('netIP').textContent    = data.sta ? data.staIP : '4.3.2.1';
var dot = document.getElementById('statusDot');
var st  = document.getElementById('statusText');
var btn = document.getElementById('btnSwitchAction');
if (isAPMode) {
if (data.sta && data.staIP) {
dot.className = 'dot on'; st.textContent = 'Sistem terhubung ke Hotspot HP!'; st.style.color = 'var(--ok)';
btn.href = 'http://' + data.staIP + '/app'; btn.textContent = 'Beralih ke Mode Kontrol + Internet'; btn.style.display = 'flex';
} else {
dot.className = 'dot'; st.textContent = 'Menunggu ESP32 terhubung ke Hotspot HP...'; st.style.color = 'var(--tx2)'; btn.style.display = 'none';
}
} else {
if (data.ap) {
dot.className = 'dot on'; st.textContent = 'HP Anda terhubung ke Wi-Fi Mizuma!'; st.style.color = 'var(--ok)';
btn.href = 'http://4.3.2.1/app'; btn.textContent = 'Beralih ke Mode Kontrol Only'; btn.style.display = 'flex';
} else {
dot.className = 'dot'; st.textContent = 'Menunggu HP terhubung ke Wi-Fi Mizuma...'; st.style.color = 'var(--tx2)'; btn.style.display = 'none';
}
}
} catch (e) {}
}
refreshConn();
setInterval(refreshConn, 3000);
</script>
</body>
</html>
)rawliteral";

// =====================================================================================================================================
// =====================================================================================================================================
// BLOK 5 — LED (REVISI: fokus sisi, side-info L/R, slider vertikal V/K, wheel besar, quick color ringkas, pairing efek→palette)
// =====================================================================================================================================
const char MIZUMA_RIDING_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
<meta name="theme-color" content="#0a0a0a">
<title>Riding - Mizuma</title>
<style>
:root{--bg:#0a0a0a;--s1:#151515;--s2:#1c1c1c;--s3:#242424;--bd:#262626;--bd2:#333;--tx:#f2f2f2;--tx2:#9a9a9a;--tx3:#666;--ac:#f5a524;--acd:rgba(245,165,36,.14);--ok:#3ddc84;--bad:#e05555;--nav:62px;color-scheme:dark;box-sizing:border-box;padding-top:env(safe-area-inset-top,0px)}
*{box-sizing:border-box;-webkit-tap-highlight-color:transparent;scrollbar-width:none}*::-webkit-scrollbar{display:none}
html,body{height:100%;margin:0;overflow:hidden}
body{display:flex;flex-direction:column;background:var(--bg);color:var(--tx);font:14px/1.4 -apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Arial,sans-serif;padding-bottom:calc(var(--nav) + env(safe-area-inset-bottom,0px))}
button{font:inherit;color:inherit;background:none;border:0;cursor:pointer}
.hd{display:flex;align-items:center;justify-content:space-between;padding:10px 16px;background:rgba(10,10,10,.9);border-bottom:1px solid var(--bd);flex:none}
.br{display:flex;align-items:center;gap:10px}.logo{width:34px;height:34px;border-radius:10px;background:linear-gradient(135deg,var(--ac),#c46a10);display:flex;align-items:center;justify-content:center;font-weight:800;color:#0a0a0a}
.b1{font-weight:800;letter-spacing:2px;font-size:14px}.b2{font-size:9px;color:var(--ac);letter-spacing:1.5px}
.volt{display:flex;align-items:center;gap:6px;font-size:12px;font-weight:700;background:var(--s2);padding:5px 10px;border-radius:20px;border:1px solid var(--bd)}.vd{width:20px;height:20px;border-radius:50%;background:var(--ok);display:flex;align-items:center;justify-content:center;font-size:12px;font-weight:900;color:#111}
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
.pg{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:7px}.pw{position:relative;display:flex;align-items:center;gap:6px;background:var(--s2);border:1px solid var(--bd);border-radius:11px;padding:8px 6px 12px;overflow:hidden;min-width:0}.pw.on{border-color:var(--ac)}
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
.b2{display:none}.b1{letter-spacing:1px}.hd{gap:8px}.ri{flex:1;min-width:0;border-left:1px solid var(--bd2);padding-left:10px;font-size:10px;line-height:1.3}.ri b{display:block;font-size:12px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}.ri span{display:flex;align-items:center;gap:5px;color:var(--tx2);white-space:nowrap;overflow:hidden}
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
#sh.fix .pbody{overflow-y:auto;-webkit-overflow-scrolling:touch;overscroll-behavior:contain}
#sh.fix .pbody .grow{flex:none;min-height:auto;overflow:visible}

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
.color-actions{display:grid;grid-template-columns:1fr 1fr;gap:8px;width:100%}
.color-actions .btn{min-width:0}
.pg .pw .ib{width:28px;height:28px;padding:0;font-size:12px}
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
.pkx:not(.ed){margin-bottom:18px}.pkx .wh{width:min(230px,52vw,25vh);height:auto;margin:0 auto 6px;display:block}.pkx .pk{gap:4px}
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
.sbn canvas{width:100%;height:16px;image-rendering:pixelated;background:#000;border-radius:4px;display:block}
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
.sls.g2 .sc{aspect-ratio:1;min-height:0;padding:8px}.sls.g2 .sc .sw{height:auto;flex:1;min-height:34px}.sls.g2 .sc b{font-size:13px}.sls.g2 .sc .m{font-size:12px}.sls.g2 .sc .tg{font-size:10px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}.sls.g2 .sc.em{min-height:0}
.sls.g2 .sc .gh{width:36px;height:34px;top:6px;right:8px}
.sls.g2 .sc .se{width:32px;height:32px}
#sheet.sxe{background:rgba(0,0,0,.7)}#sheet.sxe #sh{max-width:none;width:100%;height:calc(100dvh - 14px);max-height:none;margin:auto auto 0;border-radius:16px 16px 0 0;display:flex;flex-direction:column;overflow:hidden;padding:12px;overscroll-behavior:contain}
#sheet.sxe.sev #sh{display:block;overflow:auto}
.sepv{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-bottom:8px;flex:none}.sepv canvas{width:100%;height:14px;image-rendering:pixelated;background:#000;border-radius:5px;display:block}
.se3{flex:1;min-height:0;display:grid;grid-template-columns:1fr;gap:8px;margin-bottom:8px}
.se3 .seb{display:flex;flex-direction:column;min-height:0;margin:0;padding:8px}
.se3 .cols2{flex:1;min-height:0;display:grid;grid-template-columns:minmax(0,1fr) minmax(0,1fr);gap:8px}.se3 .cols1{flex:1;min-height:0;display:flex}
.se3 .sec{display:flex;flex-direction:column;min-height:0;flex:1;min-width:0}.se3 .fsc{flex:1;height:auto;min-height:0}
.se3 .sopt{flex:none;min-height:0;align-items:flex-start;margin-bottom:6px}.se3 .sopt.v{flex-direction:column;align-items:stretch}.se3 .sopt .tgl{flex:1;padding:0 4px;font-size:11px;white-space:nowrap}.se3 .sopt.v .tgl{flex:none}
.se3 .fi{font-size:12px;padding:0 6px;height:32px;line-height:32px}
.se3 .skb{flex-direction:column;align-items:stretch;gap:3px;min-height:44px;padding:4px;margin-top:6px;flex:none}.se3 .skb i{width:100%;height:14px}.se3 .skb span{font-size:11px;text-align:center}
.se3 .sect{font-size:11px;display:flex;align-items:center;justify-content:space-between;gap:4px}
.se3 .sect .tgl{min-height:30px;padding:0 6px;font-size:10px}
.se-actions{display:grid;grid-template-columns:minmax(0,1fr) auto minmax(0,1fr);gap:8px;align-items:center;margin-bottom:8px}.se-actions .tgl{min-width:0;padding:0 6px;font-size:11px;white-space:nowrap}.se-swap{width:64px;min-height:36px;border-radius:10px;border:1px solid var(--bd2);background:var(--s3);color:var(--ac);font-weight:800}.se-swap.on{background:var(--ac);border-color:var(--ac);color:#000}.se-swap:active{background:var(--acd)}
.separams{flex:none;max-height:24vh;overflow-y:auto;overscroll-behavior:contain;margin:6px 0 0;padding:6px;background:var(--s1);border:1px solid var(--bd);border-radius:9px}.separam{margin-bottom:6px}.separam:last-child{margin-bottom:0}.separam label{display:flex;align-items:center;justify-content:space-between;gap:4px;font-size:10px;color:var(--tx2)}.separam input{width:100%;height:24px;margin:0}.separam .sepv{min-width:22px;text-align:right;color:var(--tx)}
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
/* v17 fixes */
.sdseg{display:flex;gap:6px;align-items:center;padding:0 12px 8px}.sdseg span{font-size:12px;font-weight:700;color:var(--tx2);margin-right:2px}
.sdseg button{flex:1;min-height:38px;border-radius:10px;border:1px solid var(--bd2);background:var(--s2);color:var(--tx);font-weight:600;font-size:13px}
.sdseg button.on{background:var(--ac);border-color:var(--ac);color:#000}
.lnk{background:none;border:0;color:var(--ac);font-weight:700;padding:0 2px;text-decoration:underline}
.sc .gh{position:absolute;top:5px;right:5px;width:32px;height:28px;display:flex;align-items:center;justify-content:center;font-style:normal;font-size:16px;color:#fff;background:rgba(0,0,0,.5);border-radius:0 6px 6px 0;touch-action:none;cursor:grab}
.foot .row .fbtn,.foot .fbtn{flex:1 1 0;width:auto;min-width:0}
.blk,.blk.on{border:0}
.sc .sw{display:block;width:100%;height:28px}
.sc{padding-bottom:5px;min-height:76px;touch-action:pan-y;user-select:none;-webkit-user-select:none;-webkit-touch-callout:none}
.fxlist{padding-right:22px}
.sc .se{position:absolute;right:3px;bottom:3px;top:auto;width:26px;height:26px;min-width:0;min-height:0;padding:0;font-size:11px;background:var(--s3);border:1px solid var(--bd2)}
.sc .m{padding:0;white-space:nowrap;font-size:11px;align-self:flex-start;margin-top:4px}
.sc.drg{z-index:20;transition:none;box-shadow:0 10px 24px rgba(0,0,0,.65);border-color:var(--ac);overflow:visible}
.sc.tgt{outline:2px dashed var(--ac);outline-offset:-2px}
.cpf{position:sticky;bottom:-16px;z-index:5;background:var(--s1);margin:12px -16px -16px;padding:12px 16px calc(16px + env(safe-area-inset-bottom,0px));border-top:1px solid var(--bd)}.row.cpf .btn{flex:1}

.sbn small{font-size:10px;color:var(--tx3);white-space:nowrap;overflow:hidden;text-overflow:ellipsis;line-height:1.1}
.sbn i.sbs{display:block;height:16px;border-radius:4px;background:#000}
.sbk .sbn span{line-height:16px}
.fxc-prev{display:flex;align-items:center;gap:10px;padding:9px 10px;background:var(--s2);border:1px solid var(--bd);border-radius:11px;margin-bottom:8px}
.fxc-prev>div{flex:1;min-width:0}.fxc-prev small{display:block;color:var(--tx2);font-size:10px}.fxc-prev b{display:block;font-size:13px}
.fxc-chip{width:66px;height:32px;flex:none;border:1px solid rgba(255,255,255,.25);border-radius:8px}
.ri i{display:inline-block}
.fr b .cb{font-style:normal;font-size:10px;color:var(--ac);margin-left:6px}

</style></head><body>
<div class="hd"><div class="br"><div class="logo">M</div><div><div class="b1">MIZUMA</div><div class="b2">PERFORMANCE</div></div></div><div class="ri" id="ri"></div><div id="hr"></div></div><div id="ban"></div>
<div class="frz" id="frz"></div><div class="scr" id="scr"></div><div class="foot" id="foot"></div>
<nav id="nav"></nav><div id="sheet" onclick="if(event.target===this)closeSheet()"><div id="sh"></div></div><div id="toast"></div><div id="pop" onclick="if(event.target===this)popOut()"></div>
<script>
/* ===================================================================================================
   MIZUMA — Riding (UI v19 + integrasi engine). Tanpa Peek: tidak ada WebSocket, tidak ada /json/live.
   Efek: WLED (/json/eff) + 55 show Mizuma (/mizuma/shows). Skin Custom = palette kustom WLED (/upload palette{n}.json).
   Playlist: disimpan & dijalankan di engine (/mizuma/pl/*).
   =================================================================================================== */
const $=s=>(s.startsWith('#pk')&&document.querySelector('#sheet.on '+s))||document.querySelector(s);
const esc=s=>String(s).replace(/[&<>"]/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;'}[c]));
const hex=h=>[1,3,5].map(i=>parseInt(h.substr(i,2),16)),css=r=>'rgb('+r+')';
const fd=d=>Number.isInteger(d)?d:(+d).toFixed(1);
const rx=r=>'#'+r.map(v=>Math.round(v).toString(16).padStart(2,'0')).join(''),QC=['#ff0000','#00ff00','#0000ff','#ffffff','#ffff00','#ff00ff'];
let QS=['#00ff00',null,null,null,null,null],legacyQS=false,solidSlotsEdited=false,solidSlotsRevision=0,solidSlotsPollAt=0,solidColorsWrite=Promise.resolve();try{const q=JSON.parse(localStorage.getItem('mzQs2')||'null');if(Array.isArray(q)&&q.length===6){QS=q.map(function(c){return typeof c==='string'&&/^#[0-9a-f]{6}$/i.test(c)?c.toUpperCase():null;});legacyQS=true;}}catch(e){}
const qsSave=()=>{try{localStorage.setItem('mzQs2',JSON.stringify(QS));}catch(e){}};
function saveSolidColorSlots(slots){const body=JSON.stringify({slots:slots}),task=solidColorsWrite.then(function(){return fetch('/mizuma/colors',{method:'POST',headers:{'Content-Type':'application/json'},body:body});}).then(function(r){if(!r.ok)throw new Error('color slots');return r.json();}).then(function(d){if(!d.ok)throw new Error('color slots');});solidColorsWrite=task.catch(function(){});return task;}
function loadSolidColorSlots(){const revision=solidSlotsRevision;solidSlotsPollAt=Date.now();return fetch('/mizuma/colors').then(function(r){if(!r.ok)throw new Error('color slots');return r.json();}).then(function(d){
  if(revision!==solidSlotsRevision)return;
  if(d.initialized&&Array.isArray(d.slots)&&d.slots.length===6){if(!solidSlotsEdited){QS=d.slots.map(function(c){return typeof c==='string'&&/^#[0-9a-f]{6}$/i.test(c)?c.toUpperCase():null;});legacyQS=false;qsSave();}return;}
  if(legacyQS&&!solidSlotsEdited)return saveSolidColorSlots(QS).then(function(){legacyQS=false;});
}).catch(function(){});}
function refreshSolidColorSlots(){if(document.visibilityState!=='visible'||Date.now()-solidSlotsPollAt<1500)return;loadSolidColorSlots().then(pkSlots);}
window.addEventListener('focus',refreshSolidColorSlots);
document.addEventListener('visibilitychange',refreshSolidColorSlots);
function pkRand(){pkSet(Math.random()*360,60+Math.random()*40,70+Math.random()*30);}
function hsv(h,s,v){s/=100;v/=100;const f=n=>{const k=(n+h/60)%6;return v-v*s*Math.max(0,Math.min(k,4-k,1))};return[f(5),f(3),f(1)].map(x=>Math.round(x*255));}
function rgb2hsv(r,g,b){r/=255;g/=255;b/=255;const m=Math.max(r,g,b),d=m-Math.min(r,g,b);let h=0;if(d){h=m===r?((g-b)/d)%6:m===g?(b-r)/d+2:(r-g)/d+4;h=(h*60+360)%360;}return[h,m?d/m*100:0,m*100];}
function k2rgb(k){const t=k/100,c=x=>Math.max(0,Math.min(255,Math.round(x)));return[c(t<=66?255:329.7*Math.pow(t-60,-.1332)),c(t<=66?99.47*Math.log(t)-161.12:288.12*Math.pow(t-60,-.0755)),c(t>=66?255:t<=19?0:138.52*Math.log(t-10)-305.04)];}
let PK,WI;
function pkHTML(c){return'<div class="pkx" id="pkx"><div class="wbox"><canvas id="pkw" class="wh" width="360" height="360"></canvas></div><div class="ctl"><button class="rgbx" id="pkrgbb" aria-label="Atur RGB manual" onclick="pkRgbOpen()">'+[['r','#e5332b'],['g','#1fa84a'],['b','#2f6bff']].map(k=>'<span><b style="background:'+k[1]+'">'+k[0].toUpperCase()+'</b><em id="pk'+k[0]+'v"></em></span>').join('')+'</button><div class="pk"><div style="font-size:13px;font-weight:700">Kecerahan: <span id="pkvl"></span></div><input id="pkv" type="range" min="0" max="100" aria-label="Kecerahan warna"></div><button class="ib rnd" id="pkrand" aria-label="Warna acak">🎲</button></div><div class="smp" id="pks"></div><div class="slt" id="pkq"></div><div class="cf"><button class="btn" onclick="pkCancel()">Batal</button><button class="btn p" id="pkcl" onclick="pkConfirm()">Simpan ke slot</button></div></div>';}
function pkRgbOpen(){const L={r:'#e5332b',g:'#1fa84a',b:'#2f6bff'};$('#pop').innerHTML='<div><div style="font-weight:800;margin-bottom:10px">Warna manual</div><div class="row" style="margin-bottom:10px"><i id="pkpv" style="width:56px;height:40px;border-radius:9px;border:1px solid rgba(255,255,255,.3)"></i><span class="m">Hex</span><input id="pkh" class="srch" maxlength="7" autocapitalize="off" spellcheck="false" style="margin:0;flex:1" placeholder="#rrggbb"></div><div class="pk">'+['r','g','b'].map(k=>'<label class="rg"><b style="background:'+L[k]+'">'+k.toUpperCase()+'</b><input id="pk'+k+'" type="range" min="0" max="255"><input id="pk'+k+'n" class="dur" type="number" inputmode="numeric" min="0" max="255"></label>').join('')+'</div><button class="btn p" style="width:100%;margin-top:12px" onclick="pkRgbClose()">Selesai</button></div>';$('#pop').className='on';S.popKind='other';
['r','g','b'].forEach(k=>{$('#pk'+k).oninput=()=>{const a=rgb2hsv(...['r','g','b'].map(x=>+$('#pk'+x).value));pkSet(a[0],a[1],a[2],'rgb');};$('#pk'+k+'n').oninput=e=>{const v=Math.max(0,Math.min(255,Math.round(+e.target.value)||0));$('#pk'+k).value=v;$('#pk'+k).oninput();};});
$('#pkh').oninput=e=>{let v=e.target.value.trim();if(/^[0-9a-f]{6}$/i.test(v))v='#'+v;if(/^#[0-9a-f]{6}$/i.test(v))pkHex(v);};pkUpd();}
function pkRgbClose(){const back=S.popKind==='other'&&S.ie&&S.ie.pop&&!S.ie.open;$('#pop').className='';S.popKind=null;if(back)slotPopShow();}
function popOut(){S.popKind==='slot'?slotPopClose():pkRgbClose();}
function pkSlots(){const q=$('#pkq');if(!q)return;q.innerHTML=QS.map((x,k)=>x?'<i data-k="'+k+'" style="background:'+x+'"><b class="pe" data-e="'+k+'" aria-label="Edit slot '+(k+1)+'">✎</b></i>':'<i data-k="'+k+'" class="em" aria-label="Slot kosong '+(k+1)+'">+</i>').join('');}
function pkSlotEdit(k){const o=PK.o;PK.sl=k;PK.pv=[o.h,o.s,o.v];if(QS[k])pkHex(QS[k]);$('#pkx').classList.add('ed');$('#pkcl').textContent='Simpan ke slot '+(k+1);}
function pkExit(){PK.sl=null;$('#pkx').classList.remove('ed');}
function pkCancel(){const p=PK.pv;pkExit();pkSet(p[0],p[1],p[2]);}
function pkConfirm(){const o=PK.o,k=PK.sl;QS[k]=rx(hsv(o.h,o.s,o.v));solidSlotsEdited=true;solidSlotsRevision++;qsSave();pkExit();pkSlots();saveSolidColorSlots(QS).then(function(){legacyQS=false;solidSlotsEdited=false;commit('Slot warna '+(k+1)+' tersimpan di perangkat');}).catch(function(){commit('Gagal menyimpan slot warna ke perangkat');});}
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
$('#pks').innerHTML=QC.map(x=>'<i style="background:'+x+'" data-c="'+x+'" aria-label="Warna sampel"></i>').join('');pkSlots();
$('#pks').onclick=e=>{const t=e.target.closest('i');if(t)pkHex(t.dataset.c);};
$('#pkq').onclick=e=>{const t=e.target.closest('i');if(!t)return;const k=+t.dataset.k;if(e.target.closest('[data-e]')||!QS[k])pkSlotEdit(k);else pkHex(QS[k]);};
$('#pkrand').onclick=pkRand;}
function slotPopClose(){}function slotPopShow(){}
function commit(m){toast(m);}

/* ===== antrean /json: WLED hanya melayani satu request /json sekaligus ===== */
let jqTail=Promise.resolve();
function jtry(url,opt,n){return fetch(url,opt).then(function(r){if(r.status===503&&n<6)return new Promise(function(w){setTimeout(w,250);}).then(function(){return jtry(url,opt,n+1);});return r;});}
function jfetch(url,opt){const task=jqTail.then(function(){return jtry(url,opt,0);});jqTail=task.catch(function(){});return task;}
function post(o){return jfetch('/json/state',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(o)}).catch(function(){});}
function debounce(fn,ms){let t;return function(){const a=arguments;clearTimeout(t);t=setTimeout(function(){fn.apply(null,a);},ms);};}
function throttleLatest(fn,ms){let last=0,t=null,args;return function(){args=arguments;const now=Date.now(),wait=ms-(now-last);if(wait<=0){clearTimeout(t);t=null;last=now;fn.apply(null,args);}else if(!t){t=setTimeout(function(){t=null;last=Date.now();fn.apply(null,args);},wait);}};}
const mpost=u=>fetch(u,{method:'POST',cache:'no-store'});

/* ===== konstanta ===== */
const SEG_COLOR_K=0,SEG_COLOR_L=1,SEG_K=2,SEG_L=3,SEG_LINK=4,SH=1000; // layer fisik: warna, efek per sisi, efek sambung
const FX_BLACKLIST=['frizzles','funky plank','game of life','geq','ghost rider','matrix','metaballs','tartan','polar lights','swirl','lissajous','rubix','invaders','maze','pulser','dna','plasma ball','hipnotic','black hole','cubes','fireworks','akemi','colored bursts','freqmatrix'];
const MZW_RE=/^(?:MZ\d\d|W\d\d) /;
const HIDE_AUDIO_FX=true;
const initialColorTab=new URLSearchParams(location.search).get('t')==='warna';
const FX_PAL_DEFAULTS={'Fire 2012':'Fire','Fireworks 1D':'Rainbow','Fireworks':'Rainbow','Noise 1':'Pastel','Noise':'Pastel','Pride':'Pride','Aurora':'Aurora','Pacifica':'Pacifica'};
const SDN={all:'Semua',L:'Kiri',R:'Kanan'};
const DC=()=>({cs:'one',col:{h:39,s:100,v:100}});

/* ===== state ===== */
const S={tb:'fx',lastRideMode:'fx',fside:'all',sel:0,fs:{L:null,R:null},q:'',fo:false,
  fx:[],fxd:[],shows:[],showMs:[],list:[],palNames:[],palList:[],palRaw:{},palInfo:{cpal:0,cmax:10,um:[],major:0},palSig:'',
  fav:new Set(),fxs:{},fxsSaved:{},pm:{},cu:[],act:{L:null,R:null},lists:[],
  rideColors:{L:{cs:'pal',pal:2,col:{h:39,s:100,v:100}},R:{cs:'pal',pal:2,col:{h:39,s:100,v:100}}},
  pi:null,si:-1,se:null,dr:null,sp:0,qc:null,listsLoaded:false,
  pl:{use:-1,run:false,hold:false,list:0,slot:0,el:0,at:0,ui:false},showLive:{L:false,R:false},
  sideSwap:false,rev:false,bri:180,net:true,sv:'ok',volt:null,pv:false,ready:false,popKind:null};
let legacyCu=[];
function loadLocal(){try{const d=JSON.parse(localStorage.getItem('mzRid4')||'null');if(d){S.fav=new Set(d.fav||[]);S.fxs=d.fxs||{};S.pm=d.pm||{};legacyCu=Array.isArray(d.cu)?d.cu:[];}}catch(e){}
  Object.keys(S.fxs).forEach(function(k){S.fxsSaved[k]=JSON.stringify(S.fxs[k]);});}
function saveLocal(){try{localStorage.setItem('mzRid4',JSON.stringify({fav:[...S.fav],fxs:S.fxs,pm:S.pm}));}catch(e){}}
loadLocal();

/* ===== data efek ===== */
const isShowId=i=>i>=SH;
function fxPart(i,k){if(isShowId(i))return null;const m=(S.fxd[i]||'').split(';');return m.length>k?m[k]:null;}
function fxUsesPal(i){if(isShowId(i))return true;const p=fxPart(i,2);return p===null?true:p!=='';}
function fxDefs(i){const d={},s=fxPart(i,4);if(s)s.split(',').forEach(function(kv){const p=kv.split('=');if(p.length===2){const v=parseInt(p[1],10);if(!isNaN(v))d[p[0].trim()]=v;}});return d;}
function isRsvd(n){const t=(n||'').toLowerCase();return t===''||t==='rsvd'||t==='-';}
function fxOk(i,name){if(isRsvd(name)||MZW_RE.test(name))return false;const t=name.toLowerCase();if(name.indexOf('2D')===0||FX_BLACKLIST.indexOf(t)>=0)return false;const f=fxPart(i,3)||'';if(f.indexOf('2')>=0&&f.indexOf('1')<0&&f.indexOf('0')<0)return false;if(HIDE_AUDIO_FX&&(f.indexOf('v')>=0||f.indexOf('f')>=0))return false;return true;}
function parseFxData(meta){const parts=(meta||'').split(';'),labels=(parts[0]||'').split(','),def=['Kecepatan','Intensitas','Custom 1','Custom 2','Custom 3'],keys=['sx','ix','c1','c2','c3'],sl=[];for(let i=0;i<5;i++){let l=labels[i];if(l===undefined||l==='')continue;if(l==='!')l=def[i];sl.push({key:keys[i],label:l});}const tg=[];for(let i=0;i<3;i++){let l=labels[5+i];if(l===undefined||l==='')continue;if(l==='!')l='Opsi '+(i+1);tg.push({key:['o1','o2','o3'][i],label:l});}return{sl:sl,tg:tg};}
function buildFxList(){const a=S.fx.map(function(n,i){return{n:n,i:i};}).filter(function(e){return fxOk(e.i,e.n);});
  S.shows.forEach(function(n,k){a.push({n:n,i:SH+k,show:true});});
  a.sort(function(x,y){if(x.i===0)return -1;if(y.i===0)return 1;return x.n.localeCompare(y.n);});S.list=a;}
function fxName(i){return isShowId(i)?(S.shows[i-SH]||('Show '+(i-SH))):(S.fx[i]||('Efek '+i));}
function showSec(i){const m=S.showMs[i-SH];return m?Math.round(m/100)/10:0;}
function pm(i){if(!S.pm[i]){const d=fxDefs(i),g=function(k,df){return d[k]!=null?d[k]:df;};S.pm[i]={sx:g('sx',128),ix:g('ix',128),c1:g('c1',128),c2:g('c2',128),c3:g('c3',128),o1:g('o1',0),o2:g('o2',0),o3:g('o3',0)};}return S.pm[i];}
function palIdByName(n){return S.palNames.findIndex(function(x){return x===n;});}
function cfg(i){if(!S.fxs[i]){let c=DC();if(isShowId(i))c={cs:'pal',pal:0,col:{h:39,s:100,v:100}};else{const d=fxDefs(i);let pi=-1;if(d.pal!=null)pi=d.pal;else if(FX_PAL_DEFAULTS[fxName(i)])pi=palIdByName(FX_PAL_DEFAULTS[fxName(i)]);if(pi>0&&S.palNames[pi]&&S.palNames[pi].charAt(0)!=='*')c={cs:'pal',pal:pi,col:{h:39,s:100,v:100}};}S.fxs[i]=c;}return S.fxs[i];}
function rideColor(k){return S.rideColors[k]||(S.rideColors[k]=DC());}
function rideColorKeys(){return sideKeys();}
function rideColorCurrent(){return rideColor(S.fside==='L'?'L':'R');}
function rideColorMode(){return rideMode()!=='pl';}
function rideColorPayload(c){const v=cfgCols(c,false),rgb=v.col[0];return{palette:c.cs==='one'?0:1,pal:v.pal,r:rgb[0],g:rgb[1],b:rgb[2]};}
function rideColorStore(k){const c=rideColor(k);return cuEnsure(c).then(function(){const v=rideColorPayload(c);
  return mpost('/mizuma/riding/colors?side='+sideName(k)+'&palette='+v.palette+'&pal='+v.pal+'&r='+v.r+'&g='+v.g+'&b='+v.b);
}).then(function(r){if(!r.ok)throw new Error('riding color');return r.json();}).then(function(d){if(!d.ok)throw new Error(d.error||'riding color');});}
function loadRideColors(){return fetch('/mizuma/riding/colors').then(function(r){if(!r.ok)throw new Error('riding colors');return r.json();}).then(function(d){
  [['R',d.right],['L',d.left]].forEach(function(x){const q=x[1];if(!q)return;const rgb=q.rgb||[255,255,255],hv=rgb2hsv(rgb[0],rgb[1],rgb[2]),skin=S.cu.find(function(k){return k.slot!=null&&cuId(k)===(q.pal|0);});
    S.rideColors[x[0]]=q.palette?(skin?{cs:'cus',cu:skin.id,col:{h:hv[0],s:hv[1],v:hv[2]}}:{cs:'pal',pal:q.pal|0,col:{h:hv[0],s:hv[1],v:hv[2]}}):{cs:'one',col:{h:hv[0],s:hv[1],v:hv[2]}};});
}).catch(function(){toast('Gagal memuat warna kanan/kiri dari perangkat');});}

/* ===== Skin Custom (model v19: titik [posisi%, #hex], 2–8 titik) → palette kustom WLED ===== */
const findCu=id=>S.cu.find(function(k){return k.id===id;});
const srt=k=>k.st.slice().sort(function(a,b){return a[0]-b[0];});
const cuG=k=>'linear-gradient(90deg,'+srt(k).map(function(s){return s[1]+' '+s[0]+'%';}).join(',')+')';
const palBase=()=>S.palInfo.major>=16?200:255;
function cuFile(k){const st=srt(k).map(function(s){return[Math.round(s[0]*2.55)].concat(hex(s[1]));});if(st[0][0]>0)st.unshift([0,st[0][1],st[0][2],st[0][3]]);const l=st[st.length-1];if(l[0]<255)st.push([255,l[1],l[2],l[3]]);return JSON.stringify({palette:[].concat.apply([],st)});}
const cuMax=()=>Math.min(30,S.palInfo.cmax||10);
function cuFreeSlot(){const u={};S.cu.forEach(function(k){if(k.slot!=null)u[k.slot]=1;});for(let s=0;s<(S.palInfo.cmax||10);s++)if(!u[s])return s;return -1;}
const cuNeed=k=>k.slot==null||k.sig!==cuFile(k);
/* WLED membaca /palette0..N.json berurutan dan berhenti di file pertama yang hilang, jadi slot diisi dari yang terkecil
   dan berkas lama tidak dihapus saat skin dihapus (slot dipakai ulang). */
function cuInstall(k){if(k.slot==null){const f=cuFreeSlot();if(f<0)return Promise.reject(new Error('penuh'));k.slot=f;}
  const body=cuFile(k),fd2=new FormData();fd2.append('data',new Blob([body],{type:'application/json'}),'/palette'+k.slot+'.json');
  return fetch('/upload',{method:'POST',body:fd2}).then(function(r){if(!r.ok)throw new Error('upload');k.sig=body;return saveCuMeta(k);}).then(function(){saveLocal();});}
function saveCuMeta(k){return fetch('/mizuma/skins',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({id:k.id,n:k.n,st:k.st,slot:k.slot})})
  .then(function(r){if(!r.ok)throw new Error('metadata');return r.json();}).then(function(d){if(!d.ok)throw new Error('metadata');});}
function cuEnsure(c){const k=c&&c.cs==='cus'&&findCu(c.cu);return k&&cuNeed(k)?cuInstall(k):Promise.resolve();}
function cuId(k){return k.slot==null?0:palBase()-k.slot;}
function cfgCols(c,show){
  if(show){
    if(c.cs==='one')return{pal:0,col:[hsv(c.col.h,c.col.s,c.col.v),[0,0,0],[0,0,0]]};
    return{pal:c.cs==='pal'?c.pal:(c.cs==='cus'&&findCu(c.cu)&&findCu(c.cu).slot!=null?cuId(findCu(c.cu)):0),col:[[255,165,0],[0,0,0],[0,0,0]]};
  }
  if(c.cs==='pal')return{pal:c.pal,col:[hsv(c.col.h,c.col.s,c.col.v),[0,0,0],[0,0,0]]};
  if(c.cs==='cus'){const k=findCu(c.cu);if(k&&k.slot!=null)return{pal:cuId(k),col:[hsv(c.col.h,c.col.s,c.col.v),[0,0,0],[0,0,0]]};}
  return{pal:0,col:[hsv(c.col.h,c.col.s,c.col.v),[0,0,0],[0,0,0]]};}
function cfgGrad(c){
  if(c.cs==='pal')return palGrad(c.pal)||'var(--s3)';
  if(c.cs==='cus'){const k=findCu(c.cu);if(k)return cuG(k);}
  const r=css(hsv(c.col.h,c.col.s,c.col.v));return 'linear-gradient(90deg,'+r+','+r+')';}
function cfgName(c,show){
  if(c.cs==='pal')return show&&c.pal===0?'Mizuma Asli':'Skin · '+(S.palNames[c.pal]||('Palette '+c.pal));
  if(c.cs==='cus'){const k=findCu(c.cu);return 'Skin Custom · '+(k?k.n:'-');}
  return 'Warna Solid · '+rx(hsv(c.col.h,c.col.s,c.col.v));}
const ckn=(c,show)=>cfgName(c,show).replace(/^Skin( Custom)? · /,'').replace(/^Warna Solid · /,'');

/* ===== sisi & segmen ===== */
function mapSegId(logical){const right=(logical==='kanan');return(right!==S.sideSwap)?SEG_K:SEG_L;}
function mapColorSegId(logical){const right=(logical==='kanan');return(right!==S.sideSwap)?SEG_COLOR_K:SEG_COLOR_L;}
const sideKeys=()=>S.fside==='all'?['R','L']:[S.fside];
const sideName=k=>k==='R'?'kanan':'kiri';
const sideArg=()=>S.fside==='all'?'both':sideName(S.fside);
function segIds(){return sideKeys().map(function(k){return mapSegId(sideName(k));});}
function rideMode(){const m=S.tb==='col'?S.lastRideMode:S.tb;return m==='fx'?'manual':m;}
function effectTargetIds(){const mode=rideMode();return mode==='semi'||mode==='auto'?[SEG_LINK]:segIds();}
function colorTargets(){return sideKeys().map(function(k){return mapColorSegId(sideName(k));});}
function selOf(){if(rideColorMode()||S.fside==='all')return S.sel;const f=S.fs[S.fside];if(f!=null)return f;const a=S.act[S.fside];return a?a.fx:S.sel;}
const snap=i=>JSON.stringify({fx:i,c:cfg(i),p:isShowId(i)?0:pm(i)});
function isActive(i){return S.pl.use<0&&sideKeys().every(function(k){return S.act[k]&&S.act[k].fx===i;});}
function isSaved(i){if(rideColorMode())return S.pl.use<0&&sideKeys().every(function(k){return S.act[k]&&S.act[k].fx===i;});
  const s=snap(i);return S.pl.use<0&&sideKeys().every(function(k){return S.act[k]&&S.act[k].s===s;});}

/* ===== lampu: pratinjau langsung ===== */
function plHold(){if(S.pl.run&&!S.pl.hold){S.pl.hold=true;S.pl.ui=true;mpost('/mizuma/pl/pause?on=1').catch(function(){});}}
function colorSegsFor(i,keys){return(keys||sideKeys()).map(function(k){
    const separate=rideColorMode(),c=separate?rideColor(k):cfg(i),v=cfgCols(c,!separate&&isShowId(i));
    return{id:mapColorSegId(sideName(k)),on:true,fx:65,pal:v.pal,sx:0,ix:0,c1:0,c2:0,c3:0,o1:0,o2:0,o3:0,col:v.col};
  });}
function segsFor(i,ids,keys){const p=pm(i),white=[[255,255,255],[255,255,255],[255,255,255]];
  const colors=colorSegsFor(i,keys);
  const effects=ids.map(function(id){return{id:id,on:true,fx:i,pal:0,rev:S.rev,sx:p.sx,ix:p.ix,c1:p.c1,c2:p.c2,c3:p.c3,o1:p.o1,o2:p.o2,o3:p.o3,col:white};});
  return colors.concat(effects);}
function stopShowSides(keys){mpost('/mizuma/show/stop?side='+(keys.length===2?'both':sideName(keys[0]))).catch(function(){});}
function applyLive(i){
  const c=cfg(i),ks=sideKeys(),needs=[];
  if(c.cs==='cus'&&findCu(c.cu)&&cuNeed(findCu(c.cu)))needs.push(cuEnsure(c));
  if(rideColorMode())ks.forEach(function(k){const color=rideColor(k);if(color.cs==='cus'&&findCu(color.cu)&&cuNeed(findCu(color.cu)))needs.push(cuEnsure(color));});
  if(needs.length){Promise.all(needs).then(function(){applyLiveReady(i);}).catch(function(){toast('Gagal memasang skin ke lampu');});return;}
  applyLiveReady(i);
}
function applyLiveReady(i){
  const ks=sideKeys(),mode=rideMode(),ids=mode==='semi'||mode==='auto'?[SEG_LINK]:segIds(),c=cfg(i);
  plHold();
  if(isShowId(i)){ks.forEach(function(k){S.showLive[k]=true;});const side=mode==='auto'?'both':sideArg();
    post({on:true,seg:segsFor(i,[],ks)}).then(function(){return mpost('/mizuma/show?index='+(i-SH)+'&side='+side);}).catch(function(){});}
  else{const st=ks.filter(function(k){return S.showLive[k];});if(st.length)stopShowSides(st);ks.forEach(function(k){S.showLive[k]=false;});
    const o={on:true,seg:segsFor(i,ids,ks)};if(mode==='semi'||mode==='auto')o.seg.push({id:SEG_K,on:false},{id:SEG_L,on:false},{id:SEG_LINK,on:true});if(S.bri>0)o.bri=S.bri;post(o);}
  S.pv=!isSaved(i);updInfo();}
const applyLiveD=throttleLatest(function(){applyLive(selOf());},120);
const rideColorSaveD=debounce(function(keys){Promise.all(keys.map(rideColorStore)).catch(function(){toast('Gagal menyimpan warna sisi ke perangkat');});},450);
const rideColorLiveD=throttleLatest(function(keys){
  Promise.all(keys.map(function(k){return cuEnsure(rideColor(k));})).then(function(){post({on:true,seg:colorSegsFor(selOf(),keys)});})
    .catch(function(){toast('Gagal memasang skin warna ke lampu');});
},120);
function rideColorChanged(){
  if(S.fside==='all')S.rideColors.L=JSON.parse(JSON.stringify(rideColor('R')));
  const keys=rideColorKeys();rideColorLiveD(keys);rideColorSaveD(keys);S.pv=false;updInfo();
}
function postParam(k,v){post({seg:effectTargetIds().map(function(id){const o={id:id};o[k]=v;return o;})});}
const postParamD=throttleLatest(postParam,120);

/* ===== simpan ke engine ===== */
function presetQuery(i,k){const show=isShowId(i),color=rideColorMode()?rideColor(k):cfg(i),c=cfgCols(color,show&&!rideColorMode()),p=show?{sx:128,ix:128,c1:128,c2:128,c3:128,o1:0,o2:0,o3:0}:pm(i);
  const q={fx:show?0:i,pal:c.pal,r:c.col[0][0],g:c.col[0][1],b:c.col[0][2],r1:c.col[1][0],g1:c.col[1][1],b1:c.col[1][2],r2:c.col[2][0],g2:c.col[2][1],b2:c.col[2][2],sx:p.sx,ix:p.ix,c1:p.c1,c2:p.c2,c3:p.c3,o1:p.o1,o2:p.o2,o3:p.o3};
  if(show){q.isShow=1;q.showIndex=i-SH;}else q.isShow=0;return new URLSearchParams(q).toString();}
function savePreset(k,i){return fetch('/mizuma/preset?slot=riding'+(k==='R'?'Kanan':'Kiri')+'&'+presetQuery(i,k),{method:'POST'}).then(function(r){if(!r.ok)throw new Error('HTTP '+r.status);return r.json();}).then(function(d){if(!d.ok)throw new Error('ditolak');});}
function persist(i,msg){const mode=rideMode(),ks=(mode==='semi'||mode==='auto')?['R','L']:sideKeys();S.sv='saving';rs();
  return Promise.all(ks.map(function(k){return cuEnsure(rideColorMode()?rideColor(k):cfg(i));})).then(function(){return Promise.all(ks.map(function(k){return savePreset(k,i);}));})
   .then(function(){if(S.pl.use>=0)return mpost('/mizuma/pl/use?list=-1').then(function(){return setRidingMode(mode==='manual'?'manual':mode);});})
   .then(function(){applyLive(i);const s=snap(i);ks.forEach(function(k){S.act[k]={fx:i,s:s};S.showLive[k]=isShowId(i);});S.fxsSaved[i]=JSON.stringify(cfg(i));saveLocal();
     S.pl.use=-1;S.pl.run=false;S.pl.hold=false;S.pl.ui=false;S.pv=false;S.sv='ok';S.net=true;rs();toast(msg);R();})
   .catch(function(){S.sv='fail';rs();toast('Gagal menyimpan. Mizuma terputus.');});}
function useFx(i){if(rideColorMode()||S.fside==='all')S.sel=i;else S.fs[S.fside]=i;persist(i,fxName(i)+(S.fside==='all'?'':' di LED '+SDN[S.fside].toLowerCase())+' dipakai untuk Riding');}
function restoreSaved(){const mode=rideMode();S.pv=false;S.pl.ui=false;mpost('/mizuma/apply?mode=riding&side=both').then(function(){return setRidingMode(mode);}).then(loadSaved).then(function(){applyLive(selOf());}).catch(function(){toast('Gagal membatalkan pratinjau');});toast('Pratinjau dibatalkan');setTimeout(pollPl,500);R();}

/* ===== muat data ===== */
function cfgFromPreset(st){const c0=st.col[0],hv=rgb2hsv(c0[0],c0[1],c0[2]),col={h:hv[0],s:hv[1],v:hv[2]};
  if(st.pal===0)return{cs:'one',col:col};
  if(st.pal>palBase()-(S.palInfo.cmax||10)&&st.pal<=palBase()){const slot=palBase()-st.pal;let k=S.cu.find(function(z){return z.slot===slot;});
    if(!k&&S.palRaw[st.pal]&&S.palRaw[st.pal].length){const raw=S.palRaw[st.pal].filter(Array.isArray);let stops=raw.map(function(e){return[Math.round(e[0]/2.55),rx([e[1],e[2],e[3]])];});
      while(stops.length>8)stops.splice(1+Math.floor(Math.random()*(stops.length-2)),1);if(stops.length<2)stops=[[0,'#ff0000'],[100,'#0000ff']];
      k={id:'w'+slot,n:'Skin WLED '+(slot+1),st:stops,slot:slot,sig:null,ephemeral:true};S.cu.push(k);}
    if(k)return{cs:'cus',cu:k.id,col:col};}
  return{cs:'pal',pal:st.pal,col:col};}
function applyPresetState(d){['R','L'].forEach(function(k){const st=d&&d['riding'+(k==='R'?'Kanan':'Kiri')];if(!st||!st.valid)return;
  const fx=st.isShow?SH+(st.showIndex|0):(st.fx|0);
  if(st.isShow){
    const c0=st.col&&st.col[0]?st.col[0]:[255,165,0],hv=rgb2hsv(c0[0],c0[1],c0[2]);
    S.fxs[fx]=st.pal===0?{cs:'one',col:{h:hv[0],s:hv[1],v:hv[2]}}:{cs:'pal',pal:st.pal|0,col:{h:hv[0],s:hv[1],v:hv[2]}};
  }else S.fxs[fx]=cfgFromPreset(st);S.fxsSaved[fx]=JSON.stringify(S.fxs[fx]);
  if(!st.isShow)S.pm[fx]={sx:st.sx,ix:st.ix,c1:st.c1,c2:st.c2,c3:st.c3,o1:st.o1?1:0,o2:st.o2?1:0,o3:st.o3?1:0};
  S.act[k]={fx:fx,s:snap(fx)};S.showLive[k]=!!st.isShow;});
  if(S.act.R)S.sel=S.act.R.fx;else if(S.act.L)S.sel=S.act.L.fx;else{const f=S.list[0];S.sel=f?f.i:0;}}
function loadSaved(){return fetch('/mizuma/presets').then(function(r){return r.json();}).then(applyPresetState).catch(function(){});}
function loadFxData(){return Promise.all([jfetch('/json/eff').then(function(r){return r.json();}),jfetch('/json/fxdata').then(function(r){return r.json();}),
    fetch('/mizuma/shows').then(function(r){return r.json();}).catch(function(){return[];}),fetch('/mizuma/show-durations').then(function(r){return r.json();}).catch(function(){return[];})])
  .then(function(v){S.fx=v[0];S.fxd=v[1];S.shows=v[2]||[];S.showMs=v[3]||[];buildFxList();return true;}).catch(function(){return false;});}

/* palette WLED: nama + gradasi dari /json/palx */
let palTry=0;
function palGrad(id){const d=S.palRaw[id];if(!d||!d.length)return null;const st=[];for(let j=0;j<d.length;j++){const e=d[j];let c,idx=false;if(Array.isArray(e)){idx=e[0]/255*100;c=[e[1],e[2],e[3]];}else if(e==='r'){c=[128,128,128];}else{c=[255,165,0];}const rgb='rgb('+c[0]+','+c[1]+','+c[2]+')';st.push(idx===false?rgb:rgb+' '+idx+'%');}if(st.length===1)st.push(st[0]);return 'linear-gradient(90deg,'+st.join(',')+')';}
function refreshPalStrips(){document.querySelectorAll('.pw[data-pid]').forEach(function(row){const g=palGrad(parseInt(row.dataset.pid,10));if(g){const s=row.querySelector('.ps');if(s)s.style.background=g;}});}
function palCacheLoad(sig){try{const c=JSON.parse(localStorage.getItem('mzPalCache')||'null');if(c&&c.sig===sig&&c.raw&&Date.now()-(c.t||0)<300000){S.palRaw=c.raw;return true;}}catch(e){}return false;}
function palCacheSave(){try{localStorage.setItem('mzPalCache',JSON.stringify({sig:S.palSig,t:Date.now(),raw:S.palRaw}));}catch(e){}}
let palXLoaded=false;
function ensurePalX(){if(palXLoaded)return;palXLoaded=true;if(!Object.keys(S.palRaw).length)loadPalX(0);}
function loadPalX(page){page=page||0;jfetch('/json/palx?page='+page).then(function(r){return r.ok?r.json():Promise.reject();}).then(function(d){const map=d&&d.p;if(!map||typeof map!=='object')throw 0;Object.keys(map).forEach(function(k){if(Array.isArray(map[k]))S.palRaw[parseInt(k,10)]=map[k];});palTry=0;if(typeof d.m==='number'&&page<d.m)loadPalX(page+1);else{palXLoaded=true;palCacheSave();buildPalList();refreshPalStrips();if(S.ready)R();}}).catch(function(){if(palTry++<4)setTimeout(function(){loadPalX(page);},1500);else palXLoaded=false;});}
function buildPalList(){const arr=[];S.palNames.forEach(function(n,i){if(n==='r'||n.charAt(0)==='*')return;arr.push({n:n,i:i});});
  arr.sort(function(a,b){if(a.i===0)return -1;if(b.i===0)return 1;return a.n.localeCompare(b.n);});
  if(S.palInfo.major>=16)S.palInfo.um.forEach(function(n,k){arr.push({n:n||('Usermod '+(k+1)),i:255-k});});
  S.palList=arr;}
function loadPalettes(){return Promise.all([jfetch('/json/pal').then(function(r){return r.json();}),jfetch('/json/info').then(function(r){return r.json();})]).then(function(v){const names=v[0],inf=v[1]||{};
  S.palInfo.major=parseInt(String(inf.ver||'0'),10)||0;if(inf.umpalnames!==undefined||inf.cpalmax!==undefined)S.palInfo.major=Math.max(S.palInfo.major,16);
  S.palInfo.cpal=inf.cpalcount||0;S.palInfo.cmax=inf.cpalmax||10;S.palInfo.um=Array.isArray(inf.umpalnames)?inf.umpalnames:[];
  S.cu.forEach(function(k){if(k.slot!=null&&k.slot>=S.palInfo.cpal)k.slot=null;});   // berkas palette hilang (mis. FS dibersihkan): pasang ulang saat dipakai
  S.palSig=names.length+'|'+S.palInfo.cpal+'|'+S.palInfo.um.length+'|'+S.palInfo.major;palXLoaded=palCacheLoad(S.palSig);S.palNames=names;buildPalList();palTry=0;return true;}).catch(function(){return false;});}
function loadSkinMeta(){return fetch('/mizuma/skins').then(function(r){if(!r.ok)throw new Error('skins');return r.json();}).then(function(rows){
  S.cu=Array.isArray(rows)?rows:[];S.cu.forEach(function(k){if(k.slot<0)k.slot=null;k.sig=k.slot==null?null:cuFile(k);});const known={};S.cu.forEach(function(k){known[k.id]=true;});
  const pending=legacyCu.filter(function(k){return k&&k.id&&!known[k.id];});
  return Promise.all(pending.map(function(k){return saveCuMeta(k).then(function(){S.cu.push(k);});})).then(function(){legacyCu=[];saveLocal();});
}).catch(function(){toast('Gagal memuat skin dari perangkat');});}

/* ===== toast / sheet / dialog ===== */
let TT;function toast(m){const t=$('#toast');t.innerHTML='<span>'+m+'</span>';t.className='on';clearTimeout(TT);TT=setTimeout(function(){t.className='';},1900);}
function openSheet(h,fx){$('#sh').innerHTML=h;$('#sh').className=fx?'fix':'';$('#sheet').className='on';}
function closeSheet(){$('#sheet').className='';wheel();}
function confirmDel(t,m,fn){S.ak=fn;$('#pop').innerHTML='<div><div style="font-weight:800;font-size:16px;margin-bottom:6px">'+t+'</div><div class="m" style="margin-bottom:16px;font-size:13px">'+m+'</div><div class="row"><button class="btn" onclick="pkRgbClose()">Batal</button><button class="btn p" style="background:var(--bad);border-color:var(--bad);color:#fff" onclick="pkRgbClose();S.ak()">Hapus</button></div></div>';$('#pop').className='on dlg';S.popKind='other';}

/* ===== header, info, banner ===== */
function riInfo0(){const L=S.act.L,R=S.act.R;let name,sub;
  if(S.pl.use>=0&&S.lists[S.pl.use]){name=S.lists[S.pl.use].n;sub='Playlist · '+S.lists[S.pl.use].it.length+' slot';}
  else if(!L&&!R){name='Riding';sub='Belum ada efek tersimpan';}
  else if(L&&R&&L.fx===R.fx){name=fxName(R.fx);sub=esc(cfgName(cfg(R.fx),isShowId(R.fx)));}
  else{name='Kiri '+(L?fxName(L.fx):'-')+' · Kanan '+(R?fxName(R.fx):'-');sub='Dua efek berbeda';}
  return'<b>'+esc(name)+'</b><span>'+sub+'</span>';}
function riInfo(){const st=!S.net?['Terputus','bad']:S.sv==='saving'?['Menyimpan…','sv']:S.sv==='fail'?['Gagal menyimpan','bad']:['✓ Tersimpan','ok'];
  return'<span style="gap:6px"><em style="font-style:normal;color:var(--ac);font-weight:700">Sedang menyala</em><em class="svs '+st[1]+'">'+st[0]+'</em></span>'+riInfo0();}
function hdr(){return'<div class="volt" title="'+(S.net?'Tersambung':'Terputus')+'" aria-label="Status koneksi"><span class="vd" style="background:'+(S.net?'var(--ok)':'var(--bad)')+'">'+(S.net?'✓':'!')+'</span></div><button class="ib" style="border-radius:50%" aria-label="Kecerahan dan pengaturan LED" onclick="briOpen()">☀</button>';}
function banner(){if(S.net)return'';return'<div class="ban"><span style="flex:1">Terputus dari Mizuma. Perubahan belum bisa disimpan.</span><button class="btn" style="flex:none;padding:6px 12px" onclick="pingNow()">Coba lagi</button></div>';}
function rs(){$('#ri').innerHTML=riInfo();$('#hr').innerHTML=hdr();$('#ban').innerHTML=banner();}
function updInfo(){rs();const p=$('#pvl');if(p)p.outerHTML=pvl();}
function pingNow(){fetch('/mizuma/status').then(function(r){return r.json();}).then(function(d){S.net=true;S.volt=d.volt?d.volt:null;rs();}).catch(function(){S.net=false;rs();});}
setInterval(function(){if(!document.hidden)pingNow();},10000);

/* ===== blok sisi (pengganti Peek: strip warna statis dari preset tersimpan) ===== */
function sideBlock(){const fx=S.tb==='fx',on=k=>S.fside===k?' on':'',oc=k=>' onclick="setSide(\''+k+'\')"';
  const cell=function(k,lbl){const a=S.act[k];return'<button class="sbn '+k+on(k)+'"'+oc(k)+' aria-label="LED '+lbl.toLowerCase()+'"><span>'+lbl+'</span><small>'+esc(a&&S.pl.use<0?fxName(a.fx):(S.pl.use>=0?'Playlist':'—'))+'</small></button>';};
  return'<div class="sbk act" role="group" aria-label="LED kiri dan kanan">'+cell('L','Kiri')+'<div class="sbm">'+(fx?'<button class="chip'+(S.fo?' on':'')+'" onclick="S.fo=!S.fo;R()">★ Favorit</button>':'')+'<button class="sbn A'+on('all')+'"'+oc('all')+'>Semua</button></div>'+cell('R','Kanan')+'</div>';}
function setSide(sd){S.fside=sd;R();}
function tbs(a){return'<div class="tabbar">'+a.map(function(x){return'<button class="'+(x[3]?'on':'')+'" onclick="'+x[2]+'">'+x[0]+'</button>';}).join('')+'</div>';}
function frz(){const active=S.tb==='col'?S.lastRideMode:S.tb,t=function(n,k){return[n,'',"goTb('"+k+"')",active===k];};
  return tbs([t('Manual','fx'),t('Semi-Auto','semi'),t('Auto','auto'),t('Playlist','pl')])+(active==='pl'?'':sideBlock());}
function setRidingMode(k){const m=k==='semi'?'semi-auto':k;return mpost('/mizuma/riding/mode?mode='+encodeURIComponent(m)).then(function(r){if(!r.ok)throw new Error('riding mode');return r.json();}).then(function(d){if(!d.ok)throw new Error(d.error||'riding mode');});}
function goTb(k){S.tb=k;S.q='';S.pi=null;if(k!=='col')S.lastRideMode=k;R();$('#scr').scrollTop=0;
  if(k==='pl'){ensureLists();return;}
  const selected=selOf();
  if(k==='semi'&&isShowId(selected)){const f=S.list.find(function(x){return !x.show;});if(f)S.sel=f.i;}
  if(k==='auto'&&!isShowId(selected)){if(S.shows.length)S.sel=SH;}
  setRidingMode(k==='fx'?'manual':k).then(function(){applyLive(selOf());}).catch(function(){toast('GPIO 0/1 belum siap atau mode gagal diaktifkan');});
}

/* ===== Single Effect ===== */
function pvl(){
  if(S.tb==='pl'){const nm=S.pl.use>=0&&S.lists[S.pl.use]?S.lists[S.pl.use].n:(S.pl.run&&S.lists[S.pl.list]?S.lists[S.pl.list].n:'-');const on=S.pl.use>=0&&!S.pl.ui;
    return'<div class="pvl" id="pvl"><span>Di lampu · <b>'+esc(nm)+'</b></span><span class="bdg'+(on?' ok':'')+'">'+(on?'Menyala di motor':'Pratinjau saja')+'</span></div>';}
  const i=selOf(),sv=isSaved(i);return'<div class="pvl" id="pvl"><span>Di lampu · <b>'+esc(fxName(i))+'</b>'+(S.pv||S.pl.ui?' <button class="lnk" onclick="restoreSaved()">Batalkan</button>':'')+'</span><span class="bdg'+(sv?' ok':'')+'">'+(sv?'Menyala di motor':'Pratinjau saja')+'</span></div>';}
function fxList(){const sel=selOf(),mode=rideMode();let h='',n=0;
  S.list.forEach(function(f){const i=f.i;if(mode==='semi'&&f.show)return;if(mode==='auto'&&!f.show)return;if(S.fo&&!S.fav.has(i))return;n++;
    const on=sel===i,fv=S.fav.has(i),hid=S.q&&f.n.toLowerCase().indexOf(S.q.toLowerCase())<0,ac=isActive(i),sv=ac&&isSaved(i);
    const lab=sv?'✓ Dipakai':(ac?'Simpan':'Pakai');
    let actions='';
    if(on&&mode==='manual')actions='<button class="fxcolor" aria-label="Ubah warna sisi" onclick="event.stopPropagation();goNav(\'col\')"><i style="background:'+cfgGrad(rideColorCurrent())+'"></i><span>Warna sisi</span></button><button class="rp fxride'+(sv?' on':'')+'" onclick="event.stopPropagation();useFx('+i+')">'+lab+(S.fside==='all'?'':' · '+SDN[S.fside])+'</button>';
    else if(on&&mode==='semi')actions='<button class="fxcolor" onclick="event.stopPropagation();goNav(\'col\')"><i style="background:'+cfgGrad(rideColorCurrent())+'"></i><span>Warna sisi</span></button><button class="rp fxride'+(sv?' on':'')+'" onclick="event.stopPropagation();useFx('+i+')">'+lab+'</button>';
    else if(on&&mode==='auto')actions='<button class="rp fxride'+(sv?' on':'')+'" onclick="event.stopPropagation();useFx('+i+')">'+lab+'</button>';
    if(on)actions+='<button class="star'+(fv?' on':'')+'" aria-label="Favorit" onclick="event.stopPropagation();tf('+i+')">'+(fv?'★':'☆')+'</button>';
    h+='<div class="fr'+(on?' on':'')+'" data-n="'+esc(f.n.toLowerCase())+'" style="'+(hid?'display:none':'')+'" onclick="pick('+i+')"><span class="no">'+n+'</span><b>'+esc(f.n)+(f.show?' <em class="cb">✦ Mizuma</em>':'')+'</b>'+actions+'</div>';});
  if(!S.fx.length)return'<div class="emp">Memuat daftar efek…</div>';
  if(!h)return'<div class="emp">'+(S.fo?'Belum ada efek favorit. Ketuk efek lalu ★ untuk menambah.':'Tidak ada efek.')+'</div>';
  return h;}
function vFx(){const mode=rideMode(),title=mode==='semi'?'Pilih satu efek untuk seluruh LED. Warna kanan dan kiri diatur terpisah.':mode==='auto'?'Pilih animasi Mizuma untuk seluruh LED. Warna tiap sisi tetap berasal dari layer warna.':mode==='manual'?'Efek kanan dan kiri berjalan terpisah. Warna diatur per sisi pada tab Warna.':'Pilih playlist animasi.';return pvl()+'<div class="m" style="padding:0 12px 8px">'+title+'</div><div class="fxlist">'+fxList()+'</div>';}
function fqx(v){S.q=v;document.querySelectorAll('#scr .fr[data-n]').forEach(function(e){e.style.display=e.dataset.n.indexOf(v.toLowerCase())<0?'none':'';});}
function pick(i){if(rideColorMode()||S.fside==='all'){S.sel=i;S.fs={L:null,R:null};}else S.fs[S.fside]=i;applyLive(i);R();}
function tf(i){S.fav.has(i)?S.fav.delete(i):S.fav.add(i);saveLocal();R();}
function pbar(i){if(isShowId(i))return'<div class="m" style="width:100%;text-align:center">Show Mizuma · '+fd(showSec(i))+' dtk · diulang terus di Riding. Warna lewat tab Warna kanan/kiri.</div>';
  const p=parseFxData(S.fxd[i]||''),v=pm(i);
  if(!p.sl.length&&!p.tg.length)return'<div class="m" style="width:100%;text-align:center">Efek ini tidak punya parameter tambahan.</div>';
  return'<div class="pb"><div class="pg2">'+p.sl.map(function(s){return'<label>'+esc(s.label)+'<input type="range" min="0" max="255" value="'+v[s.key]+'" oninput="prm(\''+s.key+'\',this.value)" onchange="updInfo()"></label>';}).join('')+'</div>'+
    (p.tg.length?'<div class="row" style="margin-top:6px;flex-wrap:wrap">'+p.tg.map(function(s){return'<button class="tgl'+(v[s.key]?' on':'')+'" onclick="tgp(\''+s.key+'\',this)">'+esc(s.label)+'</button>';}).join('')+'</div>':'')+'</div>';}
function prm(k,val){const i=selOf();pm(i)[k]=+val;plHold();postParamD(k,+val);saveLocalD();S.pv=!isSaved(i);}
function tgp(k,el){const i=selOf(),n=pm(i)[k]?0:1;pm(i)[k]=n;el.classList.toggle('on',!!n);plHold();postParam(k,n);saveLocal();S.pv=!isSaved(i);updInfo();}
const saveLocalD=debounce(saveLocal,400);

/* ===== tab Warna (efek terpilih) ===== */
function skinRows(c,fp,fc,show){ensurePalX();const cu=[...S.cu.keys()].reverse().map(function(i){const k=S.cu[i],tools=k.ephemeral?'':'<button class="ib" aria-label="Edit skin" onclick="event.stopPropagation();cpEdit('+i+')">✎</button><button class="ib" aria-label="Hapus skin" style="color:var(--bad)" onclick="event.stopPropagation();cpDel('+i+')">🗑</button>';return'<div class="pw cu'+(c.cs==='cus'&&c.cu===k.id?' on':'')+'" onclick="'+fc+'(\''+k.id+'\')"><span class="rd"></span><span class="pn" style="text-align:left">'+esc(k.n)+'<em class="cb">✦ Custom</em></span><span class="ps" style="background:'+cuG(k)+'"></span>'+tools+'</div>';}).join('');
  const bi=S.palList.map(function(e){return'<div class="pw'+(c.cs==='pal'&&c.pal===e.i?' on':'')+'" data-pid="'+e.i+'" onclick="'+fp+'('+e.i+')"><span class="rd"></span><span class="pn">'+esc(e.i===0?(show?'Mizuma Asli':'Default'):e.n)+'</span><span class="ps" style="background:'+(palGrad(e.i)||'var(--s3)')+'"></span></div>';}).join('');
  return cu+'<div class="pg">'+(bi||'<div class="m">Memuat palette…</div>')+'</div>';}
function vCol(){const i=selOf(),separate=rideColorMode(),c=separate?rideColorCurrent():cfg(i),show=!separate&&isShowId(i),note=!separate&&!fxUsesPal(i)?'<div class="m" style="margin:0 2px 8px">Efek ini tidak menggunakan palette.</div>':'';
  const head=separate?'<div class="m" style="margin:0 2px 8px">Warna sisi · <b style="color:var(--tx)">'+(S.fside==='all'?'Kanan & kiri':SDN[S.fside])+'</b>. Disimpan di perangkat.</div>':'<div class="m" style="margin:0 2px 8px">Warna untuk <b style="color:var(--tx)">'+esc(fxName(i))+'</b> · '+SDN[S.fside]+'</div>';
  return head+note+'<div class="pkx">'+pkHTML(1)+'</div>'+skinRows(c,'setPal','setCu',show);}
function colorChanged(){if(rideColorMode()){rideColorChanged();R();return;}applyLive(selOf());R();}
function setMode(k){const i=selOf(),c=rideColorMode()?rideColorCurrent():cfg(i);if(k==='one'){if(!rideColorMode()&&isShowId(i))return;c.cs='one';}else if(c.cs==='one'){c.cs='pal';c.pal=S.palList.length>1?S.palList[1].i:0;}colorChanged();}
function setPal(id){const c=rideColorMode()?rideColorCurrent():cfg(selOf());c.cs='pal';c.pal=id;colorChanged();}
function setCu(id){const c=rideColorMode()?rideColorCurrent():cfg(selOf());c.cs='cus';c.cu=id;colorChanged();}
function colDirty(i){return S.fxsSaved[i]!==JSON.stringify(cfg(i));}
function saveCol(){const i=selOf();if(rideColorMode()){
    const keys=rideColorKeys();Promise.all(keys.map(rideColorStore)).then(function(){toast('Warna sisi tersimpan di perangkat');}).catch(function(){toast('Gagal menyimpan warna sisi ke perangkat');});return;}
  S.fxsSaved[i]=JSON.stringify(cfg(i));saveLocal();
  if(isActive(i)){persist(i,'Warna '+fxName(i)+' tersimpan');}else{applyLive(i);R();toast('Warna '+fxName(i)+' tersimpan. Ketuk Pakai untuk memasangnya.');}}
function foot(){const i=selOf();
  if(S.tb==='fx')return pbar(i);
  if(S.tb==='col'){const d=rideColorMode()?true:(colDirty(i)||!isSaved(i));return'<div class="color-actions"><button class="btn fbtn sk" onclick="cpEdit(-1)">＋ Skin Baru</button><button class="btn p fbtn" style="'+(d?'':'opacity:.55')+'" onclick="saveCol()">'+(d?'Simpan warna':'Warna tersimpan ✓')+'</button></div>';}
  return'';}
function wheel(){if(!$('#scr #pkw')||document.querySelector('#sheet.on'))return;const i=selOf(),separate=rideColorMode(),c=separate?rideColorCurrent():cfg(i);pkBind(c.col,function(){c.cs='one';if(separate)rideColorChanged();else colLive(i);});}
function colLive(i){const c=rideColorMode()?rideColorCurrent():cfg(i);document.querySelectorAll('.blk').forEach(function(b,k){b.classList.toggle('on',(k===0)===(c.cs==='one'));const r=b.querySelector('.rad');if(r)r.classList.toggle('on',(k===0)===(c.cs==='one'));});
  const m=document.querySelector('.blk .mini'),sp=document.querySelector('.blk .mt span');if(m)m.style.background='linear-gradient(90deg,'+css(hsv(c.col.h,c.col.s,c.col.v))+','+css(hsv(c.col.h,c.col.s,c.col.v))+')';if(sp)sp.textContent=rx(hsv(c.col.h,c.col.s,c.col.v));
  $('#foot').innerHTML=foot();if(rideColorMode())rideColorChanged();else applyLiveD();}

/* ===== sheet Warna cepat (tombol Warna di baris efek) ===== */
function qcOpen(i){const c=JSON.parse(JSON.stringify(cfg(i)));S.qc={i:i,c:c,orig:JSON.parse(JSON.stringify(c)),show:isShowId(i)};qcR();}
function qcPreview(){const d=S.qc;if(!d)return;S.fxs[d.i]=d.c;applyLive(d.i);}
function qcPal(id){S.qc.c.cs='pal';S.qc.c.pal=id;const b=$('#qcg').scrollTop;qcR();$('#qcg').scrollTop=b;qcPreview();}
function qcCu(id){S.qc.c.cs='cus';S.qc.c.cu=id;const b=$('#qcg').scrollTop;qcR();$('#qcg').scrollTop=b;qcPreview();}
function qcR(){const d=S.qc,c=d.c;
  openSheet('<div style="font-weight:800;margin-bottom:9px">Ubah warna · '+esc(fxName(d.i))+'</div><div class="pbody"><div>'+pkHTML(1)+'</div><div class="grow" id="qcg">'+skinRows(c,'qcPal','qcCu',d.show)+'</div></div><div class="row" style="margin-top:12px;flex:none"><button class="btn" onclick="qcCancel()">Batal</button><button class="btn p" onclick="qcApply()">Simpan warna</button></div>',true);
  pkBind(c.col,function(){c.cs='one';qcPreview();});refreshPalStrips();}
function qcCancel(){const d=S.qc;if(!d)return;S.fxs[d.i]=d.orig;S.qc=null;closeSheet();applyLive(d.i);R();}
function qcApply(){const d=S.qc;if(!d)return;S.fxs[d.i]=d.c;if(S.fside==='all')S.sel=d.i;else S.fs[S.fside]=d.i;S.qc=null;closeSheet();saveCol();}

/* ===== editor Skin Efek (persis v19: bar gradasi, geser titik, +Titik, Hapus titik, Acak) ===== */
function cpEdit(i){if(i<0&&S.cu.length>=cuMax()){toast('Maksimal '+cuMax()+' skin custom. Hapus salah satu untuk menambah.');return;}
  S.dr={i:i,from:S.qc?'qc':(S.se&&S.se.v==='col'?'se':null),c:i<0?{id:'c'+Date.now().toString(36),n:'',st:[[0,'#ff0000'],[100,'#0000ff']],slot:null,sig:null}:JSON.parse(JSON.stringify(S.cu[i]))};S.sp=0;cpR();}
function cpR(){const c=S.dr.c;openSheet('<div style="font-weight:800;margin-bottom:8px">'+(S.dr.i<0?'Skin efek baru':'Edit skin efek')+'</div><input class="srch" type="text" maxlength="24" placeholder="Isi nama Skin Efek" value="'+esc(c.n)+'" oninput="S.dr.c.n=this.value"><div class="gb" id="gbar" style="background:'+cuG(c)+'">'+c.st.map(function(s,j){return'<i class="stp'+(j===S.sp?' on':'')+'" style="left:'+s[0]+'%;background:'+s[1]+'"></i>';}).join('')+'</div><div class="row" style="margin-bottom:8px"><button class="btn sm" onclick="cpAdd()"'+(c.st.length>=8?' disabled':'')+'>＋ Titik</button><button class="btn sm" onclick="cpRm()"'+(c.st.length<=2?' disabled':'')+'>− Titik</button><button class="btn sm" onclick="cpRand()">🎲 Acak</button></div><div class="m" style="margin-bottom:6px">Geser titik pada bar untuk mengatur posisi · maks 8 titik</div>'+pkHTML(1)+'<div class="row cpf"><button class="btn" onclick="cpCancel()">Batal</button><button class="btn p" onclick="cpSave()">Simpan</button></div>');cpBind();}
function cpU(){const c=S.dr.c;$('#gbar').style.background=cuG(c);document.querySelectorAll('#gbar .stp').forEach(function(e,j){e.style.left=c.st[j][0]+'%';e.style.background=c.st[j][1];});}
function cpAdd(){const c=S.dr.c;if(c.st.length>=8)return;const s=srt(c);let bi=0,bg=0;for(let k=0;k<s.length-1;k++){const g=s[k+1][0]-s[k][0];if(g>bg){bg=g;bi=k;}}const a=s[bi],b=s[bi+1],pos=Math.round((a[0]+b[0])/2),ca=hex(a[1]),cb=hex(b[1]);c.st.push([pos,rx([(ca[0]+cb[0])/2,(ca[1]+cb[1])/2,(ca[2]+cb[2])/2])]);S.sp=c.st.length-1;cpR();}
function cpRm(){const c=S.dr.c;if(c.st.length<=2)return;c.st.splice(S.sp,1);S.sp=0;cpR();}
function cpRand(){const n=S.dr.c.st.length,b=Math.random()*360,m=Math.random()<.5;S.dr.c.st.forEach(function(s,k){s[1]=rx(hsv(m?(b+k*360/n+Math.random()*30)%360:Math.random()*360,65+Math.random()*35,70+Math.random()*30));});cpR();}
function cpBind(){const bar=$('#gbar'),sel=function(j){S.sp=j;bar.querySelectorAll('.stp').forEach(function(x,k){x.classList.toggle('on',k===j);});const a=rgb2hsv.apply(null,hex(S.dr.c.st[j][1]));S.dpk={h:a[0],s:a[1],v:a[2]};pkBind(S.dpk,function(){S.dr.c.st[S.sp][1]=rx(hsv(S.dpk.h,S.dpk.s,S.dpk.v));cpU();});};let dg=false;
  bar.onpointerdown=function(e){const b=bar.getBoundingClientRect(),x=(e.clientX-b.left)/b.width*100;let j=-1,m=1e9;S.dr.c.st.forEach(function(s,k){const d=Math.abs(s[0]-x);if(d<m){m=d;j=k;}});if(j<0||m*b.width/100>28)return;e.preventDefault();try{bar.setPointerCapture(e.pointerId);}catch(x2){}dg=true;if(S.sp!==j)sel(j);};
  bar.onpointermove=function(e){if(!dg)return;const b=bar.getBoundingClientRect();S.dr.c.st[S.sp][0]=Math.round(Math.max(0,Math.min(100,(e.clientX-b.left)/b.width*100)));cpU();};bar.onpointerup=bar.onpointercancel=function(){dg=false;};sel(S.sp);}
function cpCancel(){const f=S.dr&&S.dr.from;S.dr=null;if(f==='qc'&&S.qc)qcR();else if(f==='se'&&S.se)seR();else closeSheet();}
function cpSave(){const d=S.dr,c=d.c,old=d.i<0?null:JSON.parse(JSON.stringify(S.cu[d.i]));c.n=(c.n||'').trim()||('Skin '+(d.i<0?S.cu.length+1:d.i+1));
  if(d.i<0)S.cu.push(c);else{c.slot=S.cu[d.i].slot;S.cu[d.i]=c;}
  const f=d.from;S.sig=null;
  cuInstall(c).then(function(){S.dr=null;
    if(f==='qc'&&S.qc){S.qc.c.cs='cus';S.qc.c.cu=c.id;qcR();qcPreview();toast('Skin “'+esc(c.n)+'” tersimpan');}
    else if(f==='se'&&S.se){S.se.c={cs:'cus',cu:c.id,col:S.se.c.col||DC().col};seR();seColorPreview();toast('Skin tersimpan');}
    else{const i=selOf(),fc=rideColorMode()?rideColorCurrent():cfg(i);fc.cs='cus';fc.cu=c.id;closeSheet();if(rideColorMode())rideColorChanged();else applyLive(i);R();toast('Skin “'+esc(c.n)+'” tersimpan');}
    saveLocal();if(S.pi!=null)plSyncAll();})
   .catch(function(e){if(d.i<0)S.cu.pop();else S.cu[d.i]=old;toast(e&&e.message==='penuh'?'Slot palette WLED penuh ('+cuMax()+').':'Gagal memasang skin ke lampu.');});}
function cpDel0(i){const c=S.cu[i];fetch('/mizuma/skins/delete?id='+encodeURIComponent(c.id),{method:'POST'}).then(function(r){if(!r.ok)throw new Error('delete');return r.json();}).then(function(d){if(!d.ok)throw new Error('delete');S.cu.splice(i,1);let liveChanged=false;Object.keys(S.fxs).forEach(function(x){const f=S.fxs[x];if(f.cs==='cus'&&f.cu===c.id){if(+x===selOf())liveChanged=true;f.cs='pal';f.pal=0;}});
  if(S.qc&&S.qc.c.cs==='cus'&&S.qc.c.cu===c.id)S.qc.c={cs:'pal',pal:0,col:S.qc.c.col};
  S.lists.forEach(function(p,li){let changed=false;p.it.forEach(function(x){['L','R'].forEach(function(k){const sd=sideOf(x,k);if(sd.c.cs==='cus'&&sd.c.cu===c.id){sd.c={cs:'pal',pal:0,col:sd.c.col};if(x.man)x[k].c=sd.c;else x.c=sd.c;changed=true;}});});if(changed)plMark(li);});
  saveLocal();
  if(S.qc)qcR();else if(S.se&&S.se.v==='col'){const e=S.se;if(e.c.cs==='cus'&&e.c.cu===c.id)e.c={cs:'pal',pal:0,col:e.c.col};seR();}else{closeSheet();R();}if(liveChanged)applyLive(selOf());toast('Skin “'+esc(c.n)+'” dihapus');}
).catch(function(){toast('Gagal menghapus skin dari perangkat');});}
function cpDel(i){confirmDel('Hapus skin?','Skin “'+esc(S.cu[i].n)+'” akan dihapus. Efek yang memakainya kembali ke palette Default.',function(){cpDel0(i);});}

/* ================= Playlist Animasi (disimpan & dijalankan di engine) ================= */
const tot=p=>Math.round(p.it.reduce(function(z,x){return z+x.d;},0)*10)/10;
const sideOf=(x,k)=>x.man?x[k]:{f:x.f,c:x.c,p:x.p,rv:!!x.rv};
function fxDefId(){const e=S.list.find(function(z){return z.n==='Scan';})||S.list.find(function(z){return !z.show;})||S.list[0];return e?e.i:0;}
function newSide(f,c){return{f:f,c:c||(isShowId(f)?{cs:'pal',pal:0,col:DC().col}:cfg(f)?JSON.parse(JSON.stringify(cfg(f))):DC()),p:isShowId(f)?null:Object.assign({},pm(f)),rv:false};}
function slotBg(x){const l=sideOf(x,'L'),r=sideOf(x,'R');if(!x.man||JSON.stringify(l.c)===JSON.stringify(r.c))return cfgGrad(l.c);return cfgGrad(l.c)+' top/100% 50% no-repeat,'+cfgGrad(r.c)+' bottom/100% 50% no-repeat';}
function slotTitle(x){const l=sideOf(x,'L'),r=sideOf(x,'R');return esc(x.man&&l.f!==r.f?fxName(l.f)+' · '+fxName(r.f):fxName(l.f));}
function thumb(p){return p.it.length?'<div class="th">'+p.it.map(function(x){return'<i style="background:'+slotBg(x)+'"></i>';}).join('')+'</div>':'<div class="th em2">Kosong</div>';}
/* konversi UI <-> engine */
function sideOut(sd){const f=sd.f,c=sd.c;if(isShowId(f)){const m=cfgCols(c,true);return{show:1,si:f-SH,fx:0,pal:m.pal,col:m.col,sx:128,ix:128,c1:128,c2:128,c3:128,o1:0,o2:0,o3:0,rv:sd.rv?1:0};}
  const m=cfgCols(c),p=sd.p||pm(f);return{show:0,si:0,fx:f,pal:m.pal,col:m.col,sx:p.sx,ix:p.ix,c1:p.c1,c2:p.c2,c3:p.c3,o1:p.o1,o2:p.o2,o3:p.o3,rv:sd.rv?1:0};}
function listOut(i){const p=S.lists[i];return{list:i,name:p.n,it:p.it.map(function(x){return{d:Math.round(x.d*10),R:sideOut(sideOf(x,'R')),L:sideOut(sideOf(x,'L'))};})};}
function sideIn(o,legacyRv){const f=o.show?SH+(o.si|0):(o.fx|0),c0=o.col[0],hv=rgb2hsv(c0[0],c0[1],c0[2]),col={h:hv[0],s:hv[1],v:hv[2]},rv=o.rv==null?!!legacyRv:!!o.rv;
  if(o.show)return{f:f,c:{cs:'pal',pal:o.pal|0,col:col},p:null,rv:rv};
  const st={pal:o.pal|0,col:o.col},c=cfgFromPreset(st);
  return{f:f,c:c,p:{sx:o.sx,ix:o.ix,c1:o.c1,c2:o.c2,c3:o.c3,o1:o.o1?1:0,o2:o.o2?1:0,o3:o.o3?1:0},rv:rv};}
function listIn(d){const it=(d.it||[]).map(function(s){const R=sideIn(s.R,s.rv),L=sideIn(s.L,s.rv),same=JSON.stringify(R)===JSON.stringify(L);
  return same?{d:s.d/10,rv:R.rv,man:false,f:R.f,c:R.c,p:R.p}:{d:s.d/10,rv:false,man:true,f:R.f,c:R.c,p:R.p,L:L,R:R};});return{n:d.name||'Playlist',it:it};}
function loadLists(){return fetch('/mizuma/playlists').then(function(r){if(!r.ok)throw new Error('lists');return r.json();}).then(function(d){S.pl.use=d.use;const n=d.lists.filter(function(l){return l.n!==''||l.c>0;}).length;
  return Promise.all(d.lists.slice(0,n).map(function(l,i){return fetch('/mizuma/pl/list?i='+i).then(function(r){if(!r.ok)throw new Error('list');return r.json();});})).then(function(a){S.lists=a.map(listIn);S.listsLoaded=true;});});}
function ensureLists(){if(S.listsLoaded)return;loadLists().then(function(){if(S.tb==='pl')R();}).catch(function(){toast('Gagal memuat playlist dari perangkat');});}
/* simpan ke engine (ditunda 700 ms; skin dipasang dulu bila perlu) */
const plDirty={};
function plMark(i){plDirty[i]=true;S.sv='saving';rs();plSyncD();}
const plSyncD=debounce(function(){plSyncAll();},700);
function plSyncAll(){const ids=Object.keys(plDirty).filter(function(i){return plDirty[i];}).map(Number);if(!ids.length){S.sv='ok';rs();return Promise.resolve();}
  ids.forEach(function(i){plDirty[i]=false;});
  return Promise.all(ids.map(function(i){const p=S.lists[i];if(!p)return null;const need=[];p.it.forEach(function(x){['L','R'].forEach(function(k){const c=sideOf(x,k).c;const sk=c.cs==='cus'&&findCu(c.cu);if(sk&&cuNeed(sk)&&need.indexOf(sk)<0)need.push(sk);});});
    return Promise.all(need.map(cuInstall)).then(function(){return fetch('/mizuma/pl/save',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(listOut(i))});}).then(function(r){if(!r.ok)throw new Error('save');});}))
   .then(function(){S.sv='ok';S.net=true;rs();}).catch(function(){ids.forEach(function(i){plDirty[i]=true;});S.sv='fail';rs();toast('Gagal menyimpan playlist.');});}
function plFlush(){return plSyncAll();}
/* status runner dari engine */
function pollPl(){fetch('/mizuma/pl/state').then(function(r){return r.json();}).then(function(d){const ch=d.use!==S.pl.use||d.run!==S.pl.run||d.hold!==S.pl.hold||d.list!==S.pl.list||d.slot!==S.pl.slot;
  S.pl.use=d.use;S.pl.run=d.run;S.pl.hold=d.hold;S.pl.list=d.list;S.pl.slot=d.slot;S.pl.el=d.el/1000;S.pl.at=Date.now();if(!d.hold&&d.use<0)S.pl.ui=S.pl.ui&&d.run;
  if(ch&&S.tb==='pl')R();}).catch(function(){});}
setInterval(function(){if(!document.hidden&&(S.tb==='pl'||S.pl.use>=0))pollPl();},5000);
function plEl(){const p=S.pl;return p.run&&!p.hold?p.el+(Date.now()-p.at)/1000:p.el;}
setInterval(function(){if(S.tb!=='pl'||!S.pl.run||S.pl.hold)return;const l=S.lists[S.pl.list],x=l&&l.it[S.pl.slot];if(!x)return;let e=plEl();
  if(e>=x.d&&l.it.length>1){S.pl.el=e-x.d;S.pl.at=Date.now();S.pl.slot=(S.pl.slot+1)%l.it.length;if(!(SCD&&SCD.on)&&S.pi===S.pl.list)R();return;}
  const b=$('#pgb');if(b)b.style.width=Math.min(100,e/x.d*100)+'%';},200);

function plOpen(i){S.pi=i;S.si=-1;R();$('#scr').scrollTop=0;}
function plBack(){S.pi=null;S.si=-1;R();}
function plGallery(){const E='event.stopPropagation();',ib=function(f,l,x){return'<button class="ib" '+(x||'')+' onclick="'+E+f+'">'+l+'</button>';};
  let h='<div class="gal">';
  S.lists.forEach(function(p,i){const a=S.pl.use===i,run=S.pl.run&&S.pl.list===i&&!S.pl.hold;
    h+='<div class="tile'+(a?' on':'')+'" data-pl onclick="plOpen('+i+')" role="button" aria-label="Buka playlist '+esc(p.n)+'">'+thumb(p)+(a?'<span class="tbg">● Menyala</span>':'')+'<div class="tn"><b>'+esc(p.n)+'</b><span class="m">'+p.it.length+'/12 slot · '+fd(tot(p))+' dtk</span></div><div class="tb">'+ib('plPlay('+i+')',run?'⏸':'▶','style="color:var(--ac)" aria-label="'+(run?'Jeda':'Putar')+'"')+ib('plStop('+i+')','■','aria-label="Stop"')+ib('delPl('+i+')','🗑','style="color:var(--bad)" aria-label="Hapus playlist"')+'</div></div>';});
  if(S.lists.length<4)h+='<button class="tile add" onclick="newPl()"><b>＋</b><span class="m">Playlist baru</span><span class="m">'+S.lists.length+'/4</span></button>';
  return h+'</div>';}
function plFolder(i){const p=S.lists[i],E='event.stopPropagation();',run=S.pl.run&&S.pl.list===i&&!S.pl.hold,here=S.pl.run&&S.pl.list===i;
  let h='<div class="fh"><button class="ib" aria-label="Kembali ke daftar playlist" onclick="plBack()">‹</button><input class="srch" style="margin:0;flex:1;min-width:0" type="text" maxlength="20" value="'+esc(p.n)+'" oninput="S.lists['+i+'].n=this.value;plMark('+i+')" aria-label="Nama playlist"></div><div class="fh2"><span class="m" style="flex:1">'+p.it.length+'/12 slot · '+fd(tot(p))+' dtk</span><button class="ib" style="color:var(--ac)" onclick="plPlay('+i+')" aria-label="Putar">'+(run?'⏸':'▶')+'</button><button class="ib" onclick="plStop('+i+')" aria-label="Stop">■</button></div><div class="sls g2">';
  for(let k=0;k<12;k++){const x=p.it[k];if(!x){h+='<button class="sc em" aria-label="Tambah slot" onclick="slotAdd('+i+')">+</button>';continue;}
    h+='<div class="sc'+(S.si===k?' on':'')+'" data-sc="'+k+'" onpointerdown="scDown(event,'+i+','+k+')" oncontextmenu="return false" onclick="if(Date.now()-SCB<400)return;slotSel('+i+','+k+')"><span class="sw" style="background:'+slotBg(x)+'"><span class="sn">'+(k+1)+'</span></span><i class="gh" aria-label="Geser untuk ubah urutan slot">⠿</i><b>'+slotTitle(x)+'</b><span class="m">'+fd(x.d)+' dtk</span><button class="se" aria-label="Edit slot '+(k+1)+'" onclick="'+E+'seOpen('+i+','+k+')">✎</button>'+(here&&S.pl.slot===k?'<i class="pgb" id="pgb" style="width:'+Math.min(100,plEl()/x.d*100)+'%"></i>':'')+'</div>';}
  return h+'</div>'+(p.it.length>1?'<div class="m" style="margin-top:8px">Geser ikon ⠿ pada slot untuk mengubah urutan.</div>':'');}
function vPl(){return pvl()+(S.listsLoaded?(S.pi!=null&&S.lists[S.pi]?plFolder(S.pi):plGallery()):'<div class="emp">Memuat playlist dari perangkat…</div>');}
/* aksi playlist */
function plUseCall(i){return mpost('/mizuma/pl/use?list='+i).then(function(r){if(!r.ok)throw new Error('pl');});}
function plPlay(i){const l=S.lists[i];if(!l.it.length)return toast('Playlist kosong');
  if(S.pl.use===i&&S.pl.run){const on=!S.pl.hold;S.pl.hold=on;S.pl.at=Date.now();mpost('/mizuma/pl/pause?on='+(on?1:0)).catch(function(){});if(!on)S.pl.ui=false;R();return;}
  plFlush().then(function(){return plUseCall(i);}).then(function(){S.pl.use=i;S.pl.run=true;S.pl.hold=false;S.pl.list=i;S.pl.slot=0;S.pl.el=0;S.pl.at=Date.now();S.pl.ui=false;S.pv=false;toast('Playlist “'+esc(l.n)+'” diputar dan dipakai untuk Riding');R();}).catch(function(){toast('Gagal memutar playlist');});}
function plStop(i){if(S.pl.use===i){mpost('/mizuma/pl/use?list=-1').then(function(){S.pl.use=-1;S.pl.run=false;S.pl.hold=false;S.pl.ui=false;toast('Playlist dihentikan, kembali ke Single Effect');setTimeout(function(){loadSaved().then(R);},400);R();}).catch(function(){});}
  else if(S.pl.run&&S.pl.list===i){mpost('/mizuma/pl/stop').then(function(){S.pl.run=false;S.pl.ui=false;R();});}}
function newPl(){if(S.lists.length>=4)return toast('Maksimal 4 playlist');S.lists.push({n:'Playlist '+(S.lists.length+1),it:[]});S.pi=S.lists.length-1;S.si=-1;plMark(S.pi);R();}
function delPl(i){confirmDel('Hapus playlist?','“'+esc(S.lists[i].n)+'” beserta '+S.lists[i].it.length+' slot di dalamnya akan dihapus.',function(){delPl0(i);});}
function delPl0(i){const p=S.lists[i];plFlush().then(function(){return mpost('/mizuma/pl/delete?list='+i);}).then(function(){S.lists.splice(i,1);if(S.pl.use===i){S.pl.use=-1;S.pl.run=false;}else if(S.pl.use>i)S.pl.use--;S.pi=null;closeSheet();R();toast('Playlist “'+esc(p.n)+'” dihapus');if(S.pl.use<0)setTimeout(function(){loadSaved().then(R);},400);}).catch(function(){toast('Gagal menghapus playlist');});}
function slotSel(i,j){S.si=j;plFlush().then(function(){return mpost('/mizuma/pl/play?list='+i+'&slot='+j+'&hold=1');}).then(function(){S.pl.run=true;S.pl.hold=true;S.pl.list=i;S.pl.slot=j;S.pl.el=0;S.pl.at=Date.now();S.pl.ui=true;R();}).catch(function(){toast('Gagal menampilkan slot');});}
function slotAdd(i){const l=S.lists[i];if(l.it.length>=12)return toast('Playlist penuh (12 slot)');const f=fxDefId(),s=newSide(f);l.it.push({d:5,rv:false,man:false,f:f,c:s.c,p:s.p,L:s,R:s});S.pi=i;plMark(i);seOpen(i,l.it.length-1);}
function slotDel(i,j){confirmDel('Hapus efek ini?','“'+slotTitle(S.lists[i].it[j])+'” akan dihapus dari slot '+(j+1)+'.',function(){slotDel0(i,j);});}
function slotDel0(i,j){S.lists[i].it.splice(j,1);S.se=null;S.si=-1;closeSheet();plMark(i);R();toast('Slot dihapus');}
/* geser slot (sama dengan v19) */
let SCD=null,SCB=0;
const scTM=e=>{if(SCD&&SCD.on)e.preventDefault();};
function scDown(ev,i,k){if(ev.target.closest('.se')||(ev.pointerType==='mouse'&&ev.button!==0))return;const el=ev.currentTarget,st={i:i,k:k,el:el,x0:ev.clientX,y0:ev.clientY,on:false,to:k,id:ev.pointerId,touch:ev.pointerType!=='mouse'};SCD=st;
  if(ev.target.closest('.gh'))scStart(st);else if(st.touch)st.t=setTimeout(function(){scStart(st);},240);
  el.onpointermove=function(e){scMove(e);};el.onpointerup=el.onpointercancel=function(e){scEnd(e);};}
function scStart(st){if(SCD!==st||st.on)return;st.on=true;try{st.el.setPointerCapture(st.id);}catch(e){}st.el.classList.add('drg');if(navigator.vibrate)navigator.vibrate(12);
  st.rects=[...st.el.parentNode.querySelectorAll('[data-sc]')].map(function(e){return{k:+e.dataset.sc,e:e,r:e.getBoundingClientRect()};});document.addEventListener('touchmove',scTM,{passive:false});}
function scMove(e){const st=SCD;if(!st)return;const dx=e.clientX-st.x0,dy=e.clientY-st.y0;
  if(!st.on){if(Math.hypot(dx,dy)>(st.touch?10:6)){if(st.touch){clearTimeout(st.t);SCD=null;st.el.onpointermove=st.el.onpointerup=st.el.onpointercancel=null;}else scStart(st);}if(!st.on)return;}
  st.el.style.transform='translate('+dx+'px,'+dy+'px) scale(1.06)';
  for(const q of st.rects){const r=q.r;if(e.clientX>=r.left&&e.clientX<=r.right&&e.clientY>=r.top&&e.clientY<=r.bottom){st.to=q.k;break;}}
  st.rects.forEach(function(q){q.e.classList.toggle('tgt',q.k===st.to&&st.to!==st.k);});}
function scEnd(e){const st=SCD;if(!st)return;clearTimeout(st.t);document.removeEventListener('touchmove',scTM);st.el.onpointermove=st.el.onpointerup=st.el.onpointercancel=null;
  if(!st.on){SCD=null;return;}SCB=Date.now();SCD=null;if(e.type==='pointercancel'||st.to===st.k)R();else slotReorder(st.i,st.k,st.to);}
function slotReorder(i,from,to,quiet){const a=S.lists[i].it,x=a.splice(from,1)[0];a.splice(to,0,x);
  const m=function(q){return q===from?to:(from<to&&q>from&&q<=to)?q-1:(from>to&&q>=to&&q<from)?q+1:q;};
  if(S.pi===i&&S.si>=0)S.si=m(S.si);plMark(i);R();if(!quiet)toast('Urutan slot diubah');}

/* ===== editor slot (Main Effect Kiri/Kanan, Reverse, Tukar, durasi) ===== */
const seX=()=>S.lists[S.se.i].it[S.se.j];
function seOpen(i,j){const x=S.lists[i].it[j];if(!x.man){x.L={f:x.f,c:JSON.parse(JSON.stringify(x.c)),p:x.p?Object.assign({},x.p):null,rv:!!x.rv};x.R={f:x.f,c:JSON.parse(JSON.stringify(x.c)),p:x.p?Object.assign({},x.p):null,rv:!!x.rv};x.man=true;}
  S.pi=i;S.si=j;S.se={i:i,j:j,v:'main',dirty:false};R();seR();slotSelQuiet(i,j);}
function slotSelQuiet(i,j){plFlush().then(function(){return mpost('/mizuma/pl/play?list='+i+'&slot='+j+'&hold=1');}).then(function(){S.pl.run=true;S.pl.hold=true;S.pl.list=i;S.pl.slot=j;S.pl.ui=true;}).catch(function(){});}
function seClose(){const d=S.se&&S.se.dirty,i=S.se&&S.se.i;S.se=null;closeSheet();R();if(d){plFlush();toast('Slot tersimpan');}}
const favFirst=()=>{const a=S.list.map(function(e){return e.i;});return a.filter(function(f){return S.fav.has(f);}).concat(a.filter(function(f){return !S.fav.has(f);}));};
function seList(k){const x=seX(),cur=sideOf(x,k).f;
  return'<div class="fsc" id="fsc_'+k+'">'+favFirst().map(function(f){return'<div class="fi'+(f===cur?' on':'')+'" onclick="seFx(\''+k+'\','+f+')">'+(S.fav.has(f)?'★ ':'')+esc(fxName(f))+(isShowId(f)?' <em class="cb">✦</em>':'')+'</div>';}).join('')+'</div>';}
function seColBtn(k){const c=sideOf(seX(),k).c,f=sideOf(seX(),k).f;return'<button class="skb" onclick="seColOpen(\''+k+'\')"><i style="background:'+cfgGrad(c)+'"></i><span>'+esc(ckn(c,isShowId(f)))+'</span></button>';}
function seParams(k){const sd=sideOf(seX(),k);if(isShowId(sd.f))return'<div class="m separam-hint">Parameter efek tidak tersedia pada animasi Mizuma.</div>';
  const m=parseFxData(S.fxd[sd.f]||'');if(!m.sl.length&&!m.tg.length)return'<div class="m separam-hint">Efek ini tidak memiliki parameter tambahan.</div>';
  const p=sd.p||(sd.p=Object.assign({},pm(sd.f)));
  return'<div class="separams">'+m.sl.map(function(s){const v=p[s.key]!=null?p[s.key]:128;return'<div class="separam"><label><span>'+esc(s.label)+'</span><span class="sepv">'+v+'</span></label><input type="range" min="0" max="255" value="'+v+'" oninput="seParam(\''+k+'\',\''+s.key+'\',this.value,this)"></div>';}).join('')+
    m.tg.map(function(s){const on=!!p[s.key];return'<button class="tgl'+(on?' on':'')+'" data-label="'+esc(s.label)+'" style="margin:2px" onclick="seParamToggle(\''+k+'\',\''+s.key+'\',this)">'+esc(s.label)+': '+(on?'On':'Off')+'</button>';}).join('')+'</div>';}
function seColumn(k,t){return'<div class="sec"><div class="sect">'+t+'</div>'+seList(k)+seParams(k)+seColBtn(k)+'</div>';}
const tgl=(on,onc,l)=>'<button class="tgl'+(on?' on':'')+'" onclick="'+onc+'">'+l+'</button>';
function seMain(){const x=seX(),e=S.se;
  return'<div class="seh"><b>Edit slot '+(e.j+1)+'</b><button class="ib" aria-label="Tutup" onclick="seClose()">✕</button></div>'+
  '<div class="se-actions"><button class="tgl'+(sideOf(x,'L').rv?' on':'')+'" onclick="seRv(\'L\')">⟲ Reverse Kiri</button><button class="se-swap'+(S.sideSwap?' on':'')+'" id="seSwapBtn" onclick="seSwap()" aria-pressed="'+(S.sideSwap?'true':'false')+'">⇄ Tukar</button><button class="tgl'+(sideOf(x,'R').rv?' on':'')+'" onclick="seRv(\'R\')">⟲ Reverse Kanan</button></div>'+
  '<div class="se3"><div class="seb"><div class="sbt">Main Effect</div><div class="cols2">'+seColumn('L','Kiri')+seColumn('R','Kanan')+'</div></div></div>'+
  '<div class="seb dur"><label for="sedur">Durasi (detik)</label><input id="sedur" type="number" inputmode="decimal" min="0.1" max="999" step="0.5" value="'+fd(x.d)+'" onchange="seDur(this)"></div>'+
  '<div class="row cpf"><button class="btn" style="color:var(--bad)" onclick="slotDel('+e.i+','+e.j+')">🗑 Hapus</button><button class="btn p" onclick="seClose()">Selesai</button></div>';}
function seColView(){const e=S.se,c=e.c,show=isShowId(sideOf(seX(),e.tg).f),nm={L:'Kiri',R:'Kanan'}[e.tg];
  return'<div class="seh"><button class="ib" aria-label="Kembali" onclick="seColBack()">‹</button><b style="flex:1;margin-left:8px">Warna · '+nm+'</b></div><div id="ieg"><div>'+pkHTML(1)+'</div>'+skinRows(c,'seColPal','seColCu',show)+'</div><div class="row cpf"><button class="btn" onclick="seColBack()">Batal</button><button class="btn p" onclick="seColSet()">Pilih warna</button></div>';}
function seR(){const e=S.se;if(!e)return;const sh=$('#sh'),keep={};sh.querySelectorAll('.fsc').forEach(function(a){keep[a.id]=a.scrollTop;});const top=sh.scrollTop;
  openSheet(e.v==='col'?seColView():seMain());$('#sheet').classList.add('sxe');$('#sheet').classList.toggle('sev',e.v==='col');
  if(e.v==='main'){sh.querySelectorAll('.fsc').forEach(function(a){if(keep[a.id]!=null)a.scrollTop=keep[a.id];else{const o=a.querySelector('.fi.on');if(o)a.scrollTop=Math.max(0,o.offsetTop-a.offsetTop-50);}});sh.scrollTop=top;}
  else if(e.v==='col'){const o=e.c.col;pkBind(o,function(){e.c.cs='one';const ch=$('#iech'),n=$('#iecn');if(ch)ch.style.background=cfgGrad(e.c);if(n)n.textContent=cfgName(e.c);seColorPreview();});}
  refreshPalStrips();}
function seTouch(){S.se.dirty=true;plMark(S.se.i);R();seR();seLive();}
function seLive(){const e=S.se;if(!e)return;plFlush().then(function(){return mpost('/mizuma/pl/play?list='+e.i+'&slot='+e.j+'&hold=1');}).catch(function(){});}
const seLiveD=debounce(seLive,180);
function seParam(k,key,val,el){const sd=sideOf(seX(),k);if(!sd.p)sd.p=Object.assign({},pm(sd.f));sd.p[key]=+val;
  if(el&&el.tagName==='INPUT'){const value=el.parentNode.querySelector('.sepv');if(value)value.textContent=val;}
  if(el&&el.tagName==='BUTTON'){const on=!!(+val);el.classList.toggle('on',on);el.textContent=(el.dataset.label||'Opsi')+': '+(on?'On':'Off');}
  S.se.dirty=true;plMark(S.se.i);seLiveD();}
function seParamToggle(k,key,el){const sd=sideOf(seX(),k);if(!sd.p)sd.p=Object.assign({},pm(sd.f));seParam(k,key,sd.p[key]?0:1,el);}
function seColorPreview(){const e=S.se;if(!e||e.v!=='col')return;sideOf(seX(),e.tg).c=JSON.parse(JSON.stringify(e.c));e.dirty=true;plMark(e.i);seLiveD();}
function seFx(k,f){const x=seX(),old=sideOf(x,k),s=newSide(f,isShowId(f)&&!isShowId(old.f)?null:(isShowId(old.f)&&!isShowId(f)?null:JSON.parse(JSON.stringify(old.c))));s.rv=!!old.rv;x[k]=s;seTouch();}
function seRv(k){const x=seX();sideOf(x,k).rv=!sideOf(x,k).rv;seTouch();}
function seSwap(){const next=!S.sideSwap,b=$('#seSwapBtn');if(b)b.disabled=true;fetch('/mizuma/side-swap?enabled='+(next?'1':'0'),{method:'POST',cache:'no-store'}).then(function(r){if(!r.ok)throw new Error('HTTP '+r.status);return r.json();}).then(function(d){if(!d.ok)throw new Error('swap rejected');S.sideSwap=!!d.enabled;seR();toast(S.sideSwap?'⇄ Output fisik ditukar':'⇄ Output fisik normal');setTimeout(function(){if(S.se)slotSelQuiet(S.se.i,S.se.j);},400);}).catch(function(){toast('Gagal menukar output fisik');}).finally(function(){const current=$('#seSwapBtn');if(current)current.disabled=false;});}
function seDur(el){const v=Math.max(.1,Math.min(999,Math.round((+el.value||5)*10)/10));seX().d=v;el.value=fd(v);S.se.dirty=true;plMark(S.se.i);R();}
function seColOpen(k){const e=S.se,sd=sideOf(seX(),k);e.v='col';e.tg=k;e.orig=JSON.parse(JSON.stringify(sd.c));e.c=JSON.parse(JSON.stringify(sd.c));e.mode=e.c.cs==='one'&&!isShowId(sd.f)?'one':'skin';seR();}
function seColBack(){const e=S.se;if(e&&e.orig){e.c=e.orig;sideOf(seX(),e.tg).c=JSON.parse(JSON.stringify(e.orig));e.orig=null;e.dirty=true;plMark(e.i);seLiveD();}S.se.v='main';seR();}
function seColMode(m){const e=S.se;if(m==='one')e.c.cs='one';else if(e.c.cs==='one'){e.c.cs='pal';e.c.pal=S.palList.length>1?S.palList[1].i:0;}e.mode=m;seR();seColorPreview();}
function seColPal(id){const e=S.se;e.c.cs='pal';e.c.pal=id;const b=$('#ieg').scrollTop;seR();$('#ieg').scrollTop=b;seColorPreview();}
function seColCu(id){const e=S.se;e.c.cs='cus';e.c.cu=id;const b=$('#ieg').scrollTop;seR();$('#ieg').scrollTop=b;seColorPreview();}
function seColSet(){const e=S.se,x=seX();sideOf(x,e.tg).c=JSON.parse(JSON.stringify(e.c));e.orig=null;e.v='main';seTouch();}

/* ===== kecerahan + arah + posisi ===== */
function briOpen(){openSheet('<div style="font-weight:800;margin-bottom:12px">Kecerahan LED</div><div class="row"><span>☀</span><input id="gbi" type="range" min="1" max="255" value="'+S.bri+'" style="flex:1"><b id="gbv" style="min-width:42px;text-align:right">'+Math.round(S.bri/2.55)+'%</b></div>'+
  '<div class="row" style="margin-top:14px;gap:8px"><button class="btn'+(S.rev?' p':'')+'" onclick="reqRev()">↔ Arah: '+(S.rev?'Dibalik':'Normal')+'</button><button class="btn'+(S.sideSwap?' p':'')+'" onclick="reqSwap()">⇄ Posisi: '+(S.sideSwap?'Ditukar':'Normal')+'</button></div><div class="m" style="margin-top:8px">Arah dan Posisi berlaku untuk semua efek.</div><div class="row" style="margin-top:12px"><button class="btn p" onclick="closeSheet()">Selesai</button></div>');
  $('#gbi').oninput=function(e){S.bri=+e.target.value;$('#gbv').textContent=Math.round(S.bri/2.55)+'%';briSend(S.bri);};}
const briSend=debounce(function(v){post({bri:v});mpost('/mizuma/bri?v='+v).catch(function(){});},80);
function reqSwap(){const next=!S.sideSwap;jfetch('/mizuma/side-swap?enabled='+(next?'1':'0'),{method:'POST',cache:'no-store'}).then(function(r){if(!r.ok)throw 0;return r.json();}).then(function(d){if(!d.ok)throw 0;S.sideSwap=!!d.enabled;toast(S.sideSwap?'⇄ Output kanan-kiri ditukar':'Pemetaan kanan-kiri dikembalikan');setTimeout(function(){loadSaved().then(R);},400);briOpen();}).catch(function(){toast('Gagal mengubah posisi');});}
function reqRev(){const next=!S.rev;jfetch('/mizuma/show/reverse?enabled='+(next?'1':'0'),{method:'POST',cache:'no-store'}).then(function(r){if(!r.ok)throw 0;return r.json();}).then(function(d){if(!d.ok)throw 0;S.rev=!!d.enabled;toast(S.rev?'↔ Arah dibalik':'↔ Arah normal');briOpen();}).catch(function(){toast('Gagal mengubah arah');});}

/* ===== navigasi bawah ===== */
const IC={dash:'<rect x="3" y="3" width="7" height="9" rx="1"/><rect x="14" y="3" width="7" height="5" rx="1"/><rect x="14" y="12" width="7" height="9" rx="1"/><rect x="3" y="16" width="7" height="5" rx="1"/>',
 wel:'<path d="M12 3l2.4 5.2 5.6.7-4.1 3.9 1 5.6L12 15.6 7.1 18.4l1-5.6L4 8.9l5.6-.7z"/>',rid:'<path d="M9 18h6M10 21h4M12 3a6 6 0 0 0-3.6 10.8c.7.6 1.1 1.4 1.3 2.2h4.6c.2-.8.6-1.6 1.3-2.2A6 6 0 0 0 12 3z"/>',
 col:'<path d="M12 3s6 6.5 6 11a6 6 0 0 1-12 0c0-4.5 6-11 6-11z"/>',rem:'<path d="M3 12h13M11 6l6 6-6 6M20 6v12"/>',set:'<circle cx="12" cy="12" r="3"/><path d="M12 2v3M12 19v3M2 12h3M19 12h3M5 5l2 2M17 17l2 2M19 5l-2 2M7 17l-2 2"/>'};
/* Tab utama: Dashboard · Warna · Welcoming · Riding · Sein-Rem · Pengaturan. Welcoming dan Sein-Rem memakai halaman /led. */
const TABS=[['dash','Dashboard',"location.href='/app'"],['col','Warna',"goNav('col')"],['wel','Welcoming',"location.href='/led?t=welcoming'"],['rid','Riding',"goNav('fx')"],['rem','Sein-Rem',"location.href='/led?t=sein'"],['set','Pengaturan',"location.href='/pengaturan'"]];
function goNav(k){
  if(k==='col'){if(S.tb!=='col'&&['fx','semi','auto','pl'].indexOf(S.tb)>=0)S.lastRideMode=S.tb;S.tb='col';}
  else if(k==='fx')S.tb=['fx','semi','auto','pl'].indexOf(S.lastRideMode)>=0?S.lastRideMode:'fx';
  else S.tb=k;
  S.q='';S.pi=null;R();$('#scr').scrollTop=0;if(S.tb==='pl')ensureLists();}
function navHTML(){const cur=S.tb==='col'?'col':'rid';return TABS.map(function(t){return'<button class="'+(t[0]===cur?'on':'')+'" onclick="'+t[2]+'"><span><svg viewBox="0 0 24 24">'+IC[t[0]]+'</svg></span>'+t[1]+'</button>';}).join('');}

/* ===== render ===== */
function R(){const top=$('#scr').scrollTop;rs();$('#frz').innerHTML=frz();
  $('#scr').innerHTML=S.tb==='col'?vCol():S.tb==='pl'?vPl():vFx();
  const f=(S.tb==='pl')?'':foot();$('#foot').innerHTML=f;$('#foot').style.display=f?'flex':'none';
  $('#nav').innerHTML=navHTML();$('#scr').scrollTop=top;wheel();refreshPalStrips();}

/* ===== keluar halaman: kembalikan lampu ke Riding tersimpan bila masih pratinjau ===== */
window.addEventListener('pagehide',function(){if(!(S.pv||S.pl.ui))return;try{const mode=rideMode();navigator.sendBeacon(mode==='manual'?'/mizuma/apply?mode=riding&side=both':'/mizuma/riding/mode?mode='+(mode==='semi'?'semi-auto':mode));}catch(e){}});

/* ===== init ===== */
function boot(){
  Promise.all([loadFxData(),loadPalettes()]).then(function(r){
    if(!r[0]){setTimeout(boot,1500);return null;}
    return Promise.all([jfetch('/mizuma/side-swap').then(function(x){return x.json();}).then(function(d){S.sideSwap=!!d.enabled;}).catch(function(){}),
      jfetch('/mizuma/show/reverse').then(function(x){return x.json();}).then(function(d){S.rev=!!d.enabled;}).catch(function(){}),
      jfetch('/json/state').then(function(x){return x.json();}).then(function(j){if(j&&j.bri)S.bri=j.bri;}).catch(function(){}),loadSkinMeta().then(loadRideColors),loadSolidColorSlots(),
      fetch('/mizuma/riding/mode').then(function(x){return x.json();}).then(function(d){
        const m=d.mode==='semi-auto'?'semi':d.mode==='auto'?'auto':d.mode==='playlist'?'pl':'fx';S.lastRideMode=m;S.tb=initialColorTab?'col':m;if(m==='pl')ensureLists();
      }).catch(function(){})]).then(loadSaved);
  }).then(function(x){if(x===null)return;S.ready=true;pingNow();pollPl();R();});
}
if(initialColorTab)S.tb='col';
R();boot();

</script></body></html>
)rawliteral";

const char MIZUMA_LED_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Lampu - Mizuma</title>
<link rel="stylesheet" href="/mizuma.css">
<style>
html,body{height:100%;overflow:hidden;}
body{display:flex;flex-direction:column;}
.freeze{flex-shrink:0;background:var(--bg);border-bottom:1px solid var(--bd);}
.tabbar{display:flex;overflow-x:auto;background:var(--s1);border-bottom:1px solid var(--bd);}
.tabbar button{flex:1 0 auto;padding:10px 10px;background:none;border:none;color:var(--tx2);font-size:12px;font-weight:600;white-space:nowrap;border-bottom:3px solid transparent;}
.tabbar button.active{color:var(--tx);border-bottom-color:var(--ac);}
/* Satu baris, dua kategori berbeda:
   [Pilih sisi lampu: Kiri | Semua | Kanan] = pilihan, hanya satu yang aktif (bentuk segmen menyatu).
   [Atur semua sisi: Posisi, Arah]    = sakelar nyala/mati, berlaku ke semua sisi (bentuk tombol terpisah + lampu status). */
.control-panel{display:grid;grid-template-columns:minmax(0,3fr) minmax(0,2fr);column-gap:12px;padding:7px 8px 0;align-items:start;}
.ctl-group{position:relative;min-width:0;display:flex;flex-direction:column;gap:4px;}
.ctl-tools::before{content:"";position:absolute;left:-7px;top:2px;bottom:0;width:1px;background:var(--bd2);}
.control-caption{display:flex;align-items:center;gap:6px;min-width:0;padding:0 2px;font-size:10px;font-weight:700;line-height:13px;color:var(--tx3);white-space:nowrap;}
.control-caption span{min-width:0;overflow:hidden;text-overflow:ellipsis;}
.control-caption::after{content:"";flex:1 0 8px;height:1px;background:var(--bd);}
.side-row{display:flex;gap:2px;min-width:0;padding:3px;background:var(--s1);border:1px solid var(--bd);border-radius:12px;}
.side-row .side-btn{flex:1;min-width:0;min-height:40px;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:5px;padding:4px 2px;border:1px solid transparent;border-radius:9px;background:transparent;color:var(--tx2);font-size:clamp(10px,3vw,11.5px);font-weight:700;line-height:1;white-space:nowrap;}
.side-row .side-btn:active{transform:scale(.96);}
.side-btn .lamp{display:flex;gap:3px;}
.side-btn .lamp i{width:10px;height:4px;border-radius:2px;background:var(--bd2);transition:background .2s,box-shadow .2s;}
.side-btn .lamp i.on{background:var(--tx3);}
.side-row .side-btn.active{background:var(--ac-dim);border-color:var(--ac);color:var(--ac);}
.side-row .side-btn.active .lamp i.on{background:var(--ac);box-shadow:0 0 6px rgba(245,165,36,.6);}
.tool-row{display:flex;gap:6px;min-width:0;}
.control-panel .tool-btn{position:relative;flex:1;min-width:0;min-height:46px;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:2px;padding:5px 2px 4px;border:1px solid var(--bd2);border-radius:10px;background:var(--s2);color:var(--tx);line-height:1;white-space:nowrap;}
.control-panel .tool-btn:active{transform:scale(.96);}
.control-panel .tool-btn::after{content:"";position:absolute;top:5px;right:5px;width:5px;height:5px;border-radius:50%;background:var(--bd2);transition:background .2s,box-shadow .2s;}
.control-panel .tool-icon{width:16px;height:16px;color:var(--tx2);}
.control-panel .tool-icon svg{width:16px;height:16px;fill:none;stroke:currentColor;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
.control-panel .tool-label{display:block;max-width:100%;overflow:hidden;text-overflow:clip;font-size:clamp(10px,3vw,11px);font-weight:700;}
.control-panel .tool-state{display:block;max-width:100%;overflow:hidden;text-overflow:clip;font-size:8.5px;font-weight:600;color:var(--tx3);}
.control-panel .tool-btn.active{background:var(--ac-dim);border-color:var(--ac);color:var(--ac);}
.control-panel .tool-btn.active::after{background:var(--ac);box-shadow:0 0 5px var(--ac);}
.control-panel .tool-btn.active .tool-icon,.control-panel .tool-btn.active .tool-state{color:var(--ac);}
.control-panel .tool-btn:disabled{opacity:.55;cursor:wait;}
@media(prefers-reduced-motion:reduce){.control-panel *{transition:none !important;}}
@media(min-width:520px){.control-panel{column-gap:14px;padding-left:12px;padding-right:12px;}.ctl-tools::before{left:-8px;}}
.preview-row{display:grid;grid-template-columns:1fr 26px 1fr;gap:8px;align-items:center;padding:8px 12px;}
.pv-canvas{width:100%;height:14px;image-rendering:pixelated;background:#000;border-radius:5px;display:block;transition:opacity .25s;}
.pv-canvas.off{opacity:.22;}
.live-btn{width:24px;height:24px;border-radius:50%;background:var(--s2);border:1px solid var(--bd2);display:flex;align-items:center;justify-content:center;color:var(--tx3);padding:0;}
.live-btn svg{width:12px;height:12px;stroke:currentColor;fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
.live-btn.on{color:var(--ac);border-color:var(--ac);background:var(--ac-dim);animation:livePulse 2s infinite;}
@keyframes livePulse{0%{box-shadow:0 0 0 0 rgba(245,165,36,.35);}70%{box-shadow:0 0 0 6px rgba(245,165,36,0);}100%{box-shadow:0 0 0 0 rgba(245,165,36,0);}}
.info-row3{display:grid;grid-template-columns:minmax(0,1fr) minmax(0,1.5fr) minmax(0,1fr);gap:8px;align-items:center;padding:4px 12px 10px;background:var(--bg);border-bottom:1px solid var(--bd);}
.info-row3 .i3{font-size:9.5px;color:var(--tx2);white-space:nowrap;overflow:hidden;text-overflow:ellipsis;}
.i3.left{text-align:left;}
.i3.right{text-align:right;}
.i3.mid{display:flex;align-items:center;gap:6px;}
.i3.mid svg{width:13px;height:13px;stroke:var(--tx2);fill:none;stroke-width:2;stroke-linecap:round;flex-shrink:0;}
.i3.mid #brightVal{font-size:9.5px;color:var(--tx);width:22px;text-align:right;}
.body-layout{flex:1;min-height:0;display:flex;}
.subnav{width:86px;flex-shrink:0;background:var(--s1);border-right:1px solid var(--bd);display:flex;flex-direction:column;}
.subnav .nav-btns{flex:1;overflow-y:auto;}
.subnav button.nav-b{display:block;width:100%;padding:11px 2px;background:none;border:none;color:var(--tx3);font-size:9.5px;line-height:1.2;text-align:center;border-left:3px solid transparent;}
.subnav button.nav-b.active{color:var(--tx);border-left-color:var(--ac);background:var(--s2);}
.subnav .icon{display:flex;justify-content:center;margin-bottom:3px;}
.subnav .icon svg{width:17px;height:17px;stroke:currentColor;fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
.sub-save{flex-shrink:0;padding:10px 0 12px;border-top:1px solid var(--bd);display:flex;justify-content:center;}
.sub-save button{position:relative;width:44px;height:44px;border-radius:50%;background:linear-gradient(135deg,var(--ac),#c46a10);color:#0a0a0a;border:none;display:flex;align-items:center;justify-content:center;box-shadow:0 3px 12px rgba(245,165,36,.35);}
.sub-save button svg{width:18px;height:18px;stroke:currentColor;fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
.sub-save .dotd{position:absolute;top:1px;right:1px;width:9px;height:9px;border-radius:50%;background:var(--bad);border:2px solid var(--s1);display:none;}
.sub-save.dirty .dotd{display:block;}
.sub-save.dirty button{animation:savePulse 1.6s infinite;}
@keyframes savePulse{0%{box-shadow:0 0 0 0 rgba(245,165,36,.45);}70%{box-shadow:0 0 0 10px rgba(245,165,36,0);}100%{box-shadow:0 0 0 0 rgba(245,165,36,0);}}
.right-col{flex:1;min-width:0;display:flex;flex-direction:column;min-height:0;}
.pane-head{flex-shrink:0;background:var(--bg);padding:9px 12px;border-bottom:1px solid var(--bd);display:flex;flex-direction:column;gap:7px;}
.pane-scroll{flex:1;min-height:0;overflow-y:auto;-webkit-overflow-scrolling:touch;padding:12px;}
.pane-foot{flex-shrink:0;background:var(--s1);border-top:1px solid var(--bd);padding:12px 14px;max-height:45%;overflow-y:auto;}
.mode-row{font-size:10.5px;color:var(--tx2);}
.mode-row b{color:var(--tx);}
.search-box{width:100%;padding:8px 12px;background:var(--s1);border:1px solid var(--bd);border-radius:10px;color:var(--tx);font-size:12px;}
.fx-head-row{display:flex;align-items:center;gap:8px;}
.fx-chip{font-size:10.5px;font-weight:700;color:var(--ac);background:var(--ac-dim);padding:4px 10px;border-radius:14px;white-space:nowrap;max-width:42%;overflow:hidden;text-overflow:ellipsis;}
.fx-search{flex:1;min-width:0;padding:7px 10px;background:var(--s1);border:1px solid var(--bd);border-radius:10px;color:var(--tx);font-size:11px;}
.custom-grid{display:grid;grid-template-columns:1fr auto;gap:12px;align-items:stretch;margin-bottom:10px;}
.cg-wheel canvas{width:100%;max-width:250px;height:auto;border-radius:50%;touch-action:none;display:block;}
.cg-vsl{display:flex;gap:16px;align-items:stretch;justify-content:center;padding:6px 4px 16px;}
.vsl{position:relative;width:30px;}
.vsl input{position:absolute;top:50%;left:50%;width:190px;height:24px;transform:translate(-50%,-50%) rotate(-90deg);}
.vsl-l{position:absolute;bottom:-6px;left:50%;transform:translateX(-50%);font-size:9px;color:var(--tx3);font-weight:800;}
.quick-colors{display:flex;flex-wrap:wrap;gap:8px;margin-bottom:10px;}
.qc{width:28px;height:28px;border-radius:50%;border:2px solid rgba(255,255,255,.15);cursor:pointer;}
.crow{display:flex;gap:8px;flex-wrap:wrap;}
.crow-btn{width:42px;height:42px;border-radius:50%;border:2px solid var(--bd2);color:#fff;font-size:13px;font-weight:800;text-shadow:0 1px 2px rgba(0,0,0,.7);}
.crow-btn.active{border-color:var(--ac);box-shadow:0 0 0 2px var(--ac-dim);}
.palette-list{display:grid;grid-template-columns:1fr 1fr;gap:7px;}
.pal-row{position:relative;display:flex;align-items:center;gap:6px;background:var(--s1);border:1px solid var(--bd);border-radius:11px;padding:8px 6px 12px;overflow:hidden;}
.pal-row .pradio{width:12px;height:12px;border-radius:50%;border:2px solid var(--bd2);flex-shrink:0;}
.pal-row.active{border-color:var(--ac);}
.pal-row.active .pradio{border-color:var(--ac);background:var(--ac);}
.pal-row .pnum{font-size:8.5px;color:var(--tx3);width:14px;}
.pal-row .pname{flex:1;text-align:center;font-size:11px;font-weight:600;color:var(--tx);overflow:hidden;text-overflow:ellipsis;white-space:nowrap;}
.pal-row .pstrip{position:absolute;left:0;right:0;bottom:0;height:4px;}
.sec-title{font-size:10px;color:var(--tx2);text-transform:uppercase;letter-spacing:.5px;margin:12px 0 6px;font-weight:700;}
.fx-grid{display:flex;flex-direction:column;gap:6px;}
.fx-item{display:flex;align-items:center;min-height:48px;gap:8px;background:var(--s1);border:1px solid var(--bd);border-radius:12px;padding:8px 12px;font-size:13px;color:var(--tx);}
.fx-item.active{border-color:var(--ac);background:var(--ac-dim);}
.fx-item.unavailable{opacity:.38;cursor:not-allowed;border-style:dashed;}
.fx-num{color:var(--tx3);font-size:9px;margin-right:3px;}
.fx-name{font-weight:700;}
.param-row{margin-bottom:12px;}
.param-row input[type=range]{width:100%;}
.param-label{display:flex;justify-content:space-between;font-size:11px;color:var(--tx2);margin-bottom:5px;}
.foot-hint{font-size:11px;color:var(--tx3);text-align:center;padding:4px 0;}
.restricted-swatches{display:grid;grid-template-columns:repeat(4,1fr);gap:8px;margin-bottom:14px;}
.rswatch{aspect-ratio:1;border-radius:10px;border:3px solid transparent;}
.rswatch.active{border-color:#fff;}
.restricted-note{font-size:11px;color:var(--tx3);text-align:center;padding:10px 0 4px;}
.saved-row{display:flex;align-items:center;gap:10px;padding:9px 0;border-bottom:1px solid var(--bd);font-size:12px;}
.saved-row:last-child{border-bottom:none;}
.saved-row .sk{flex:1;color:var(--tx);}
.saved-row .st{color:var(--tx3);}
.saved-row .st.on{color:var(--ok);font-weight:700;}
.toast{position:fixed;left:50%;bottom:calc(var(--nav-h) + env(safe-area-inset-bottom) + 78px);transform:translateX(-50%);background:var(--s2);border:1px solid var(--bd2);color:var(--ok);padding:8px 16px;border-radius:20px;font-size:12px;font-weight:700;z-index:60;opacity:0;transition:opacity .25s;pointer-events:none;}
.moverlay{position:fixed;inset:0;background:rgba(0,0,0,.6);z-index:70;display:none;align-items:center;justify-content:center;}
.mbox{background:var(--s1);border:1px solid var(--bd2);border-radius:16px;padding:18px;width:82%;max-width:300px;}
.mtitle{font-size:14px;font-weight:800;color:var(--tx);margin-bottom:4px;}
.msub{font-size:12px;color:var(--tx2);margin-bottom:14px;}
.mbtns{display:flex;gap:8px;}
.mbtns .btn-sm{flex:1;}
.btn-sm{padding:9px 14px;border-radius:8px;background:var(--s2);border:1px solid var(--bd2);font-size:12px;font-weight:700;color:var(--tx);}
.btn-sm.primary{background:var(--ac);color:#0a0a0a;border-color:var(--ac);}
input[type=range]{-webkit-appearance:none;appearance:none;background:transparent;height:18px;--p:50%;}
input[type=range]::-webkit-slider-runnable-track{height:4px;border-radius:2px;background:linear-gradient(90deg,var(--ac) var(--p),var(--s3) var(--p));}
input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;width:14px;height:14px;border-radius:50%;background:#fff;margin-top:-5px;box-shadow:0 1px 4px rgba(0,0,0,.6);}
/* Welcoming/Sein memakai pola Riding: subtab horizontal, lalu daftar Single Effect satu kolom. */
.body-layout{flex-direction:column;}
.subnav{width:100%;height:52px;flex-direction:row;border-right:0;border-bottom:1px solid var(--bd);}
.subnav .nav-btns{display:flex;overflow-x:auto;min-width:0;}
.subnav button.nav-b{flex:1 0 auto;width:auto;min-width:88px;padding:8px 10px;border-left:0;border-bottom:3px solid transparent;font-size:11px;}
.subnav button.nav-b.active{border-left-color:transparent;border-bottom-color:var(--ac);}
.subnav .icon{display:none;}
.sub-save{flex:0 0 52px;padding:4px;border-top:0;border-left:1px solid var(--bd);}
.sub-save button{width:40px;height:40px;}
.right-col{flex:1;min-height:0;}
.fx-num{flex:none;width:26px;height:26px;display:flex;align-items:center;justify-content:center;border-radius:50%;background:var(--s2);font-size:11px;font-weight:700;}
.fx-name{flex:1;min-width:0;}
</style>
</head>
<body>
<div class="freeze">
<div id="mzH"></div>
<div class="tabbar" id="tabbar">
<button data-tab="welcoming">Welcoming</button>
<button data-tab="riding" class="active">Riding</button>
<button data-tab="sein">Sein</button>
<button data-tab="rem">Rem</button>
<button data-tab="hazard">Hazard</button>
</div>
<div class="control-panel" id="controlPanel">
  <div class="ctl-group ctl-sides">
    <div class="control-caption"><span>Pilih sisi lampu</span></div>
    <div class="side-row" id="sideRow" role="group" aria-label="Pilih sisi lampu yang dikontrol">
      <button class="side-btn" data-side="kiri" type="button" aria-label="Pilih lampu kiri"><span class="lamp" aria-hidden="true"><i class="on"></i><i></i></span><span class="side-label">Kiri</span></button>
      <button class="side-btn active" data-side="both" type="button" aria-label="Pilih kedua sisi"><span class="lamp" aria-hidden="true"><i class="on"></i><i class="on"></i></span><span class="side-label">Semua</span></button>
      <button class="side-btn" data-side="kanan" type="button" aria-label="Pilih lampu kanan"><span class="lamp" aria-hidden="true"><i></i><i class="on"></i></span><span class="side-label">Kanan</span></button>
    </div>
  </div>
  <div class="ctl-group ctl-tools">
    <div class="control-caption"><span>Atur semua sisi</span></div>
    <div class="tool-row" role="group" aria-label="Pengaturan output lampu">
      <button class="tool-btn swap-btn" id="swapSideBtn" data-action="swap" type="button" aria-pressed="false" title="Menukar pemetaan output fisik kiri dan kanan">
        <span class="tool-icon" aria-hidden="true"><svg viewBox="0 0 24 24"><path d="M4 8h15"/><path d="M15 4l4 4-4 4"/><path d="M20 16H5"/><path d="M9 12l-4 4 4 4"/></svg></span><span class="tool-label">Posisi</span><span class="tool-state">Normal</span>
      </button>
      <button class="tool-btn reverse-btn" id="reverseFxBtn" data-action="reverse" type="button" aria-pressed="false" title="Membalik arah gerak animasi LED pada semua sisi">
        <span class="tool-icon" aria-hidden="true"><svg viewBox="0 0 24 24"><path d="M3 12h18"/><path d="M7 8l-4 4 4 4"/><path d="M17 8l4 4-4 4"/></svg></span><span class="tool-label">Arah</span><span class="tool-state">Normal</span>
      </button>
    </div>
  </div>
</div>
<div class="info-row3">
<div class="i3 left" id="ctxInfo">Kiri: -</div>
<div class="i3 mid">
<svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="4"/><path d="M12 2v2M12 20v2M4.9 4.9l1.4 1.4M17.7 17.7l1.4 1.4M2 12h2M20 12h2M4.9 19.1l1.4-1.4M17.7 6.3l1.4-1.4"/></svg>
<input type="range" min="0" max="255" value="180" id="brightSlider">
<span id="brightVal">180</span>
</div>
<div class="i3 right" id="savedInfo">Kanan: -</div>
</div>
</div>
<div class="body-layout">
<div class="subnav" id="subnav">
<div class="nav-btns">
<button class="nav-b active" data-sub="efek"><span class="icon"><svg viewBox="0 0 24 24"><path d="M12 3l1.9 5.1L19 10l-5.1 1.9L12 17l-1.9-5.1L5 10l5.1-1.9z"/></svg></span>Pilih Efek</button>
<button class="nav-b" data-sub="template"><span class="icon"><svg viewBox="0 0 24 24"><path d="M12 2s6 6.5 6 11a6 6 0 0 1-12 0c0-4.5 6-11 6-11z"/></svg></span>Warna Template</button>
<button class="nav-b" data-sub="custom"><span class="icon"><svg viewBox="0 0 24 24"><path d="M4 21v-7"/><path d="M4 10V3"/><path d="M12 21v-9"/><path d="M12 8V3"/><path d="M20 21v-5"/><path d="M20 12V3"/><path d="M1 14h6"/><path d="M9 8h6"/><path d="M17 16h6"/></svg></span>Warna Custom</button>
<button class="nav-b" data-sub="simpan"><span class="icon"><svg viewBox="0 0 24 24"><path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"/><path d="M17 21v-8H7v8"/><path d="M7 3v5h8"/></svg></span>Riwayat Simpan</button>
</div>
<div class="sub-save" id="subSave">
<button id="btnSaveSub"><svg viewBox="0 0 24 24"><path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"/><path d="M17 21v-8H7v8"/><path d="M7 3v5h8"/></svg><span class="dotd"></span></button>
</div>
</div>
<div class="right-col">
<div class="pane-head" id="headWarna" style="display:none;">
<div class="mode-row">Mode aktif: <b id="modeAktif">Template</b></div>
</div>
<div class="pane-head" id="headEfek">
<div class="fx-head-row">
<span class="mode-row">Efek tersimpan:</span>
<span class="fx-chip" id="fxSavedName">-</span>
</div>
</div>
<div class="pane-head" id="headTemplate" style="display:none;">
<input class="search-box" id="searchBox" placeholder="Cari palette...">
</div>
<div class="pane-head" id="headSimpan" style="display:none;">
<div class="mode-row">Preset tersimpan <b>permanen di ESP32</b> per kombinasi mode × sisi.</div>
</div>
<div class="pane-scroll" id="scrollTemplate" style="display:none;">
<div class="palette-list" id="paletteGrid"></div>
</div>
<div class="pane-scroll" id="scrollCustom" style="display:none;">
<div class="custom-grid">
<div class="cg-wheel"><canvas id="colorWheel" width="220" height="220"></canvas></div>
<div class="cg-vsl">
<div class="vsl"><input type="range" min="0" max="100" value="100" id="valSlider"><span class="vsl-l">V</span></div>
<div class="vsl"><input type="range" min="2000" max="10000" step="50" value="6500" id="kelvinSlider"><span class="vsl-l">K</span></div>
</div>
</div>
<div class="quick-colors" id="quickColors"></div>
<div class="crow" id="colorRow"></div>
<div class="sec-title">Palette Custom (*)</div>
<div class="palette-list" id="customPalGrid"></div>
</div>
<div class="pane-scroll" id="scrollRestricted" style="display:none;">
<div class="restricted-swatches" id="restrictedGrid"></div>
<div class="cg-wheel" style="max-width:200px;margin:0 auto;"><canvas id="wheelR" width="220" height="220"></canvas></div>
<div class="restricted-note">Area abu-abu dibatasi otomatis sesuai regulasi lampu sinyal</div>
</div>
<div class="pane-scroll" id="scrollEfek">
<div class="fx-grid" id="fxGrid"></div>
</div>
<div class="pane-scroll" id="scrollSimpan" style="display:none;">
<div class="card"><div class="card-title">Status Simpan</div><div id="savedList"></div></div>
</div>
<div class="pane-foot" id="footEfek">
<div id="fxParams"></div>
<div class="foot-hint" id="footHint" style="display:none;">Efek ini tidak memiliki parameter tambahan.</div>
</div>
</div>
</div>
<div class="toast" id="toast"></div>
<div class="moverlay" id="saveModal">
<div class="mbox">
<div class="mtitle">Simpan perubahan?</div>
<div class="msub" id="modalCtx"></div>
<div class="mbtns">
<button class="btn-sm" id="mDiscard">Buang</button>
<button class="btn-sm" id="mCancel">Batal</button>
<button class="btn-sm primary" id="mSave">Simpan</button>
</div>
</div>
</div>
<div id="mzN"></div>
<script>
/* WLED hanya melayani SATU request /json pada satu waktu (selebihnya 503). Semua request /json diantrekan satu per satu + retry. */
let jqTail=Promise.resolve();
function jtry(url,opt,n){return fetch(url,opt).then(function(r){if(r.status===503&&n<6)return new Promise(function(w){setTimeout(w,250);}).then(function(){return jtry(url,opt,n+1);});return r;});}
function jfetch(url,opt){const task=jqTail.then(function(){return jtry(url,opt,0);});jqTail=task.catch(function(){});return task;}
let activeTab=new URLSearchParams(location.search).get('t')||localStorage.getItem('mzActiveTab')||'riding';
if(activeTab==='riding'||activeTab==='warna'){location.replace(activeTab==='warna'?'/riding?t=warna':'/riding');}
let activeSide='both',activeSub='efek',sideSwap=false,showReverse=false;
let dirty=false,pendingAction=null,restrictedName='';
let allFx=[],fxData=[],curList=[],palList=[],palNamesRaw=[],showNames=[],showDurations=[];
let cur={fx:0,pal:0,col:null,palName:'',isShow:false,showIndex:0,params:{sx:128,ix:128,c1:128,c2:128,c3:128,o1:0,o2:0,o3:0}};
let fxParamsCache={},fxPalCache={};
function paramsFor(i){if(!fxParamsCache[i]){const d=fxData.length?fxDefs(i):{};const g=function(k,df){return d[k]!=null?d[k]:df;};const p={sx:g('sx',128),ix:g('ix',128),c1:g('c1',128),c2:g('c2',128),c3:g('c3',128),o1:g('o1',0),o2:g('o2',0),o3:g('o3',0)};if(!fxData.length)return p;fxParamsCache[i]=p;}return fxParamsCache[i];}
cur.params=paramsFor(0);
let wHue=0,wSat=1,wVal=1,liveEnabled=true;
let segColors=[[255,165,0],[255,255,255],[255,26,0]],colorTarget=0,slotCount=3;
const SEG_K=0,SEG_L=1;
function mapSegId(logicalSide){const right=(logicalSide==='kanan');return (right!==sideSwap)?SEG_K:SEG_L;}
function segById(state,id){if(!state||!state.seg)return null;return state.seg.find(function(x){return x&&x.id===id;})||state.seg[id]||null;}
const HUE_RULES={sein:[25,60],hazard:[25,60],rem:[345,15]};
const RESTRICTED_COLORS={sein:[{n:'Amber',h:'FFA500'},{n:'Kuning Tua',h:'FF8C00'},{n:'Amber Muda',h:'FFB347'},{n:'Kuning',h:'FFD700'}],rem:[{n:'Merah',h:'FF0000'},{n:'Merah Tua',h:'CC0000'},{n:'Merah Terang',h:'FF3333'},{n:'Merah Gelap',h:'8B0000'}],hazard:[{n:'Amber',h:'FFA500'},{n:'Kuning Tua',h:'FF8C00'},{n:'Amber Muda',h:'FFB347'},{n:'Kuning',h:'FFD700'}]};
/* Filter default milik tab asli dipertahankan. Katalog khusus hanya berlaku di tab Welcoming. */
const FX_BLACKLIST=['frizzles','funky plank','game of life','geq','ghost rider','matrix','metaballs','tartan','polar lights','swirl','lissajous','rubix','invaders','maze','pulser','dna','plasma ball','hipnotic','black hole','cubes','fireworks','akemi','colored bursts','freqmatrix'];
const WELCOMING_NAMES=['Fade','Breathe','Wipe','Sweep','Chase','Chase Rainbow','Colorwaves','Rainbow','Rainbow Runner','Twinkle','Twinklefox','Sparkle','Glitter','Meteor','Meteor Smooth','Ripple','Ripple Rainbow','Pacifica','Aurora','Lake','Plasma','Colortwinkles','Sinelon','Sinelon Rainbow','Bpm','Sunrise','Phased','Dissolve','Noise Pal','Blends'];
const RESTRICTED_FX=['Solid','Blink','Strobe','Chase','Chase Flash','Wipe','Fade','Breathe','Sweep','Strobe Mega'];
const MZW_RE=/^(?:MZ\d\d|W\d\d) /; /* seluruh efek custom: MZ01..MZ05 + W01..W50 */
const HIDE_AUDIO_FX=true; /* perilaku asli Riding: sembunyikan efek audio-reaktif */
/* Whitelist 15 efek native ini hanya ditambahkan pada tab Welcoming (posisi 56..70). */
const MIZUMA_NATIVE_FX=[
  {match:'Bouncing Balls',label:'Bouncing Balls'},
  {match:'Chase Rainbow',label:'Chase Rainbow'},
  {match:'Colortwinkles',label:'Colortwinkles'},
  {match:'Dissolve',label:'Dissolve'},
  {match:'Drip',label:'Drip'},
  {match:'Fireworks 1D',fallback:'PS Fireworks 1D',label:'Fireworks 1D'},
  {match:'PacMan',label:'Pacman'},
  {match:'PS 1D Balance',label:'PS 1D Balance'},
  {match:'PS Spray 1D',label:'PS Spray 1D'},
  {match:'PS Starburst',fallback:'Starburst',label:'PS Starburst'},
  {match:'Shimmer',label:'Shimmer'},
  {match:'Sinelon Dual',label:'Sinelon Dual'},
  {match:'Sweep',label:'Sweep'},
  {match:'Tetrix',label:'Tetrix'},
  {match:'Wavesins',label:'Wavesins'}
];
const QUICK_COLORS=['FF0000','FFA500','FFD200','FFFFFF','000000','0000FF','00FF00'];
/* Pairing efek->palette default (meniru perilaku UI WLED asli; bisa diperluas) */
const FX_PAL_DEFAULTS={'Fire 2012':'Fire','Fireworks 1D':'Rainbow','Fireworks':'Rainbow','Noise 1':'Pastel','Noise':'Pastel','Pride':'Pride','Aurora':'Aurora','Pacifica':'Pacifica'};
function palIdByName(n){return palNamesRaw.findIndex(function(x){return x===n;});}
/* PAL_GRADS (gradient karangan manual) dihapus: thumbnail kini 100% dari /json/palx */
function post(o){jfetch('/json/state',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(o)}).catch(function(){});}
function debounce(fn,ms){let t;return function(){const a=arguments;clearTimeout(t);t=setTimeout(function(){fn.apply(null,a);},ms);};}
function segIds(){if(activeSide==='kanan')return[mapSegId('kanan')];if(activeSide==='kiri')return[mapSegId('kiri')];return[mapSegId('kanan'),mapSegId('kiri')];}
function isRestrictedTab(){return activeTab==='sein'||activeTab==='rem'||activeTab==='hazard';}
function cap(s){return s.charAt(0).toUpperCase()+s.slice(1);}
function markDirty(){dirty=true;document.getElementById('subSave').classList.add('dirty');}
function clearDirty(){dirty=false;document.getElementById('subSave').classList.remove('dirty');}
function toast(msg){const t=document.getElementById('toast');t.textContent=msg;t.style.opacity='1';setTimeout(function(){t.style.opacity='0';},1800);}
function paintRange(el){const min=+el.min||0,max=+el.max||255;el.style.setProperty('--p',(((+el.value-min)/(max-min))*100)+'%');}
document.addEventListener('input',function(e){if(e.target.matches&&e.target.matches('input[type=range]')&&!e.target.classList.contains('kelvin'))paintRange(e.target);});
/* Peek dihapus. Hanya pencatat geometri segmen yang tersisa. */
function captureSegs(j){}
/* ===== COLOR MATH ===== */
function hsvToRgb(h,s,v){const c=v*s,x=c*(1-Math.abs((h/60)%2-1)),m=v-c;let r,g,b;if(h<60){r=c;g=x;b=0;}else if(h<120){r=x;g=c;b=0;}else if(h<180){r=0;g=c;b=x;}else if(h<240){r=0;g=x;b=c;}else if(h<300){r=x;g=0;b=c;}else{r=c;g=0;b=x;}return[Math.round((r+m)*255),Math.round((g+m)*255),Math.round((b+m)*255)];}
function rgb2hsv(r,g,b){r/=255;g/=255;b/=255;const mx=Math.max(r,g,b),mn=Math.min(r,g,b),d=mx-mn;let h=0;if(d){if(mx===r)h=((g-b)/d)%6;else if(mx===g)h=(b-r)/d+2;else h=(r-g)/d+4;h*=60;if(h<0)h+=360;}return[h,(mx?d/mx:0),mx];}
function hueOk(h,r){if(!r)return true;if(r[0]<=r[1])return h>=r[0]&&h<=r[1];return h>=r[0]||h<=r[1];}
function clampHue(h,r){if(!r)return h;if(r[0]<=r[1])return h<r[0]?r[0]:(h>r[1]?r[1]:h);if(h>r[1]&&h<r[0])return (h-r[1])<=(r[0]-h)?r[1]:r[0];return h;}
const wheelImgs={};
function buildWheelImg(cv,rule){const ctx=cv.getContext('2d'),wr=cv.width/2,img=ctx.createImageData(cv.width,cv.height);for(let y=0;y<cv.height;y++){for(let x=0;x<cv.width;x++){const dx=x-wr,dy=y-wr,d=Math.sqrt(dx*dx+dy*dy),idx=(y*cv.width+x)*4;if(d<=wr){const raw=Math.atan2(dy,dx)*180/Math.PI+180,hue=clampHue(raw,rule),sat=d/wr;if(rule&&!hueOk(raw,rule)){img.data[idx]=40;img.data[idx+1]=40;img.data[idx+2]=40;}else{const rgb=hsvToRgb(hue,sat,1);img.data[idx]=rgb[0];img.data[idx+1]=rgb[1];img.data[idx+2]=rgb[2];}img.data[idx+3]=255;}else{img.data[idx+3]=0;}}}wheelImgs[cv.id]={img:img};ctx.putImageData(img,0,0);}
function paintWheel(cv,hue,sat){const e=wheelImgs[cv.id];if(!e)return;const ctx=cv.getContext('2d');ctx.putImageData(e.img,0,0);const wr=cv.width/2,th=(hue-180)*Math.PI/180,r=sat*(wr-2),x=wr+Math.cos(th)*r,y=wr+Math.sin(th)*r;ctx.beginPath();ctx.arc(x,y,6,0,6.2832);ctx.strokeStyle='#fff';ctx.lineWidth=2;ctx.stroke();ctx.beginPath();ctx.arc(x,y,8,0,6.2832);ctx.strokeStyle='rgba(0,0,0,.65)';ctx.lineWidth=1.5;ctx.stroke();}
const wheel=document.getElementById('colorWheel');buildWheelImg(wheel,null);paintWheel(wheel,wHue,wSat);
function wheelPick(e){const rect=wheel.getBoundingClientRect(),wr=wheel.width/2,x=(e.clientX-rect.left)*(wheel.width/rect.width),y=(e.clientY-rect.top)*(wheel.height/rect.height),dx=x-wr,dy=y-wr,d=Math.sqrt(dx*dx+dy*dy);if(d>wr)return;wHue=Math.atan2(dy,dx)*180/Math.PI+180;wSat=Math.min(1,d/wr);paintWheel(wheel,wHue,wSat);const rgb=hsvToRgb(wHue,wSat,wVal);setColor(rgb[0],rgb[1],rgb[2],false);}
let dragging=false;
wheel.addEventListener('pointerdown',function(e){dragging=true;wheel.setPointerCapture(e.pointerId);wheelPick(e);});
wheel.addEventListener('pointermove',function(e){if(dragging)wheelPick(e);});
wheel.addEventListener('pointerup',function(){dragging=false;});
document.getElementById('valSlider').addEventListener('input',function(e){wVal=(+e.target.value)/100;const rgb=hsvToRgb(wHue,wSat,wVal);setColor(rgb[0],rgb[1],rgb[2],false);});
function kelvinToRgb(k){k/=100;let r,g,b;if(k<=66){r=255;g=99.47*Math.log(k)-161.12;}else{r=329.7*Math.pow(k-60,-0.133);g=288.12*Math.pow(k-60,-0.0755);}if(k>=66)b=255;else if(k<=19)b=0;else b=138.52*Math.log(k-10)-305.04;const cl=function(v){return Math.max(0,Math.min(255,Math.round(v)));};return[cl(r),cl(g),cl(b)];}
document.getElementById('kelvinSlider').addEventListener('input',function(e){document.getElementById('kelvinVal')?0:0;const rgb=kelvinToRgb(+e.target.value);setColor(rgb[0],rgb[1],rgb[2],false);});
/* ===== STATE CONTROL ===== */
function palNameById(id){const e=palList.find(function(x){return x.i===id;});return e?e.n:(palNamesRaw[id]||'');}
function isStarPal(){const n=palNameById(cur.pal);return n.charAt(0)==='*';}
function normalizeRestrictedState(forceDefault){if(!isRestrictedTab())return;colorTarget=0;cur.pal=0;cur.palName=palNamesRaw[0]||'Default';const colors=RESTRICTED_COLORS[activeTab]||RESTRICTED_COLORS.sein;const defNum=parseInt(colors[0].h,16);const defRgb=[(defNum>>16)&255,(defNum>>8)&255,defNum&255];const current=segColors[0]||[0,0,0];const hsv=rgb2hsv(current[0],current[1],current[2]);if(forceDefault||!hueOk(hsv[0],HUE_RULES[activeTab])||(current[0]===0&&current[1]===0&&current[2]===0)){segColors[0]=defRgb;restrictedName=colors[0].n;}segColors[1]=[0,0,0];segColors[2]=[0,0,0];}
const postColorD=throttleLatest(post,120);
function sendColor(r,g,b,silent){if(isRestrictedTab()){colorTarget=0;cur.pal=0;segColors[0]=[r,g,b];segColors[1]=[0,0,0];segColors[2]=[0,0,0];}else{segColors[colorTarget]=[r,g,b];}const cols=[segColors[0]||[255,255,255],segColors[1]||[0,0,0],segColors[2]||[0,0,0]];const segs=segIds().map(function(id){return{id:id,col:cols,pal:isRestrictedTab()?0:cur.pal};});postColorD({seg:segs});cur.col=[r,g,b];if(!silent)markDirty();renderColorRow();updateSideInfo();}
function sendPalette(i){if(isRestrictedTab())return;const segs=segIds().map(function(id){return{id:id,pal:i};});if(activeTab==='welcoming'){fetch('/mizuma/show/palette?pal='+i+'&side='+encodeURIComponent(activeSide),{method:'POST'}).then(function(r){if(!r.ok)post({seg:segs});}).catch(function(){post({seg:segs});});}else post({seg:segs});cur.pal=i;fxPalCache[cur.fx]=i;markDirty();renderColorRow();updateSideInfo();}
function renderShowInfo(){const box=document.getElementById('fxParams');box.innerHTML='';const hint=document.getElementById('footHint');const ms=Number(showDurations[cur.showIndex]||0);const sec=ms>0?(ms/1000).toFixed(2).replace('.',',')+' detik':'mengikuti seluruh tahap animasi';hint.textContent='Durasi: '+sec+'. Setiap animasi memiliki waktu berbeda dan berjalan sampai seluruh tahap selesai.';hint.style.display='block';}
function sendShow(index){if(activeTab!=='welcoming'||index<0||index>=showNames.length)return;cur.isShow=true;cur.showIndex=index;cur.fx=0;document.getElementById('fxSavedName').textContent=showNames[index];fetch('/mizuma/show?index='+index+'&side='+encodeURIComponent(activeSide),{method:'POST'}).then(function(r){if(!r.ok)throw new Error('HTTP '+r.status);return r.json();}).then(function(d){if(!d.ok)throw new Error('show rejected');markDirty();renderShowInfo();updateCtx();updateSideInfo();}).catch(function(){toast('Gagal menjalankan animasi Mizuma');});}
function sendEffect(i){cur.isShow=false;cur.showIndex=0;if(isRestrictedTab())normalizeRestrictedState(false);cur.fx=i;cur.params=paramsFor(i);let palToSend=cur.pal;if(!isRestrictedTab()){let pi=-1;if(fxPalCache[i]!=null)pi=fxPalCache[i];else{const df=fxDefs(i);if(df.pal!=null)pi=df.pal;else{const nm=allFx[i]||'';if(FX_PAL_DEFAULTS[nm])pi=palIdByName(FX_PAL_DEFAULTS[nm]);}}if(pi>=0&&palNameById(pi)){cur.pal=pi;cur.palName=palNameById(pi);palToSend=pi;document.querySelectorAll('.pal-row').forEach(function(el){el.classList.toggle('active',el.dataset.palname===cur.palName);});}}const cols=[segColors[0]||[255,255,255],isRestrictedTab()?[0,0,0]:(segColors[1]||[0,0,0]),isRestrictedTab()?[0,0,0]:(segColors[2]||[0,0,0])];const segs=segIds().map(function(id){const o={id:id,fx:i,rev:showReverse,sx:cur.params.sx,ix:cur.params.ix,c1:cur.params.c1,c2:cur.params.c2,c3:cur.params.c3,o1:cur.params.o1,o2:cur.params.o2,o3:cur.params.o3};if(isRestrictedTab()){o.pal=0;o.col=cols;}else{o.pal=palToSend;}return o;});const applyFx=function(){post({seg:segs});};if(activeTab==='welcoming'){fetch('/mizuma/show/stop?side='+encodeURIComponent(activeSide),{method:'POST'}).then(applyFx).catch(applyFx);}else applyFx();if(isRestrictedTab())cur.pal=0;markDirty();renderColorRow();renderParams(i);updateSideInfo();}
const sendParamD=throttleLatest(function(k,v){const segs=segIds().map(function(id){const o={id:id};o[k]=v;return o;});post({seg:segs});cur.params[k]=v;markDirty();},120);
const sendBriD=debounce(function(v){post({bri:v});fetch('/mizuma/bri?v='+v,{method:'POST'}).catch(function(){});},80);
/* ===== SIDE INFO (kiri/kanan dari state device) ===== */
let usiT=null;
function updateSideInfo(){if(usiT)return;usiT=setTimeout(function(){usiT=null;updateSideInfoNow();},900);}
function updateSideInfoNow(){jfetch('/json/state').then(function(r){return r.json();}).then(function(j){captureSegs(j);if(!j.seg)return;const nm=function(s){if(!s)return '-';if(s.on===false)return 'Off';return (allFx[s.fx]||'Efek')+' - '+(palNameById(s.pal)||'Palette');};document.getElementById('ctxInfo').textContent=nm(segById(j,mapSegId('kiri')));document.getElementById('savedInfo').textContent=nm(segById(j,mapSegId('kanan')));}).catch(function(){});}
/* ===== FX / BG / CS ===== */
function isPSFx(){const n=allFx[cur.fx]||'';return n.indexOf('PS ')===0;}
function gstr(a){return 'rgb('+a[0]+','+a[1]+','+a[2]+')';}
function renderColorRow(){const box=document.getElementById('colorRow');if(!box)return;box.innerHTML='';const star=isStarPal(),ps=isPSFx();const mk=function(label,idx){const b=document.createElement('button');b.className='crow-btn'+(colorTarget===idx?' active':'');b.textContent=label;b.style.background=gstr(segColors[idx]||[128,128,128]);b.addEventListener('click',function(){colorTarget=idx;renderColorRow();});box.appendChild(b);};if(!(ps&&!star))mk('Fx',0);mk('Bg',1);if(star&&slotCount>=3)mk('3',2);updateStarStrips();}
(function(){const box=document.getElementById('quickColors');QUICK_COLORS.forEach(function(h){const d=document.createElement('div');d.className='qc';d.style.background='#'+h;d.addEventListener('click',function(){const n=parseInt(h,16);setColor((n>>16)&255,(n>>8)&255,n&255,false);});box.appendChild(d);});const r=document.createElement('div');r.className='qc';r.style.background='conic-gradient(#f00,#ff0,#0f0,#0ff,#00f,#f0f,#f00)';r.style.position='relative';r.innerHTML='<span style="position:absolute;inset:5px;border-radius:50%;background:var(--bg);color:var(--tx);display:flex;align-items:center;justify-content:center;font-size:9px;font-weight:800;">R</span>';r.addEventListener('click',function(){setColor(Math.random()*256|0,Math.random()*256|0,Math.random()*256|0,false);});box.appendChild(r);})();
function setColor(r,g,b,silent){const hv=rgb2hsv(r,g,b);wHue=hv[0];wSat=hv[1];wVal=hv[2];document.getElementById('valSlider').value=Math.round(wVal*100);paintRange(document.getElementById('valSlider'));paintWheel(wheel,wHue,wSat);sendColor(r,g,b,silent);updateModeAktif();}
/* ===== RESTRICTED ===== */
function buildRestricted(){const grid=document.getElementById('restrictedGrid');grid.innerHTML='';const colors=RESTRICTED_COLORS[activeTab]||RESTRICTED_COLORS.sein;colors.forEach(function(c,i){const d=document.createElement('div');d.className='rswatch'+(i===0?' active':'');d.style.background='#'+c.h;d.addEventListener('click',function(){document.querySelectorAll('.rswatch').forEach(function(x){x.classList.remove('active');});d.classList.add('active');restrictedName=c.n;const n=parseInt(c.h,16);sendColor((n>>16)&255,(n>>8)&255,n&255,false);updateModeAktif();});grid.appendChild(d);});buildWheelImg(document.getElementById('wheelR'),HUE_RULES[activeTab]);restrictedName=colors[0].n;}
document.getElementById('wheelR').addEventListener('pointerdown',function(e){const cv=document.getElementById('wheelR'),rect=cv.getBoundingClientRect(),wr=cv.width/2,x=(e.clientX-rect.left)*(cv.width/rect.width),y=(e.clientY-rect.top)*(cv.height/rect.height),dx=x-wr,dy=y-wr;if(Math.sqrt(dx*dx+dy*dy)>wr)return;const h=clampHue(Math.atan2(dy,dx)*180/Math.PI+180,HUE_RULES[activeTab]),sat=Math.min(1,Math.sqrt(dx*dx+dy*dy)/wr),rgb=hsvToRgb(h,sat,1);restrictedName='Wheel (dibatasi)';setColor(rgb[0],rgb[1],rgb[2],false);updateModeAktif();});
/* ===== PALETTES ===== */
const paletteGrid=document.getElementById('paletteGrid'),customPalGrid=document.getElementById('customPalGrid');
/* Thumbnail palette = data /json/palx (sama dengan UI stock). Entri: [pos,r,g,b] | 'r' (acak) | 'c1'..'c3' (warna segmen) */
let palRaw={},palSlot={},palRand={},palTry=0,palXLoaded=false;
function palGrad(id){const d=palRaw[id];if(!d||!d.length)return null;const st=[];for(let j=0;j<d.length;j++){const e=d[j];let c,idx=false;if(Array.isArray(e)){idx=e[0]/255*100;c=[e[1],e[2],e[3]];}else if(e==='r'){const key=id+'_'+j;c=palRand[key]||(palRand[key]=[Math.random()*255|0,Math.random()*255|0,Math.random()*255|0]);}else{c=segColors[(parseInt(String(e).charAt(1),10)||1)-1]||[0,0,0];}const rgb='rgb('+c[0]+','+c[1]+','+c[2]+')';st.push(idx===false?rgb:rgb+' '+idx+'%');}if(st.length===1)st.push(st[0]);return 'linear-gradient(90deg,'+st.join(',')+')';}
function refreshPalStrips(all){document.querySelectorAll('.pal-row').forEach(function(row){const id=parseInt(row.dataset.pid,10);if(isNaN(id))return;if(!all&&!palSlot[id])return;const g=palGrad(id);if(g)row.querySelector('.pstrip').style.background=g;});}
function updateStarStrips(){refreshPalStrips(false);}
function ensurePalX(){if(palXLoaded)return;palXLoaded=true;if(!Object.keys(palRaw).length)loadPalX(0);}
function loadPalX(page){page=page||0;jfetch('/json/palx?page='+page).then(function(r){return r.ok?r.json():Promise.reject();}).then(function(d){const map=d&&d.p;if(!map||typeof map!=='object')throw 0;Object.keys(map).forEach(function(k){const v=map[k];if(Array.isArray(v)){const id=parseInt(k,10);palRaw[id]=v;palSlot[id]=v.some(function(x){return typeof x==='string'&&x!=='r';});}});palTry=0;refreshPalStrips(true);if(typeof d.m==='number'&&page<d.m)loadPalX(page+1);else{palXLoaded=true;palCacheSave();if(palInfo.cpal)renderPaletteRows(document.getElementById('searchBox').value.toLowerCase());}}).catch(function(){if(palTry++<4)setTimeout(function(){loadPalX(page);},1500);else palXLoaded=false;});}
function makePalRow(e,grid,numbered,seq){const row=document.createElement('div');row.className='pal-row'+(cur.palName===e.n?' active':'');row.dataset.name=e.n.toLowerCase();row.dataset.palname=e.n;row.dataset.pid=e.i;const grad=palGrad(e.i)||'var(--s3)';row.innerHTML='<span class="pradio"></span><span class="pnum">'+(numbered?(seq+1):'')+'</span><span class="pname">'+e.n+'</span><span class="pstrip" style="background:'+grad+'"></span>';row.addEventListener('click',function(){cur.palName=e.n;document.querySelectorAll('.pal-row').forEach(function(x){x.classList.remove('active');});row.classList.add('active');sendPalette(e.i);updateModeAktif();});grid.appendChild(row);}
function renderPaletteRows(q){paletteGrid.innerHTML='';customPalGrid.innerHTML='';let t=0;palList.forEach(function(e){if(e.custom&&isPlaceholderPal(e.i))return;if(e.n.charAt(0)==='*'){makePalRow(e,customPalGrid,false,0);return;}if(q&&e.n.toLowerCase().indexOf(q)<0)return;makePalRow(e,paletteGrid,true,t);t++;});updateStarStrips();}
let palInfo={cpal:0,um:[],major:0},palSig='';
function palIdBase(){return palInfo.major>=16?200:255;}
function isPlaceholderPal(id){const d=palRaw[id];if(!d||!d.length||!Array.isArray(d[0]))return false;const f=d[0];if(!(f[1]===f[2]&&f[2]===f[3]))return false;return d.every(function(e){return Array.isArray(e)&&e[1]===f[1]&&e[2]===f[2]&&e[3]===f[3];});}
function palCacheLoad(sig){try{const c=JSON.parse(localStorage.getItem('mzPalCache')||'null');if(c&&c.sig===sig&&c.raw){palRaw=c.raw;Object.keys(palRaw).forEach(function(k){palSlot[k]=palRaw[k].some(function(x){return typeof x==='string'&&x!=='r';});});}}catch(e){}}
function palCacheSave(){try{localStorage.setItem('mzPalCache',JSON.stringify({sig:palSig,raw:palRaw}));}catch(e){}}
function loadPalettes(force){Promise.all([jfetch('/json/pal').then(function(r){return r.json();}),jfetch('/json/info').then(function(r){return r.json();})]).then(function(v){const names=v[0],inf=v[1]||{};palInfo.major=parseInt(String(inf.ver||'0'),10)||0;if(inf.umpalnames!==undefined||inf.cpalmax!==undefined)palInfo.major=Math.max(palInfo.major,16);palInfo.cpal=inf.cpalcount||0;palInfo.um=Array.isArray(inf.umpalnames)?inf.umpalnames:[];const sig=names.length+'|'+palInfo.cpal+'|'+palInfo.um.length+'|'+palInfo.major;if(!force&&sig===palSig)return;const first=(palSig==='');palSig=sig;if(first)palCacheLoad(sig);palNamesRaw=names;const arr=[];names.forEach(function(n,i){if(n!=='r')arr.push({n:n,i:i});});arr.sort(function(a,b){if(a.i===0)return -1;if(b.i===0)return 1;const sa=a.n.charAt(0)==='*',sb=b.n.charAt(0)==='*';if(sa&&sb)return a.i-b.i;return a.n.localeCompare(b.n);});for(let k=0;k<palInfo.cpal;k++)arr.push({n:'~ Custom '+(k+1)+' ~',i:palIdBase()-k,custom:true});if(palInfo.major>=16)palInfo.um.forEach(function(n,k){arr.push({n:n||('Usermod '+(k+1)),i:255-k,custom:true});});palList=arr;renderPaletteRows(document.getElementById('searchBox').value.toLowerCase());renderColorRow();palTry=0;updateSideInfo();}).catch(function(){setTimeout(function(){loadPalettes(force);},1500);});}
loadPalettes(true);
document.getElementById('searchBox').addEventListener('input',function(e){renderPaletteRows(e.target.value.toLowerCase());});
/* ===== EFFECTS ===== */
/* metadata efek WLED: "slider;warna;palette;flags;default" (sama seperti UI stock) */
function fxPart(i,k){const m=(fxData[i]||'').split(';');return m.length>k?m[k]:null;}
function fxUsesPal(i){const p=fxPart(i,2);return p===null?true:p!=='';}
function fxDefs(i){const d={},s=fxPart(i,4);if(s)s.split(',').forEach(function(kv){const p=kv.split('=');if(p.length===2){const v=parseInt(p[1],10);if(!isNaN(v))d[p[0].trim()]=v;}});return d;}
function isRsvd(n){const t=(n||'').toLowerCase();return t===''||t==='rsvd'||t==='-';}
function fxFlags(i){return fxPart(i,3)||'';}
function fxOk(i,name){if(isRsvd(name)||MZW_RE.test(name))return false;const t=name.toLowerCase();if(name.indexOf('2D')===0||FX_BLACKLIST.indexOf(t)>=0)return false;const f=fxFlags(i);if(f.indexOf('2')>=0&&f.indexOf('1')<0&&f.indexOf('0')<0)return false;if(HIDE_AUDIO_FX&&(f.indexOf('v')>=0||f.indexOf('f')>=0))return false;return true;}
function lookupExact(n){const t=n.toLowerCase();return allFx.findIndex(function(x){return x.toLowerCase()===t;});}
function lookup(n){const i=lookupExact(n);if(i>=0)return i;const t=n.toLowerCase();return allFx.findIndex(function(x){return x.toLowerCase().includes(t);});}
function mzwOrder(name){const m=/^(MZ|W)(\d\d) /.exec(name);if(!m)return 999;return m[1]==='MZ'?parseInt(m[2],10):(5+parseInt(m[2],10));}
/* Katalog 1..70 hanya untuk Welcoming. idx selalu ID asli WLED, bukan nomor urut tampilan. */
function listForTab(){
  if(activeTab==='welcoming'){
    const custom=showNames.map(function(n,i){return{name:n,isShow:true,showIndex:i};});
    const native=[];
    MIZUMA_NATIVE_FX.forEach(function(spec){
      let idx=lookupExact(spec.match);
      if(idx<0&&spec.fallback)idx=lookupExact(spec.fallback);
      if(idx>=0)native.push({name:spec.label,idx:idx,isShow:false});
      else { native.push({name:spec.label+' (tidak tersedia)',idx:-1,isShow:false,unavailable:true}); console.warn('[Mizuma] Efek native tidak ditemukan pada firmware:',spec.match); }
    });
    return custom.concat(native);
  }
  /* Riding, Sein, Rem, dan Hazard kembali memakai penyaringan bawaan versi sebelumnya. */
  if(activeTab==='riding')return allFx.map(function(n,i){return{name:n,idx:i};}).filter(function(e){return fxOk(e.idx,e.name);});
  return RESTRICTED_FX.map(lookupExact).filter(function(i){return i>=0;}).map(function(i){return{name:allFx[i],idx:i};});
}
function parseFxData(meta){const parts=(meta||'').split(';'),labels=(parts[0]||'').split(','),def=['Speed','Intensity','Custom 1','Custom 2','Custom 3'],keys=['sx','ix','c1','c2','c3'],sl=[];for(let i=0;i<5;i++){let l=labels[i];if(l===undefined||l==='')continue;if(l==='!')l=def[i];sl.push({key:keys[i],label:l});}const tg=[];for(let i=0;i<3;i++){let l=labels[5+i];if(l===undefined||l==='')continue;if(l==='!')l='Opsi '+(i+1);tg.push({key:['o1','o2','o3'][i],label:l});}return{sl:sl,tg:tg};}
function renderParams(idx){if(cur.isShow){renderShowInfo();return;}const box=document.getElementById('fxParams');box.innerHTML='';const hint=document.getElementById('footHint');hint.textContent='Efek ini tidak memiliki parameter tambahan.';const p=parseFxData(fxData[idx]||'');const iv=function(k){return cur.params[k]!=null?cur.params[k]:128;};p.sl.forEach(function(s){const v=iv(s.key);const row=document.createElement('div');row.className='param-row';row.innerHTML='<div class="param-label"><span>'+s.label+'</span><span>'+v+'</span></div><input type="range" min="0" max="255" value="'+v+'">';const inp=row.querySelector('input');inp.addEventListener('input',function(e){row.querySelectorAll('.param-label span')[1].textContent=e.target.value;sendParamD(s.key,+e.target.value);});box.appendChild(row);paintRange(inp);});p.tg.forEach(function(s){const on0=cur.params[s.key]===1;const row=document.createElement('div');row.className='param-row';row.style.cssText='display:flex;justify-content:space-between;align-items:center;';row.innerHTML='<span style="font-size:12px;color:var(--tx2);">'+s.label+'</span><button class="btn-sm'+(on0?' primary':'')+'" data-on="'+(on0?'1':'0')+'">'+(on0?'On':'Off')+'</button>';const b=row.querySelector('button');b.addEventListener('click',function(){const on=b.dataset.on==='1';b.dataset.on=on?'0':'1';b.textContent=on?'Off':'On';b.className=on?'btn-sm':'btn-sm primary';sendParamD(s.key,on?0:1);});box.appendChild(row);});document.getElementById('footHint').style.display=(p.sl.length===0&&p.tg.length===0)?'block':'none';}
function buildEffects(){const grid=document.getElementById('fxGrid');grid.innerHTML='';if(!allFx.length){grid.innerHTML='<div class="foot-hint">Memuat daftar efek...</div>';return;}if(activeTab==='welcoming'&&!showNames.length){grid.innerHTML='<div class="foot-hint">Katalog animasi Mizuma belum termuat. Muat ulang halaman.</div>';return;}curList=listForTab();if(activeTab!=='welcoming'){curList=curList.slice().sort(function(a,b){if(a.idx===0)return -1;if(b.idx===0)return 1;const wa=MZW_RE.test(a.name),wb=MZW_RE.test(b.name);if(wa!==wb)return wa?-1:1;return a.name.localeCompare(b.name);});} /* Welcoming: 55 show + 15 FX native; tab lain tetap memakai filter lama. */curList.forEach(function(e,i){const el=document.createElement('div');el.className='fx-item'+(e.unavailable?' unavailable':'');if(e.isShow)el.dataset.showIndex=e.showIndex;else if(e.idx>=0)el.dataset.fxIndex=e.idx;el.innerHTML='<span class="fx-num">'+(i+1)+'.</span><span class="fx-name">'+e.name+'</span>';if(!e.unavailable)el.addEventListener('click',function(){document.querySelectorAll('.fx-item').forEach(function(x){x.classList.remove('active');});el.classList.add('active');document.getElementById('fxSavedName').textContent=e.name;if(e.isShow){sendShow(e.showIndex);renderShowInfo();}else{sendEffect(e.idx);renderParams(e.idx);}updateCtx();});else el.title='Efek ini tidak tersedia pada build WLED saat ini';grid.appendChild(el);});updateCtx();}
function loadFxData(cb){Promise.all([jfetch('/json/eff').then(function(r){return r.json();}),jfetch('/json/fxdata').then(function(r){return r.json();}),fetch('/mizuma/shows').then(function(r){if(!r.ok)throw new Error('shows');return r.json();}),fetch('/mizuma/show-durations').then(function(r){if(!r.ok)throw new Error('show durations');return r.json();})]).then(function(v){allFx=v[0];fxData=v[1];showNames=v[2]||[];showDurations=v[3]||[];cb(true);}).catch(function(){cb(false);});}
function captureParams(s){if(s.sx!=null)cur.params.sx=s.sx;if(s.ix!=null)cur.params.ix=s.ix;if(s.c1!=null)cur.params.c1=s.c1;if(s.c2!=null)cur.params.c2=s.c2;if(s.c3!=null)cur.params.c3=s.c3;if(s.o1!=null)cur.params.o1=s.o1?1:0;if(s.o2!=null)cur.params.o2=s.o2?1:0;if(s.o3!=null)cur.params.o3=s.o3?1:0;}
/* ===== LABELS & CONTEXT ===== */
function sideLabel(){return{kanan:'Kanan',kiri:'Kiri',both:'Semua'}[activeSide];}
function tabLabel(){return{welcoming:'Welcoming',riding:'Riding',sein:'Sein',rem:'Rem',hazard:'Hazard'}[activeTab];}
function subLabel(){return{template:'Warna Template',custom:'Warna Custom',efek:'Pilih Efek',simpan:'Riwayat Simpan'}[activeSub];}
function updateCtx(){document.getElementById('fxSavedName')?0:0;}
function updateModeAktif(){if(isRestrictedTab()){document.getElementById('modeAktif').textContent='Terbatas \u2014 '+restrictedName+' ('+tabLabel()+')';return;}if(activeTab==='welcoming'&&cur.isShow){document.getElementById('modeAktif').textContent='Animasi Mizuma \u2014 '+(showNames[cur.showIndex]||('MZ'+String(cur.showIndex+1).padStart(2,'0')));return;}if(activeSub==='custom'){document.getElementById('modeAktif').textContent='Custom \u2014 warna manual';}else{document.getElementById('modeAktif').textContent='Template \u2014 '+(cur.palName||'-')+(fxUsesPal(cur.fx)?'':' (efek ini tidak memakai palette)');}}
function refreshSavedInfo(){updateSideInfo();}
/* ===== SAVE ===== */
function doSave(){if(isRestrictedTab())normalizeRestrictedState(false);const sides=activeSide==='both'?['Kanan','Kiri']:[cap(activeSide)];const c0=segColors[0]||[255,255,255],c1=isRestrictedTab()?[0,0,0]:(segColors[1]||[0,0,0]),c2=isRestrictedTab()?[0,0,0]:(segColors[2]||[0,0,0]);const params=new URLSearchParams({fx:cur.fx,pal:isRestrictedTab()?0:cur.pal,isShow:(activeTab==='welcoming'&&cur.isShow)?1:0,showIndex:(activeTab==='welcoming'&&cur.isShow)?cur.showIndex:0,r:c0[0],g:c0[1],b:c0[2],r1:c1[0],g1:c1[1],b1:c1[2],r2:c2[0],g2:c2[1],b2:c2[2],sx:cur.params.sx!=null?cur.params.sx:128,ix:cur.params.ix!=null?cur.params.ix:128,c1:cur.params.c1!=null?cur.params.c1:128,c2:cur.params.c2!=null?cur.params.c2:128,c3:cur.params.c3!=null?cur.params.c3:128,o1:cur.params.o1!=null?cur.params.o1:0,o2:cur.params.o2!=null?cur.params.o2:0,o3:cur.params.o3!=null?cur.params.o3:0});sides.forEach(function(sd){fetch('/mizuma/preset?slot='+encodeURIComponent(activeTab+sd)+'&'+params.toString(),{method:'POST'}).catch(function(){});});clearDirty();const d=new Date();toast('\u2713 Tersimpan '+('0'+d.getHours()).slice(-2)+':'+('0'+d.getMinutes()).slice(-2));renderSavedList();refreshSavedInfo();}
function renderSavedList(){const box=document.getElementById('savedList');box.innerHTML='Memuat...';fetch('/mizuma/presets').then(function(r){return r.json();}).then(function(d){box.innerHTML='';['welcoming','riding','sein','rem','hazard'].forEach(function(t){['Kanan','Kiri'].forEach(function(s){const st=d[t+s];const row=document.createElement('div');row.className='saved-row';const on=!!(st&&st.valid);const txt=on?'Tersimpan':'Default';row.innerHTML='<span class="sk">'+cap(t)+' \u2014 '+s+'</span><span class="st'+(on?' on':'')+'">'+txt+'</span>';box.appendChild(row);});});}).catch(function(){box.innerHTML='Gagal memuat status.';});}
document.getElementById('btnSaveSub').addEventListener('click',doSave);
/* ===== MODAL ===== */
function showModal(action){pendingAction=action;document.getElementById('modalCtx').textContent=tabLabel()+' \u2014 '+sideLabel();document.getElementById('saveModal').style.display='flex';}
document.getElementById('mSave').addEventListener('click',function(){doSave();document.getElementById('saveModal').style.display='none';if(pendingAction)pendingAction();pendingAction=null;});
document.getElementById('mDiscard').addEventListener('click',function(){clearDirty();document.getElementById('saveModal').style.display='none';if(pendingAction)pendingAction();pendingAction=null;});
document.getElementById('mCancel').addEventListener('click',function(){document.getElementById('saveModal').style.display='none';pendingAction=null;});
/* ===== PRESET APPLY & UI UPDATE ===== */
function updateControlsFromPreset(tab,st){if(!st)return;cur.isShow=(tab==='welcoming'&&!!st.isShow);cur.showIndex=cur.isShow?Math.max(0,Math.min(showNames.length-1,Number(st.showIndex)||0)):0;cur.fx=cur.isShow?0:(st.fx!=null?st.fx:0);cur.pal=(tab==='sein'||tab==='rem'||tab==='hazard')?0:(st.pal!=null?st.pal:0);const np=paramsFor(cur.fx);np.sx=st.sx!=null?st.sx:128;np.ix=st.ix!=null?st.ix:128;np.c1=st.c1!=null?st.c1:128;np.c2=st.c2!=null?st.c2:128;np.c3=st.c3!=null?st.c3:128;np.o1=st.o1!=null?st.o1:0;np.o2=st.o2!=null?st.o2:0;np.o3=st.o3!=null?st.o3:0;cur.params=np;if(st.col&&st.col.length){const cols=Array.isArray(st.col[0])?st.col:[st.col];for(let i=0;i<3;i++){if(cols[i])segColors[i]=[cols[i][0],cols[i][1],cols[i][2]];}slotCount=cols.length;}const pName=palNameById(cur.pal);cur.palName=pName;if(isRestrictedTab())normalizeRestrictedState(false);renderColorRow();if(cur.isShow)renderShowInfo();else renderParams(cur.fx);document.querySelectorAll('.fx-item').forEach(function(el){const match=cur.isShow?(Number(el.dataset.showIndex)===cur.showIndex):(el.dataset.fxIndex!=null&&Number(el.dataset.fxIndex)===cur.fx);el.classList.toggle('active',match);});document.querySelectorAll('.pal-row').forEach(function(el){el.classList.toggle('active',el.dataset.palname===cur.palName);});updateModeAktif();if(cur.isShow)document.getElementById('fxSavedName').textContent=showNames[cur.showIndex]||'Animasi Mizuma';}
function applySaved(tab){fetch('/mizuma/presets').then(function(r){return r.json();}).then(function(d){const uiSide=activeSide==='kiri'?'Kiri':'Kanan';const st=d[tab+uiSide];updateControlsFromPreset(tab,st);if(activeSide!==''){fetch('/mizuma/apply?mode='+encodeURIComponent(tab)+'&side='+encodeURIComponent(activeSide),{method:'POST'}).catch(function(){});}
/* FOKUS SISI: matikan sisi yang tidak diedit */
if(activeSide==='kanan'){post({seg:[{id:mapSegId('kiri'),on:false}]});}
else if(activeSide==='kiri'){post({seg:[{id:mapSegId('kanan'),on:false}]});}
updateCtx();updateSideInfo();}).catch(function(){});}
/* ===== NAVIGATION ===== */
function setSub(k){activeSub=k;const r=isRestrictedTab();
document.getElementById('headEfek').style.display=k==='efek'?'flex':'none';
document.getElementById('headWarna').style.display=((k==='template'||k==='custom')&&!r)?'flex':'none';
document.getElementById('headTemplate').style.display=(k==='template'&&!r)?'flex':'none';
document.getElementById('headSimpan').style.display=k==='simpan'?'flex':'none';
document.getElementById('scrollEfek').style.display=k==='efek'?'block':'none';
document.getElementById('scrollTemplate').style.display=(k==='template'&&!r)?'block':'none';
document.getElementById('scrollCustom').style.display=(k==='custom'&&!r)?'block':'none';
document.getElementById('scrollRestricted').style.display=((k==='template'||k==='custom')&&r)?'block':'none';
document.getElementById('scrollSimpan').style.display=k==='simpan'?'block':'none';
document.getElementById('footEfek').style.display=k==='efek'?'block':'none';
if(r&&(k==='template'||k==='custom')){buildRestricted();normalizeRestrictedState(false);}
if(k==='simpan')renderSavedList();
if(k==='template'||k==='custom'){loadPalettes(false);ensurePalX();}
updateModeAktif();}
function switchTab(t){activeTab=t;localStorage.setItem('mzActiveTab',t);document.querySelectorAll('#tabbar button').forEach(function(b){b.classList.toggle('active',b.dataset.tab===t);});updateDirectionButton();setSub(activeSub);if(!allFx.length){loadFxData(function(ok){buildEffects();applySaved(t);});}else{buildEffects();applySaved(t);}updateSideInfo();}
function commitSide(side){activeSide=side;document.querySelectorAll('#sideRow .side-btn').forEach(function(b){b.classList.toggle('active',b.dataset.side===side);});applySaved(activeTab);}
function updateSwapButton(){const b=document.getElementById('swapSideBtn');if(!b)return;b.classList.toggle('active',sideSwap);b.setAttribute('aria-pressed',sideSwap?'true':'false');const st=b.querySelector('.tool-state');if(st)st.textContent=sideSwap?'Ditukar':'Normal';b.title=sideSwap?'Output fisik kiri dan kanan sedang ditukar. Tekan untuk mengembalikan.':'Menukar pemetaan output fisik kiri dan kanan.';}
function updateDirectionButton(){const b=document.getElementById('reverseFxBtn');if(!b)return;b.classList.toggle('active',showReverse);b.setAttribute('aria-pressed',showReverse?'true':'false');const st=b.querySelector('.tool-state');if(st)st.textContent=showReverse?'Dibalik':'Normal';b.title=showReverse?'Arah gerak animasi sedang dibalik pada semua sisi. Tekan untuk mengembalikan.':'Membalik arah gerak animasi LED pada semua sisi dan berlaku di semua tab.';}
function loadSideSwap(){return jfetch('/mizuma/side-swap').then(function(r){if(!r.ok)throw new Error('HTTP '+r.status);return r.json();}).then(function(d){sideSwap=!!d.enabled;updateSwapButton();}).catch(function(){updateSwapButton();});}
function requestSideSwap(){const next=!sideSwap;const b=document.getElementById('swapSideBtn');if(b)b.disabled=true;jfetch('/mizuma/side-swap?enabled='+(next?'1':'0'),{method:'POST',cache:'no-store'}).then(function(r){if(!r.ok)throw new Error('HTTP '+r.status);return r.json();}).then(function(d){if(!d.ok)throw new Error('swap rejected');sideSwap=!!d.enabled;updateSwapButton();applySaved(activeTab);updateSideInfo();toast(sideSwap?'⇄ Output kanan-kiri ditukar':'Pemetaan kanan-kiri dikembalikan');}).catch(function(){toast('Gagal mengubah pemetaan sisi');}).finally(function(){if(b)b.disabled=false;});}
function loadShowReverse(){return jfetch('/mizuma/show/reverse').then(function(r){if(!r.ok)throw new Error('HTTP '+r.status);return r.json();}).then(function(d){showReverse=!!d.enabled;updateDirectionButton();}).catch(function(){updateDirectionButton();});}
function requestShowReverse(){
  const previous=showReverse,next=!previous,b=document.getElementById('reverseFxBtn');
  // Update optimistis supaya efek yang dipilih saat request menunggu ikut memakai arah baru.
  showReverse=next;updateDirectionButton();if(b)b.disabled=true;
  jfetch('/mizuma/show/reverse?enabled='+(next?'1':'0'),{method:'POST',cache:'no-store'})
    .then(function(r){
      if(!r.ok){return r.json().catch(function(){return {};}).then(function(d){throw new Error(d.error||('HTTP '+r.status));});}
      return r.json();
    })
    .then(function(d){
      if(!d.ok)throw new Error(d.error||'reverse rejected');
      if(!Number(d.activeSegments))throw new Error('no_active_segment');
      showReverse=!!d.enabled;updateDirectionButton();updateSideInfo();
      toast(showReverse?'↔ Arah animasi dibalik':'↔ Arah animasi normal');
    })
    .catch(function(err){
      if(window.console)console.error('reverse gagal:',err);
      showReverse=previous;updateDirectionButton();
      toast(err&&err.message==='no_active_segment'?'Tidak ada segmen LED aktif':'Gagal mengubah arah LED');
    })
    .finally(function(){if(b)b.disabled=false;});
}
document.getElementById('tabbar').addEventListener('click',function(e){if(e.target.tagName!=='BUTTON')return;const t=e.target.dataset.tab;if(t===activeTab)return;if(t==='riding'){try{localStorage.setItem('mzActiveTab','riding');}catch(x){}location.href='/riding';return;}if(dirty){showModal(function(){switchTab(t);});}else{switchTab(t);}});
document.getElementById('controlPanel').addEventListener('click',function(e){const btn=e.target.closest('button');if(!btn)return;/* Posisi dan arah adalah kontrol langsung, bukan perubahan preset: jangan ditahan dialog Simpan. */if(btn.dataset.action==='swap'){requestSideSwap();return;}if(btn.dataset.action==='reverse'){requestShowReverse();return;}if(!btn.dataset.side)return;const side=btn.dataset.side;if(side===activeSide)return;if(dirty){showModal(function(){commitSide(side);});}else{commitSide(side);}});
document.getElementById('subnav').addEventListener('click',function(e){const btn=e.target.closest('button.nav-b');if(!btn)return;document.querySelectorAll('#subnav .nav-b').forEach(function(b){b.classList.remove('active');});btn.classList.add('active');setSub(btn.dataset.sub);});
document.getElementById('brightSlider').addEventListener('input',function(e){document.getElementById('brightVal').textContent=e.target.value;sendBriD(+e.target.value);});
/* ===== INIT ===== */
document.querySelectorAll('#tabbar button').forEach(function(b){b.classList.toggle('active',b.dataset.tab===activeTab);});
/* Tab utama Welcoming dan Sein-Rem memakai halaman ini: tampilkan hanya sub-tab kelompoknya */
const TAB_GRP=(activeTab==='welcoming')?['welcoming']:['sein','rem','hazard'];
document.querySelectorAll('#tabbar button').forEach(function(b){if(TAB_GRP.indexOf(b.dataset.tab)<0)b.style.display='none';});
if(TAB_GRP.length===1)document.getElementById('tabbar').style.display='none';
updateDirectionButton();
setSub(activeSub);renderSavedList();
loadFxData(function(ok){if(!ok){setTimeout(function(){loadFxData(function(ok2){if(ok2)initFromState();else buildEffects();});},1500);}else{initFromState();}});
function initFromState(){loadSideSwap().then(loadShowReverse).then(function(){return jfetch('/json/state');}).then(function(r){return r.json();}).then(function(j){if(j.bri!=null){document.getElementById('brightSlider').value=j.bri;document.getElementById('brightVal').textContent=j.bri;paintRange(document.getElementById('brightSlider'));}const s=segById(j,mapSegId('kanan'));if(s){if(s.fx!=null)cur.fx=s.fx;if(s.pal!=null)cur.pal=s.pal;cur.params=paramsFor(cur.fx);captureParams(s);if(s.col&&s.col.length){for(let i=0;i<3;i++){if(s.col[i])segColors[i]=[s.col[i][0],s.col[i][1],s.col[i][2]];}slotCount=s.col.length;}}}).catch(function(){}).then(function(){setSub(activeSub);buildEffects();renderColorRow();document.querySelectorAll('input[type=range]').forEach(function(el){if(!el.classList.contains('kelvin'))paintRange(el);});setTimeout(function(){applySaved(activeTab);},400);});}
</script>
<script>
(function(){
function inj(url,cb){fetch(url).then(function(r){return r.text();}).then(cb).catch(function(){});}
inj('/mizuma/frag/header',function(t){const el=document.getElementById('mzH');if(el)el.outerHTML=t;
inj('/mizuma/frag/nav',function(t2){const el2=document.getElementById('mzN');if(el2)el2.outerHTML=t2;const a=document.querySelector('.bottomnav a[href="'+(activeTab==='welcoming'?'/led?t=welcoming':'/led?t=sein')+'"]');if(a)a.classList.add('active');
inj('/mizuma/frag/script',function(t3){eval(t3);});});});
window.addEventListener('pagehide',function(){if(activeSide==='both')return;try{navigator.sendBeacon('/json/state',new Blob([JSON.stringify({seg:[{id:SEG_K,on:true},{id:SEG_L,on:true}]})],{type:'application/json'}));}catch(e){}});
})();
</script>
</body>
</html>
)rawliteral";

// --------------------------------------------------------------------------------------------------------------------------------------
// Perender halaman + pendaftaran rute UI
// --------------------------------------------------------------------------------------------------------------------------------------
inline void mizumaApplyCommon(String &html, const char *activeKey, const MizumaVehicleInfo &v) {
  html.replace("%HEADER%", FPSTR(MIZUMA_HEADER_HTML));
  String navHtml = FPSTR(MIZUMA_BOTTOMNAV_HTML);
  navHtml.replace("__ACTIVE_BERANDA__",   strcmp(activeKey, "beranda")==0   ? "active" : "");
  navHtml.replace("__ACTIVE_PENGATURAN__",strcmp(activeKey, "pengaturan")==0? "active" : "");
  html.replace("%BOTTOMNAV%", navHtml);
  html.replace("%VEHICLE_NAME%",  v.name.length()  ? v.name  : String("Motor Anda"));
  html.replace("%VEHICLE_BRAND%", v.brand.length() ? v.brand : String("-"));
  html.replace("%VEHICLE_YEAR%",  v.year ? String(v.year) : String("-"));
  html.replace("%VEHICLE_PLATE%", v.plate.length() ? v.plate : String("-"));
}
inline String mizumaRenderPage(const char *pageTemplate, const char *activeKey, const MizumaVehicleInfo &v) {
  String html = FPSTR(pageTemplate);
  mizumaApplyCommon(html, activeKey, v);
  return html;
}

// Satu-satunya pintu masuk UI dari engine. getVehicle dipanggil setiap halaman diminta (selalu data terbaru).
inline void mizumaUiRegister(AsyncWebServer &server, std::function<MizumaVehicleInfo()> getVehicle) {
  server.on("/mizuma.css", HTTP_GET, [](AsyncWebServerRequest *req){
    AsyncWebServerResponse *res = req->beginResponse_P(200, "text/css", MIZUMA_SHARED_CSS);
    res->addHeader("Cache-Control", "max-age=86400"); req->send(res);});
  server.on("/mizuma.js", HTTP_GET, [](AsyncWebServerRequest *req){
    AsyncWebServerResponse *res = req->beginResponse_P(200, "text/javascript", MIZUMA_HEADER_SCRIPT);
    res->addHeader("Cache-Control", "max-age=86400"); req->send(res);});
  server.on("/riding", HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/html", MIZUMA_RIDING_HTML); });
  server.on("/led", HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/html", MIZUMA_LED_HTML); });
  server.on("/app", HTTP_GET, [getVehicle](AsyncWebServerRequest *req){
    req->send(200, "text/html", mizumaRenderPage(MIZUMA_HOME_HTML, "beranda", getVehicle())); });
  server.on("/pengaturan", HTTP_GET, [getVehicle](AsyncWebServerRequest *req){
    req->send(200, "text/html", mizumaRenderPage(MIZUMA_SETTINGS_HTML, "pengaturan", getVehicle())); });
  server.on("/mizuma/frag/header", HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/html", MIZUMA_HEADER_HTML); });
  server.on("/mizuma/frag/script", HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/javascript", MIZUMA_HEADER_SCRIPT); });
  server.on("/mizuma/frag/nav",    HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/html", MIZUMA_BOTTOMNAV_HTML); });
}
