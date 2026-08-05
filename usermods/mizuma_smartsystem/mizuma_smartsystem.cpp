// ======================================================================================================================================
// MIZUMA SMARTSYSTEM — FINAL ASSEMBLY — PART 1/2
// Tempel file ini MENGGANTIKAN SELURUH isi mizuma_smartsystem.cpp lama.
// PART 1: Blok 1 (native-feel final) + Blok 2 (Dashboard) + Blok 3 (Pengaturan v2) + Blok 4 (Placeholder)
// ======================================================================================================================================
#include "wled.h"

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
<a href="/led" class="__ACTIVE_LAMPU__"><span class="bn-pill"><svg viewBox="0 0 24 24"><path d="M9 18h6"/><path d="M10 21h4"/><path d="M12 3a6 6 0 0 0-3.6 10.8c.7.6 1.1 1.4 1.3 2.2h4.6c.2-.8.6-1.6 1.3-2.2A6 6 0 0 0 12 3z"/></svg></span>Lampu</a>
<a href="/servis" class="__ACTIVE_SERVIS__"><span class="bn-pill"><svg viewBox="0 0 24 24"><path d="M14.7 6.3a1 1 0 0 0 0 1.4l1.6 1.6a1 1 0 0 0 1.4 0l3.77-3.77a6 6 0 0 1-7.94 7.94l-6.91 6.91a2.12 2.12 0 0 1-3-3l6.91-6.91a6 6 0 0 1 7.94-7.94l-3.76 3.76z"/></svg></span>Servis</a>
<a href="/keamanan" class="__ACTIVE_KEAMANAN__"><span class="bn-pill"><svg viewBox="0 0 24 24"><path d="M12 22s8-3.5 8-10V5l-8-3-8 3v7c0 6.5 8 10 8 10z"/></svg></span>Keamanan</a>
<a href="/pengaturan" class="__ACTIVE_PENGATURAN__"><span class="bn-pill"><svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 1 1-2.83 2.83l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 1 1-4 0v-.09a1.65 1.65 0 0 0-1-1.51 1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 1 1-2.83-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 1 1 0-4h.09a1.65 1.65 0 0 0 1.51-1 1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 1 1 2.83-2.83l.06.06a1.65 1.65 0 0 0 1.82.33h.01a1.65 1.65 0 0 0 1-1.51V3a2 2 0 1 1 4 0v.09a1.65 1.65 0 0 0 1 1.51h.01a1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 1 1 2.83 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82v.01a1.65 1.65 0 0 0 1.51 1H21a2 2 0 1 1 0 4h-.09a1.65 1.65 0 0 0-1.51 1z"/></svg></span>Pengaturan</a>
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
<style>
%SHARED_CSS%
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
<div class="card">
<div class="card-title">Reminder Servis</div>
<div class="placeholder-text">Belum ada data servis &mdash; lihat tab Servis</div>
</div>
<div class="quick-tiles">
<a class="tile" href="/led"><div class="ti ac"><svg viewBox="0 0 24 24"><path d="M9 18h6"/><path d="M10 21h4"/><path d="M12 3a6 6 0 0 0-3.6 10.8c.7.6 1.1 1.4 1.3 2.2h4.6c.2-.8.6-1.6 1.3-2.2A6 6 0 0 0 12 3z"/></svg></div><div><div class="tl">Kontrol Lampu</div><div class="ts">5 mode &bull; 2 sisi</div></div></a>
<a class="tile" href="/keamanan"><div class="ti ok"><svg viewBox="0 0 24 24"><path d="M12 21s7-6 7-11a7 7 0 0 0-14 0c0 5 7 11 7 11z"/><circle cx="12" cy="10" r="2.5"/></svg></div><div><div class="tl">Lokasi GPS</div><div class="ts">Fase 13</div></div></a>
</div>
<div class="card">
<div class="card-title">Riwayat Kejadian</div>
<div class="placeholder-text">Belum ada riwayat</div>
</div>
</div>
%BOTTOMNAV%
<script>
%HEADER_SCRIPT%
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
<style>
%SHARED_CSS%
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
<div class="feat-row"><span class="fi off"><svg viewBox="0 0 24 24"><path d="M5 12h14"/></svg></span><span class="fn">Reminder Servis &amp; Sparepart</span><span class="fs off">Fase 10</span></div>
<div class="feat-row"><span class="fi off"><svg viewBox="0 0 24 24"><path d="M5 12h14"/></svg></span><span class="fn">Kesehatan Aki (Voltase)</span><span class="fs off">Fase 11</span></div>
<div class="feat-row"><span class="fi off"><svg viewBox="0 0 24 24"><path d="M5 12h14"/></svg></span><span class="fn">Anti-Theft</span><span class="fs off">Fase 12</span></div>
<div class="feat-row"><span class="fi off"><svg viewBox="0 0 24 24"><path d="M5 12h14"/></svg></span><span class="fn">GPS Tracker</span><span class="fs off">Fase 13</span></div>
</div>
<div class="card danger">
<div class="card-title">Reset Pabrik</div>
<div class="placeholder-text" style="color:#c99;">Menghapus semua konfigurasi dan mengembalikan ke pengaturan awal</div>
<a class="btn danger" href="/settings/sec">Buka Halaman Reset</a>
</div>
</div>
%BOTTOMNAV%
<script>
%HEADER_SCRIPT%
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

