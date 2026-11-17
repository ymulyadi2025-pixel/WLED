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
.bottomnav a{flex:1;min-width:0;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:3px;padding:7px 1px 8px;text-decoration:none;color:var(--tx3);font-size:9px;font-weight:600;white-space:nowrap;}
.bottomnav a:active{transform:none;}
.bottomnav .bn-pill{display:flex;align-items:center;justify-content:center;padding:3px 10px;border-radius:16px;transition:background .25s var(--ease),transform .12s var(--ease);}
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
<a href="/warna" class="__ACTIVE_WARNA__"><span class="bn-pill"><svg viewBox="0 0 24 24"><path d="M12 3a9 9 0 1 0 0 18h1.2a2 2 0 0 0 1.4-3.4 1.8 1.8 0 0 1 1.3-3.1H18a3 3 0 0 0 3-3c0-4.7-4-8.5-9-8.5Z"/><circle cx="7.5" cy="10" r="1"/><circle cx="11" cy="6.8" r="1"/><circle cx="16" cy="8.2" r="1"/></svg></span>Warna</a>
<a href="/welcoming" class="__ACTIVE_WELCOMING__"><span class="bn-pill"><svg viewBox="0 0 24 24"><path d="M12 2v3M12 19v3M4.9 4.9 7 7M17 17l2.1 2.1M2 12h3M19 12h3M4.9 19.1 7 17M17 7l2.1-2.1"/><circle cx="12" cy="12" r="5"/></svg></span>Welcoming</a>
<a href="/led" class="__ACTIVE_RIDING__"><span class="bn-pill"><svg viewBox="0 0 24 24"><path d="M3 15h2l2-5h8l3 5h3"/><circle cx="7" cy="16" r="3"/><circle cx="17" cy="16" r="3"/><path d="M9 10l2-3h4"/></svg></span>Riding</a>
<a href="/sein-rem" class="__ACTIVE_SEINREM__"><span class="bn-pill"><svg viewBox="0 0 24 24"><path d="M4 12h5l3-8 3 16 3-8h2"/></svg></span>Sein-Rem</a>
<a href="/pengaturan" class="__ACTIVE_PENGATURAN__"><span class="bn-pill"><svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 1 1-2.83 2.83l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 1 1-4 0v-.09a1.65 1.65 0 0 0-1-1.51 1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 1 1-2.83-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 1 1 0-4h.09a1.65 1.65 0 0 0 1.51-1 1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 1 1 2.83-2.83l.06.06a1.65 1.65 0 0 0 1.82.33h.01a1.65 1.65 0 0 0 1-1.51V3a2 2 0 1 1 4 0v.09a1.65 1.65 0 0 0 1 1.51h.01a1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 1 1 2.83 2.83l.06.06a1.65 1.65 0 0 0-.33 1.82v.01a1.65 1.65 0 0 0 1.51 1H21a2 2 0 1 1 0 4h-.09a1.65 1.65 0 0 0-1.51 1z"/></svg></span>Pengaturan</a>
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
<div class="stat-box"><div class="si ok"><svg viewBox="0 0 24 24"><path d="M12 22s8-3.5 8-10V5l-8-3-8 3v7c0 6.5 8 10 8 10z"/></svg></div><div class="sv">--</div><div class="sl">Koneksi</div></div>
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
<div class="card">
<div class="card-title">Status Sistem</div>
<div class="placeholder-text">Informasi kendaraan dan sistem Mizuma</div>
</div>
<div class="quick-tiles">
<a class="tile" href="/led"><div class="ti ac"><svg viewBox="0 0 24 24"><path d="M9 18h6"/><path d="M10 21h4"/><path d="M12 3a6 6 0 0 0-3.6 10.8c.7.6 1.1 1.4 1.3 2.2h4.6c.2-.8.6-1.6 1.3-2.2A6 6 0 0 0 12 3z"/></svg></div><div><div class="tl">Kontrol Lampu</div><div class="ts">Riding &amp; Welcoming</div></div></a>
<a class="tile" href="/welcoming"><div class="ti ok"><svg viewBox="0 0 24 24"><path d="M12 2v3M12 19v3M4.9 4.9 7 7M17 17l2.1 2.1M2 12h3M19 12h3M4.9 19.1 7 17M17 7l2.1-2.1"/><circle cx="12" cy="12" r="5"/></svg></div><div><div class="tl">Welcoming</div><div class="ts">Animasi pembuka</div></div></a>
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
// BLOK 4 — LED / RIDING UI (katalog WLED + katalog Welcoming Mizuma)
// =====================================================================================================================================
const char MIZUMA_LED_LEGACY_HTML[] PROGMEM = R"rawliteral(
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

