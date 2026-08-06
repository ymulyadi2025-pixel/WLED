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
// ================= BLOK 5 : LED =====================================================================
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
.tabbar button{flex:1 0 auto;padding:10px 10px;background:none;border:none;color:var(--tx2);font-size:12px;font-weight:600;white-space:nowrap;border-bottom:3px solid transparent;}
.tabbar button.active{color:var(--tx);border-bottom-color:var(--ac);}
.side-row{display:flex;background:var(--s1);border:1px solid var(--bd);border-radius:10px;padding:2px;margin:8px 12px 0;}
.side-row .side-btn{flex:1;padding:5px 0;border:none;background:transparent;color:var(--tx3);font-size:10px;font-weight:700;border-radius:8px;}
.side-row .side-btn.active{background:var(--s3);color:var(--ac);}
.preview-row{display:grid;grid-template-columns:1fr 26px 1fr;gap:8px;align-items:center;padding:8px 12px;}
.pv-canvas{width:100%;height:14px;image-rendering:pixelated;background:#000;border-radius:5px;display:block;transition:opacity .25s;}
.pv-canvas.off{opacity:.22;}
.live-btn{width:24px;height:24px;border-radius:50%;background:var(--s2);border:1px solid var(--bd2);display:flex;align-items:center;justify-content:center;color:var(--tx3);padding:0;}
.live-btn svg{width:12px;height:12px;stroke:currentColor;fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
.live-btn.on{color:var(--ac);border-color:var(--ac);background:var(--ac-dim);animation:livePulse 2s infinite;}
@keyframes livePulse{0%{box-shadow:0 0 0 0 rgba(245,165,36,.35);}70%{box-shadow:0 0 0 6px rgba(245,165,36,0);}100%{box-shadow:0 0 0 0 rgba(245,165,36,0);}}
.info-row3{display:grid;grid-template-columns:minmax(0,1fr) minmax(0,1.7fr) minmax(0,1fr);gap:8px;align-items:center;padding:4px 12px 10px;background:var(--bg);border-bottom:1px solid var(--bd);}
.info-row3 .i3{font-size:9.5px;color:var(--tx2);white-space:nowrap;overflow:hidden;text-overflow:ellipsis;}
.i3.left{text-align:left;} .i3.right{text-align:right;}
.i3.mid{display:flex;align-items:center;gap:6px;}
.i3.mid svg{width:13px;height:13px;stroke:var(--tx2);fill:none;stroke-width:2;stroke-linecap:round;flex-shrink:0;}
.i3.mid #brightVal{font-size:9.5px;color:var(--tx);width:22px;text-align:right;}
.body-layout{flex:1;min-height:0;display:flex;}
.subnav{width:74px;flex-shrink:0;background:var(--s1);border-right:1px solid var(--bd);display:flex;flex-direction:column;}
.subnav .nav-btns{flex:1;overflow-y:auto;}
.subnav button.nav-b{display:block;width:100%;padding:13px 4px;background:none;border:none;color:var(--tx3);font-size:10.5px;text-align:center;border-left:3px solid transparent;}
.subnav button.nav-b.active{color:var(--tx);border-left-color:var(--ac);background:var(--s2);}
.subnav .icon{display:flex;justify-content:center;margin-bottom:4px;}
.subnav .icon svg{width:18px;height:18px;stroke:currentColor;fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
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
.toggle-pair{display:flex;background:var(--s1);border:1px solid var(--bd);border-radius:10px;padding:2px;}
.toggle-pair button{flex:1;padding:6px 0;background:transparent;border:none;color:var(--tx3);font-size:11px;font-weight:700;border-radius:8px;}
.toggle-pair button.active{background:var(--s3);color:var(--ac);}
.mode-row{font-size:10.5px;color:var(--tx2);}
.mode-row b{color:var(--tx);}
.search-box{width:100%;padding:8px 12px;background:var(--s1);border:1px solid var(--bd);border-radius:10px;color:var(--tx);font-size:12px;}
.fx-head-row{display:flex;align-items:center;gap:8px;}
.fx-chip{font-size:10.5px;font-weight:700;color:var(--ac);background:var(--ac-dim);padding:4px 10px;border-radius:14px;white-space:nowrap;max-width:42%;overflow:hidden;text-overflow:ellipsis;}
.fx-search{flex:1;min-width:0;padding:7px 10px;background:var(--s1);border:1px solid var(--bd);border-radius:10px;color:var(--tx);font-size:11px;}
.wheel-wrap{display:flex;flex-direction:column;align-items:center;margin-bottom:6px;}
.wheel-wrap canvas{border-radius:50%;touch-action:none;}
.slots-row{display:flex;gap:14px;margin-top:12px;align-items:center;justify-content:center;}
.slot{width:30px;height:30px;border-radius:50%;border:3px solid #888;background:transparent;cursor:pointer;transition:transform .12s var(--ease);}
.slot:active{transform:scale(.9);}
.slot.active{outline:2px solid #fff;outline-offset:2px;}
.ring-r{position:relative;width:30px;height:30px;border-radius:50%;background:conic-gradient(#f00,#ff0,#0f0,#0ff,#00f,#f0f,#f00);cursor:pointer;border:none;}
.ring-r::after{content:'R';position:absolute;inset:4px;border-radius:50%;background:var(--bg);color:var(--tx);display:flex;align-items:center;justify-content:center;font-size:10px;font-weight:800;}
.ctl-row{display:flex;align-items:center;gap:10px;margin-bottom:6px;}
.ctl-row label{width:64px;font-size:11px;font-weight:700;color:var(--tx2);}
.ctl-row .val{width:34px;font-size:11px;color:var(--tx2);text-align:right;}
.palette-list{display:grid;grid-template-columns:1fr 1fr;gap:8px;}
.pal-row{padding:8px 8px 12px;}
.pal-row .pnum{display:none;}
.sec-title{font-size:10px;color:var(--tx2);text-transform:uppercase;letter-spacing:.5px;margin:12px 0 6px;font-weight:700;}
.pal-row{position:relative;display:flex;align-items:center;gap:8px;background:var(--s1);border:1px solid var(--bd);border-radius:12px;padding:10px 12px 14px;overflow:hidden;}
.pal-row .pradio{width:14px;height:14px;border-radius:50%;border:2px solid var(--bd2);flex-shrink:0;}
.pal-row.active{border-color:var(--ac);}
.pal-row.active .pradio{border-color:var(--ac);background:var(--ac);}
.pal-row .pnum{font-size:9px;color:var(--tx3);width:18px;}
.pal-row .pname{flex:1;text-align:center;font-size:12px;font-weight:600;color:var(--tx);}
.pal-row .pstrip{position:absolute;left:0;right:0;bottom:0;height:4px;}
.fx-grid{display:grid;grid-template-columns:repeat(2,1fr);gap:8px;}
@media(min-width:420px){.fx-grid{grid-template-columns:repeat(3,1fr);}}
.fx-item{background:var(--s1);border:1px solid var(--bd);border-radius:10px;padding:10px 8px;font-size:11px;color:var(--tx);}
.fx-item.active{border-color:var(--ac);background:var(--ac-dim);}
.fx-num{color:var(--tx3);font-size:9px;margin-right:3px;}
.fx-name{font-weight:700;}
.param-row{margin-bottom:12px;}
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
.mbox{background:var(--s1);border:1px solid var(--bd2);border-radius:16px;padding:18px;width:82%;max-width:320px;}
.mtitle{font-size:14px;font-weight:800;color:var(--tx);margin-bottom:4px;}
.msub{font-size:12px;color:var(--tx2);margin-bottom:14px;}
.mbtns{display:flex;gap:8px;}
.mbtns .btn-sm{flex:1;}
.btn-sm{padding:9px 14px;border-radius:8px;background:var(--s2);border:1px solid var(--bd2);font-size:12px;font-weight:700;color:var(--tx);}
.btn-sm.primary{background:var(--ac);color:#0a0a0a;border-color:var(--ac);}
/* ===== Slider custom (track terisi aksen + thumb putih) ===== */
input[type=range]{-webkit-appearance:none;appearance:none;background:transparent;height:18px;--p:50%;}
input[type=range]::-webkit-slider-runnable-track{height:4px;border-radius:2px;background:linear-gradient(90deg,var(--ac) var(--p),var(--s3) var(--p));}
input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;width:14px;height:14px;border-radius:50%;background:#fff;margin-top:-5px;box-shadow:0 1px 4px rgba(0,0,0,.6);}
input[type=range]::-moz-range-track{height:4px;border-radius:2px;background:linear-gradient(90deg,var(--ac) var(--p),var(--s3) var(--p));}
input[type=range]::-moz-range-thumb{width:14px;height:14px;border-radius:50%;background:#fff;border:none;box-shadow:0 1px 4px rgba(0,0,0,.6);}
</style>
</head>
<body>
<div class="freeze">
<div id="mzH"></div>
<div class="tabbar" id="tabbar">
<button data-tab="welcoming" class="active">Welcoming</button>
<button data-tab="riding">Riding</button>
<button data-tab="sein">Sein</button>
<button data-tab="rem">Rem</button>
<button data-tab="hazard">Hazard</button>
</div>
<div class="side-row" id="sideRow">
<button class="side-btn" data-side="kiri">Kiri</button>
<button class="side-btn" data-side="both">Semua</button>
<button class="side-btn active" data-side="kanan">Kanan</button>
</div>
<div class="preview-row">
<canvas class="pv-canvas" id="pvKiri" width="48" height="1"></canvas>
<button class="live-btn on" id="liveToggle" title="Live preview on/off"><svg viewBox="0 0 24 24"><path d="M2 12h4l3-8 4 16 3-8h4"/></svg></button>
<canvas class="pv-canvas" id="pvKanan" width="48" height="1"></canvas>
</div>
<div class="info-row3">
<div class="i3 left" id="ctxInfo">Efek &mdash; Kanan</div>
<div class="i3 mid">
<svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="4"/><path d="M12 2v2M12 20v2M4.9 4.9l1.4 1.4M17.7 17.7l1.4 1.4M2 12h2M20 12h2M4.9 19.1l1.4-1.4M17.7 6.3l1.4-1.4"/></svg>
<input type="range" min="0" max="255" value="180" id="brightSlider">
<span id="brightVal">180</span>
</div>
<div class="i3 right" id="savedInfo">Memuat...</div>
</div>
</div>
<div class="body-layout">
<div class="subnav" id="subnav">
<div class="nav-btns">
<button class="nav-b active" data-sub="efek"><span class="icon"><svg viewBox="0 0 24 24"><path d="M12 3l1.9 5.1L19 10l-5.1 1.9L12 17l-1.9-5.1L5 10l5.1-1.9z"/></svg></span>Efek</button>
<button class="nav-b" data-sub="warna"><span class="icon"><svg viewBox="0 0 24 24"><path d="M12 2s6 6.5 6 11a6 6 0 0 1-12 0c0-4.5 6-11 6-11z"/></svg></span>Pola Warna</button>
<button class="nav-b" data-sub="simpan"><span class="icon"><svg viewBox="0 0 24 24"><path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"/><path d="M17 21v-8H7v8"/><path d="M7 3v5h8"/></svg></span>Simpan</button>
</div>
<div class="sub-save" id="subSave">
<button id="btnSaveSub"><svg viewBox="0 0 24 24"><path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"/><path d="M17 21v-8H7v8"/><path d="M7 3v5h8"/></svg><span class="dotd"></span></button>
</div>
</div>
<div class="right-col">
<div class="pane-head" id="headWarna" style="display:none;">
<div class="toggle-pair" id="colorToggle">
<button data-ct="template">Template</button>
<button data-ct="custom" class="active">Custom</button>
</div>
<div class="mode-row">Mode aktif: <b id="modeAktif">Custom &mdash; Slot 1</b></div>
<input class="search-box" id="searchBox" placeholder="Cari palette..." style="display:none;">
</div>
<div class="pane-head" id="headEfek">
<div class="fx-head-row">
<span class="mode-row">Efek aktif:</span>
<span class="fx-chip" id="fxActiveName">Solid</span>
<input class="fx-search" id="fxSearch" placeholder="Cari efek...">
</div>
</div>
<div class="pane-head" id="headSimpan" style="display:none;">
<div class="mode-row">Preset tersimpan <b>permanen di ESP32</b> per kombinasi mode &times; sisi.</div>
</div>
<div class="pane-scroll" id="scrollWarna" style="display:none;">
<div id="ctCustom">
<div class="card"><div class="card-title">Roda Warna</div>
<div class="wheel-wrap">
<canvas id="colorWheel" width="220" height="220"></canvas>
<div class="slots-row">
<div class="slot active" style="border-color:#ff3b3b;" data-slot="0"></div>
<div class="slot" style="border-color:#3ddc84;" data-slot="1"></div>
<div class="slot" style="border-color:#3b8cff;" data-slot="2"></div>
<div class="slot" style="border-color:#f5a524;" data-slot="3"></div>
<button class="ring-r" id="rndRing" title="Acak"></button>
</div>
</div>
<div class="ctl-row"><label>Saturation</label><input type="range" min="0" max="100" value="100" id="satSlider"><span class="val" id="satVal">100</span></div>
<div class="sec-title">Warna Custom</div>
<div class="palette-list" id="customPalGrid"></div>
</div>
</div>
<div id="ctTemplate" style="display:none;">
<div class="palette-list" id="paletteGrid"></div>
</div>
<div id="ctRestricted" style="display:none;">
<div class="restricted-swatches" id="restrictedGrid"></div>
<div class="wheel-wrap"><canvas id="wheelR" width="220" height="220"></canvas></div>
<div class="restricted-note">Area abu-abu dibatasi otomatis sesuai regulasi lampu sinyal</div>
</div>
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
let activeTab='welcoming',activeSide='kanan',activeSub='efek',currentCT='custom';
let dirty=false,pendingTab=null,restrictedName='';
let allFx=[],fxData=[],curList=[],palList=[],palNamesRaw=[];
let cur={fx:0,pal:0,col:null,bri:180,params:{},palName:''};
let wHue=0,wSat=1,liveEnabled=true;
const LIVE_CO=[2,0,1];
const SEG_K=0,SEG_L=1;
const HUE_RULES={sein:[25,60],hazard:[25,60],rem:[345,15]};
const RESTRICTED_COLORS={
sein:[{n:'Amber',h:'FFA500'},{n:'Kuning Tua',h:'FF8C00'},{n:'Amber Muda',h:'FFB347'},{n:'Kuning',h:'FFD700'}],
rem:[{n:'Merah',h:'FF0000'},{n:'Merah Tua',h:'CC0000'},{n:'Merah Terang',h:'FF3333'},{n:'Merah Gelap',h:'8B0000'}],
hazard:[{n:'Amber',h:'FFA500'},{n:'Kuning Tua',h:'FF8C00'},{n:'Amber Muda',h:'FFB347'},{n:'Kuning',h:'FFD700'}]};
const FX_BLACKLIST=['frizzles','funky plank','game of life','geq','ghost rider','matrix','metaballs','tartan','polar lights','swirl','lissajous','rubix','invaders','maze','pulser','dna','plasma ball','hipnotic','black hole','cubes','fireworks','akemi','colored bursts','freqmatrix'];
const WELCOMING_NAMES=['Fade','Breathe','Wipe','Sweep','Chase','Chase Rainbow','Colorwaves','Rainbow','Rainbow Runner','Twinkle','Twinklefox','Sparkle','Glitter','Meteor','Meteor Smooth','Ripple','Ripple Rainbow','Pacifica','Aurora','Lake','Plasma','Colortwinkles','Sinelon','Sinelon Rainbow','Bpm','Sunrise','Phased','Dissolve','Noise Pal','Blends'];
const RESTRICTED_FX=['Solid','Blink','Strobe','Chase','Chase Flash','Wipe','Fade','Breathe','Sweep','Strobe Mega'];
const PAL_GRADS={'Lava':'linear-gradient(90deg,#000000,#880000,#ff0000,#ffff00,#ffffff)','Rainbow':'linear-gradient(90deg,#ff0000,#ffff00,#00ff00,#00ffff,#0000ff,#ff00ff)','Rainbow Runner':'linear-gradient(90deg,#ff0000,#ffff00,#00ff00,#00ffff,#0000ff,#ff00ff)','Party':'linear-gradient(90deg,#ff00ff,#00ffff,#ffff00,#ff00ff)','Fire':'linear-gradient(90deg,#000000,#ff0000,#ff8800,#ffff00)','Ocean':'linear-gradient(90deg,#000011,#003366,#0066cc,#0099ff)','Sunset':'linear-gradient(90deg,#000033,#660033,#ff6600,#ffff00)','Spring':'linear-gradient(90deg,#ffaa00,#00ff00,#00cc66)','Autumn':'linear-gradient(90deg,#663300,#996600,#cc9900)','Ice':'linear-gradient(90deg,#00ffff,#ffffff,#00ffff)','Neon':'linear-gradient(90deg,#ff00ff,#00ffff,#ffff00)','Hot':'linear-gradient(90deg,#880000,#ff0000,#ff8800,#ffffff)','Cool':'linear-gradient(90deg,#00ff00,#00ffff,#0000ff)','Rain':'linear-gradient(90deg,#000066,#0099ff,#00ff00)','Breeze':'linear-gradient(90deg,#006699,#00ccff,#99ff99)','Colorwaves':'linear-gradient(90deg,#ff00ff,#0000ff,#00ffff,#00ff00)','Bpm':'linear-gradient(90deg,#ff0000,#00ff00,#0000ff)','Plasma':'linear-gradient(90deg,#ff00ff,#00ffff,#ffff00,#ff00ff)','Aurora':'linear-gradient(90deg,#003366,#00ff99,#ff00ff)','Pacifica':'linear-gradient(90deg,#003366,#006699,#0099cc)','Ripple':'linear-gradient(90deg,#0000ff,#00ffff,#0000ff)','Meteor':'linear-gradient(90deg,#333333,#ff8800,#333333)','Twinkle':'linear-gradient(90deg,#222222,#ffff00,#222222)','Sparkle':'linear-gradient(90deg,#111111,#ffffff,#111111)','Glitter':'linear-gradient(90deg,#222222,#ffff00,#ffffff,#222222)','Sinelon':'linear-gradient(90deg,#0000ff,#ff0000,#0000ff)','Fade':'linear-gradient(90deg,#ff0000,#0000ff)','Breathe':'linear-gradient(90deg,#333333,#ffffff,#333333)','Blink':'linear-gradient(90deg,#ffffff,#000000,#ffffff)','Strobe':'linear-gradient(90deg,#ffffff,#000000,#ffffff,#000000)','Chase':'linear-gradient(90deg,#ff0000,#000000,#ff0000,#000000)','Wipe':'linear-gradient(90deg,#000000,#ff0000,#000000)','Sweep':'linear-gradient(90deg,#000000,#00ffff,#000000)','Solid':'linear-gradient(90deg,#f5a524,#f5a524)'};
function post(o){fetch('/json/state',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(o)}).catch(function(){});}
function debounce(fn,ms){let t;return function(){const a=arguments;clearTimeout(t);t=setTimeout(function(){fn.apply(null,a);},ms);};}
function segIds(){if(activeSide==='kanan')return[SEG_K];if(activeSide==='kiri')return[SEG_L];return[SEG_K,SEG_L];}
function isRestrictedTab(){return activeTab==='sein'||activeTab==='rem'||activeTab==='hazard';}
function cap(s){return s.charAt(0).toUpperCase()+s.slice(1);}
function markDirty(){dirty=true;document.getElementById('subSave').classList.add('dirty');}
function clearDirty(){dirty=false;document.getElementById('subSave').classList.remove('dirty');}
function toast(msg){const t=document.getElementById('toast');t.textContent=msg;t.style.opacity='1';setTimeout(function(){t.style.opacity='0';},1800);}
/* ===== Slider fill helper ===== */
function paintRange(el){const min=+el.min||0,max=+el.max||255;el.style.setProperty('--p',(((+el.value-min)/(max-min))*100)+'%');}
document.addEventListener('input',function(e){if(e.target.matches&&e.target.matches('input[type=range]'))paintRange(e.target);});
/* ===== LIVE PREVIEW (WS peek asli + toggle) ===== */
const pvR=document.getElementById('pvKanan'),pvL=document.getElementById('pvKiri');
function drawLive(flat){
const cR=pvR.getContext('2d'),cL=pvL.getContext('2d');
for(let i=0;i<48;i++){
const k=(segR.start+i)*3,k2=(segL.start+i)*3;
cR.fillStyle=(flat.length>=k+3)?'rgb('+flat[k+LIVE_CO[0]]+','+flat[k+LIVE_CO[1]]+','+flat[k+LIVE_CO[2]]+')':'#000';
cL.fillStyle=(flat.length>=k2+3)?'rgb('+flat[k2+LIVE_CO[0]]+','+flat[k2+LIVE_CO[1]]+','+flat[k2+LIVE_CO[2]]+')':'#000';
cR.fillRect(i,0,1,1);cL.fillRect(i,0,1,1);}}
let segR={start:0,stop:48},segL={start:48,stop:96};
fetch('/json/state').then(function(r){return r.json();}).then(function(j){
if(j.seg&&j.seg.length>0){
segR={start:j.seg[0].start||0,stop:j.seg[0].stop||48};
if(j.seg.length>1)segL={start:j.seg[1].start||48,stop:j.seg[1].stop||96};
if(segL.start<segR.stop)segR.stop=segL.start;
}
}).catch(function(){});
let liveOk=false,mzWS=null,mzWSRetry=null,lastDraw=0;
function mzConnectLive(){
if(mzWSRetry){clearTimeout(mzWSRetry);mzWSRetry=null;}
try{mzWS=new WebSocket('ws://'+window.location.host+'/ws');}catch(e){mzWSRetry=setTimeout(mzConnectLive,3000);return;}
mzWS.binaryType='arraybuffer';
mzWS.onopen=function(){try{mzWS.send(JSON.stringify({lv:true}));}catch(e){}};
mzWS.onmessage=function(evt){
if(!liveEnabled)return;
const now=performance.now();if(now-lastDraw<33)return;lastDraw=now;
let flat=null;
if(evt.data instanceof ArrayBuffer){flat=new Uint8Array(evt.data);}
else if(typeof evt.data==='string'){try{const j=JSON.parse(evt.data);const L=j.leds;if(!L)return;
if(Array.isArray(L)){if(Array.isArray(L[0])){flat=new Uint8Array(L.length*3);for(let i=0;i<L.length;i++){flat[i*3]=L[i][0];flat[i*3+1]=L[i][1];flat[i*3+2]=L[i][2];}}else{flat=new Uint8Array(L);}}}catch(e){return;}}
if(!flat||flat.length<3)return;
liveOk=true;drawLive(flat);};
mzWS.onclose=function(){liveOk=false;mzWS=null;if(!document.hidden)mzWSRetry=setTimeout(mzConnectLive,2000);};
mzWS.onerror=function(){try{mzWS.close();}catch(e){}};}
mzConnectLive();
window.addEventListener('beforeunload',function(){if(mzWS)try{mzWS.close();}catch(e){}});
function pollState(){if(!liveEnabled)return;fetch('/json/state').then(function(r){return r.json();}).then(function(j){const s=j.seg||[];
const f=function(cv,seg){if(!seg)return;const c=(seg.col&&seg.col[0])?seg.col[0]:[20,20,20];const ctx=cv.getContext('2d');ctx.fillStyle='rgb('+c[0]+','+c[1]+','+c[2]+')';ctx.fillRect(0,0,48,1);};
f(pvR,s[0]);f(pvL,s[1]||s[0]);}).catch(function(){});}
setInterval(function(){if(!liveOk)pollState();},1000);
document.getElementById('liveToggle').addEventListener('click',function(){
liveEnabled=!liveEnabled;
this.classList.toggle('on',liveEnabled);
pvR.classList.toggle('off',!liveEnabled);
pvL.classList.toggle('off',!liveEnabled);});
/* ===== Wheel + knob ===== */
function hsvToRgb(h,s,v){const c=v*s,x=c*(1-Math.abs((h/60)%2-1)),m=v-c;let r,g,b;
if(h<60){r=c;g=x;b=0;}else if(h<120){r=x;g=c;b=0;}else if(h<180){r=0;g=c;b=x;}else if(h<240){r=0;g=x;b=c;}else if(h<300){r=x;g=0;b=c;}else{r=c;g=0;b=x;}
return[Math.round((r+m)*255),Math.round((g+m)*255),Math.round((b+m)*255)];}
function rgb2hsv(r,g,b){r/=255;g/=255;b/=255;const mx=Math.max(r,g,b),mn=Math.min(r,g,b),d=mx-mn;let h=0;
if(d){if(mx===r)h=((g-b)/d)%6;else if(mx===g)h=(b-r)/d+2;else h=(r-g)/d+4;h*=60;if(h<0)h+=360;}
return[h,(mx?d/mx:0),mx];}
function hueOk(h,r){if(!r)return true;if(r[0]<=r[1])return h>=r[0]&&h<=r[1];return h>=r[0]||h<=r[1];}
function clampHue(h,r){if(!r)return h;
if(r[0]<=r[1])return h<r[0]?r[0]:(h>r[1]?r[1]:h);
if(h>r[1]&&h<r[0]){return (h-r[1])<=(r[0]-h)?r[1]:r[0];}return h;}
const wheelImgs={};
function buildWheelImg(cv,rule){const ctx=cv.getContext('2d');const wr=110;const img=ctx.createImageData(220,220);
for(let y=0;y<220;y++){for(let x=0;x<220;x++){const dx=x-wr,dy=y-wr;const d=Math.sqrt(dx*dx+dy*dy);const idx=(y*220+x)*4;
if(d<=wr){const raw=Math.atan2(dy,dx)*180/Math.PI+180;const hue=clampHue(raw,rule);const sat=d/wr;
if(rule&&!hueOk(raw,rule)){img.data[idx]=40;img.data[idx+1]=40;img.data[idx+2]=40;}
else{const rgb=hsvToRgb(hue,sat,1);img.data[idx]=rgb[0];img.data[idx+1]=rgb[1];img.data[idx+2]=rgb[2];}
img.data[idx+3]=255;}else{img.data[idx+3]=0;}}}
wheelImgs[cv.id]={img:img};ctx.putImageData(img,0,0);}
function paintWheel(cv,hue,sat){const e=wheelImgs[cv.id];if(!e)return;const ctx=cv.getContext('2d');ctx.putImageData(e.img,0,0);
const th=(hue-180)*Math.PI/180;const r=sat*108;const x=110+Math.cos(th)*r,y=110+Math.sin(th)*r;
ctx.beginPath();ctx.arc(x,y,7,0,6.2832);ctx.strokeStyle='#fff';ctx.lineWidth=2.5;ctx.stroke();
ctx.beginPath();ctx.arc(x,y,9,0,6.2832);ctx.strokeStyle='rgba(0,0,0,.65)';ctx.lineWidth=1.5;ctx.stroke();}
const wheel=document.getElementById('colorWheel');
buildWheelImg(wheel,null);paintWheel(wheel,wHue,wSat);
function wheelPick(e){const rect=wheel.getBoundingClientRect();const x=(e.clientX-rect.left)*(220/rect.width),y=(e.clientY-rect.top)*(220/rect.height);
const dx=x-110,dy=y-110;const d=Math.sqrt(dx*dx+dy*dy);if(d>110)return;
wHue=Math.atan2(dy,dx)*180/Math.PI+180;wSat=Math.min(1,d/110);
document.getElementById('satSlider').value=Math.round(wSat*100);document.getElementById('satVal').textContent=Math.round(wSat*100);paintRange(document.getElementById('satSlider'));
paintWheel(wheel,wHue,wSat);
const rgb=hsvToRgb(wHue,wSat,1);setColor(rgb[0],rgb[1],rgb[2],false);}
let dragging=false;
wheel.addEventListener('pointerdown',function(e){dragging=true;wheel.setPointerCapture(e.pointerId);wheelPick(e);});
wheel.addEventListener('pointermove',function(e){if(dragging)wheelPick(e);});
wheel.addEventListener('pointerup',function(){dragging=false;});
document.getElementById('satSlider').addEventListener('input',function(e){wSat=(+e.target.value)/100;document.getElementById('satVal').textContent=e.target.value;
paintWheel(wheel,wHue,wSat);const rgb=hsvToRgb(wHue,wSat,1);setColor(rgb[0],rgb[1],rgb[2],false);});
/* ===== Kirim state ===== */
function sendColor(r,g,b,silent){const segs=segIds().map(function(id){return{id:id,col:[[r,g,b]]};});post({seg:segs});cur.col=[r,g,b];if(!silent)markDirty();}
function sendColorSilent(r,g,b){const segs=segIds().map(function(id){return{id:id,col:[[r,g,b]]};});post({seg:segs});cur.col=[r,g,b];}
function sendPalette(i){const segs=segIds().map(function(id){return{id:id,pal:i};});post({seg:segs});cur.pal=i;markDirty();}
function sendEffect(i){const segs=segIds().map(function(id){const o={id:id,fx:i};if(isRestrictedTab())o.pal=0;return o;});post({seg:segs});cur.fx=i;if(isRestrictedTab())cur.pal=0;markDirty();}
const sendParamD=debounce(function(k,v){const segs=segIds().map(function(id){const o={id:id};o[k]=v;return o;});post({seg:segs});cur.params[k]=v;markDirty();},80);
const sendBriD=debounce(function(v){post({bri:v});cur.bri=v;markDirty();},80);
function syncUiToState(){fetch('/json/state').then(function(r){return r.json();}).then(function(j){
if(j.bri!=null){document.getElementById('brightSlider').value=j.bri;document.getElementById('brightVal').textContent=j.bri;cur.bri=j.bri;paintRange(document.getElementById('brightSlider'));}
const s=(j.seg&&j.seg[0])?j.seg[0]:null;if(!s)return;
if(s.fx!=null)cur.fx=s.fx;
if(s.pal!=null)cur.pal=s.pal;
const name=allFx[cur.fx]||'';
if(name){document.getElementById('fxActiveName').textContent=name;
document.querySelectorAll('.fx-item').forEach(function(el){el.classList.toggle('active',el.querySelector('.fx-name').textContent===name);});}
updateCtx();}).catch(function(){});}
/* ===== Sync custom ===== */
function setColor(r,g,b,silent){const act=document.querySelector('.slot.active');if(act)act.style.borderColor='rgb('+r+','+g+','+b+')';
const hv=rgb2hsv(r,g,b);wHue=hv[0];wSat=hv[1];
document.getElementById('satSlider').value=Math.round(wSat*100);document.getElementById('satVal').textContent=Math.round(wSat*100);paintRange(document.getElementById('satSlider'));
paintWheel(wheel,wHue,wSat);
sendColor(r,g,b,silent);updateModeAktif();}
document.getElementById('rndRing').addEventListener('click',function(){setColor(Math.random()*256|0,Math.random()*256|0,Math.random()*256|0,false);});
document.querySelectorAll('.slot').forEach(function(s){s.addEventListener('click',function(){document.querySelectorAll('.slot').forEach(function(x){x.classList.remove('active');});s.classList.add('active');
const m=getComputedStyle(s).borderColor.match(/\d+/g);if(m)setColor(+m[0],+m[1],+m[2],false);});});
/* ===== Restricted ===== */
function buildRestricted(){const grid=document.getElementById('restrictedGrid');grid.innerHTML='';
const colors=RESTRICTED_COLORS[activeTab]||RESTRICTED_COLORS.sein;
colors.forEach(function(c,i){const d=document.createElement('div');d.className='rswatch'+(i===0?' active':'');d.style.background='#'+c.h;
d.addEventListener('click',function(){document.querySelectorAll('.rswatch').forEach(function(x){x.classList.remove('active');});d.classList.add('active');
restrictedName=c.n;const n=parseInt(c.h,16);sendColor((n>>16)&255,(n>>8)&255,n&255,false);updateModeAktif();});
grid.appendChild(d);});
buildWheelImg(document.getElementById('wheelR'),HUE_RULES[activeTab]);
restrictedName=colors[0].n;
const n=parseInt(colors[0].h,16);sendColorSilent((n>>16)&255,(n>>8)&255,n&255);}
document.getElementById('wheelR').addEventListener('pointerdown',function(e){const cv=document.getElementById('wheelR');
const rect=cv.getBoundingClientRect();const x=(e.clientX-rect.left)*(220/rect.width),y=(e.clientY-rect.top)*(220/rect.height);
const dx=x-110,dy=y-110;if(Math.sqrt(dx*dx+dy*dy)>110)return;
const h=clampHue(Math.atan2(dy,dx)*180/Math.PI+180,HUE_RULES[activeTab]);const sat=Math.min(1,Math.sqrt(dx*dx+dy*dy)/110);
const rgb=hsvToRgb(h,sat,1);restrictedName='Wheel (dibatasi)';sendColor(rgb[0],rgb[1],rgb[2],false);updateModeAktif();});
/* ===== Palettes: list row tipis ala WLED ===== */
const paletteGrid=document.getElementById('paletteGrid');
function seedGrad(i){return 'linear-gradient(90deg,hsl('+((i*37)%360)+',80%,50%),hsl('+(((i*37)+120)%360)+',80%,50%))';}
const customPalGrid=document.getElementById('customPalGrid');
function makePalRow(e,grid,numbered,seq){
const row=document.createElement('div');row.className='pal-row'+(cur.palName===e.n?' active':'');row.dataset.name=e.n.toLowerCase();
const grad=PAL_GRADS[e.n]||seedGrad(e.i);
row.innerHTML=(numbered?'<span class="pnum" style="display:block;font-size:9px;color:var(--tx3);">'+(seq+1)+'</span>':'')+'<span class="pradio"></span><span class="pname">'+e.n+'</span><span class="pstrip" style="background:'+grad+'"></span>';
row.addEventListener('click',function(){cur.palName=e.n;
document.querySelectorAll('.pal-row').forEach(function(x){x.classList.remove('active');});
row.classList.add('active');sendPalette(e.i);updateModeAktif();});
grid.appendChild(row);}
function renderPaletteRows(q){paletteGrid.innerHTML='';customPalGrid.innerHTML='';
let t=0;
palList.forEach(function(e){
if(e.n.charAt(0)==='*'){makePalRow(e,customPalGrid,false,0);return;}
if(q&&e.n.toLowerCase().indexOf(q)<0)return;
makePalRow(e,paletteGrid,true,t);t++;});}
fetch('/json/pal').then(function(r){return r.json();}).then(function(names){palNamesRaw=names;
const arr=[];names.forEach(function(n,i){if(n!=='r')arr.push({n:n,i:i});});
arr.sort(function(a,b){return a.n.localeCompare(b.n);});palList=arr;renderPaletteRows('');}).catch(function(){});
document.getElementById('searchBox').addEventListener('input',function(e){renderPaletteRows(e.target.value.toLowerCase());});
/* ===== Efek ===== */
function lookup(n){const t=n.toLowerCase();let i=allFx.findIndex(function(x){return x.toLowerCase()===t;});if(i>=0)return i;return allFx.findIndex(function(x){return x.toLowerCase().includes(t);});}
function listForTab(){if(activeTab==='riding')return allFx.map(function(n,i){return{name:n,idx:i};}).filter(function(e){return e.name.indexOf('2D')!==0&&FX_BLACKLIST.indexOf(e.name.toLowerCase())<0;});
if(activeTab==='welcoming')return WELCOMING_NAMES.map(lookup).filter(function(i){return i>=0;}).map(function(i){return{name:allFx[i],idx:i};});
return RESTRICTED_FX.map(lookup).filter(function(i){return i>=0;}).map(function(i){return{name:allFx[i],idx:i};});}
function parseFxData(meta){const parts=(meta||'').split(';');const labels=(parts[0]||'').split(',');
const def=['Speed','Intensity','Custom 1','Custom 2','Custom 3'];const keys=['sx','ix','c1','c2','c3'];const sl=[];
for(let i=0;i<5;i++){let l=labels[i];if(l===undefined||l==='')continue;if(l==='!')l=def[i];sl.push({key:keys[i],label:l});}
const tg=[];for(let i=0;i<3;i++){let l=labels[5+i];if(l===undefined||l==='')continue;if(l==='!')l='Opsi '+(i+1);tg.push({key:['o1','o2','o3'][i],label:l});}
return{sl:sl,tg:tg};}
function renderParams(idx){const box=document.getElementById('fxParams');box.innerHTML='';
const p=parseFxData(fxData[idx]||'');
p.sl.forEach(function(s){const row=document.createElement('div');row.className='param-row';
row.innerHTML='<div class="param-label"><span>'+s.label+'</span><span>128</span></div><input type="range" min="0" max="255" value="128">';
const inp=row.querySelector('input');
inp.addEventListener('input',function(e){row.querySelectorAll('.param-label span')[1].textContent=e.target.value;sendParamD(s.key,+e.target.value);});
box.appendChild(row);paintRange(inp);});
p.tg.forEach(function(s){const row=document.createElement('div');row.className='param-row';row.style.cssText='display:flex;justify-content:space-between;align-items:center;';
row.innerHTML='<span style="font-size:12px;color:var(--tx2);">'+s.label+'</span><button class="btn-sm" data-on="0">Off</button>';
const b=row.querySelector('button');
b.addEventListener('click',function(){const on=b.dataset.on==='1';b.dataset.on=on?'0':'1';b.textContent=on?'Off':'On';b.className=on?'btn-sm':'btn-sm primary';sendParamD(s.key,on?0:1);});
box.appendChild(row);});
document.getElementById('footHint').style.display=(p.sl.length===0&&p.tg.length===0)?'block':'none';}
function buildEffects(){const grid=document.getElementById('fxGrid');grid.innerHTML='';
curList=listForTab().slice().sort(function(a,b){return a.name.localeCompare(b.name);});
curList.forEach(function(e,i){const el=document.createElement('div');el.className='fx-item'+(i===0?' active':'');
el.innerHTML='<span class="fx-num">'+(i+1)+'.</span><span class="fx-name">'+e.name+'</span>';
el.addEventListener('click',function(){document.querySelectorAll('.fx-item').forEach(function(x){x.classList.remove('active');});el.classList.add('active');
document.getElementById('fxActiveName').textContent=e.name;sendEffect(e.idx);renderParams(e.idx);updateCtx();});
grid.appendChild(el);});
if(curList.length){document.getElementById('fxActiveName').textContent=curList[0].name;renderParams(curList[0].idx);}
updateCtx();}
document.getElementById('fxSearch').addEventListener('input',function(e){const q=e.target.value.toLowerCase();
document.querySelectorAll('#fxGrid .fx-item').forEach(function(el){el.style.display=el.querySelector('.fx-name').textContent.toLowerCase().indexOf(q)>=0?'':'none';});});
Promise.all([fetch('/json/eff').then(function(r){return r.json();}),fetch('/json/fxdata').then(function(r){return r.json();})]).then(function(v){allFx=v[0];fxData=v[1];buildEffects();syncUiToState();refreshSavedInfo();}).catch(function(){});
/* ===== Label & konteks ===== */
function sideLabel(){return{kanan:'Kanan',kiri:'Kiri',both:'Semua'}[activeSide];}
function tabLabel(){return{welcoming:'Welcoming',riding:'Riding',sein:'Sein',rem:'Rem',hazard:'Hazard'}[activeTab];}
function subLabel(){return{warna:'Pola Warna',efek:'Efek',simpan:'Simpan'}[activeSub];}
function updateCtx(){document.getElementById('ctxInfo').textContent=subLabel()+' \u2014 '+sideLabel();}
function updateModeAktif(){if(isRestrictedTab()){document.getElementById('modeAktif').textContent='Terbatas \u2014 '+restrictedName+' ('+tabLabel()+')';return;}
if(currentCT==='custom'){const i=Array.from(document.querySelectorAll('.slot')).findIndex(function(s){return s.classList.contains('active');});document.getElementById('modeAktif').textContent='Custom \u2014 Slot '+((i<0?0:i)+1);}
else{document.getElementById('modeAktif').textContent='Template \u2014 '+(cur.palName||'-');}}
function refreshSavedInfo(){const el=document.getElementById('savedInfo');if(!el||!allFx.length)return;
fetch('/mizuma/presets').then(function(r){return r.json();}).then(function(d){
const sides=activeSide==='both'?['Kanan','Kiri']:[cap(activeSide)];
const names=[];
sides.forEach(function(sd){const st=d[activeTab+sd];
names.push(st&&st.valid?((allFx[st.fx]||'Efek')+' \u2022 '+(palNamesRaw[st.pal]||'Palette')):'Belum disimpan');});
el.textContent=(names.length>1&&names[0]!==names[1])?('Ki: '+names[0]+' | Kn: '+names[1]):names[0];
}).catch(function(){});}
/* ===== Simpan ===== */
function doSave(){const sides=activeSide==='both'?['Kanan','Kiri']:[cap(activeSide)];
const params=new URLSearchParams({fx:cur.fx,pal:cur.pal,r:cur.col?cur.col[0]:255,g:cur.col?cur.col[1]:255,b:cur.col?cur.col[2]:255,sx:cur.params.sx!=null?cur.params.sx:128,ix:cur.params.ix!=null?cur.params.ix:128,bri:cur.bri});
sides.forEach(function(sd){fetch('/mizuma/preset?slot='+activeTab+sd+'&'+params.toString()).catch(function(){});
localStorage.setItem('mzts_'+activeTab+'_'+sd,String(Date.now()));});
clearDirty();
const d=new Date();toast('\u2713 Tersimpan '+('0'+d.getHours()).slice(-2)+':'+('0'+d.getMinutes()).slice(-2));
renderSavedList();refreshSavedInfo();}
function renderSavedList(){const box=document.getElementById('savedList');box.innerHTML='';
['welcoming','riding','sein','rem','hazard'].forEach(function(t){['Kanan','Kiri'].forEach(function(s){
const ts=localStorage.getItem('mzts_'+t+'_'+s);const row=document.createElement('div');row.className='saved-row';
let txt='Belum disimpan',on=false;
if(ts){const d=new Date(+ts);txt='\u2713 '+('0'+d.getHours()).slice(-2)+':'+('0'+d.getMinutes()).slice(-2)+' '+('0'+d.getDate()).slice(-2)+'/'+('0'+(d.getMonth()+1)).slice(-2);on=true;}
row.innerHTML='<span class="sk">'+{welcoming:'Welcoming',riding:'Riding',sein:'Sein',rem:'Rem',hazard:'Hazard'}[t]+' \u2014 '+s+'</span><span class="st'+(on?' on':'')+'">'+txt+'</span>';
box.appendChild(row);});});}
document.getElementById('btnSaveSub').addEventListener('click',doSave);
/* ===== Modal ===== */
function showModal(cb){pendingTab=cb;document.getElementById('modalCtx').textContent=tabLabel()+' \u2014 '+sideLabel();
document.getElementById('saveModal').style.display='flex';}
document.getElementById('mSave').addEventListener('click',function(){doSave();document.getElementById('saveModal').style.display='none';if(pendingTab)switchTab(pendingTab);pendingTab=null;});
document.getElementById('mDiscard').addEventListener('click',function(){clearDirty();document.getElementById('saveModal').style.display='none';if(pendingTab)switchTab(pendingTab);pendingTab=null;});
document.getElementById('mCancel').addEventListener('click',function(){document.getElementById('saveModal').style.display='none';pendingTab=null;});
/* ===== Navigasi ===== */
function applySaved(tab){fetch('/mizuma/presets').then(function(r){return r.json();}).then(function(d){
[['Kanan',0],['Kiri',1]].forEach(function(pr){const st=d[tab+pr[0]];
if(st&&st.valid){const o={id:pr[1],fx:st.fx,pal:st.pal,sx:st.sx,ix:st.ix,col:[st.col]};post({seg:[o]});post({bri:st.bri});}});
}).catch(function(){});}
function refreshColorModeVisibility(){const r=isRestrictedTab();
document.getElementById('colorToggle').style.display=r?'none':'flex';
document.getElementById('ctCustom').style.display=(!r&&currentCT==='custom')?'block':'none';
document.getElementById('ctTemplate').style.display=(!r&&currentCT==='template')?'block':'none';
document.getElementById('ctRestricted').style.display=r?'block':'none';
document.getElementById('searchBox').style.display=(!r&&currentCT==='template')?'block':'none';
if(r)buildRestricted();updateModeAktif();}
function switchTab(t){activeTab=t;
document.querySelectorAll('#tabbar button').forEach(function(b){b.classList.toggle('active',b.dataset.tab===t);});
refreshColorModeVisibility();buildEffects();
applySaved(t);
syncUiToState()
updateCtx();refreshSavedInfo();}
document.getElementById('tabbar').addEventListener('click',function(e){if(e.target.tagName!=='BUTTON')return;
const t=e.target.dataset.tab;if(t===activeTab)return;
if(dirty)showModal(t);else switchTab(t);});
document.getElementById('sideRow').addEventListener('click',function(e){const btn=e.target.closest('button');if(!btn)return;
document.querySelectorAll('#sideRow .side-btn').forEach(function(b){b.classList.remove('active');});
btn.classList.add('active');activeSide=btn.dataset.side;updateCtx();refreshSavedInfo();});
function setSub(k){activeSub=k;
document.getElementById('headWarna').style.display=k==='warna'?'flex':'none';
document.getElementById('headEfek').style.display=k==='efek'?'flex':'none';
document.getElementById('headSimpan').style.display=k==='simpan'?'flex':'none';
document.getElementById('scrollWarna').style.display=k==='warna'?'block':'none';
document.getElementById('scrollEfek').style.display=k==='efek'?'block':'none';
document.getElementById('scrollSimpan').style.display=k==='simpan'?'block':'none';
document.getElementById('footEfek').style.display=k==='efek'?'block':'none';
if(k==='simpan')renderSavedList();}
document.getElementById('subnav').addEventListener('click',function(e){const btn=e.target.closest('button.nav-b');if(!btn)return;
document.querySelectorAll('#subnav .nav-b').forEach(function(b){b.classList.remove('active');});btn.classList.add('active');setSub(btn.dataset.sub);});
document.getElementById('colorToggle').addEventListener('click',function(e){if(e.target.tagName!=='BUTTON')return;
document.querySelectorAll('#colorToggle button').forEach(function(b){b.classList.remove('active');});e.target.classList.add('active');
currentCT=e.target.dataset.ct;refreshColorModeVisibility();});
document.getElementById('brightSlider').addEventListener('input',function(e){document.getElementById('brightVal').textContent=e.target.value;sendBriD(+e.target.value);});
setSub(activeSub);
refreshColorModeVisibility();renderSavedList();updateCtx();
document.querySelectorAll('input[type=range]').forEach(paintRange);
</script>
<script>
(function(){
function inj(url,cb){fetch(url).then(function(r){return r.text();}).then(cb).catch(function(){});}
inj('/mizuma/frag/header',function(t){var el=document.getElementById('mzH');if(el)el.outerHTML=t;});
inj('/mizuma/frag/nav',function(t){var el=document.getElementById('mzN');if(el)el.outerHTML=t;var a=document.querySelector('.bottomnav a[href="/led"]');if(a)a.classList.add('active');});
inj('/mizuma/frag/script',function(t){eval(t);});
})();
</script>
</body>
</html>
)rawliteral";

// --------------------------------------------------------------------------------------------------------------------------------------
// Blok 6 v3 — Fase 8 (preset permanen 10 slot) + Fase 9 (boot Welcoming→Riding)
//           + endpoint fragment (header/nav/script) + preset API
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
struct ReminderItem { unsigned long lastServiceEpoch = 0; uint16_t intervalDays = 0; };
ReminderItem oliMesin, oliRem, oliGardan, cvt, filter;

struct PresetSlot { bool valid=false; uint8_t fx=0; uint8_t pal=0;
  uint8_t r=255,g=255,b=255; uint8_t sx=128,ix=128,bri=180; };
PresetSlot pslots[10];
uint16_t welcomeDur = 7000;
int bootStage = 0; unsigned long bootT = 0; bool bootDone = false;

const char* slotKey(int i){ switch(i){
  case 0:return "welcomingKanan"; case 1:return "welcomingKiri";
  case 2:return "ridingKanan";    case 3:return "ridingKiri";
  case 4:return "seinKanan";      case 5:return "seinKiri";
  case 6:return "remKanan";       case 7:return "remKiri";
  case 8:return "hazardKanan";    default:return "hazardKiri"; }}
int slotIdx(const String& k){ for(int i=0;i<10;i++) if(k==slotKey(i)) return i; return -1; }

void applySlotToSeg(int i, int segId){
  PresetSlot& p = pslots[i]; if(!p.valid) return;
  StaticJsonDocument<512> doc; JsonObject root = doc.to<JsonObject>();
  JsonArray seg = root.createNestedArray("seg"); JsonObject s = seg.createNestedObject();
  s["id"]=segId; s["fx"]=p.fx; s["pal"]=p.pal; s["sx"]=p.sx; s["ix"]=p.ix;
  JsonArray col = s.createNestedArray("col"); JsonArray c = col.createNestedArray();
  c.add(p.r); c.add(p.g); c.add(p.b);
  deserializeState(root);
}

String renderPage(const char* pageTemplate, const char* activeKey) {
  String html = FPSTR(pageTemplate);
  html.replace("%SHARED_CSS%", FPSTR(MIZUMA_SHARED_CSS));
  html.replace("%HEADER%", FPSTR(MIZUMA_HEADER_HTML));
  html.replace("%HEADER_SCRIPT%", FPSTR(MIZUMA_HEADER_SCRIPT));
  String navHtml = FPSTR(MIZUMA_BOTTOMNAV_HTML);
  navHtml.replace("__ACTIVE_BERANDA__",    strcmp(activeKey,"beranda")    ==0 ? "active" : "");
  navHtml.replace("__ACTIVE_LAMPU__",      strcmp(activeKey,"lampu")      ==0 ? "active" : "");
  navHtml.replace("__ACTIVE_SERVIS__",     strcmp(activeKey,"servis")     ==0 ? "active" : "");
  navHtml.replace("__ACTIVE_KEAMANAN__",   strcmp(activeKey,"keamanan")   ==0 ? "active" : "");
  navHtml.replace("__ACTIVE_PENGATURAN__", strcmp(activeKey,"pengaturan") ==0 ? "active" : "");
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

  server.on("/app", HTTP_GET, [this](AsyncWebServerRequest *req){
    req->send(200,"text/html",renderPage(MIZUMA_HOME_HTML,"beranda")); });
  server.on("/led", HTTP_GET, [this](AsyncWebServerRequest *req){
    req->send(200,"text/html",renderPage(MIZUMA_LED_HTML,"lampu")); });
  server.on("/pengaturan", HTTP_GET, [this](AsyncWebServerRequest *req){
    req->send(200,"text/html",renderPage(MIZUMA_SETTINGS_HTML,"pengaturan")); });
  server.on("/servis", HTTP_GET, [this](AsyncWebServerRequest *req){
    req->send(200,"text/html",renderPlaceholder("Servis","&#128736;","servis")); });
  server.on("/keamanan", HTTP_GET, [this](AsyncWebServerRequest *req){
    req->send(200,"text/html",renderPlaceholder("Keamanan & GPS","&#128274;","keamanan")); });

  // Fragments (dipakai bootstrap injeksi di /led agar hemat heap)
  server.on("/mizuma/frag/header", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send_P(200,"text/html", MIZUMA_HEADER_HTML); });
  server.on("/mizuma/frag/script", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send_P(200,"text/javascript", MIZUMA_HEADER_SCRIPT); });
  server.on("/mizuma/frag/nav", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send_P(200,"text/html", MIZUMA_BOTTOMNAV_HTML); });

  server.on("/mizuma/status", HTTP_GET, [](AsyncWebServerRequest *req){
    bool apOn  = (WiFi.softAPgetStationNum() > 0);
    bool staOn = (WiFi.status() == WL_CONNECTED);
    String staIP = staOn ? WiFi.localIP().toString() : "";
    String json = "{\"ap\":"; json += apOn?"true":"false";
    json += ",\"sta\":";      json += staOn?"true":"false";
    json += ",\"staIP\":\"";  json += staIP;
    json += "\",\"ssid\":\""; json += staOn ? WiFi.SSID() : String("Mizuma Smart System");
    json += "\"}";
    req->send(200,"application/json", json);
  });

  // Fase 8: simpan 1 slot preset via GET params (ringan, tanpa parse JSON body)
  server.on("/mizuma/preset", HTTP_GET, [this](AsyncWebServerRequest *req){
    String slotName = req->arg("slot");
    int i = slotIdx(slotName);
    if(i < 0){ req->send(400,"application/json","{\"ok\":false}"); return; }
    PresetSlot& p = pslots[i];
    p.valid = true;
    p.fx  = req->arg("fx").toInt();
    p.pal = req->arg("pal").toInt();
    p.r   = req->arg("r").toInt();
    p.g   = req->arg("g").toInt();
    p.b   = req->arg("b").toInt();
    p.sx  = req->arg("sx").toInt();
    p.ix  = req->arg("ix").toInt();
    p.bri = req->arg("bri").toInt();
doSerializeConfig(); // persist ke cfg.json agar tahan power cycle
req->send(200, "application/json", "{\"ok\":true}");
  });

  // Fase 8.4: baca semua slot (untuk load preset saat tab switch)
  server.on("/mizuma/presets", HTTP_GET, [this](AsyncWebServerRequest *req){
    DynamicJsonDocument doc(2048); JsonObject root = doc.to<JsonObject>();
    for(int i=0;i<10;i++){ JsonObject s = root.createNestedObject(slotKey(i));
      s["valid"]=pslots[i].valid; s["fx"]=pslots[i].fx; s["pal"]=pslots[i].pal;
      JsonArray c = s.createNestedArray("col"); c.add(pslots[i].r); c.add(pslots[i].g); c.add(pslots[i].b);
      s["sx"]=pslots[i].sx; s["ix"]=pslots[i].ix; s["bri"]=pslots[i].bri; }
    String out; serializeJson(doc,out);
    req->send(200,"application/json", out);
  });
}

void loop() override {
  if(bootDone) return;
  unsigned long m = millis();
  if(bootStage==0 && m>2500){ applySlotToSeg(0,0); applySlotToSeg(1,1); bootStage=1; bootT=m; }
  else if(bootStage==1 && m-bootT>=welcomeDur){ applySlotToSeg(2,0); applySlotToSeg(3,1); bootDone=true; }
}

void addToConfig(JsonObject& root) override {
  JsonObject top = root.createNestedObject("Mizuma");
  JsonObject vehicle = top.createNestedObject("vehicle");
  vehicle["name"]=vehicleName; vehicle["brand"]=vehicleBrand;
  vehicle["year"]=vehicleYear; vehicle["plate"]=vehiclePlate;
  JsonObject rem = top.createNestedObject("reminder");
  rem["oliMesin_last"]=oliMesin.lastServiceEpoch; rem["oliMesin_int"]=oliMesin.intervalDays;
  rem["oliRem_last"]=oliRem.lastServiceEpoch;     rem["oliRem_int"]=oliRem.intervalDays;
  rem["oliGardan_last"]=oliGardan.lastServiceEpoch; rem["oliGardan_int"]=oliGardan.intervalDays;
  rem["cvt_last"]=cvt.lastServiceEpoch;           rem["cvt_int"]=cvt.intervalDays;
  rem["filter_last"]=filter.lastServiceEpoch;     rem["filter_int"]=filter.intervalDays;
  JsonObject pm = top.createNestedObject("presets");
  for(int i=0;i<10;i++){ JsonObject s = pm.createNestedObject(slotKey(i));
    s["valid"]=pslots[i].valid; s["fx"]=pslots[i].fx; s["pal"]=pslots[i].pal;
    s["r"]=pslots[i].r; s["g"]=pslots[i].g; s["b"]=pslots[i].b;
    s["sx"]=pslots[i].sx; s["ix"]=pslots[i].ix; s["bri"]=pslots[i].bri; }
  top["welcomeDur"] = welcomeDur;
}

bool readFromConfig(JsonObject& root) override {
  JsonObject top = root["Mizuma"]; if(top.isNull()) return false;
  JsonObject vehicle = top["vehicle"];
  vehicleName=vehicle["name"]|""; vehicleBrand=vehicle["brand"]|"";
  vehicleYear=vehicle["year"]|0;  vehiclePlate=vehicle["plate"]|"";
  JsonObject rem = top["reminder"];
  oliMesin.lastServiceEpoch=rem["oliMesin_last"]|0; oliMesin.intervalDays=rem["oliMesin_int"]|0;
  oliRem.lastServiceEpoch=rem["oliRem_last"]|0;     oliRem.intervalDays=rem["oliRem_int"]|0;
  oliGardan.lastServiceEpoch=rem["oliGardan_last"]|0; oliGardan.intervalDays=rem["oliGardan_int"]|0;
  cvt.lastServiceEpoch=rem["cvt_last"]|0; cvt.intervalDays=rem["cvt_int"]|0;
  filter.lastServiceEpoch=rem["filter_last"]|0; filter.intervalDays=rem["filter_int"]|0;
  JsonObject pm = top["presets"];
  if(!pm.isNull()){ for(int i=0;i<10;i++){ JsonObject s = pm[slotKey(i)]; if(s.isNull()) continue;
    pslots[i].valid=s["valid"]|false; pslots[i].fx=s["fx"]|0; pslots[i].pal=s["pal"]|0;
    pslots[i].r=s["r"]|255; pslots[i].g=s["g"]|255; pslots[i].b=s["b"]|255;
    pslots[i].sx=s["sx"]|128; pslots[i].ix=s["ix"]|128; pslots[i].bri=s["bri"]|180; }}
  welcomeDur = top["welcomeDur"] | 7000;
  return true;
}

uint16_t getId() override { return USERMOD_ID_MIZUMA_SYSTEM; }
};
static MizumaSmartSystem mizuma_smartsystem;
REGISTER_USERMOD(mizuma_smartsystem);
// ================= AKHIR FILE =================