// --------------------------------------------------------------------------------------------------------------------------------------
// Blok 4 — Placeholder (Servis & Keamanan)
// --------------------------------------------------------------------------------------------------------------------------------------
const char MIZUMA_PLACEHOLDER_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>%PAGE_TITLE% - Mizuma</title>
<style>
%SHARED_CSS%
.container { padding:16px; max-width:520px; margin:0 auto; }
.center-card { text-align:center; padding:40px 20px; }
.center-card .ci { font-size:40px; margin-bottom:14px; }
.center-card .ct { font-size:15px; font-weight:700; color:#fff; margin-bottom:6px; }
</style>
</head>
<body>
%HEADER%
<div class="container">
<div class="card center-card">
<div class="ci">%PAGE_ICON%</div>
<div class="ct">%PAGE_TITLE%</div>
<div class="placeholder-text">Fitur ini sedang dikembangkan dan akan tersedia di tahap pengembangan berikutnya.</div>
</div>
</div>
%BOTTOMNAV%
<script>%HEADER_SCRIPT%</script>
</body>
</html>
)rawliteral";
// ================= AKHIR PART 1/2 — balas "lanjut" untuk PART 2/2 (Blok 5 + Blok 6) =================

// --------------------------------------------------------------------------------------------------------------------------------------
// Blok 5 v4 FINAL — LED (dengan polish: label mode terbatas, simpan pilihan per tab, debounce slider RGB)
// GANTI HANYA konstanta MIZUMA_LED_HTML di file dengan ini. Blok lain tidak berubah.
// --------------------------------------------------------------------------------------------------------------------------------------
const char MIZUMA_LED_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Lampu - Mizuma</title>
<style>
%SHARED_CSS%
html,body{height:100%;overflow:hidden;}
body{display:flex;flex-direction:column;}
.freeze{flex-shrink:0;background:var(--bg);border-bottom:1px solid var(--bd);}
.tabbar{display:flex;overflow-x:auto;background:var(--s1);border-bottom:1px solid var(--bd);}
.tabbar button{flex:1 0 auto;padding:11px 10px;background:none;border:none;color:var(--tx2);font-size:12.5px;font-weight:600;white-space:nowrap;border-bottom:3px solid transparent;}
.tabbar button.active{color:var(--tx);border-bottom-color:var(--ac);}
.preview-grid{display:grid;grid-template-columns:1fr 62px 78px;grid-template-rows:34px 34px;gap:5px;padding:10px 12px;background:var(--bg);}
.pv-bar{border-radius:6px;background:#000;display:flex;align-items:center;padding:0 3px;gap:1px;overflow:hidden;}
.pv-bar .seg{flex:1;height:20px;background:var(--s3);border-radius:1px;}
.pv-bar.kanan{grid-column:1;grid-row:1;} .pv-bar.kiri{grid-column:1;grid-row:2;}
.side-btn{grid-column:2;background:var(--s2);border:1px solid var(--bd2);color:var(--tx2);border-radius:6px;font-size:11px;font-weight:600;}
.side-btn.kanan-btn{grid-row:1;} .side-btn.kiri-btn{grid-row:2;}
.side-btn-both{grid-column:3;grid-row:1 / span 2;background:var(--s2);border:1px solid var(--bd2);color:var(--tx2);border-radius:6px;font-size:11px;font-weight:600;}
.side-btn.active,.side-btn-both.active{background:var(--ac);color:#0a0a0a;border-color:var(--ac);}
.body-layout{flex:1;min-height:0;display:flex;}
.subnav{width:74px;flex-shrink:0;background:var(--s1);border-right:1px solid var(--bd);overflow-y:auto;}
.subnav button{display:block;width:100%;padding:14px 4px;background:none;border:none;color:var(--tx3);font-size:10.5px;text-align:center;border-left:3px solid transparent;}
.subnav button.active{color:var(--tx);border-left-color:var(--ac);background:var(--s2);}
.subnav .icon{display:flex;justify-content:center;margin-bottom:4px;}
.subnav .icon svg{width:18px;height:18px;stroke:currentColor;fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
.right-col{flex:1;min-width:0;display:flex;flex-direction:column;min-height:0;}
.pane-head{flex-shrink:0;background:var(--bg);padding:10px 12px;border-bottom:1px solid var(--bd);display:flex;flex-direction:column;gap:8px;}
.pane-scroll{flex:1;min-height:0;overflow-y:auto;-webkit-overflow-scrolling:touch;padding:14px;}
.pane-foot{flex-shrink:0;background:var(--s1);border-top:1px solid var(--bd);padding:12px 14px;max-height:45%;overflow-y:auto;}
.toggle-pair{display:flex;gap:6px;}
.toggle-pair button{flex:1;padding:9px;background:var(--s1);border:1px solid var(--bd);color:var(--tx2);border-radius:8px;font-size:12px;font-weight:700;}
.toggle-pair button.active{background:var(--ac);color:#0a0a0a;border-color:var(--ac);}
.mode-row{font-size:11px;color:var(--tx2);}
.mode-row b{color:var(--tx);}
.search-box{width:100%;padding:9px 12px;background:var(--s1);border:1px solid var(--bd);border-radius:8px;color:var(--tx);font-size:13px;}
.brightness-row{display:flex;align-items:center;gap:10px;}
.brightness-row .bi svg{width:16px;height:16px;stroke:var(--tx2);fill:none;stroke-width:2;stroke-linecap:round;}
.brightness-row input[type=range]{flex:1;}
#brightVal{font-size:12px;color:var(--tx2);width:30px;}
.fx-chip{font-size:11px;font-weight:700;color:var(--ac);background:var(--ac-dim);padding:4px 10px;border-radius:14px;white-space:nowrap;}
.head-label{font-size:12px;color:var(--tx2);} .head-label b{color:var(--tx);}
.wheel-wrap{display:flex;flex-direction:column;align-items:center;margin-bottom:14px;}
#colorWheel{border-radius:50%;touch-action:none;}
.slots-row{display:flex;gap:10px;margin-top:12px;}
.slot{width:38px;height:38px;border-radius:10px;border:2px solid var(--bd2);cursor:pointer;}
.slot.active{border-color:var(--ac);}
.ctl-row{display:flex;align-items:center;gap:10px;margin-bottom:10px;}
.ctl-row label{width:14px;font-size:11px;font-weight:800;color:var(--tx2);}
.ctl-row input[type=range]{flex:1;}
.ctl-row .val{width:30px;font-size:11px;color:var(--tx2);text-align:right;}
.hex-row{display:flex;gap:8px;margin:12px 0 4px;}
.hex-row input{flex:1;background:var(--s1);border:1px solid var(--bd);border-radius:8px;padding:9px 12px;color:var(--tx);font-size:13px;text-transform:uppercase;}
.btn-sm{padding:9px 14px;border-radius:8px;background:var(--s2);border:1px solid var(--bd2);font-size:12px;font-weight:700;color:var(--tx);}
.btn-sm.primary{background:var(--ac);color:#0a0a0a;border-color:var(--ac);}
.palette-grid{display:grid;grid-template-columns:repeat(3,1fr);gap:8px;}
.palette-card{border-radius:8px;overflow:hidden;border:2px solid var(--bd);cursor:pointer;}
.palette-card.active{border-color:var(--ac);}
.palette-card .swatch{height:34px;}
.palette-card .pname{font-size:9px;text-align:center;padding:3px;background:var(--s1);color:var(--tx2);}
.download-link{display:block;text-align:center;margin-top:14px;padding:10px;background:var(--s1);border-radius:8px;color:var(--ac);text-decoration:none;font-size:12px;border:1px dashed var(--bd2);}
.fx-grid{display:grid;grid-template-columns:repeat(2,1fr);gap:8px;}
@media(min-width:420px){.fx-grid{grid-template-columns:repeat(3,1fr);}}
.fx-item{background:var(--s1);border:1px solid var(--bd);border-radius:10px;padding:8px;font-size:11px;color:var(--tx);}
.fx-item.active{border-color:var(--ac);background:var(--ac-dim);}
.fx-prev{height:10px;border-radius:5px;margin-bottom:6px;background:linear-gradient(90deg,#f5a524,#ff3b3b,#3b8cff,#3ddc84);background-size:300% 100%;}
.fx-num{color:var(--tx3);font-size:9px;margin-right:3px;}
.fx-name{font-weight:700;}
@keyframes kfMove{from{background-position:0% 0;}to{background-position:300% 0;}}
@keyframes kfBlink{0%,55%{opacity:1;}56%,100%{opacity:.15;}}
@keyframes kfBreathe{0%,100%{opacity:.25;}50%{opacity:1;}}
@keyframes kfHue{from{filter:hue-rotate(0deg);}to{filter:hue-rotate(360deg);}}
.anim-move{animation:kfMove 2s linear infinite;}
.anim-move-fast{animation:kfMove .8s linear infinite;}
.anim-blink{animation:kfBlink 1s steps(1) infinite;}
.anim-breathe{animation:kfBreathe 2.4s ease-in-out infinite;}
.anim-hue{animation:kfHue 3s linear infinite;}
.anim-sparkle{animation:kfBlink .35s steps(1) infinite;}
.anim-solid{animation:none;background:var(--ac);}
.param-row{margin-bottom:12px;}
.param-label{display:flex;justify-content:space-between;font-size:11px;color:var(--tx2);margin-bottom:5px;}
.param-row input[type=range]{width:100%;}
.foot-hint{font-size:11px;color:var(--tx3);text-align:center;padding:4px 0;}
.save-btn{display:block;width:100%;text-align:center;padding:12px;background:var(--ac);color:#0a0a0a;border-radius:10px;font-weight:800;font-size:14px;border:none;margin-top:10px;}
.save-hint{font-size:11px;color:var(--tx3);text-align:center;margin-top:8px;}
.restricted-swatches{display:grid;grid-template-columns:repeat(2,1fr);gap:10px;}
.rswatch{aspect-ratio:1.6;border-radius:10px;border:3px solid transparent;display:flex;align-items:center;justify-content:center;color:#0a0a0a;font-size:11px;font-weight:800;}
.rswatch.active{border-color:#fff;}
.restricted-note{font-size:11px;color:var(--tx3);text-align:center;padding:10px 0 4px;}
</style>
</head>
<body>
<div class="freeze">
%HEADER%
<div class="tabbar" id="tabbar">
<button data-tab="welcoming" class="active">Welcoming</button>
<button data-tab="riding">Riding</button>
<button data-tab="sein">Sein</button>
<button data-tab="rem">Rem</button>
<button data-tab="hazard">Hazard</button>
</div>
<div class="preview-grid">
<div class="pv-bar kanan" id="pvKanan"></div>
<div class="pv-bar kiri" id="pvKiri"></div>
<button class="side-btn kanan-btn active" data-side="kanan">Kanan</button>
<button class="side-btn kiri-btn" data-side="kiri">Kiri</button>
<button class="side-btn-both" data-side="both">Kanan<br>+ Kiri</button>
</div>
</div>
<div class="body-layout">
<div class="subnav" id="subnav">
<button data-sub="warna" class="active"><span class="icon"><svg viewBox="0 0 24 24"><path d="M12 2s6 6.5 6 11a6 6 0 0 1-12 0c0-4.5 6-11 6-11z"/></svg></span>Pola Warna</button>
<button data-sub="efek"><span class="icon"><svg viewBox="0 0 24 24"><path d="M12 3l1.9 5.1L19 10l-5.1 1.9L12 17l-1.9-5.1L5 10l5.1-1.9z"/></svg></span>Efek</button>
<button data-sub="simpan"><span class="icon"><svg viewBox="0 0 24 24"><path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"/><path d="M17 21v-8H7v8"/><path d="M7 3v5h8"/></svg></span>Simpan</button>
</div>
<div class="right-col">
<div class="pane-head" id="headWarna">
<div class="toggle-pair" id="colorToggle">
<button data-ct="custom" class="active">Custom</button>
<button data-ct="template">Template</button>
</div>
<div class="mode-row">Mode aktif: <b id="modeAktif">Custom &mdash; Slot 1</b></div>
<input class="search-box" id="searchBox" placeholder="Cari palette..." style="display:none;">
</div>
<div class="pane-head" id="headEfek" style="display:none;">
<div class="brightness-row">
<span class="bi"><svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="4"/><path d="M12 2v2M12 20v2M4.9 4.9l1.4 1.4M17.7 17.7l1.4 1.4M2 12h2M20 12h2M4.9 19.1l1.4-1.4M17.7 6.3l1.4-1.4"/></svg></span>
<input type="range" min="0" max="255" value="180" id="brightSlider">
<span id="brightVal">180</span>
<span class="fx-chip" id="fxActiveName">Solid</span>
</div>
</div>
<div class="pane-head" id="headSimpan" style="display:none;">
<div class="head-label">Menyimpan: <b id="saveContext">Welcoming &mdash; Kanan</b></div>
</div>
<div class="pane-scroll" id="scrollWarna">
<div id="ctCustom">
<div class="wheel-wrap">
<canvas id="colorWheel" width="220" height="220"></canvas>
<div class="slots-row">
<div class="slot active" style="background:#ff3b3b;" data-slot="0"></div>
<div class="slot" style="background:#3ddc84;" data-slot="1"></div>
<div class="slot" style="background:#3b8cff;" data-slot="2"></div>
<div class="slot" style="background:#f5a524;" data-slot="3"></div>
</div>
</div>
<div class="ctl-row"><label>R</label><input type="range" min="0" max="255" value="255" id="rR"><span class="val" id="vR">255</span></div>
<div class="ctl-row"><label>G</label><input type="range" min="0" max="255" value="59" id="rG"><span class="val" id="vG">59</span></div>
<div class="ctl-row"><label>B</label><input type="range" min="0" max="255" value="59" id="rB"><span class="val" id="vB">59</span></div>
<div class="hex-row"><input id="hexIn" maxlength="6" placeholder="FF3B3B"><button class="btn-sm primary" id="hexSet">Set</button><button class="btn-sm" id="rndBtn">Acak</button></div>
</div>
<div id="ctTemplate" style="display:none;">
<div class="palette-grid" id="paletteGrid"></div>
<a class="download-link" href="#">&#8681; Download Palette Lainnya</a>
</div>
<div id="ctRestricted" style="display:none;">
<div class="restricted-swatches" id="restrictedGrid"></div>
<div class="restricted-note">Warna dibatasi otomatis sesuai regulasi lampu sinyal</div>
</div>
</div>
<div class="pane-scroll" id="scrollEfek" style="display:none;">
<div class="fx-grid" id="fxGrid"></div>
</div>
<div class="pane-scroll" id="scrollSimpan" style="display:none;">
<div class="card">
<div class="card-title">Simpan Pengaturan</div>
<button class="save-btn">Simpan Sekarang</button>
<div class="save-hint">Menyimpan permanen ke memori ESP32</div>
</div>
</div>
<div class="pane-foot" id="footEfek" style="display:none;">
<div id="fxParams"></div>
<div class="foot-hint" id="footHint" style="display:none;">Efek ini tidak memiliki parameter tambahan.</div>
</div>
</div>
</div>
%BOTTOMNAV%
<script>
%HEADER_SCRIPT%
let activeTab='welcoming', activeSide='kanan', activeSub='warna', currentCT='custom';
let restrictedSelection = {sein:0, rem:0, hazard:0};
const SEG_KANAN = 0, SEG_KIRI = 1;
const RESTRICTED_COLORS = {
sein:    [{n:'Amber',h:'FFA500'},{n:'Kuning Tua',h:'FF8C00'},{n:'Amber Muda',h:'FFB347'},{n:'Kuning',h:'FFD700'}],
rem:     [{n:'Merah',h:'FF0000'},{n:'Merah Tua',h:'CC0000'},{n:'Merah Terang',h:'FF3333'},{n:'Merah Gelap',h:'8B0000'}],
hazard:  [{n:'Amber',h:'FFA500'},{n:'Kuning Tua',h:'FF8C00'},{n:'Amber Muda',h:'FFB347'},{n:'Kuning',h:'FFD700'}]
};
function isRestrictedTab(){ return activeTab==='sein' || activeTab==='rem' || activeTab==='hazard'; }
function getActiveSegIds(){
if(activeSide==='kanan')return[SEG_KANAN];
if(activeSide==='kiri')return[SEG_KIRI];
return[SEG_KANAN,SEG_KIRI];
}
function sendColorToSegments(r,g,b){
const segs=getActiveSegIds().map(id=>({id:id,col:[[r,g,b]]}));
fetch('/json/state',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({seg:segs})}).catch(()=>{});
updatePreviewColor(r,g,b);
}
function sendPaletteToSegments(idx){
const segs=getActiveSegIds().map(id=>({id:id,pal:idx}));
fetch('/json/state',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({seg:segs})}).catch(()=>{});
}
function updatePreviewColor(r,g,b){
const c='rgb('+r+','+g+','+b+')';
const ids=getActiveSegIds();
if(ids.includes(SEG_KANAN))document.querySelectorAll('#pvKanan .seg').forEach(s=>s.style.background=c);
if(ids.includes(SEG_KIRI))document.querySelectorAll('#pvKiri .seg').forEach(s=>s.style.background=c);
}
function debounce(fn, ms){
let t;
return function(...args){ clearTimeout(t); t=setTimeout(()=>fn.apply(this,args), ms); };
}
const sendColorDebounced = debounce(sendColorToSegments, 80);
function buildPreviewBar(id){const el=document.getElementById(id);el.innerHTML='';for(let i=0;i<48;i++){const s=document.createElement('div');s.className='seg';el.appendChild(s);}}
buildPreviewBar('pvKanan');buildPreviewBar('pvKiri');
function refreshColorModeVisibility(){
const restricted=isRestrictedTab();
document.getElementById('colorToggle').style.display=restricted?'none':'flex';
document.getElementById('ctCustom').style.display=(!restricted&&currentCT==='custom')?'block':'none';
document.getElementById('ctTemplate').style.display=(!restricted&&currentCT==='template')?'block':'none';
document.getElementById('ctRestricted').style.display=restricted?'block':'none';
document.getElementById('searchBox').style.display=(!restricted&&currentCT==='template')?'block':'none';
if(restricted)buildRestrictedGrid();
}
function buildRestrictedGrid(){
const grid=document.getElementById('restrictedGrid');
grid.innerHTML='';
const colors=RESTRICTED_COLORS[activeTab]||RESTRICTED_COLORS.sein;
const savedIdx=restrictedSelection[activeTab]||0;
colors.forEach((c,i)=>{
const div=document.createElement('div');
div.className='rswatch'+(i===savedIdx?' active':'');
div.style.background='#'+c.h;
div.textContent=c.n;
div.addEventListener('click',()=>{
document.querySelectorAll('.rswatch').forEach(x=>x.classList.remove('active'));
div.classList.add('active');
restrictedSelection[activeTab]=i;
const n=parseInt(c.h,16);
sendColorToSegments((n>>16)&255,(n>>8)&255,n&255);
updateModeAktif();
});
grid.appendChild(div);
});
const sel=colors[savedIdx];const n=parseInt(sel.h,16);
sendColorToSegments((n>>16)&255,(n>>8)&255,n&255);
updateModeAktif();
}
document.getElementById('tabbar').addEventListener('click',e=>{
if(e.target.tagName!=='BUTTON')return;
document.querySelectorAll('#tabbar button').forEach(b=>b.classList.remove('active'));
e.target.classList.add('active');activeTab=e.target.dataset.tab;
updateSaveContext();refreshColorModeVisibility();
});
document.querySelector('.preview-grid').addEventListener('click',e=>{
const btn=e.target.closest('button');if(!btn)return;
document.querySelectorAll('.side-btn,.side-btn-both').forEach(b=>b.classList.remove('active'));
btn.classList.add('active');activeSide=btn.dataset.side;updateSaveContext();
});
function setSub(k){activeSub=k;
document.getElementById('headWarna').style.display=k==='warna'?'flex':'none';
document.getElementById('headEfek').style.display=k==='efek'?'flex':'none';
document.getElementById('headSimpan').style.display=k==='simpan'?'flex':'none';
document.getElementById('scrollWarna').style.display=k==='warna'?'block':'none';
document.getElementById('scrollEfek').style.display=k==='efek'?'block':'none';
document.getElementById('scrollSimpan').style.display=k==='simpan'?'block':'none';
document.getElementById('footEfek').style.display=k==='efek'?'block':'none';
updateSaveContext();}
document.getElementById('subnav').addEventListener('click',e=>{const btn=e.target.closest('button');if(!btn)return;document.querySelectorAll('#subnav button').forEach(b=>b.classList.remove('active'));btn.classList.add('active');setSub(btn.dataset.sub);});
function updateSaveContext(){const t={welcoming:'Welcoming',riding:'Riding',sein:'Sein',rem:'Rem',hazard:'Hazard'};const s={kanan:'Kanan',kiri:'Kiri',both:'Kanan + Kiri'};document.getElementById('saveContext').textContent=t[activeTab]+' \u2014 '+s[activeSide];}
updateSaveContext();refreshColorModeVisibility();
function updateModeAktif(){
if(isRestrictedTab()){
const colors=RESTRICTED_COLORS[activeTab]||RESTRICTED_COLORS.sein;
const idx=restrictedSelection[activeTab]||0;
const tabLabel={sein:'Sein',rem:'Rem',hazard:'Hazard'}[activeTab];
document.getElementById('modeAktif').textContent='Terbatas \u2014 '+colors[idx].n+' ('+tabLabel+')';
return;
}
if(currentCT==='custom'){const i=Array.from(document.querySelectorAll('.slot')).findIndex(s=>s.classList.contains('active'));document.getElementById('modeAktif').textContent='Custom \u2014 Slot '+((i<0?0:i)+1);}
else{const p=Array.from(document.querySelectorAll('.palette-card')).findIndex(c=>c.classList.contains('active'));document.getElementById('modeAktif').textContent='Template \u2014 Palette '+((p<0?0:p)+1);}
}
document.getElementById('colorToggle').addEventListener('click',e=>{
if(e.target.tagName!=='BUTTON')return;
document.querySelectorAll('#colorToggle button').forEach(b=>b.classList.remove('active'));
e.target.classList.add('active');currentCT=e.target.dataset.ct;
document.getElementById('ctCustom').style.display=currentCT==='custom'?'block':'none';
document.getElementById('ctTemplate').style.display=currentCT==='custom'?'none':'block';
document.getElementById('searchBox').style.display=currentCT==='custom'?'none':'block';
updateModeAktif();});
function rgb2hex(r,g,b){return('#'+[r,g,b].map(x=>('0'+x.toString(16)).slice(-2)).join('')).toUpperCase();}
function setSlider(id,vid,v){document.getElementById(id).value=v;document.getElementById(vid).textContent=v;}
function setColor(r,g,b){
const act=document.querySelector('.slot.active');if(act)act.style.background='rgb('+r+','+g+','+b+')';
document.getElementById('hexIn').value=rgb2hex(r,g,b).slice(1);
setSlider('rR','vR',r);setSlider('rG','vG',g);setSlider('rB','vB',b);
sendColorToSegments(r,g,b);
}
['rR','rG','rB'].forEach(id=>{
document.getElementById(id).addEventListener('input',()=>{
const r=+document.getElementById('rR').value, g=+document.getElementById('rG').value, b=+document.getElementById('rB').value;
const act=document.querySelector('.slot.active');if(act)act.style.background='rgb('+r+','+g+','+b+')';
document.getElementById('hexIn').value=rgb2hex(r,g,b).slice(1);
setSlider('rR','vR',r);setSlider('rG','vG',g);setSlider('rB','vB',b);
updatePreviewColor(r,g,b);
sendColorDebounced(r,g,b);
});
});
document.getElementById('hexSet').addEventListener('click',()=>{let h=document.getElementById('hexIn').value.replace('#','');if(/^[0-9a-fA-F]{6}$/.test(h)){const n=parseInt(h,16);setColor((n>>16)&255,(n>>8)&255,n&255);}});
document.getElementById('rndBtn').addEventListener('click',()=>{setColor(Math.random()*256|0,Math.random()*256|0,Math.random()*256|0);});
document.querySelectorAll('.slot').forEach(s=>{s.addEventListener('click',()=>{document.querySelectorAll('.slot').forEach(x=>x.classList.remove('active'));s.classList.add('active');
const m=getComputedStyle(s).backgroundColor.match(/\d+/g);if(m)setColor(+m[0],+m[1],+m[2]);updateModeAktif();});});
const wheel=document.getElementById('colorWheel');const wctx=wheel.getContext('2d');const wr=110;
const img=wctx.createImageData(220,220);
for(let y=0;y<220;y++){for(let x=0;x<220;x++){const dx=x-wr,dy=y-wr;const dist=Math.sqrt(dx*dx+dy*dy);const idx=(y*220+x)*4;
if(dist<=wr){const angle=Math.atan2(dy,dx)*180/Math.PI+180;const sat=dist/wr;const[r,g,b]=hsvToRgb(angle,sat,1);img.data[idx]=r;img.data[idx+1]=g;img.data[idx+2]=b;img.data[idx+3]=255;}else{img.data[idx+3]=0;}}}
wctx.putImageData(img,0,0);
function hsvToRgb(h,s,v){const c=v*s,x=c*(1-Math.abs((h/60)%2-1)),m=v-c;let r,g,b;
if(h<60){r=c;g=x;b=0;}else if(h<120){r=x;g=c;b=0;}else if(h<180){r=0;g=c;b=x;}else if(h<240){r=0;g=x;b=c;}else if(h<300){r=x;g=0;b=c;}else{r=c;g=0;b=x;}
return[Math.round((r+m)*255),Math.round((g+m)*255),Math.round((b+m)*255)];}
wheel.addEventListener('click',e=>{const rect=wheel.getBoundingClientRect();const x=e.clientX-rect.left,y=e.clientY-rect.top;const dx=x-wr,dy=y-wr;
if(Math.sqrt(dx*dx+dy*dy)>wr)return;const angle=Math.atan2(dy,dx)*180/Math.PI+180;const sat=Math.min(1,Math.sqrt(dx*dx+dy*dy)/wr);
const[r,g,b]=hsvToRgb(angle,sat,1);setColor(r,g,b);});
const paletteGrid=document.getElementById('paletteGrid');
const paletteGradientSeed=['linear-gradient(90deg,#ff3b3b,#ffd23b)','linear-gradient(90deg,#3ddc84,#3b8cff)','linear-gradient(90deg,#a83bff,#ff3bd6)','linear-gradient(90deg,#3bfff0,#3b5cff)','linear-gradient(90deg,#ffae3b,#ff3b3b)','linear-gradient(90deg,#3bff5c,#d6ff3b)'];
fetch('/json/pal').then(r=>r.json()).then(names=>{
paletteGrid.innerHTML='';
names.forEach((name,i)=>{
if(name==='r')return;
const grad=paletteGradientSeed[i%paletteGradientSeed.length];
const card=document.createElement('div');
card.className='palette-card'+(i===0?' active':'');
card.innerHTML='<div class="swatch" style="background:'+grad+';"></div><div class="pname">'+name+'</div>';
card.addEventListener('click',()=>{document.querySelectorAll('.palette-card').forEach(c=>c.classList.remove('active'));card.classList.add('active');sendPaletteToSegments(i);updateModeAktif();});
paletteGrid.appendChild(card);
});
}).catch(()=>{});
const FX_PARAMS={'Solid':[],'Blink':[['Speed',128],['Intensity',128]],'Breathe':[['Speed',128]],'Wipe':[['Speed',128],['Intensity',128]],'Chase':[['Speed',128],['Intensity',128],['Size',50]],'Rainbow':[['Speed',128],['Intensity',128]],'Sparkle':[['Speed',128],['Intensity',128]],'Fade':[['Speed',128]]};
const FX_ANIM={'Solid':'anim-solid','Blink':'anim-blink','Breathe':'anim-breathe','Wipe':'anim-move','Chase':'anim-move-fast','Rainbow':'anim-hue','Sparkle':'anim-sparkle','Fade':'anim-breathe'};
function renderParams(name){const box=document.getElementById('fxParams');box.innerHTML='';
(FX_PARAMS[name]||[]).forEach(function(p){const row=document.createElement('div');row.className='param-row';
row.innerHTML='<div class="param-label"><span>'+p[0]+'</span><span>'+p[1]+'</span></div><input type="range" min="0" max="255" value="'+p[1]+'">';
row.querySelector('input').addEventListener('input',function(e){row.querySelectorAll('.param-label span')[1].textContent=e.target.value;});
box.appendChild(row);});
document.getElementById('footHint').style.display=(FX_PARAMS[name]&&FX_PARAMS[name].length)?'none':'block';}
const fxGrid=document.getElementById('fxGrid');
const sortedFx=Object.keys(FX_PARAMS).sort();
sortedFx.forEach((name,i)=>{const item=document.createElement('div');item.className='fx-item'+(name==='Solid'?' active':'');
item.innerHTML='<div class="fx-prev '+(FX_ANIM[name]||'')+'"></div><span class="fx-num">'+(i+1)+'.</span><span class="fx-name">'+name+'</span>';
item.addEventListener('click',()=>{document.querySelectorAll('.fx-item').forEach(x=>x.classList.remove('active'));item.classList.add('active');
document.getElementById('fxActiveName').textContent=name;renderParams(name);});
fxGrid.appendChild(item);});
renderParams('Solid');
document.getElementById('brightSlider').addEventListener('input',e=>{document.getElementById('brightVal').textContent=e.target.value;});
</script>
</body>
</html>
)rawliteral";


// --------------------------------------------------------------------------------------------------------------------------------------
// Blok 6 — class MizumaSmartSystem (FINAL: kunci "Mizuma" + ssid + getId, tanpa route reserved)
// --------------------------------------------------------------------------------------------------------------------------------------
#ifndef USERMOD_ID_MIZUMA_SYSTEM
#define USERMOD_ID_MIZUMA_SYSTEM 0x9001
#endif
class MizumaSmartSystem : public Usermod {
private:
String vehicleName  = "";
String vehicleBrand = "";
uint16_t vehicleYear = 0;
String vehiclePlate = "";
struct ReminderItem {
unsigned long lastServiceEpoch = 0;
uint16_t intervalDays = 0;
};
ReminderItem oliMesin, oliRem, oliGardan, cvt, filter;
uint8_t presetWelcomingKanan = 1, presetWelcomingKiri = 2;
uint8_t presetRidingKanan    = 3, presetRidingKiri    = 4;
uint8_t presetSeinKanan      = 5, presetSeinKiri      = 6;
uint8_t presetRemKanan       = 7, presetRemKiri       = 8;
uint8_t presetHazardKanan    = 9, presetHazardKiri    = 10;
String renderPage(const char* pageTemplate, const char* activeKey) {
String html = FPSTR(pageTemplate);
html.replace("%SHARED_CSS%", FPSTR(MIZUMA_SHARED_CSS));
html.replace("%HEADER%", FPSTR(MIZUMA_HEADER_HTML));
html.replace("%HEADER_SCRIPT%", FPSTR(MIZUMA_HEADER_SCRIPT));
String navHtml = FPSTR(MIZUMA_BOTTOMNAV_HTML);
navHtml.replace("__ACTIVE_BERANDA__",    strcmp(activeKey, "beranda")    == 0 ? "active" : "");
navHtml.replace("__ACTIVE_LAMPU__",      strcmp(activeKey, "lampu")      == 0 ? "active" : "");
navHtml.replace("__ACTIVE_SERVIS__",     strcmp(activeKey, "servis")     == 0 ? "active" : "");
navHtml.replace("__ACTIVE_KEAMANAN__",   strcmp(activeKey, "keamanan")   == 0 ? "active" : "");
navHtml.replace("__ACTIVE_PENGATURAN__", strcmp(activeKey, "pengaturan") == 0 ? "active" : "");
html.replace("%BOTTOMNAV%", navHtml);
html.replace("%VEHICLE_NAME%",  vehicleName.length()  ? vehicleName  : "Motor Anda");
html.replace("%VEHICLE_BRAND%", vehicleBrand.length() ? vehicleBrand : "-");
html.replace("%VEHICLE_YEAR%",  vehicleYear ? String(vehicleYear) : "-");
html.replace("%VEHICLE_PLATE%", vehiclePlate.length() ? vehiclePlate : "-");
return html;
}
String renderPlaceholder(const char* title, const char* icon, const char* activeKey) {
String html = FPSTR(MIZUMA_PLACEHOLDER_HTML);
html.replace("%PAGE_TITLE%", title);
html.replace("%PAGE_ICON%", icon);
return renderPage(html.c_str(), activeKey);
}
public:
void setup() override {
apBehavior = AP_BEHAVIOR_ALWAYS;
DEBUG_PRINTLN(F("[Mizuma] Usermod utama siap"));
server.on("/app", HTTP_GET, [this](AsyncWebServerRequest *request){
request->send(200, "text/html", renderPage(MIZUMA_HOME_HTML, "beranda"));
});
server.on("/led", HTTP_GET, [this](AsyncWebServerRequest *request){
request->send(200, "text/html", renderPage(MIZUMA_LED_HTML, "lampu"));
});
server.on("/pengaturan", HTTP_GET, [this](AsyncWebServerRequest *request){
request->send(200, "text/html", renderPage(MIZUMA_SETTINGS_HTML, "pengaturan"));
});
server.on("/servis", HTTP_GET, [this](AsyncWebServerRequest *request){
request->send(200, "text/html", renderPlaceholder("Servis", "&#128736;", "servis"));
});
server.on("/keamanan", HTTP_GET, [this](AsyncWebServerRequest *request){
request->send(200, "text/html", renderPlaceholder("Keamanan & GPS", "&#128274;", "keamanan"));
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
json += "\",\"ssid\":\"";
json += staOn ? WiFi.SSID() : String("Mizuma Smart System");
json += "\"}";
request->send(200, "application/json", json);
});
}
void loop() override {}
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
uint16_t getId() override { return USERMOD_ID_MIZUMA_SYSTEM; }
};
// WAJIB — tepat SATU pasang di seluruh file
static MizumaSmartSystem mizuma_smartsystem;
REGISTER_USERMOD(mizuma_smartsystem);
// ================= AKHIR FILE =================