.live-btn svg{width:12px;height:12px;stroke:currentColor;fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
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
.fx-grid{display:grid;grid-template-columns:repeat(2,1fr);gap:8px;}
@media(min-width:420px){.fx-grid{grid-template-columns:repeat(3,1fr);}}
.fx-item{background:var(--s1);border:1px solid var(--bd);border-radius:10px;padding:10px 8px;font-size:11px;color:var(--tx);}
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
<input class="fx-search" id="fxSearch" placeholder="Cari efek...">
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
const legacyPath=location.pathname;let activeTab=legacyPath==='/welcoming'?'welcoming':legacyPath==='/sein-rem'?'sein':(localStorage.getItem('mzActiveTab')||'riding');
let activeSide='both',activeSub='efek',sideSwap=false,showReverse=false;
let dirty=false,pendingAction=null,restrictedName='';
let allFx=[],fxData=[],curList=[],palList=[],palNamesRaw=[],showNames=[],showDurations=[];
let cur={fx:0,pal:0,col:null,palName:'',isShow:false,showIndex:0,params:{sx:128,ix:128,c1:128,c2:128,c3:128,o1:0,o2:0,o3:0}};
let fxParamsCache={},fxPalCache={};
function paramsFor(i){if(!fxParamsCache[i]){const d=fxData.length?fxDefs(i):{};const g=function(k,df){return d[k]!=null?d[k]:df;};const p={sx:g('sx',128),ix:g('ix',128),c1:g('c1',128),c2:g('c2',128),c3:g('c3',128),o1:g('o1',0),o2:g('o2',0),o3:g('o3',0)};if(!fxData.length)return p;fxParamsCache[i]=p;}return fxParamsCache[i];}
cur.params=paramsFor(0);
let wHue=0,wSat=1,wVal=1;
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
function sendColor(r,g,b,silent){if(isRestrictedTab()){colorTarget=0;cur.pal=0;segColors[0]=[r,g,b];segColors[1]=[0,0,0];segColors[2]=[0,0,0];}else{segColors[colorTarget]=[r,g,b];}const cols=[segColors[0]||[255,255,255],segColors[1]||[0,0,0],segColors[2]||[0,0,0]];const segs=segIds().map(function(id){return{id:id,col:cols,pal:isRestrictedTab()?0:cur.pal};});post({seg:segs});cur.col=[r,g,b];if(!silent)markDirty();renderColorRow();updateSideInfo();}
function sendPalette(i){if(isRestrictedTab())return;const segs=segIds().map(function(id){return{id:id,pal:i};});if(activeTab==='welcoming'){fetch('/mizuma/show/palette?pal='+i+'&side='+encodeURIComponent(activeSide),{method:'POST'}).then(function(r){if(!r.ok)post({seg:segs});}).catch(function(){post({seg:segs});});}else post({seg:segs});cur.pal=i;fxPalCache[cur.fx]=i;markDirty();renderColorRow();updateSideInfo();}
function renderShowInfo(){const box=document.getElementById('fxParams');box.innerHTML='';const hint=document.getElementById('footHint');const ms=Number(showDurations[cur.showIndex]||0);const sec=ms>0?(ms/1000).toFixed(2).replace('.',',')+' detik':'mengikuti seluruh tahap animasi';hint.textContent='Durasi: '+sec+'. Setiap animasi memiliki waktu berbeda dan berjalan sampai seluruh tahap selesai.';hint.style.display='block';}
function sendShow(index){if(activeTab!=='welcoming'||index<0||index>=showNames.length)return;cur.isShow=true;cur.showIndex=index;cur.fx=0;document.getElementById('fxSavedName').textContent=showNames[index];fetch('/mizuma/show?index='+index+'&side='+encodeURIComponent(activeSide),{method:'POST'}).then(function(r){if(!r.ok)throw new Error('HTTP '+r.status);return r.json();}).then(function(d){if(!d.ok)throw new Error('show rejected');markDirty();renderShowInfo();updateCtx();updateSideInfo();}).catch(function(){toast('Gagal menjalankan animasi Mizuma');});}
function sendEffect(i){cur.isShow=false;cur.showIndex=0;if(isRestrictedTab())normalizeRestrictedState(false);cur.fx=i;cur.params=paramsFor(i);let palToSend=cur.pal;if(!isRestrictedTab()){let pi=-1;if(fxPalCache[i]!=null)pi=fxPalCache[i];else{const df=fxDefs(i);if(df.pal!=null)pi=df.pal;else{const nm=allFx[i]||'';if(FX_PAL_DEFAULTS[nm])pi=palIdByName(FX_PAL_DEFAULTS[nm]);}}if(pi>=0&&palNameById(pi)){cur.pal=pi;cur.palName=palNameById(pi);palToSend=pi;document.querySelectorAll('.pal-row').forEach(function(el){el.classList.toggle('active',el.dataset.palname===cur.palName);});}}const cols=[segColors[0]||[255,255,255],isRestrictedTab()?[0,0,0]:(segColors[1]||[0,0,0]),isRestrictedTab()?[0,0,0]:(segColors[2]||[0,0,0])];const segs=segIds().map(function(id){const o={id:id,fx:i,rev:showReverse,sx:cur.params.sx,ix:cur.params.ix,c1:cur.params.c1,c2:cur.params.c2,c3:cur.params.c3,o1:cur.params.o1,o2:cur.params.o2,o3:cur.params.o3};if(isRestrictedTab()){o.pal=0;o.col=cols;}else{o.pal=palToSend;}return o;});const applyFx=function(){post({seg:segs});};if(activeTab==='welcoming'){fetch('/mizuma/show/stop?side='+encodeURIComponent(activeSide),{method:'POST'}).then(applyFx).catch(applyFx);}else applyFx();if(isRestrictedTab())cur.pal=0;markDirty();renderColorRow();renderParams(i);updateSideInfo();}
const sendParamD=debounce(function(k,v){const segs=segIds().map(function(id){const o={id:id};o[k]=v;return o;});post({seg:segs});cur.params[k]=v;markDirty();},80);
const sendBriD=debounce(function(v){post({bri:v});fetch('/mizuma/bri?v='+v,{method:'POST'}).catch(function(){});},80);
/* ===== SIDE INFO (kiri/kanan dari state device) ===== */
let usiT=null;
function updateSideInfo(){if(usiT)return;usiT=setTimeout(function(){usiT=null;updateSideInfoNow();},350);}
function updateSideInfoNow(){jfetch('/json/state').then(function(r){return r.json();}).then(function(j){if(!j.seg)return;const nm=function(s){if(!s)return '-';if(s.on===false)return 'Off';return (allFx[s.fx]||'Efek')+' - '+(palNameById(s.pal)||'Palette');};document.getElementById('ctxInfo').textContent=nm(segById(j,mapSegId('kiri')));document.getElementById('savedInfo').textContent=nm(segById(j,mapSegId('kanan')));}).catch(function(){});}
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
let palRaw={},palSlot={},palRand={},palTry=0;
function palGrad(id){const d=palRaw[id];if(!d||!d.length)return null;const st=[];for(let j=0;j<d.length;j++){const e=d[j];let c,idx=false;if(Array.isArray(e)){idx=e[0]/255*100;c=[e[1],e[2],e[3]];}else if(e==='r'){const key=id+'_'+j;c=palRand[key]||(palRand[key]=[Math.random()*255|0,Math.random()*255|0,Math.random()*255|0]);}else{c=segColors[(parseInt(String(e).charAt(1),10)||1)-1]||[0,0,0];}const rgb='rgb('+c[0]+','+c[1]+','+c[2]+')';st.push(idx===false?rgb:rgb+' '+idx+'%');}if(st.length===1)st.push(st[0]);return 'linear-gradient(90deg,'+st.join(',')+')';}
function refreshPalStrips(all){document.querySelectorAll('.pal-row').forEach(function(row){const id=parseInt(row.dataset.pid,10);if(isNaN(id))return;if(!all&&!palSlot[id])return;const g=palGrad(id);if(g)row.querySelector('.pstrip').style.background=g;});}
function updateStarStrips(){refreshPalStrips(false);}
function loadPalX(page){page=page||0;jfetch('/json/palx?page='+page).then(function(r){return r.ok?r.json():Promise.reject();}).then(function(d){const map=d&&d.p;if(!map||typeof map!=='object')throw 0;Object.keys(map).forEach(function(k){const v=map[k];if(Array.isArray(v)){const id=parseInt(k,10);palRaw[id]=v;palSlot[id]=v.some(function(x){return typeof x==='string'&&x!=='r';});}});palTry=0;refreshPalStrips(true);if(typeof d.m==='number'&&page<d.m)loadPalX(page+1);else{palCacheSave();if(palInfo.cpal)renderPaletteRows(document.getElementById('searchBox').value.toLowerCase());}}).catch(function(){if(palTry++<4)setTimeout(function(){loadPalX(page);},1500);});}
function makePalRow(e,grid,numbered,seq){const row=document.createElement('div');row.className='pal-row'+(cur.palName===e.n?' active':'');row.dataset.name=e.n.toLowerCase();row.dataset.palname=e.n;row.dataset.pid=e.i;const grad=palGrad(e.i)||'var(--s3)';row.innerHTML='<span class="pradio"></span><span class="pnum">'+(numbered?(seq+1):'')+'</span><span class="pname">'+e.n+'</span><span class="pstrip" style="background:'+grad+'"></span>';row.addEventListener('click',function(){cur.palName=e.n;document.querySelectorAll('.pal-row').forEach(function(x){x.classList.remove('active');});row.classList.add('active');sendPalette(e.i);updateModeAktif();});grid.appendChild(row);}
function renderPaletteRows(q){paletteGrid.innerHTML='';customPalGrid.innerHTML='';let t=0;palList.forEach(function(e){if(e.custom&&isPlaceholderPal(e.i))return;if(e.n.charAt(0)==='*'){makePalRow(e,customPalGrid,false,0);return;}if(q&&e.n.toLowerCase().indexOf(q)<0)return;makePalRow(e,paletteGrid,true,t);t++;});updateStarStrips();}
let palInfo={cpal:0,um:[],major:0},palSig='';
function palIdBase(){return palInfo.major>=16?200:255;}
function isPlaceholderPal(id){const d=palRaw[id];if(!d||!d.length||!Array.isArray(d[0]))return false;const f=d[0];if(!(f[1]===f[2]&&f[2]===f[3]))return false;return d.every(function(e){return Array.isArray(e)&&e[1]===f[1]&&e[2]===f[2]&&e[3]===f[3];});}
function palCacheLoad(sig){try{const c=JSON.parse(localStorage.getItem('mzPalCache')||'null');if(c&&c.sig===sig&&c.raw){palRaw=c.raw;Object.keys(palRaw).forEach(function(k){palSlot[k]=palRaw[k].some(function(x){return typeof x==='string'&&x!=='r';});});}}catch(e){}}
function palCacheSave(){try{localStorage.setItem('mzPalCache',JSON.stringify({sig:palSig,raw:palRaw}));}catch(e){}}
function loadPalettes(force){Promise.all([jfetch('/json/pal').then(function(r){return r.json();}),jfetch('/json/info').then(function(r){return r.json();})]).then(function(v){const names=v[0],inf=v[1]||{};palInfo.major=parseInt(String(inf.ver||'0'),10)||0;if(inf.umpalnames!==undefined||inf.cpalmax!==undefined)palInfo.major=Math.max(palInfo.major,16);palInfo.cpal=inf.cpalcount||0;palInfo.um=Array.isArray(inf.umpalnames)?inf.umpalnames:[];const sig=names.length+'|'+palInfo.cpal+'|'+palInfo.um.length+'|'+palInfo.major;if(!force&&sig===palSig)return;const first=(palSig==='');palSig=sig;if(first)palCacheLoad(sig);palNamesRaw=names;const arr=[];names.forEach(function(n,i){if(n!=='r')arr.push({n:n,i:i});});arr.sort(function(a,b){if(a.i===0)return -1;if(b.i===0)return 1;const sa=a.n.charAt(0)==='*',sb=b.n.charAt(0)==='*';if(sa&&sb)return a.i-b.i;return a.n.localeCompare(b.n);});for(let k=0;k<palInfo.cpal;k++)arr.push({n:'~ Custom '+(k+1)+' ~',i:palIdBase()-k,custom:true});if(palInfo.major>=16)palInfo.um.forEach(function(n,k){arr.push({n:n||('Usermod '+(k+1)),i:255-k,custom:true});});palList=arr;renderPaletteRows(document.getElementById('searchBox').value.toLowerCase());renderColorRow();palTry=0;loadPalX(0);updateSideInfo();}).catch(function(){setTimeout(function(){loadPalettes(force);},1500);});}
loadPalettes(true);
document.addEventListener('visibilitychange',function(){if(!document.hidden)loadPalettes(false);});
window.addEventListener('focus',function(){loadPalettes(false);});
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
document.getElementById('fxSearch').addEventListener('input',function(e){const q=e.target.value.toLowerCase();document.querySelectorAll('#fxGrid .fx-item').forEach(function(el){el.style.display=el.querySelector('.fx-name').textContent.toLowerCase().indexOf(q)>=0?'':'none';});});
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
if(k==='template'||k==='custom')loadPalettes(false);
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
document.getElementById('tabbar').addEventListener('click',function(e){if(e.target.tagName!=='BUTTON')return;const t=e.target.dataset.tab;if(t===activeTab)return;if(dirty){showModal(function(){switchTab(t);});}else{switchTab(t);}});
document.getElementById('controlPanel').addEventListener('click',function(e){const btn=e.target.closest('button');if(!btn)return;/* Posisi dan arah adalah kontrol langsung, bukan perubahan preset: jangan ditahan dialog Simpan. */if(btn.dataset.action==='swap'){requestSideSwap();return;}if(btn.dataset.action==='reverse'){requestShowReverse();return;}if(!btn.dataset.side)return;const side=btn.dataset.side;if(side===activeSide)return;if(dirty){showModal(function(){commitSide(side);});}else{commitSide(side);}});
document.getElementById('subnav').addEventListener('click',function(e){const btn=e.target.closest('button.nav-b');if(!btn)return;document.querySelectorAll('#subnav .nav-b').forEach(function(b){b.classList.remove('active');});btn.classList.add('active');setSub(btn.dataset.sub);});
document.getElementById('brightSlider').addEventListener('input',function(e){document.getElementById('brightVal').textContent=e.target.value;sendBriD(+e.target.value);});
/* ===== INIT ===== */
document.querySelectorAll('#tabbar button').forEach(function(b){b.classList.toggle('active',b.dataset.tab===activeTab);if(legacyPath==='/welcoming')b.style.display='none';else if(legacyPath==='/sein-rem'&&(b.dataset.tab==='welcoming'||b.dataset.tab==='riding'))b.style.display='none';});
updateDirectionButton();
setSub(activeSub);renderSavedList();
loadFxData(function(ok){if(!ok){setTimeout(function(){loadFxData(function(ok2){if(ok2)initFromState();else buildEffects();});},1500);}else{initFromState();}});
function initFromState(){loadSideSwap().then(loadShowReverse).then(function(){return jfetch('/json/state');}).then(function(r){return r.json();}).then(function(j){if(j.bri!=null){document.getElementById('brightSlider').value=j.bri;document.getElementById('brightVal').textContent=j.bri;paintRange(document.getElementById('brightSlider'));}const s=segById(j,mapSegId('kanan'));if(s){if(s.fx!=null)cur.fx=s.fx;if(s.pal!=null)cur.pal=s.pal;cur.params=paramsFor(cur.fx);captureParams(s);if(s.col&&s.col.length){for(let i=0;i<3;i++){if(s.col[i])segColors[i]=[s.col[i][0],s.col[i][1],s.col[i][2]];}slotCount=s.col.length;}}}).catch(function(){}).then(function(){setSub(activeSub);buildEffects();renderColorRow();document.querySelectorAll('input[type=range]').forEach(function(el){if(!el.classList.contains('kelvin'))paintRange(el);});setTimeout(function(){applySaved(activeTab);},400);});}
</script>
<script>
(function(){
function inj(url,cb){fetch(url).then(function(r){return r.text();}).then(cb).catch(function(){});}
inj('/mizuma/frag/header',function(t){const el=document.getElementById('mzH');if(el)el.outerHTML=t;
inj('/mizuma/frag/nav',function(t2){const el2=document.getElementById('mzN');if(el2)el2.outerHTML=t2;const a=document.querySelector('.bottomnav a[href="'+location.pathname+'"]');if(a)a.classList.add('active');
inj('/mizuma/frag/script',function(t3){eval(t3);});});});
window.addEventListener('pagehide',function(){if(activeSide==='both')return;try{navigator.sendBeacon('/json/state',new Blob([JSON.stringify({seg:[{id:SEG_K,on:true},{id:SEG_L,on:true}]})],{type:'application/json'}));}catch(e){}});
})();
</script>
</body>
</html>
)rawliteral";


// Riding workspace v20: UI prototype styling, real WLED catalog/colors, and ESP32 playlist engine.
const char MIZUMA_LED_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<meta name="theme-color" content="#0a0a0a"><title>Riding · Mizuma</title>
<link rel="stylesheet" href="/mizuma.css">
<style>
html,body{height:100%;margin:0;background:var(--bg);color:var(--tx)}
body{padding-bottom:calc(var(--nav-h) + env(safe-area-inset-bottom));}
.rw{max-width:620px;margin:0 auto;padding:12px 12px 24px}
.rhead{display:flex;align-items:center;gap:10px;margin:2px 2px 12px}.rhead .lead{flex:1;min-width:0}.eyebrow{font-size:10px;color:var(--ac);font-weight:800;letter-spacing:1.2px}.rtitle{font-size:22px;font-weight:850;line-height:1.15;margin-top:2px}.rsub{font-size:11px;color:var(--tx2);margin-top:3px}.legacy{font-size:11px;font-weight:700;color:var(--tx2);border:1px solid var(--bd2);background:var(--s2);padding:9px 11px;border-radius:10px;text-decoration:none;white-space:nowrap}
.panel{background:var(--s1);border:1px solid var(--bd);border-radius:15px;padding:12px;margin-bottom:10px}.cap{font-size:10px;text-transform:uppercase;letter-spacing:.6px;color:var(--tx2);font-weight:800;margin-bottom:8px}
.ctrlgrid{display:grid;grid-template-columns:1.25fr 1fr;gap:10px}.segmented{display:flex;gap:3px;padding:3px;background:var(--s2);border:1px solid var(--bd);border-radius:11px}.segmented button{flex:1;min-width:0;padding:9px 3px;border-radius:8px;color:var(--tx2);font-size:12px;font-weight:700}.segmented button.on{background:var(--ac);color:#111}.toolrow{display:flex;gap:6px}.tool{flex:1;min-width:0;padding:8px 4px;border-radius:10px;background:var(--s2);border:1px solid var(--bd2);font-size:11px;font-weight:700;color:var(--tx2)}.tool.on{color:var(--ac);border-color:var(--ac);background:var(--acd)}.tool small{display:block;font-size:9px;font-weight:600;color:inherit;opacity:.8;margin-top:2px}.brrow{display:flex;align-items:center;gap:10px;margin-top:12px}.brrow label{font-size:11px;color:var(--tx2);white-space:nowrap}.brrow input{flex:1;min-width:0}.brrow b{font-size:12px;min-width:27px;text-align:right}
.tabs{display:grid;grid-template-columns:repeat(3,1fr);gap:4px;background:var(--s1);border:1px solid var(--bd);padding:4px;border-radius:12px;margin:10px 0}.tabs button{padding:10px 4px;border-radius:9px;font-size:12px;font-weight:750;color:var(--tx2)}.tabs button.on{background:var(--acd);color:var(--ac)}
.view{display:none}.view.on{display:block}.bar{display:flex;align-items:center;gap:8px;margin-bottom:10px}.bar .search{flex:1;min-width:0}.search,input[type=text],input[type=number],select{width:100%;background:var(--s2);color:var(--tx);border:1px solid var(--bd2);border-radius:10px;padding:10px 11px;font:inherit;font-size:12px;outline:none}.barbtn{padding:9px 11px;border:1px solid var(--bd2);border-radius:10px;background:var(--s2);font-size:11px;font-weight:700;color:var(--tx2);white-space:nowrap}.barbtn.on{color:var(--ac);border-color:var(--ac);background:var(--acd)}
.fxgrid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:7px}.catalogGroup{margin-bottom:12px}.catalogGroup:last-child{margin-bottom:0}.catalogTitle{font-size:10px;color:var(--tx2);font-weight:800;letter-spacing:.55px;text-transform:uppercase;margin:4px 0 7px}.catalogTitle small{font-size:9px;color:var(--tx3);margin-left:4px}.fx{min-width:0;text-align:left;position:relative;background:var(--s1);border:1px solid var(--bd);border-radius:11px;padding:10px 9px 9px;min-height:60px}.fx.on{border-color:var(--ac);background:var(--acd)}.fx b{display:block;padding-right:24px;font-size:12px;line-height:1.25;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}.fx small{display:block;color:var(--tx3);font-size:10px;margin-top:4px}.fx .fav{position:absolute;right:5px;top:4px;padding:5px;font-size:18px;color:var(--tx3)}.fx .fav.on{color:var(--ac)}
.selected{display:flex;align-items:center;gap:9px}.selected .swatch{width:46px;height:30px;border-radius:8px;border:1px solid rgba(255,255,255,.25);flex:none}.selected .desc{flex:1;min-width:0}.selected .desc b{display:block;font-size:13px}.selected .desc small{display:block;color:var(--tx2);font-size:10px;margin-top:2px}.btn{display:flex;align-items:center;justify-content:center;gap:7px;width:100%;padding:11px;border:1px solid var(--bd2);border-radius:11px;background:var(--s2);font-size:12px;font-weight:800;color:var(--tx);margin-top:8px}.btn.primary{background:var(--ac);border-color:var(--ac);color:#111}.btn.outline{border-color:var(--ac);color:var(--ac);background:transparent}.btnrow{display:flex;gap:7px}.btnrow .btn{flex:1}
.params{display:grid;grid-template-columns:1fr 1fr;gap:9px 12px}.param{min-width:0}.param label{display:flex;justify-content:space-between;gap:5px;color:var(--tx2);font-size:10px;margin-bottom:5px}.param label b{color:var(--tx)}.param input{width:100%;accent-color:var(--ac)}.switchrow{display:flex;align-items:center;justify-content:space-between;padding:8px 0;border-bottom:1px solid var(--bd);font-size:11px;color:var(--tx2)}.switchrow:last-child{border:0}.switch{width:44px;height:25px;border-radius:20px;background:var(--s3);border:1px solid var(--bd2);position:relative}.switch:after{content:"";position:absolute;left:3px;top:3px;width:17px;height:17px;background:#888;border-radius:50%;transition:.15s}.switch.on{background:var(--acd);border-color:var(--ac)}.switch.on:after{left:22px;background:var(--ac)}
.hint{font-size:10px;color:var(--tx3);line-height:1.45;margin-top:8px}.pill{display:inline-flex;align-items:center;gap:5px;border-radius:20px;padding:4px 8px;background:var(--s3);font-size:10px;color:var(--tx2)}.statusdot{width:6px;height:6px;border-radius:50%;background:var(--ok)}
.playgrid{display:grid;grid-template-columns:1fr 1fr;gap:8px}.plcard{padding:10px;text-align:left;border:1px solid var(--bd);border-radius:13px;background:var(--s1);min-width:0}.plcard.on{border-color:var(--ac)}.plthumb{display:flex;height:28px;overflow:hidden;border-radius:7px;background:#080808;margin-bottom:8px}.plthumb i{flex:1;min-width:0}.plcard b{display:block;font-size:12px;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}.plcard small{display:block;color:var(--tx2);font-size:10px;margin-top:3px}.plstatus{font-size:9px;color:var(--ok);margin-top:5px;font-weight:800}.pldetail{margin-top:10px}.plhead{display:flex;align-items:center;gap:8px;margin-bottom:8px}.plhead b{flex:1;font-size:14px}.slotrow{display:flex;align-items:center;gap:7px;padding:8px;margin-bottom:6px;background:var(--s2);border:1px solid var(--bd);border-radius:10px}.slotrow .n{font-size:10px;color:var(--tx3);width:16px}.slotrow .swatches{display:flex;gap:2px;width:36px;height:22px;border-radius:5px;overflow:hidden;flex:none}.slotrow .swatches i{flex:1}.slotrow .meta{flex:1;min-width:0}.slotrow .meta b{display:block;font-size:11px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}.slotrow .meta small{display:block;font-size:9px;color:var(--tx2);margin-top:2px}.slotrow button{width:32px;height:32px;flex:none;border-radius:8px;background:var(--s3);border:1px solid var(--bd2);font-size:12px}.slotrow .edit{color:var(--ac)}.plname{display:flex;gap:7px;margin-bottom:8px}.plname input{flex:1;min-width:0}.plactions{display:grid;grid-template-columns:1fr 1fr 1fr;gap:6px}.plactions button{padding:10px 3px;font-size:11px;font-weight:800;border-radius:9px;background:var(--s2);border:1px solid var(--bd2)}.plactions .play{background:var(--ac);border-color:var(--ac);color:#111}
.color-mode{display:flex;gap:6px;margin-bottom:10px}.color-mode button{flex:1;padding:9px 5px;border:1px solid var(--bd);border-radius:9px;background:var(--s2);font-size:11px;color:var(--tx2);font-weight:700}.color-mode button.on{border-color:var(--ac);background:var(--acd);color:var(--ac)}.rgbrow{display:grid;grid-template-columns:repeat(3,1fr);gap:8px}.rgbitem{background:var(--s2);border:1px solid var(--bd);border-radius:10px;padding:8px;text-align:center}.rgbitem label{display:block;font-size:10px;color:var(--tx2);margin-bottom:6px}.rgbitem input[type=color]{width:100%;height:38px;border:0;padding:0;background:none}.wheelbox{display:flex;flex-direction:column;align-items:center;gap:9px;margin:12px 0 4px}.wheel-target{display:grid;grid-template-columns:repeat(3,1fr);gap:6px;width:100%}.wheel-target button{padding:7px 4px;border:1px solid var(--bd);border-radius:9px;background:var(--s2);font-size:10px;font-weight:700;color:var(--tx2)}.wheel-target button.on{border-color:var(--ac);background:var(--acd);color:var(--ac)}#rideColorWheel{display:block;width:min(230px,72vw);height:auto;aspect-ratio:1;touch-action:none;border-radius:50%}.wheel-value{display:flex;align-items:center;gap:9px;width:100%;font-size:10px;color:var(--tx2)}.wheel-value input{flex:1;min-width:0;accent-color:var(--ac)}.wheel-value b{min-width:32px;text-align:right;color:var(--tx)}.wheel-hex{font-size:10px;color:var(--tx2);font-weight:700}.quick{display:flex;gap:8px;flex-wrap:wrap;margin-top:10px}.quick button{width:30px;height:30px;border-radius:50%;border:2px solid rgba(255,255,255,.2)}.palgrid,.skingrid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:7px}.pal{position:relative;display:flex;align-items:center;gap:8px;padding:9px 8px;border:1px solid var(--bd);border-radius:10px;background:var(--s1);min-width:0;text-align:left}.pal.on{border-color:var(--ac)}.pal i{width:42px;height:18px;border-radius:5px;flex:none}.pal b{font-size:10px;font-weight:700;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}.skin{padding:9px;border:1px solid var(--bd);background:var(--s1);border-radius:10px;text-align:left}.skin.on{border-color:var(--ac)}.skin .swatches{height:26px;display:flex;overflow:hidden;border-radius:6px;margin-bottom:6px}.skin .swatches i{flex:1}.skin b{display:block;font-size:11px}.skin small{font-size:9px;color:var(--tx2)}.sectiontitle{font-size:10px;color:var(--tx2);font-weight:800;letter-spacing:.5px;text-transform:uppercase;margin:14px 0 7px}.two{display:grid;grid-template-columns:1fr 1fr;gap:8px}.field{margin-top:9px}.field label{display:block;font-size:10px;color:var(--tx2);margin-bottom:4px}.hide{display:none!important}.empty{padding:20px 12px;text-align:center;border:1px dashed var(--bd2);border-radius:12px;color:var(--tx2);font-size:12px}.toast{position:fixed;z-index:100;left:50%;bottom:calc(var(--nav-h) + 78px);transform:translateX(-50%);padding:9px 14px;border-radius:22px;background:var(--s2);border:1px solid var(--bd2);font-size:11px;font-weight:700;color:var(--ok);opacity:0;pointer-events:none;transition:opacity .18s;white-space:nowrap}.toast.show{opacity:1}
@media(min-width:520px){.rw{padding-left:18px;padding-right:18px}.fxgrid{grid-template-columns:repeat(3,minmax(0,1fr))}.ctrlgrid{grid-template-columns:1.3fr 1fr}}
</style></head>
<body>
<div id="mzH"></div>
<div class="rw">
 <div class="rhead"><div class="lead"><div class="eyebrow">MIZUMA SMART SYSTEM</div><div class="rtitle" id="ridingTitle">Riding</div><div class="rsub" id="ridingSubtitle">Atur efek, warna, dan playlist saat berkendara</div></div><span class="legacy" id="ridingPageBadge">Single / Playlist</span></div>
 <section class="panel">
  <div class="ctrlgrid">
   <div><div class="cap">Sisi lampu</div><div class="segmented" id="sideSelect"><button data-side="kiri">Kiri</button><button data-side="both" class="on">Semua</button><button data-side="kanan">Kanan</button></div></div>
   <div><div class="cap">Pengaturan output</div><div class="toolrow"><button class="tool" id="swapBtn">⇄ Posisi<small>Normal</small></button><button class="tool" id="reverseBtn">↔ Arah<small>Normal</small></button></div></div>
  </div>
  <div class="brrow"><label for="brightness">Kecerahan</label><input id="brightness" type="range" min="1" max="255" value="180"><b id="brightnessVal">180</b></div>
 </section>
 <div class="tabs" id="mainTabs"><button data-view="effects" class="on">Single Effect</button><button data-view="playlists">Playlist</button><button data-view="colors">Warna / Skin</button></div>
 <section class="view on" id="view-effects">
  <div class="panel">
   <div class="bar"><input class="search" id="fxSearch" placeholder="Cari efek WLED..."><button class="barbtn on" id="favFilter">Semua</button></div>
   <div class="fxgrid" id="fxGrid"><div class="empty">Memuat efek dari WLED...</div></div>
  </div>
  <div class="panel">
   <div class="cap">Efek terpilih</div>
   <div class="selected"><span class="swatch" id="currentSwatch"></span><div class="desc"><b id="currentFxName">Memuat...</b><small id="currentFxInfo">Atur warna dan parameter efek</small></div><button class="barbtn" id="saveSingle">Simpan</button></div>
   <div class="sectiontitle">Parameter efek</div><div id="params" class="params"><div class="hint">Parameter mengikuti metadata efek WLED.</div></div>
   <div class="hint" id="paramHint"></div>
   <div class="btnrow"><button class="btn outline" id="openColors">Atur warna</button><button class="btn primary" id="applySingle">Terapkan efek</button></div>
   <div class="field"><label for="durationSec">Durasi slot playlist (detik)</label><input id="durationSec" type="number" inputmode="numeric" min="1" max="600" step="1" value="20"></div>
   <div class="field"><label for="addTarget">Tambahkan efek ini ke playlist</label><div class="btnrow"><select id="addTarget"><option value="0">Playlist 1</option><option value="1">Playlist 2</option><option value="2">Playlist 3</option><option value="3">Playlist 4</option></select><button class="btn primary" id="addToPlaylist">＋ Tambah slot</button></div></div>
  </div>
 </section>
 <section class="view" id="view-playlists">
  <div class="panel"><div class="cap">Playlist Riding · tersimpan di ESP32</div><div class="playgrid" id="playlistGrid"><div class="empty">Memuat playlist...</div></div><div class="hint">Playlist berjalan dari engine ESP32. HP boleh ditutup setelah playlist dimulai.</div></div>
  <div class="panel pldetail" id="playlistDetail"><div class="empty">Pilih playlist untuk mengelola slot.</div></div>
 </section>
 <section class="view" id="view-colors">
  <div class="panel">
   <div class="cap">Warna untuk efek terpilih</div>
   <div class="color-mode" id="colorMode"><button data-mode="solid" class="on">Warna Solid</button><button data-mode="skin">Skin Efek</button></div>
   <div id="solidEditor"><div class="rgbrow"><div class="rgbitem"><label>Warna utama</label><input type="color" id="color0"></div><div class="rgbitem"><label>Warna kedua</label><input type="color" id="color1"></div><div class="rgbitem"><label>Warna ketiga</label><input type="color" id="color2"></div></div><div class="wheelbox"><div class="wheel-target" id="colorTarget"><button type="button" class="on" data-target="0">Utama</button><button type="button" data-target="1">Kedua</button><button type="button" data-target="2">Ketiga</button></div><canvas id="rideColorWheel" width="260" height="260" aria-label="Pilih warna dari roda warna"></canvas><div class="wheel-value"><label for="colorValue">Kecerahan</label><input id="colorValue" type="range" min="0" max="100" value="100"><b id="colorValueText">100%</b></div><div class="wheel-hex" id="colorHex">#FFA500</div></div><div class="quick" id="quickColors"></div></div>
   <div id="skinEditor" class="hide"><div class="skingrid" id="skinGrid"></div></div>
   <div class="btnrow"><button class="btn" id="saveColor">Simpan warna</button><button class="btn outline" id="newSkin">＋ Buat skin baru</button></div>
  </div>
  <div class="panel"><div class="cap">Palette WLED</div><div class="palgrid" id="paletteGrid"><div class="empty">Memuat palette...</div></div><div class="hint">Palette berasal dari firmware WLED yang sedang terpasang.</div></div>
 </section>
</div>
<div id="mzN"></div><div id="toast" class="toast"></div>
<script>
(function(){
'use strict';
const $=id=>document.getElementById(id);
let jqTail=Promise.resolve();
function jfetch(url,opt){const run=()=>fetch(url,opt).then(r=>{if(r.status===503)throw new Error('WLED sibuk');return r;});const task=jqTail.then(async()=>{for(let n=0;n<5;n++){try{return await run();}catch(e){if(n===4)throw e;await new Promise(w=>setTimeout(w,180+100*n));}}});jqTail=task.catch(()=>{});return task;}
const api=(path,method='POST')=>fetch(path,{method,cache:'no-store'}).then(async r=>{let d={};try{d=await r.json();}catch(e){}if(!r.ok||d.ok===false)throw new Error(d.error||('HTTP '+r.status));return d;});
const BLACKLIST=['frizzles','funky plank','game of life','geq','ghost rider','matrix','metaballs','tartan','polar lights','swirl','lissajous','rubix','invaders','maze','pulser','dna','plasma ball','hipnotic','black hole','cubes','fireworks','akemi','colored bursts','freqmatrix'];
const CUSTOM_RE=/^(?:MZ\d\d|W\d\d) /;
let allFx=[],fxData=[],fxList=[],showNames=[],showDurations=[],palNames=[],palStops={},selectedSide='both',sideSwap=false,reverse=false,activeView='effects',showFavorites=false;
let current={kind:'fx',fx:0,showIndex:0,pal:0,colors:[[255,165,0],[255,255,255],[255,26,0]],params:{sx:128,ix:128,c1:128,c2:128,c3:128,o1:0,o2:0,o3:0},duration:20};
let paramCache={};try{paramCache=JSON.parse(localStorage.getItem('mzRideFxParams')||'{}')||{};}catch(e){paramCache={};}
const pagePath=location.pathname;const colorPage=pagePath==='/warna';const welcomingPage=pagePath==='/welcoming';
let playlists=[],openPlaylist=-1,player={active:false,paused:false,playlist:0,slot:0,side:'both'};
let favorites=new Set(),savedColors=[],skins=[];let applyTimer=null,catalogReady=false;
try{favorites=new Set(JSON.parse(localStorage.getItem('mzRideFav')||'[]'));}catch(e){}
try{savedColors=JSON.parse(localStorage.getItem('mzRideColors')||'[]');}catch(e){savedColors=[];}
try{skins=JSON.parse(localStorage.getItem('mzRideSkins')||'[]');}catch(e){skins=[];}
const toast=m=>{const t=$('toast');t.textContent=m;t.classList.add('show');setTimeout(()=>t.classList.remove('show'),1800);};
const esc=s=>String(s??'').replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
const hex=rgb=>'#'+rgb.map(v=>Math.max(0,Math.min(255,v|0)).toString(16).padStart(2,'0')).join('').toUpperCase();
const rgb=h=>{h=String(h||'#000000').replace('#','');const n=parseInt(h,16)||0;return[(n>>16)&255,(n>>8)&255,n&255];};
const colorsOf=p=>Array.isArray(p.col)?p.col:[[p.r??255,p.g??165,p.b??0],[p.r1??255,p.g1??255,p.b1??255],[p.r2??255,p.g2??26,p.b2??0]];
function fxMeta(i){return String(fxData[i]||'').split(';');}
function fxDefs(i){const d={},part=fxMeta(i)[4]||'';part.split(',').forEach(x=>{const a=x.split('=');if(a.length===2&&Number.isFinite(parseInt(a[1],10)))d[a[0].trim()]=parseInt(a[1],10);});return d;}
function fxParams(i){const labels=(fxMeta(i)[0]||'').split(','),names=['Speed','Intensity','Custom 1','Custom 2','Custom 3'],keys=['sx','ix','c1','c2','c3'],sl=[],tg=[];for(let k=0;k<5;k++){let n=labels[k];if(n===undefined||n==='')continue;if(n==='!')n=names[k];sl.push({key:keys[k],label:n||names[k]});}for(let k=0;k<3;k++){let n=labels[k+5];if(n===undefined||n==='')continue;if(n==='!')n='Opsi '+(k+1);tg.push({key:'o'+(k+1),label:n});}return{sl,tg};}
function fxOk(i,n){const t=String(n||'').toLowerCase(),flags=fxMeta(i)[3]||'';if(!t||t==='rsvd'||t==='-'||CUSTOM_RE.test(n)||String(n).startsWith('2D')||BLACKLIST.includes(t))return false;if(flags.includes('2')&&!flags.includes('1')&&!flags.includes('0'))return false;if(flags.includes('v')||flags.includes('f'))return false;return true;}
function paramsDefaults(i){const d=fxDefs(i);return{sx:d.sx??128,ix:d.ix??128,c1:d.c1??128,c2:d.c2??128,c3:d.c3??128,o1:d.o1??0,o2:d.o2??0,o3:d.o3??0};}
function selectedSegId(side){const right=side==='kanan';return (right!==sideSwap)?0:1;}
function segmentIds(){if(selectedSide==='kanan')return[selectedSegId('kanan')];if(selectedSide==='kiri')return[selectedSegId('kiri')];return[selectedSegId('kanan'),selectedSegId('kiri')];}
function makeStateSegment(id){return{id,on:true,fx:current.fx,pal:current.pal,rev:reverse,sx:current.params.sx,ix:current.params.ix,c1:current.params.c1,c2:current.params.c2,c3:current.params.c3,o1:current.params.o1,o2:current.params.o2,o3:current.params.o3,col:current.colors};}
async function applyCurrent(silent=false,restartShow=true){try{if(player.active){await api('/mizuma/riding/playlist/stop?restore=0');player.active=false;await new Promise(r=>setTimeout(r,100));}
if(current.kind==='show'){
  const base=segmentIds().map(id=>({id,on:true,fx:0,pal:current.pal,rev:reverse,col:current.colors}));
  await jfetch('/json/state',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({on:true,seg:base})});
  if(restartShow){await api('/mizuma/show?index='+current.showIndex+'&side='+encodeURIComponent(selectedSide));if(current.pal!==0)await api('/mizuma/show/palette?pal='+current.pal+'&side='+encodeURIComponent(selectedSide));}
  else if(current.pal!==0){await api('/mizuma/show/palette?pal='+current.pal+'&side='+encodeURIComponent(selectedSide));}
}else{
  await api('/mizuma/show/stop?side='+encodeURIComponent(selectedSide));
  await jfetch('/json/state',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({on:true,seg:segmentIds().map(makeStateSegment)})});
}
if(!silent)toast('Efek diterapkan');}catch(e){toast('Gagal menerapkan efek: '+e.message);}}
function saveQuery(slot){const p=new URLSearchParams({slot,fx:current.kind==='show'?0:current.fx,isShow:current.kind==='show'?1:0,showIndex:current.kind==='show'?current.showIndex:0,pal:current.pal,r:current.colors[0][0],g:current.colors[0][1],b:current.colors[0][2],r1:current.colors[1][0],g1:current.colors[1][1],b1:current.colors[1][2],r2:current.colors[2][0],g2:current.colors[2][1],b2:current.colors[2][2],sx:current.params.sx,ix:current.params.ix,c1:current.params.c1,c2:current.params.c2,c3:current.params.c3,o1:current.params.o1,o2:current.params.o2,o3:current.params.o3,bri:parseInt($('brightness').value,10)||180});return p;}
async function saveSingle(){const prefix=welcomingPage?'welcoming':'riding';const slots=selectedSide==='both'?[prefix+'Kanan',prefix+'Kiri']:[prefix+(selectedSide==='kanan'?'Kanan':'Kiri')];try{for(const s of slots)await api('/mizuma/preset?'+saveQuery(s));toast('Preset '+(welcomingPage?'Welcoming':'Riding')+' tersimpan di ESP32');}catch(e){toast('Gagal menyimpan preset');}}
function paramCacheSave(){
  if(current.kind!=='fx')return;
  paramCache[current.fx]=Object.assign({},current.params);
  try{localStorage.setItem('mzRideFxParams',JSON.stringify(paramCache));}catch(e){}
}
function cacheCurrentFx(){
  if(current.kind!=='fx')return;
  paramCacheSave();
  try{const d=JSON.parse(localStorage.getItem('mzRideFxColors')||'{}');d[current.fx]={pal:current.pal,colors:current.colors.map(c=>c.slice())};localStorage.setItem('mzRideFxColors',JSON.stringify(d));}catch(e){}
}
const FX_DEFAULT_PALETTES={'Fire 2012':'Fire','Fireworks 1D':'Rainbow','Fireworks':'Rainbow','Noise 1':'Pastel','Noise':'Pastel','Pride':'Pride','Aurora':'Aurora','Pacifica':'Pacifica'};
function defaultPalette(i){const d=fxDefs(i);if(d.pal!=null)return d.pal;const n=FX_DEFAULT_PALETTES[allFx[i]];if(n){const p=palNames.findIndex(x=>x===n);if(p>=0)return p;}return 0;}
function loadFxCache(i){
  current.params=Object.assign({},paramsDefaults(i),paramCache[i]||{});
  current.pal=defaultPalette(i);
  try{const d=JSON.parse(localStorage.getItem('mzRideFxColors')||'{}')[i];if(d){current.pal=d.pal??current.pal;if(Array.isArray(d.colors)&&d.colors.length===3)current.colors=colorsOf({col:d.colors});}}catch(e){}
}
const WELCOME_NATIVE_FX=[
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
let welcomeNativeList=[];
function isFavoriteEntry(kind,id){return favorites.has(kind==='show'?'show:'+id:'fx:'+id)||(kind==='fx'&&favorites.has(id));}
function toggleFavoriteEntry(kind,id){
  const key=kind==='show'?'show:'+id:'fx:'+id;
  if(isFavoriteEntry(kind,id)){favorites.delete(key);if(kind==='fx')favorites.delete(id);}else favorites.add(key);
  try{localStorage.setItem('mzRideFav',JSON.stringify([...favorites]));}catch(e){}
}
function renderEffectButton(kind,id,name,sub,isSelected){
  const fav=isFavoriteEntry(kind,id),key=kind==='show'?'show:'+id:'fx:'+id;
  return '<button class="fx'+(isSelected?' on':'')+'" data-select-kind="'+kind+'" data-select-id="'+id+'"><b>'+esc(name)+'</b><small>'+esc(sub)+'</small><span class="fav'+(fav?' on':'')+'" data-fav="'+id+'" data-fav-kind="'+kind+'">'+(fav?'★':'☆')+'</span></button>';
}
function renderEffects(){
  const q=$('fxSearch').value.trim().toLowerCase();
  const groups=[];
  const ordinary=welcomingPage?[]:fxList.filter(x=>!welcomeNativeList.some(w=>w.id===x.id)).filter(x=>(!q||x.name.toLowerCase().includes(q))&&(!showFavorites||isFavoriteEntry('fx',x.id)));
  if(ordinary.length)groups.push({title:'Efek WLED',items:ordinary.map(x=>renderEffectButton('fx',x.id,x.name,(fxParams(x.id).sl.length+fxParams(x.id).tg.length)+' parameter',current.kind==='fx'&&x.id===current.fx))});
  const native=welcomeNativeList.filter(x=>(!q||x.name.toLowerCase().includes(q))&&(!showFavorites||isFavoriteEntry('fx',x.id)));
  if(native.length)groups.push({title:'Efek Native Welcoming',items:native.map(x=>renderEffectButton('fx',x.id,x.name,(fxParams(x.id).sl.length+fxParams(x.id).tg.length)+' parameter',current.kind==='fx'&&x.id===current.fx))});
  const shows=showNames.map((name,id)=>({id,name})).filter(x=>(!q||x.name.toLowerCase().includes(q))&&(!showFavorites||isFavoriteEntry('show',x.id)));
  if(shows.length)groups.push({title:'Animasi Welcoming Mizuma',items:shows.map(x=>renderEffectButton('show',x.id,x.name,((showDurations[x.id]||0)/1000).toFixed(1)+' detik · multi-stage',current.kind==='show'&&x.id===current.showIndex))});
  $('fxGrid').innerHTML=groups.length?groups.map(g=>'<div class="catalogGroup"><div class="catalogTitle">'+g.title+' <small>'+g.items.length+'</small></div><div class="fxgrid">'+g.items.join('')+'</div></div>').join(''):'<div class="empty">Tidak ada efek yang cocok.</div>';
}
function renderSelected(){
  const name=current.kind==='show'?(showNames[current.showIndex]||('Animasi Mizuma '+(current.showIndex+1))):(allFx[current.fx]||'Efek '+current.fx);
  $('currentFxName').textContent=name;
  $('currentFxInfo').textContent=(current.kind==='show'?'Animasi Welcoming · '+(((showDurations[current.showIndex]||0)/1000).toFixed(1))+' detik':(palNames[current.pal]||'Palette '+current.pal))+' · '+({both:'Semua sisi',kanan:'Kanan',kiri:'Kiri'}[selectedSide]);
  $('currentSwatch').style.background='linear-gradient(90deg,'+current.colors.map(hex).join(',')+')';
  $('color0').value=hex(current.colors[0]);$('color1').value=hex(current.colors[1]);$('color2').value=hex(current.colors[2]);
  $('durationSec').value=current.duration||20;
  $('durationSec').disabled=false;
  $('durationSec').parentElement.querySelector('label').textContent=current.kind==='show'?'Durasi slot animasi (detik)':'Durasi slot playlist (detik)';
  renderEffects();renderParams();paintRideWheel();
}
function renderParams(){
  const box=$('params');
  if(current.kind==='show'){
    box.innerHTML='<div class="hint">Animasi ini memakai parameter internal dari engine Mizuma. Durasi dan urutan tahap mengikuti resep animasi.</div>';
    $('paramHint').textContent='Warna dan palette tetap dapat disesuaikan. Parameter tahap animasi tidak diubah dari panel WLED.';return;
  }
  const p=fxParams(current.fx);let h=p.sl.map(x=>'<div class="param"><label><span>'+esc(x.label)+'</span><b id="v_'+x.key+'">'+current.params[x.key]+'</b></label><input type="range" min="0" max="255" value="'+current.params[x.key]+'" data-param="'+x.key+'"></div>').join('');
  p.tg.forEach(x=>{h+='<div class="switchrow"><span>'+esc(x.label)+'</span><button class="switch'+(current.params[x.key]?' on':'')+'" data-toggle="'+x.key+'" aria-label="'+esc(x.label)+'"></button></div>';});
  box.innerHTML=h||'<div class="hint">Efek ini tidak memiliki parameter tambahan.</div>';
  $('paramHint').textContent='Parameter mengikuti metadata efek asli WLED. Nilai parameter disimpan terpisah untuk setiap efek dan dipulihkan saat efek dipilih kembali.';
}
function selectEffect(i,apply=true){clearTimeout(applyTimer);cacheCurrentFx();current.kind='fx';current.fx=i;loadFxCache(i);renderSelected();if(apply)applyCurrent(true);}
function selectShow(i,apply=true){clearTimeout(applyTimer);cacheCurrentFx();current.kind='show';current.showIndex=i;current.fx=0;current.duration=Math.max(1,Math.ceil((showDurations[i]||20000)/1000));renderSelected();if(apply)applyCurrent(true,true);}
function renderPalettes(){const list=palNames.map((n,i)=>({n,i})).filter(p=>p.n&&p.n!=='r');$('paletteGrid').innerHTML=list.map(p=>'<button class="pal'+(current.pal===p.i?' on':'')+'" data-pal="'+p.i+'"><i style="background:'+paletteGradient(p.i)+'"></i><b>'+esc(p.n)+'</b></button>').join('')||'<div class="empty">Palette tidak ditemukan.</div>';}
function paletteGradient(i){const a=palStops[i];if(!a||!a.length)return'linear-gradient(90deg,#222,#888)';const st=a.map(x=>{if(Array.isArray(x))return'rgba('+x[1]+','+x[2]+','+x[3]+','+(x[4]??255)/255+') '+(x[0]/255*100)+'%';if(x==='r')return'#888';const k=String(x).toLowerCase(),n=parseInt(k.slice(1),10)-1,c=current.colors[n]||[128,128,128];return' rgb('+c.join(',')+')';});return'linear-gradient(90deg,'+st.join(',')+')';}
async function loadPalStops(page=0){try{const r=await jfetch('/json/palx?page='+page);const d=await r.json();Object.keys(d.p||{}).forEach(k=>palStops[k]=d.p[k]);if(typeof d.m==='number'&&page<d.m)await loadPalStops(page+1);else renderPalettes();}catch(e){}}
function renderSkins(){const built=savedColors.map((c,i)=>({name:'Warna '+(i+1),colors:c,kind:'color',i}));const custom=skins.map((c,i)=>({name:c.name,colors:c.colors,kind:'skin',i}));const all=[...custom,...built];$('skinGrid').innerHTML=all.length?all.map(x=>'<button class="skin" data-skind="'+x.kind+'" data-si="'+x.i+'"><span class="swatches">'+x.colors.map(c=>'<i style="background:'+hex(c)+'"></i>').join('')+'</span><b>'+esc(x.name)+'</b><small>'+(x.kind==='skin'?'Skin custom':'Warna tersimpan')+'</small></button>').join(''):'<div class="empty">Belum ada warna atau skin yang disimpan.</div>';}
function renderPlaylists(){if(!playlists.length){$('playlistGrid').innerHTML='<div class="empty">Belum ada playlist.</div>';return;}$('playlistGrid').innerHTML=playlists.map((p,i)=>{const cols=p.slots.flatMap(x=>colorsOf(x)).slice(0,6);const strip=cols.length?cols.map(c=>'<i style="background:'+hex(c)+'"></i>').join(''):'<i style="background:#222"></i>';return'<button class="plcard'+(i===openPlaylist?' on':'')+'" data-playlist="'+i+'"><span class="plthumb">'+strip+'</span><b>'+esc(p.name||('Playlist '+(i+1)))+'</b><small>'+p.count+'/12 slot</small>'+(player.active&&player.playlist===i?'<span class="plstatus">● '+(player.paused?'DIJEDA':'SEDANG BERJALAN')+'</span>':'')+'</button>';}).join('');renderPlaylistDetail();}
function renderPlaylistDetail(){const box=$('playlistDetail');if(openPlaylist<0||!playlists[openPlaylist]){box.innerHTML='<div class="empty">Pilih playlist untuk mengelola slot.</div>';return;}const p=playlists[openPlaylist];let rows=p.slots.map((s,j)=>{const nm=s.isShow?(showNames[s.showIndex]||('Animasi Mizuma '+(s.showIndex+1))):(allFx[s.fx]||('Efek '+s.fx));return '<div class="slotrow"><span class="n">'+(j+1)+'</span><span class="swatches">'+colorsOf(s).map(c=>'<i style="background:'+hex(c)+'"></i>').join('')+'</span><span class="meta"><b>'+esc(nm)+'</b><small>'+(s.isShow?'Animasi Mizuma':'Efek WLED')+' · '+s.duration+' detik</small></span><button data-move="up" data-slot="'+j+'" '+(!j?'disabled':'')+'>↑</button><button data-move="down" data-slot="'+j+'" '+(j===p.slots.length-1?'disabled':'')+'>↓</button><button class="edit" data-edit-slot="'+j+'">Edit</button><button data-remove-slot="'+j+'">×</button></div>';}).join('');box.innerHTML='<div class="plhead"><b>Kelola playlist</b><span class="pill"><span class="statusdot"></span>'+p.count+'/12 slot</span></div><div class="plname"><input id="playlistName" type="text" maxlength="24" value="'+esc(p.name||('Playlist '+(openPlaylist+1)))+'"><button class="barbtn" id="renamePlaylist">Simpan</button></div><div class="plactions"><button class="play" id="playPlaylist">▶ Putar</button><button id="pausePlaylist">'+(player.active&&player.playlist===openPlaylist&&!player.paused?'Ⅱ Jeda':'▶ Lanjut')+'</button><button id="stopPlaylist">■ Stop</button></div><div class="sectiontitle">Urutan slot</div>'+(rows||'<div class="empty">Playlist masih kosong. Tambahkan efek dari Single Effect.</div>')+'<div class="btnrow"><button class="btn" id="addSelectedSlot" '+(p.count>=12?'disabled':'')+'>＋ Tambah efek terpilih</button><button class="btn" id="clearPlaylist">Kosongkan</button></div><div class="hint">Geser urutan slot dengan tombol panah. Durasi setiap slot diatur saat menambahkan atau mengedit.</div>';
}
function setView(v){activeView=v;document.querySelectorAll('#mainTabs button').forEach(b=>b.classList.toggle('on',b.dataset.view===v));document.querySelectorAll('.view').forEach(x=>x.classList.toggle('on',x.id==='view-'+v));if(v==='colors'){renderPalettes();renderSkins();}}
async function refreshPlaylists(){try{const d=await fetch('/mizuma/riding/playlists',{cache:'no-store'}).then(r=>r.json());playlists=d.playlists||[];player={active:!!d.active,paused:!!d.paused,playlist:d.playlist||0,slot:d.slot||0,side:d.side||'both'};renderPlaylists();}catch(e){$('playlistGrid').innerHTML='<div class="empty">Gagal membaca playlist engine.</div>';}}
async function saveSlot(index,slot){current.duration=Math.max(1,Math.min(600,parseInt($('durationSec').value,10)||20));$('durationSec').value=current.duration;const q=new URLSearchParams({index,slot,fx:current.kind==='show'?0:current.fx,isShow:current.kind==='show'?1:0,showIndex:current.kind==='show'?current.showIndex:0,pal:current.pal,r:current.colors[0][0],g:current.colors[0][1],b:current.colors[0][2],r1:current.colors[1][0],g1:current.colors[1][1],b1:current.colors[1][2],r2:current.colors[2][0],g2:current.colors[2][1],b2:current.colors[2][2],sx:current.params.sx,ix:current.params.ix,c1:current.params.c1,c2:current.params.c2,c3:current.params.c3,o1:current.params.o1,o2:current.params.o2,o3:current.params.o3,duration:current.duration,bri:$('brightness').value});await api('/mizuma/riding/playlist/slot?'+q);await refreshPlaylists();toast('Slot tersimpan di ESP32');}
let colorTarget=0,rideWheelBase=null;
function rideHsvToRgb(h,s,v){const c=v*s,x=c*(1-Math.abs((h/60)%2-1)),m=v-c;let r=0,g=0,b=0;if(h<60){r=c;g=x;}else if(h<120){r=x;g=c;}else if(h<180){g=c;b=x;}else if(h<240){g=x;b=c;}else if(h<300){r=x;b=c;}else{r=c;b=x;}return[Math.round((r+m)*255),Math.round((g+m)*255),Math.round((b+m)*255)];}
function rideRgbToHsv(c){const r=c[0]/255,g=c[1]/255,b=c[2]/255,mx=Math.max(r,g,b),mn=Math.min(r,g,b),d=mx-mn;let h=0;if(d){if(mx===r)h=((g-b)/d)%6;else if(mx===g)h=(b-r)/d+2;else h=(r-g)/d+4;h=(h*60+360)%360;}return[h,mx?d/mx:0,mx];}
function drawRideWheel(){const cv=$('rideColorWheel');if(!cv)return;const ctx=cv.getContext('2d'),n=cv.width,r=n/2-2,img=ctx.createImageData(n,n);for(let y=0;y<n;y++)for(let x=0;x<n;x++){const dx=x-n/2,dy=y-n/2,d=Math.hypot(dx,dy),k=(y*n+x)*4;if(d<=r){const col=rideHsvToRgb((Math.atan2(dy,dx)*180/Math.PI+360)%360,Math.min(1,d/r),1);img.data[k]=col[0];img.data[k+1]=col[1];img.data[k+2]=col[2];img.data[k+3]=255;}}rideWheelBase=img;paintRideWheel();}
function paintRideWheel(){const cv=$('rideColorWheel');if(!cv||!rideWheelBase)return;const ctx=cv.getContext('2d'),n=cv.width;ctx.putImageData(rideWheelBase,0,0);const hsv=rideRgbToHsv(current.colors[colorTarget]),r=n/2-4,d=hsv[1]*r,a=hsv[0]*Math.PI/180,x=n/2+Math.cos(a)*d,y=n/2+Math.sin(a)*d;ctx.beginPath();ctx.arc(x,y,7,0,Math.PI*2);ctx.fillStyle=hex(current.colors[colorTarget]);ctx.fill();ctx.lineWidth=2;ctx.strokeStyle='#fff';ctx.stroke();$('colorValue').value=Math.round(hsv[2]*100);$('colorValueText').textContent=Math.round(hsv[2]*100)+'%';$('colorHex').textContent=hex(current.colors[colorTarget]);}
function updateWheelPoint(e){const cv=$('rideColorWheel'),rect=cv.getBoundingClientRect(),n=cv.width,x=(e.clientX-rect.left)*n/rect.width,y=(e.clientY-rect.top)*n/rect.height,dx=x-n/2,dy=y-n/2,r=n/2-4,d=Math.min(r,Math.hypot(dx,dy)),h=(Math.atan2(dy,dx)*180/Math.PI+360)%360,s=Math.min(1,d/r),v=(+$('colorValue').value)/100;current.pal=0;current.colors[colorTarget]=rideHsvToRgb(h,s,v);syncRideColor(true);}
function syncRideColor(apply){for(let i=0;i<3;i++)$('color'+i).value=hex(current.colors[i]);$('currentSwatch').style.background='linear-gradient(90deg,'+current.colors.map(hex).join(',')+')';document.querySelectorAll('#paletteGrid [data-pal]').forEach(b=>b.classList.toggle('on',+b.dataset.pal===current.pal));paintRideWheel();cacheCurrentFx();if(apply){clearTimeout(applyTimer);applyTimer=setTimeout(()=>applyCurrent(true,true),130);}}
function updateColorInputs(){for(let i=0;i<3;i++)$('color'+i).value=hex(current.colors[i]);renderSelected();renderPalettes();paintRideWheel();}
function applyPalette(i){current.pal=i;cacheCurrentFx();renderPalettes();renderSelected();applyCurrent(true,true);}
function setSide(s,skipPreset=false){selectedSide=s;document.querySelectorAll('#sideSelect button').forEach(b=>b.classList.toggle('on',b.dataset.side===s));$('currentFxInfo').textContent=(palNames[current.pal]||'Palette')+' · '+({both:'Semua sisi',kanan:'Kanan',kiri:'Kiri'}[s]);if(catalogReady&&!skipPreset)loadRidingPreset(s);}
async function loadRidingPreset(side){try{const d=await fetch('/mizuma/presets',{cache:'no-store'}).then(r=>r.json());const prefix=welcomingPage?'welcoming':'riding';const p=d[prefix+(side==='kiri'?'Kiri':'Kanan')];if(!p)return;current.kind=p.isShow?'show':'fx';current.showIndex=p.showIndex??0;current.fx=p.fx??0;current.pal=p.pal??0;current.colors=colorsOf(p);current.params={sx:p.sx??128,ix:p.ix??128,c1:p.c1??128,c2:p.c2??128,c3:p.c3??128,o1:p.o1?1:0,o2:p.o2?1:0,o3:p.o3?1:0};if(current.kind==='fx'){paramCache[current.fx]=Object.assign({},current.params);cacheCurrentFx();}renderSelected();renderPalettes();}catch(e){toast('Preset '+(welcomingPage?'Welcoming':'Riding')+' belum dapat dibaca');}}
async function loadSwitches(){try{const a=await fetch('/mizuma/side-swap').then(r=>r.json());sideSwap=!!a.enabled;}catch(e){}try{const a=await fetch('/mizuma/show/reverse').then(r=>r.json());reverse=!!a.enabled;}catch(e){}$('swapBtn').classList.toggle('on',sideSwap);$('swapBtn').querySelector('small').textContent=sideSwap?'Ditukar':'Normal';$('reverseBtn').classList.toggle('on',reverse);$('reverseBtn').querySelector('small').textContent=reverse?'Dibalik':'Normal';}
async function loadWled(){try{const [e,f,p,state,saved,shows,durations]=await Promise.all([jfetch('/json/eff').then(r=>r.json()),jfetch('/json/fxdata').then(r=>r.json()),jfetch('/json/pal').then(r=>r.json()),jfetch('/json/state').then(r=>r.json()),fetch('/mizuma/presets',{cache:'no-store'}).then(r=>r.json()),fetch('/mizuma/shows',{cache:'no-store'}).then(r=>r.json()),fetch('/mizuma/show-durations',{cache:'no-store'}).then(r=>r.json())]);allFx=e;fxData=f;palNames=p;showNames=shows||[];showDurations=durations||[];fxList=allFx.map((name,id)=>({id,name})).filter(x=>fxOk(x.id,x.name)).sort((a,b)=>a.id===0?-1:b.id===0?1:a.name.localeCompare(b.name));welcomeNativeList=WELCOME_NATIVE_FX.map(spec=>{let id=allFx.findIndex(n=>String(n).toLowerCase()===spec.match.toLowerCase());if(id<0&&spec.fallback)id=allFx.findIndex(n=>String(n).toLowerCase()===spec.fallback.toLowerCase());return id>=0?{id,name:spec.label}:null;}).filter(Boolean);const seg=state.seg||[];const id=selectedSegId('kanan'),live=seg.find(x=>x&&x.id===id)||seg[id],presetKey=welcomingPage?'welcomingKanan':'ridingKanan',s=saved[presetKey]||live;if(s){current.kind=s.isShow?'show':'fx';current.showIndex=s.showIndex??0;current.fx=s.fx??0;current.pal=s.pal??0;current.colors=colorsOf(s);current.params={sx:s.sx??128,ix:s.ix??128,c1:s.c1??128,c2:s.c2??128,c3:s.c3??128,o1:s.o1?1:0,o2:s.o2?1:0,o3:s.o3?1:0};if(current.kind==='fx'){paramCache[current.fx]=Object.assign({},current.params);cacheCurrentFx();}}if(state.bri){$('brightness').value=state.bri;$('brightnessVal').textContent=state.bri;}catalogReady=true;renderSelected();renderPalettes();loadPalStops();}catch(e){$('fxGrid').innerHTML='<div class="empty">Tidak dapat membaca katalog WLED. Pastikan WLED API aktif.</div>';toast('Gagal memuat katalog WLED');}}
function bind(){
$('mainTabs').addEventListener('click',e=>{const b=e.target.closest('button[data-view]');if(b)setView(b.dataset.view);});
$('sideSelect').addEventListener('click',e=>{const b=e.target.closest('button[data-side]');if(b)setSide(b.dataset.side);});
$('swapBtn').addEventListener('click',async()=>{try{const d=await api('/mizuma/side-swap?enabled='+(sideSwap?'0':'1'));sideSwap=!!d.enabled;$('swapBtn').classList.toggle('on',sideSwap);$('swapBtn').querySelector('small').textContent=sideSwap?'Ditukar':'Normal';toast('Pemetaan sisi diperbarui');}catch(e){toast('Gagal mengubah posisi output');}});
$('reverseBtn').addEventListener('click',async()=>{try{const d=await api('/mizuma/show/reverse?enabled='+(reverse?'0':'1'));reverse=!!d.enabled;$('reverseBtn').classList.toggle('on',reverse);$('reverseBtn').querySelector('small').textContent=reverse?'Dibalik':'Normal';toast('Arah efek diperbarui');}catch(e){toast('Tidak ada segmen aktif atau gagal mengubah arah');}});
$('brightness').addEventListener('input',e=>{$('brightnessVal').textContent=e.target.value;clearTimeout(applyTimer);applyTimer=setTimeout(()=>{api('/mizuma/bri?v='+e.target.value).catch(()=>{});jfetch('/json/state',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({bri:+e.target.value})}).catch(()=>{});},100);});
$('durationSec').addEventListener('change',e=>{current.duration=Math.max(1,Math.min(600,parseInt(e.target.value,10)||20));e.target.value=current.duration;});
$('fxSearch').addEventListener('input',renderEffects);$('favFilter').addEventListener('click',()=>{showFavorites=!showFavorites;$('favFilter').classList.toggle('on',showFavorites);$('favFilter').textContent=showFavorites?'Favorit':'Semua';renderEffects();});
$('fxGrid').addEventListener('click',e=>{const star=e.target.closest('[data-fav]');if(star){const key=star.dataset.favKind==='show'?'show:'+star.dataset.fav:+star.dataset.fav;favorites.has(key)?favorites.delete(key):favorites.add(key);localStorage.setItem('mzRideFav',JSON.stringify([...favorites]));renderEffects();return;}const fx=e.target.closest('[data-select-kind]');if(fx){const i=+fx.dataset.selectId;if(fx.dataset.selectKind==='show')selectShow(i);else selectEffect(i);}});
$('params').addEventListener('input',e=>{const k=e.target.dataset.param;if(!k)return;current.params[k]=+e.target.value;paramCacheSave();$('v_'+k).textContent=e.target.value;clearTimeout(applyTimer);applyTimer=setTimeout(()=>{applyCurrent(true);},80);});
$('params').addEventListener('click',e=>{const b=e.target.closest('[data-toggle]');if(!b)return;const k=b.dataset.toggle;current.params[k]=current.params[k]?0:1;paramCacheSave();b.classList.toggle('on',!!current.params[k]);applyCurrent(true);});
$('applySingle').addEventListener('click',()=>applyCurrent());$('saveSingle').addEventListener('click',saveSingle);$('openColors').addEventListener('click',()=>setView('colors'));
$('colorMode').addEventListener('click',e=>{const b=e.target.closest('[data-mode]');if(!b)return;document.querySelectorAll('#colorMode button').forEach(x=>x.classList.toggle('on',x===b));$('solidEditor').classList.toggle('hide',b.dataset.mode!=='solid');$('skinEditor').classList.toggle('hide',b.dataset.mode!=='skin');});
['color0','color1','color2'].forEach((id,i)=>$(id).addEventListener('input',e=>{colorTarget=i;current.colors[i]=rgb(e.target.value);current.pal=0;document.querySelectorAll('#colorTarget button').forEach(b=>b.classList.toggle('on',+b.dataset.target===i));syncRideColor(true);}));
$('colorTarget').addEventListener('click',e=>{const b=e.target.closest('[data-target]');if(!b)return;colorTarget=+b.dataset.target;document.querySelectorAll('#colorTarget button').forEach(x=>x.classList.toggle('on',x===b));paintRideWheel();});
$('rideColorWheel').addEventListener('pointerdown',e=>{e.currentTarget.setPointerCapture(e.pointerId);updateWheelPoint(e);});$('rideColorWheel').addEventListener('pointermove',e=>{if(e.buttons)updateWheelPoint(e);});
$('colorValue').addEventListener('input',e=>{const hsv=rideRgbToHsv(current.colors[colorTarget]);current.pal=0;current.colors[colorTarget]=rideHsvToRgb(hsv[0],hsv[1],(+e.target.value)/100);syncRideColor(true);});
$('quickColors').innerHTML=['#FF0000','#FFA500','#FFD200','#FFFFFF','#00AEEF','#00C853','#7C4DFF','#FF4081'].map(c=>'<button data-color="'+c+'" style="background:'+c+'" aria-label="'+c+'"></button>').join('');$('quickColors').addEventListener('click',e=>{const b=e.target.closest('[data-color]');if(!b)return;current.colors[colorTarget]=rgb(b.dataset.color);current.pal=0;syncRideColor(true);});
$('saveColor').addEventListener('click',()=>{savedColors.unshift(current.colors.map(c=>c.slice()));savedColors=savedColors.slice(0,8);localStorage.setItem('mzRideColors',JSON.stringify(savedColors));renderSkins();toast('Warna disimpan');});
$('newSkin').addEventListener('click',()=>{const name=prompt('Nama skin baru:','Skin Riding '+(skins.length+1));if(name===null)return;const n=name.trim().slice(0,24);if(!n)return toast('Nama skin belum diisi');skins.unshift({name:n,colors:current.colors.map(c=>c.slice())});skins=skins.slice(0,12);localStorage.setItem('mzRideSkins',JSON.stringify(skins));renderSkins();toast('Skin baru dibuat');});
$('skinGrid').addEventListener('click',e=>{const b=e.target.closest('[data-skind]');if(!b)return;const a=b.dataset.skind==='skin'?skins:savedColors,c=a[+b.dataset.si];if(!c)return;current.colors=c.colors?c.colors.map(x=>x.slice()):c.map(x=>x.slice());current.pal=0;updateColorInputs();cacheCurrentFx();applyCurrent(true,true);});
$('paletteGrid').addEventListener('click',e=>{const b=e.target.closest('[data-pal]');if(b)applyPalette(+b.dataset.pal);});
$('addToPlaylist').addEventListener('click',async()=>{const i=+$('addTarget').value;const edit=$('addToPlaylist').dataset.edit;const at=edit!==undefined?+edit:(playlists[i]?.count||0);try{await saveSlot(i,at);delete $('addToPlaylist').dataset.edit;$('addToPlaylist').textContent='＋ Tambah slot';openPlaylist=i;setView('playlists');}catch(e){toast('Gagal menyimpan slot: '+e.message);}});
$('playlistGrid').addEventListener('click',e=>{const b=e.target.closest('[data-playlist]');if(!b)return;openPlaylist=+b.dataset.playlist;renderPlaylists();});
$('playlistDetail').addEventListener('click',async e=>{const b=e.target.closest('button');if(!b)return;const i=openPlaylist,p=playlists[i];try{
if(b.id==='renamePlaylist'){await api('/mizuma/riding/playlist/meta?index='+i+'&name='+encodeURIComponent($('playlistName').value));toast('Nama playlist disimpan');await refreshPlaylists();}
else if(b.id==='playPlaylist'){await api('/mizuma/riding/playlist/start?index='+i+'&side='+selectedSide);await new Promise(r=>setTimeout(r,120));await refreshPlaylists();toast('Playlist dimulai dari engine ESP32');}
else if(b.id==='pausePlaylist'){await api('/mizuma/riding/playlist/pause?paused='+(player.paused?'0':'1'));await new Promise(r=>setTimeout(r,80));await refreshPlaylists();}
else if(b.id==='stopPlaylist'){await api('/mizuma/riding/playlist/stop');await new Promise(r=>setTimeout(r,100));await refreshPlaylists();toast('Playlist dihentikan');}
else if(b.id==='addSelectedSlot'){await saveSlot(i,p.count);}
else if(b.id==='clearPlaylist'){if(confirm('Kosongkan seluruh slot playlist ini?')){await api('/mizuma/riding/playlist/clear?index='+i);await new Promise(r=>setTimeout(r,100));await refreshPlaylists();}}
else if(b.dataset.removeSlot!==undefined){await api('/mizuma/riding/playlist/slot?index='+i+'&slot='+b.dataset.removeSlot+'&remove=1');await refreshPlaylists();}
else if(b.dataset.move){const from=+b.dataset.slot,to=from+(b.dataset.move==='up'?-1:1);await api('/mizuma/riding/playlist/reorder?index='+i+'&from='+from+'&to='+to);await refreshPlaylists();}
else if(b.dataset.editSlot!==undefined){const s=p.slots[+b.dataset.editSlot];if(!s)return;current.kind=s.isShow?'show':'fx';current.showIndex=s.showIndex||0;current.fx=s.fx||0;current.pal=s.pal;current.colors=colorsOf(s);current.params={sx:s.sx,ix:s.ix,c1:s.c1,c2:s.c2,c3:s.c3,o1:s.o1,o2:s.o2,o3:s.o3};if(current.kind==='fx'){paramCache[current.fx]=Object.assign({},current.params);cacheCurrentFx();}current.duration=s.duration||20;renderSelected();setView('effects');$('addTarget').value=i;$('durationSec').value=current.duration;$('addToPlaylist').textContent='Simpan perubahan slot';$('addToPlaylist').dataset.edit=String(b.dataset.editSlot);toast('Ubah efek/warna, lalu simpan kembali ke playlist');}
}catch(err){toast('Aksi playlist gagal: '+err.message);}});

}
async function init(){drawRideWheel();setSide('both',true);bind();await loadSwitches();await loadWled();await refreshPlaylists();renderSkins();if(welcomingPage){$('ridingTitle').textContent='Welcoming';$('ridingSubtitle').textContent='Pilih animasi pembuka, efek native, dan atur warna';$('ridingPageBadge').textContent='Animasi / Efek Native';$('fxSearch').placeholder='Cari animasi atau efek Welcoming...';document.querySelector('#mainTabs button[data-view=effects]').textContent='Efek';document.querySelector('#mainTabs button[data-view=colors]').textContent='Warna';document.querySelector('#mainTabs button[data-view=playlists]').style.display='none';$('view-playlists').style.display='none';$('mainTabs').style.gridTemplateColumns='repeat(2,minmax(0,1fr))';$('durationSec').parentElement.style.display='none';$('addTarget').parentElement.parentElement.style.display='none';}if(colorPage){$('ridingTitle').textContent='Warna';$('ridingSubtitle').textContent='Atur warna solid, skin, dan palette';$('ridingPageBadge').style.display='none';$('mainTabs').style.display='none';setView('colors');}setInterval(()=>{if(activeView==='playlists'&&!welcomingPage&&document.activeElement?.id!=='playlistName')refreshPlaylists();},4000);}
init();
})();
</script>
<script>
(function(){
function inject(url,done){fetch(url).then(r=>r.text()).then(done).catch(()=>{});}
inject('/mizuma/frag/header',t=>{const e=document.getElementById('mzH');if(e)e.outerHTML=t;inject('/mizuma/frag/nav',n=>{const x=document.getElementById('mzN');if(x)x.outerHTML=n;const a=document.querySelector('.bottomnav a[href="'+location.pathname+'"]');if(a)a.classList.add('active');inject('/mizuma/frag/script',s=>{try{(new Function(s))();}catch(e){}});});});
})();
</script>
</body></html>
)rawliteral";

// --------------------------------------------------------------------------------------------------------------------------------------
// Perender halaman + pendaftaran rute UI
// --------------------------------------------------------------------------------------------------------------------------------------
inline void mizumaApplyCommon(String &html, const char *activeKey, const MizumaVehicleInfo &v) {
  html.replace("%HEADER%", FPSTR(MIZUMA_HEADER_HTML));
  String navHtml = FPSTR(MIZUMA_BOTTOMNAV_HTML);
  navHtml.replace("__ACTIVE_BERANDA__",   strcmp(activeKey, "dashboard")==0   ? "active" : "");
  navHtml.replace("__ACTIVE_WARNA__",     strcmp(activeKey, "warna")==0     ? "active" : "");
  navHtml.replace("__ACTIVE_WELCOMING__", strcmp(activeKey, "welcoming")==0 ? "active" : "");
  navHtml.replace("__ACTIVE_RIDING__",    strcmp(activeKey, "riding")==0    ? "active" : "");
  navHtml.replace("__ACTIVE_SEINREM__",   strcmp(activeKey, "seinrem")==0   ? "active" : "");
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
  server.on("/led", HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/html", MIZUMA_LED_HTML); });
  server.on("/warna", HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/html", MIZUMA_LED_HTML); });
  server.on("/welcoming", HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/html", MIZUMA_LED_HTML); });
  server.on("/sein-rem", HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/html", MIZUMA_LED_LEGACY_HTML); });
  server.on("/app", HTTP_GET, [getVehicle](AsyncWebServerRequest *req){
    req->send(200, "text/html", mizumaRenderPage(MIZUMA_HOME_HTML, "dashboard", getVehicle())); });
  server.on("/pengaturan", HTTP_GET, [getVehicle](AsyncWebServerRequest *req){
    req->send(200, "text/html", mizumaRenderPage(MIZUMA_SETTINGS_HTML, "pengaturan", getVehicle())); });
  server.on("/mizuma/frag/header", HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/html", MIZUMA_HEADER_HTML); });
  server.on("/mizuma/frag/script", HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/javascript", MIZUMA_HEADER_SCRIPT); });
  server.on("/mizuma/frag/nav",    HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/html", MIZUMA_BOTTOMNAV_HTML); });
}
