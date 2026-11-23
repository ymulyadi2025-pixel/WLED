// =====================================================================================================================================
// MIZUMA UI  (mizuma_ui.h)  — HANYA tampilan web: CSS, HTML, JavaScript, dan perute halaman.
// File ini TIDAK menyentuh LED/segmen/show. Semua aksi lampu dilakukan UI lewat API engine (/mizuma/*) dan API bawaan WLED (/json/*).
// Taruh di folder yang sama dengan mizuma_smartsystem.cpp dan mizuma_shows.h.
// Hapus file ini + tambahkan  -D MZ_NO_UI=1  bila ingin build engine saja (tanpa halaman web).
// =====================================================================================================================================
#pragma once
#include "wled.h"
#include <functional>

// Versi firmware yang ditampilkan di Pengaturan (ubah di satu tempat ini, atau -D MZ_FW_VERSION=\"x.y.z\").
#ifndef MZ_FW_VERSION
#define MZ_FW_VERSION "16.0.1"
#endif
// Stempel build: dipakai sebagai ETag css/js supaya HP otomatis memuat ulang setelah firmware diperbarui.
#define MZ_UI_BUILD __DATE__ " " __TIME__

// Data kendaraan yang ditampilkan di halaman (diisi oleh engine, UI hanya membaca).
struct MizumaVehicleInfo {
  String name;
  String brand;
  String plate;
  uint16_t year = 0;
};


// --------------------------------------------------------------------------------------------------------------------------------------
// Blok 1 — Komponen Bersama (6 tab: Dashboard · Color · Welcoming · Riding · Sein-Rem · Pengaturan)
// --------------------------------------------------------------------------------------------------------------------------------------
const char MIZUMA_SHARED_CSS[] PROGMEM = R"rawliteral(
:root{
--bg:#0a0a0a; --s1:#151515; --s2:#1c1c1c; --s3:#242424;
--bd:#262626; --bd2:#333;
--tx:#f2f2f2; --tx2:#9a9a9a; --tx3:#8a8a8a;
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
.mzheader .vdot.bad{background:var(--bad);}
.mzheader .conn.off svg{stroke:var(--bad);}
.mzheader .conn{display:flex;align-items:center;justify-content:center;padding:4px;text-decoration:none;}
.mzheader .conn svg{width:16px;height:16px;stroke:var(--tx2);fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
.bottomnav{position:fixed;bottom:0;left:0;right:0;display:flex;background:rgba(18,18,18,.92);backdrop-filter:blur(14px);-webkit-backdrop-filter:blur(14px);border-top:1px solid var(--bd);z-index:40;padding-bottom:env(safe-area-inset-bottom);}
.bottomnav a{flex:1 1 0;min-width:0;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:3px;padding:7px 1px 8px;text-decoration:none;color:var(--tx3);font-size:10.5px;font-weight:600;letter-spacing:-.2px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;}
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
.info-row{display:flex;justify-content:space-between;align-items:center;gap:10px;padding:8px 0;font-size:13px;border-bottom:1px solid var(--bd);}
.info-row:last-child{border-bottom:none;}
.info-row .k{color:var(--tx2);}
.info-row .v{color:var(--tx);font-weight:600;text-align:right;}
@media (prefers-reduced-motion:reduce){*,*::before,*::after{animation:none!important;transition:none!important;}}
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
window.mzLast = null;
async function mzRefreshHeader() {
if (document.hidden) return;
var conn = document.getElementById('hdrConn'), vt = document.getElementById('hdrVolt'), vd = document.getElementById('hdrVoltDot');
if (!conn || !vt || !vd) return;
try {
const res = await fetch('/mizuma/status', { cache: 'no-store' });
if (!res.ok) throw new Error('status');
const data = await res.json();
window.mzLast = data;
conn.className = 'conn'; conn.title = 'Terhubung - buka Pengaturan';
conn.innerHTML = data.sta ? MZ_GLOBE : MZ_WIFI;
var v = parseFloat(data.volt);
if (v > 0) { vt.textContent = v.toFixed(1) + ' V'; vd.className = 'vdot ok'; }
else { vt.textContent = '-- V'; vd.className = 'vdot'; }
if (window.mzOnStatus) window.mzOnStatus(data);
} catch (e) {
conn.className = 'conn off'; conn.title = 'Terputus dari Mizuma';
conn.innerHTML = MZ_WIFI; vt.textContent = '-- V'; vd.className = 'vdot bad';
}
}
mzRefreshHeader();
setInterval(mzRefreshHeader, 5000);
document.addEventListener('visibilitychange', function(){ if (!document.hidden) mzRefreshHeader(); });
document.querySelectorAll('.card,.tile,.stat-box').forEach(function(el,i){ el.style.animationDelay = (30 + i*30) + 'ms'; });
setTimeout(function(){ document.querySelectorAll('.progress .fill[data-w]').forEach(function(f){ f.style.width = f.getAttribute('data-w') + '%'; }); }, 200);
function mzFadeTo(url){
if(window.__mzFading) return; window.__mzFading = true;
if(window.matchMedia && matchMedia('(prefers-reduced-motion: reduce)').matches){ window.location.href=url; return; }
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
<a href="/app" class="__A_dash__"><span class="bn-pill"><svg viewBox="0 0 24 24"><rect x="3" y="3" width="7" height="9" rx="1"/><rect x="14" y="3" width="7" height="5" rx="1"/><rect x="14" y="12" width="7" height="9" rx="1"/><rect x="3" y="16" width="7" height="5" rx="1"/></svg></span>Dashboard</a>
<a href="/color" class="__A_color__"><span class="bn-pill"><svg viewBox="0 0 24 24"><path d="M12 3s6 6.5 6 11a6 6 0 0 1-12 0c0-4.5 6-11 6-11z"/></svg></span>Color</a>
<a href="/welcoming" class="__A_welcoming__"><span class="bn-pill"><svg viewBox="0 0 24 24"><path d="M12 3l2.4 5.2 5.6.7-4.1 3.9 1 5.6L12 15.6 7.1 18.4l1-5.6L4 8.9l5.6-.7z"/></svg></span>Welcoming</a>
<a href="/riding" class="__A_riding__"><span class="bn-pill"><svg viewBox="0 0 24 24"><path d="M9 18h6M10 21h4M12 3a6 6 0 0 0-3.6 10.8c.7.6 1.1 1.4 1.3 2.2h4.6c.2-.8.6-1.6 1.3-2.2A6 6 0 0 0 12 3z"/></svg></span>Riding</a>
<a href="/sein-rem" class="__A_sein__"><span class="bn-pill"><svg viewBox="0 0 24 24"><path d="M3 12h13M11 6l6 6-6 6M20 6v12"/></svg></span>Sein-Rem</a>
<a href="/pengaturan" class="__A_pengaturan__"><span class="bn-pill"><svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.7 1.7 0 0 0 .3 1.8l.1.1a2 2 0 1 1-2.8 2.8l-.1-.1a1.7 1.7 0 0 0-1.8-.3 1.7 1.7 0 0 0-1 1.5V21a2 2 0 1 1-4 0v-.1a1.7 1.7 0 0 0-1.1-1.5 1.7 1.7 0 0 0-1.8.3l-.1.1a2 2 0 1 1-2.8-2.8l.1-.1a1.7 1.7 0 0 0 .3-1.8 1.7 1.7 0 0 0-1.5-1H3a2 2 0 1 1 0-4h.1a1.7 1.7 0 0 0 1.5-1.1 1.7 1.7 0 0 0-.3-1.8l-.1-.1a2 2 0 1 1 2.8-2.8l.1.1a1.7 1.7 0 0 0 1.8.3H9a1.7 1.7 0 0 0 1-1.5V3a2 2 0 1 1 4 0v.1a1.7 1.7 0 0 0 1 1.5 1.7 1.7 0 0 0 1.8-.3l.1-.1a2 2 0 1 1 2.8 2.8l-.1.1a1.7 1.7 0 0 0-.3 1.8V9a1.7 1.7 0 0 0 1.5 1H21a2 2 0 1 1 0 4h-.1a1.7 1.7 0 0 0-1.5 1z"/></svg></span>Pengaturan</a>
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
.greet-k{font-size:11px;font-weight:800;letter-spacing:1.6px;color:var(--ac);text-transform:uppercase;}
.greet-name{font-size:22px;font-weight:800;color:#fff;margin-top:2px;letter-spacing:.2px;}
.greet-sub{font-size:12px;color:var(--tx2);margin-top:3px;}
.greet-ico{width:44px;height:44px;border-radius:14px;background:var(--ac-dim);color:var(--ac);display:flex;align-items:center;justify-content:center;flex-shrink:0;}
.greet-ico svg{width:22px;height:22px;stroke:currentColor;fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
.card-head{display:flex;align-items:center;justify-content:space-between;margin-bottom:6px;}
.card-head .card-title{margin-bottom:0;}
.bri-row{display:flex;align-items:center;gap:10px;padding:10px 0 2px;font-size:13px;}
.bri-row .k{color:var(--tx2);}
.bri-row .progress{flex:1;}
.bri-row b{min-width:38px;text-align:right;}
.volt-row{display:flex;align-items:center;gap:12px;}
.volt-mid{flex:1;}
.volt-big{font-size:26px;font-weight:800;color:#fff;}
.volt-unit{font-size:13px;color:var(--tx2);}
.quick-tiles{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-bottom:12px;}
.tile{display:flex;align-items:center;gap:10px;background:var(--s1);border:1px solid var(--bd);border-radius:12px;padding:12px;text-decoration:none;}
.tile .ti{width:36px;height:36px;border-radius:11px;display:flex;align-items:center;justify-content:center;flex-shrink:0;}
.tile .ti svg{width:18px;height:18px;stroke:currentColor;fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
.ti.ac{background:var(--ac-dim);color:var(--ac);} .ti.ok{background:var(--ok-dim);color:var(--ok);} .ti.warn{background:var(--warn-dim);color:var(--warn);}
.tile .tl{font-size:13px;font-weight:700;color:var(--tx);}
.tile .ts{font-size:11px;color:var(--tx3);margin-top:1px;}
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

<div class="card">
<div class="card-head"><div class="card-title">Status Lampu</div><span class="badge gray" id="lampBadge">Memuat...</span></div>
<div class="info-row"><span class="k">Mode Riding</span><span class="v" id="rdMode">--</span></div>
<div class="info-row"><span class="k">Tukar Sisi</span><span class="v" id="rdSwap">--</span></div>
<div class="bri-row"><span class="k">Kecerahan</span><div class="progress"><div class="fill" id="rdBriFill"></div></div><b id="rdBri">--</b></div>
</div>

<div class="card">
<div class="card-title">Tegangan Aki</div>
<div class="volt-row">
<div class="icobox warn" id="voltIco"><svg viewBox="0 0 24 24"><path d="M13 2L4 14h6l-1 8 9-12h-6l1-8z"/></svg></div>
<div class="volt-mid">
<span class="volt-big" id="voltBig">--</span><span class="volt-unit"> V</span>
<div class="progress" style="margin-top:6px;"><div class="fill" id="voltFill"></div></div>
</div>
<span class="badge gray" id="voltBadge">Sensor belum terbaca</span>
</div>
</div>

<div class="quick-tiles">
<a class="tile" href="/riding"><div class="ti ac"><svg viewBox="0 0 24 24"><path d="M9 18h6"/><path d="M10 21h4"/><path d="M12 3a6 6 0 0 0-3.6 10.8c.7.6 1.1 1.4 1.3 2.2h4.6c.2-.8.6-1.6 1.3-2.2A6 6 0 0 0 12 3z"/></svg></div><div><div class="tl">Riding</div><div class="ts">Efek &amp; playlist</div></div></a>
<a class="tile" href="/color"><div class="ti ok"><svg viewBox="0 0 24 24"><path d="M12 3s6 6.5 6 11a6 6 0 0 1-12 0c0-4.5 6-11 6-11z"/></svg></div><div><div class="tl">Color</div><div class="ts">Warna &amp; skin</div></div></a>
<a class="tile" href="/welcoming"><div class="ti warn"><svg viewBox="0 0 24 24"><path d="M12 3l2.4 5.2 5.6.7-4.1 3.9 1 5.6L12 15.6 7.1 18.4l1-5.6L4 8.9l5.6-.7z"/></svg></div><div><div class="tl">Welcoming</div><div class="ts">Segera hadir</div></div></a>
<a class="tile" href="/sein-rem"><div class="ti ac"><svg viewBox="0 0 24 24"><path d="M3 12h13M11 6l6 6-6 6M20 6v12"/></svg></div><div><div class="tl">Sein-Rem</div><div class="ts">Sein &amp; rem</div></div></a>
</div>
</div>
%BOTTOMNAV%
<script src="/mizuma.js"></script>
<script>
(function(){
var h = new Date().getHours(), g = 'SELAMAT MALAM';
if (h >= 4 && h < 11) g = 'SELAMAT PAGI';
else if (h >= 11 && h < 15) g = 'SELAMAT SIANG';
else if (h >= 15 && h < 18) g = 'SELAMAT SORE';
document.getElementById('greetText').textContent = g;

var $ = function(i){ return document.getElementById(i); };
var MODE = { 'manual':'Manual', 'semi-auto':'Semi Auto', 'auto':'Auto', 'playlist':'Playlist' };
/* Ambang tegangan aki 12V (ubah di sini bila perlu). */
var V_MIN = 10.5, V_MAX = 15.0, V_LOW = 11.5, V_WARN = 12.2, V_HIGH = 14.8;
function getJ(u){ return fetch(u, { cache:'no-store' }).then(function(r){ if(!r.ok) throw 0; return r.json(); }).catch(function(){ return null; }); }

function setVolt(raw){
  var v = parseFloat(raw), big = $('voltBig'), fill = $('voltFill'), b = $('voltBadge'), ico = $('voltIco');
  if (!(v > 0)) { big.textContent = '--'; fill.style.width = '0'; b.className = 'badge gray'; b.textContent = 'Sensor belum terbaca'; ico.className = 'icobox warn'; return; }
  big.textContent = v.toFixed(1);
  fill.style.width = Math.max(0, Math.min(100, (v - V_MIN) / (V_MAX - V_MIN) * 100)) + '%';
  var lvl;
  if (v < V_LOW) lvl = ['Aki lemah', 'red', 'bad'];
  else if (v < V_WARN) lvl = ['Rendah', 'orange', 'warn'];
  else if (v <= V_HIGH) lvl = ['Normal', 'green', ''];
  else lvl = ['Terlalu tinggi', 'red', 'bad'];
  b.className = 'badge ' + lvl[1]; b.textContent = lvl[0];
  fill.className = 'fill ' + lvl[2];
  ico.className = 'icobox ' + (lvl[1] === 'green' ? 'ok' : 'warn');
}
window.mzOnStatus = function(d){ setVolt(d && d.volt); };
if (window.mzLast) window.mzOnStatus(window.mzLast);

function lamp(){
  if (document.hidden) return;
  Promise.all([ getJ('/mizuma/riding/mode'), getJ('/mizuma/side-swap'), getJ('/json/state') ]).then(function(r){
    var m = r[0], sw = r[1], st = r[2], badge = $('lampBadge');
    if (!m && !sw && !st) { badge.className = 'badge red'; badge.textContent = 'Terputus'; return; }
    if (st && typeof st.on === 'boolean') { badge.className = 'badge ' + (st.on ? 'green' : 'gray'); badge.textContent = st.on ? 'Menyala' : 'Mati'; }
    else { badge.className = 'badge gray'; badge.textContent = 'Terhubung'; }
    if (m && m.mode) {
      if (m.mode === 'playlist') {
        $('rdMode').textContent = 'Playlist';
        getJ('/mizuma/playlists').then(function(p){
          var l = p && p.lists && p.use >= 0 ? p.lists[p.use] : null;
          if (l) $('rdMode').textContent = 'Playlist \u00b7 ' + (l.n || ('Playlist ' + (p.use + 1)));
        });
      } else $('rdMode').textContent = MODE[m.mode] || m.mode;
    }
    if (sw) $('rdSwap').textContent = sw.enabled ? 'Aktif' : 'Tidak aktif';
    if (st && st.bri != null) { var p = Math.round(st.bri / 2.55); $('rdBri').textContent = p + '%'; $('rdBriFill').style.width = p + '%'; }
  });
}
lamp();
setInterval(lamp, 6000);
document.addEventListener('visibilitychange', function(){ if (!document.hidden) lamp(); });
})();
</script>
</body>
</html>
)rawliteral";

// --------------------------------------------------------------------------------------------------------------------------------------
// Blok 2b — Welcoming (sengaja dikosongkan: konsep baru sedang disusun, belum ada logika/ aksi lampu di halaman ini)
// --------------------------------------------------------------------------------------------------------------------------------------
const char MIZUMA_WELCOMING_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Welcoming - Mizuma</title>
<link rel="stylesheet" href="/mizuma.css">
<style>
.container{padding:16px;max-width:520px;margin:0 auto;}
.empty{text-align:center;padding:36px 16px;}
.empty .ei{width:56px;height:56px;border-radius:18px;background:var(--ac-dim);color:var(--ac);display:flex;align-items:center;justify-content:center;margin:0 auto 14px;}
.empty .ei svg{width:26px;height:26px;stroke:currentColor;fill:none;stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
.empty b{display:block;font-size:16px;margin-bottom:6px;}
.empty p{margin:0;font-size:13px;color:var(--tx2);line-height:1.5;}
</style>
</head>
<body>
%HEADER%
<div class="container">
<div class="card empty">
<div class="ei"><svg viewBox="0 0 24 24"><path d="M12 3l2.4 5.2 5.6.7-4.1 3.9 1 5.6L12 15.6 7.1 18.4l1-5.6L4 8.9l5.6-.7z"/></svg></div>
<b>Welcoming</b>
<p>Fitur ini sedang disusun ulang dan akan hadir pada pembaruan berikutnya.</p>
</div>
</div>
%BOTTOMNAV%
<script src="/mizuma.js"></script>
</body>
</html>
)rawliteral";

// --------------------------------------------------------------------------------------------------------------------------------------
// Blok 3 — Pengaturan (Informasi Motor · Koneksi & Jaringan · Firmware · Reset)
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
<div class="info-row"><span class="k">Firmware</span><span class="v">MIZUMA WLED %FIRMWARE%</span></div>
</div>
<div class="card danger">
<div class="card-title">Reset Pabrik</div>
<div class="placeholder-text" style="color:#c99;">Menghapus semua konfigurasi dan mengembalikan ke pengaturan awal</div>
<a class="btn danger" id="btnReset" href="/settings/sec">Buka Halaman Reset</a>
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
document.getElementById('btnReset').addEventListener('click', function(e){
if (!confirm('Semua pengaturan Mizuma akan dihapus dan perangkat kembali ke pengaturan awal. Lanjutkan?')) { e.preventDefault(); e.stopPropagation(); }
}, true);
async function refreshConn() {
if (document.hidden) return;
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
// BLOK 5 — SEIN-REM (halaman /sein-rem; nama konstanta MIZUMA_LED_HTML dipertahankan agar kompatibel)
// =====================================================================================================================================
const char MIZUMA_RIDING_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
<meta name="theme-color" content="#0a0a0a">
<title>Riding - Mizuma</title>
<style>
:root{--bg:#0a0a0a;--s1:#151515;--s2:#1c1c1c;--s3:#242424;--bd:#262626;--bd2:#333;--tx:#f2f2f2;--tx2:#9a9a9a;--tx3:#8a8a8a;--ac:#f5a524;--acd:rgba(245,165,36,.14);--ok:#3ddc84;--bad:#e05555;--nav:62px;color-scheme:dark;box-sizing:border-box;padding-top:env(safe-area-inset-top,0px)}
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
.fr.on{flex-wrap:wrap}.fx-actions{order:3;flex:1 1 100%;min-width:0;display:flex;align-items:center;gap:5px;flex-wrap:wrap;margin-left:31px}.rev-pair{display:flex;gap:4px}.rev-side{min-height:32px;padding:0 7px;border-radius:10px;border:1px solid var(--bd2);background:var(--s2);color:var(--tx2);font-size:10px;font-weight:800;white-space:nowrap}.rev-side.on{border-color:var(--ac);background:var(--acd);color:var(--ac)}.mirror-btn{min-height:32px;padding:0 8px;border-radius:10px;border:1px solid var(--bd2);background:var(--s2);color:var(--tx2);font-size:10px;font-weight:800;white-space:nowrap}.mirror-btn.on{border-color:var(--ac);background:var(--acd);color:var(--ac)}
.color-editor{height:100%;min-height:0;display:flex;flex-direction:column;gap:8px}.color-tabs{display:grid;grid-template-columns:1fr 1fr;gap:6px;flex:none}.color-tab{min-height:36px;border:1px solid var(--bd2);border-radius:10px;background:var(--s2);color:var(--tx2);font-size:12px;font-weight:700}.color-tab.on{border-color:var(--ac);background:var(--acd);color:var(--ac)}.color-pane{flex:1;min-height:0;overflow:auto;overscroll-behavior:contain}.color-pane[hidden]{display:none}.color-pane .pkx{margin:0}.color-group{font-size:11px;font-weight:800;color:var(--tx2);margin:7px 2px 6px}.color-holder{flex:1;min-height:0;overflow:hidden}.color-editor .rev-row{display:flex;align-items:center;justify-content:space-between;gap:8px;margin-bottom:8px}
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
.sls.g2 .sc .sd{position:absolute;top:6px;right:47px;width:30px;height:30px;min-width:0;min-height:0;padding:0;border:1px solid rgba(255,255,255,.35);border-radius:50%;background:rgba(0,0,0,.62);color:#fff;font-size:16px;line-height:1}
#sheet.sxe{background:rgba(0,0,0,.7)}#sheet.sxe #sh{max-width:none;width:100%;height:calc(100dvh - 14px);max-height:none;margin:auto auto 0;border-radius:16px 16px 0 0;display:flex;flex-direction:column;overflow:hidden;padding:12px;overscroll-behavior:contain}
#sheet.sxe.sev #sh{display:block;overflow:auto}
.sepv{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-bottom:8px;flex:none}.sepv canvas{width:100%;height:14px;image-rendering:pixelated;background:#000;border-radius:5px;display:block}
.se3{flex:1;min-height:0;display:grid;grid-template-columns:1fr;gap:8px;margin-bottom:8px}
.se3 .seb{display:flex;flex-direction:column;min-height:0;margin:0;padding:8px}
.se3 .cols2{flex:1;min-height:0;display:grid;grid-template-columns:minmax(0,1fr) minmax(0,1fr);gap:8px}.se3 .cols1{flex:1;min-height:0;display:flex}
.se3 .sec{display:flex;flex-direction:column;min-height:0;flex:1;min-width:0}.se3 .fsc{flex:1;height:auto;min-height:0}
.se3 .sopt{flex:none;min-height:0;align-items:flex-start;margin-bottom:6px}.se3 .sopt.v{flex-direction:column;align-items:stretch}.se3 .sopt .tgl{flex:1;padding:0 4px;font-size:11px;white-space:nowrap}.se3 .sopt.v .tgl{flex:none}
.se3 .fi{font-size:12px;padding:0 6px;height:32px;line-height:32px}
.se-color-row{display:flex;align-items:stretch;gap:4px;margin-top:6px}.se-color-row .skb{flex:1;margin-top:0}.se-color-row .tgl{flex:none;min-width:38px;padding:0 5px}
.se3 .skb{flex-direction:column;align-items:stretch;gap:3px;min-height:44px;padding:4px;margin-top:6px;flex:none}.se3 .skb i{width:100%;height:14px}.se3 .skb span{font-size:11px;text-align:center}
.se3 .se-color-row .skb{flex:1;margin-top:0}
.se3 .sect{font-size:11px;display:flex;align-items:center;justify-content:space-between;gap:4px}
.se3 .sect .tgl{min-height:30px;padding:0 6px;font-size:10px}
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

.ride-page{max-width:760px;margin:0 auto;padding-bottom:8px}
.ride-intro{margin:2px 2px 10px;color:var(--tx2);font-size:12px}
.ride-card{padding:12px;margin:9px 0;background:var(--s1);border:1px solid var(--bd);border-radius:14px}
.ride-card-head{display:flex;align-items:center;gap:8px;margin-bottom:9px}
.ride-card-head b{font-size:14px;flex:1}
.ride-card-head small{font-size:11px;color:var(--tx2)}
.ride-side-select-row{display:flex;align-items:center;gap:6px;margin:5px 0 8px}
.ride-side-tabs{display:flex;align-items:center;gap:5px;flex:1;min-width:0}
.ride-side-tab{flex:1;min-width:0;height:38px;padding:0 7px;text-align:center;border:1px solid var(--bd);border-radius:9px;background:var(--s2);color:var(--tx2);font-size:11px;font-weight:700;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.ride-side-tab.on{border-color:var(--ac);background:var(--acd);color:var(--ac)}
.ride-side-tab small{font-size:10px;color:inherit}
.ride-apply{min-height:38px;padding:0 10px;flex:none;border-radius:9px;background:var(--ac);color:#0a0a0a;font-size:11px;font-weight:800;white-space:nowrap}
.ride-side-tab.on{border-color:var(--ac);background:var(--acd)}
.ride-current{display:flex;align-items:center;gap:9px;min-width:0}
.ride-current-copy{flex:1;min-width:0}
.ride-current-copy b,.ride-current-copy small{display:block;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.ride-current-copy b{font-size:13px}
.ride-current-copy small{font-size:11px;color:var(--tx2)}
.ride-current .ride-color-cell{flex-direction:row;align-items:center}
.ride-current .ride-color-cell .ride-mini-btn{flex:1}
.ride-mini-btn{min-height:38px;padding:0 10px;border:1px solid var(--bd2);border-radius:10px;background:var(--s2);color:var(--tx);font-size:11px;font-weight:700;white-space:nowrap}
.ride-mini-btn.active{border-color:var(--ac);color:var(--ac);background:var(--acd)}
.ride-tool-row{display:flex;align-items:center;justify-content:flex-end;gap:6px;margin-top:8px}
.ride-colors{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:8px}
.ride-color-cell{display:flex;flex-direction:column;align-items:stretch;gap:4px;min-width:0}
.ride-color{display:flex;align-items:center;gap:7px;flex:1;min-width:0;padding:8px;border:1px solid var(--bd);border-radius:11px;background:var(--s2);text-align:left}
.ride-color i{width:30px;height:22px;flex:none;border:1px solid rgba(255,255,255,.25);border-radius:6px}
.ride-color span{flex:1;min-width:0;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;font-size:11px;font-weight:700}
.ride-toolbar{display:grid;grid-template-columns:minmax(0,1fr) minmax(0,1fr) minmax(102px,1.05fr);gap:6px;margin-top:9px}
.ride-toolbar.ride-toolbar-auto{grid-template-columns:minmax(0,1fr) minmax(102px,.8fr)}
.ride-toolbar .ride-mini-btn,.ride-toolbar .mirror-btn,.ride-toolbar .ride-apply,.ride-toolbar .rev-side{width:100%;min-width:0;min-height:34px;padding:0 5px;font-size:10px}
.ride-toolbar .ride-color{min-height:38px;padding:5px}
.ride-toolbar .ride-color i{width:26px;height:20px}
.ride-toolbar .ride-color span{font-size:10px}
.ride-toolbar .ride-shared-reverse{grid-column:span 2}
.ride-mode-badge{display:inline-flex;align-items:center;justify-content:center;min-width:74px;height:25px;padding:0 7px;border:1px solid var(--bd2);border-radius:7px;background:var(--s2);color:var(--tx3);font-size:9px;font-weight:800;white-space:nowrap}
.ride-mode-badge.on{border-color:var(--ok);background:rgba(61,220,132,.12);color:var(--ok);box-shadow:0 0 6px rgba(61,220,132,.18)}
.ride-mode-badge.off{opacity:.55}
.ride-section-title .ride-mini-btn{min-height:32px;padding:0 9px}
.ride-section-title{display:flex;align-items:center;gap:8px;margin:14px 2px 7px}
.ride-section-title b{font-size:14px}
.ride-section-title small{margin-left:auto;color:var(--tx2);font-size:10px}
.ride-fx-list{display:grid;gap:6px;max-height:37vh;overflow-y:auto;overscroll-behavior:contain}
.ride-fx-list .fr{min-height:48px;margin:0;padding:7px 8px;border-radius:11px}
.ride-fx-list .fr>b{font-size:12px}
.ride-fx-list .fr.on{flex-wrap:nowrap;background:var(--acd)}
.ride-fx-list .fr .no{width:30px;height:30px}
.ride-fx-list .fr .star{width:28px;min-width:28px;height:28px;min-height:28px;font-size:18px}
.ride-fx-state{flex:none;color:var(--ok);font-size:10px;font-weight:700}
.ride-fixed-params{width:100%;max-height:24vh;overflow-y:auto;overscroll-behavior:contain}
.ride-fixed-params .ride-card-head{margin:0 0 6px}
.ride-fixed-params .pt{font-size:12px}
.ride-playlist-grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:8px}
.ride-playlist-card{display:flex;flex-direction:column;align-items:flex-start;justify-content:center;gap:4px;min-width:0;min-height:66px;padding:10px;border:1px solid var(--bd);border-radius:11px;background:var(--s2);text-align:left}
.ride-playlist-card.on{border-color:var(--ac);background:var(--acd)}
.ride-playlist-card b,.ride-playlist-card small{max-width:100%;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.ride-playlist-card b{font-size:12px}
.ride-playlist-card small{color:var(--tx2);font-size:10px}
.ride-playlist-card.add{align-items:center;border-style:dashed;color:var(--tx2);font-size:11px}
.ride-slot-list{display:grid;gap:7px;margin-top:9px}
.ride-slot{display:grid;grid-template-columns:34px minmax(0,1fr) auto;align-items:center;gap:8px;min-height:58px;padding:7px;border:1px solid var(--bd);border-radius:11px;background:var(--s2)}
.ride-slot.empty{border-style:dashed}
.ride-slot-num{width:30px;height:30px;display:grid;place-items:center;border-radius:9px;background:var(--s3);font-size:12px;font-weight:800}
.ride-slot-copy{min-width:0}
.ride-slot-copy b,.ride-slot-copy small{display:block;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.ride-slot-copy b{font-size:11px}
.ride-slot-copy small{font-size:10px;color:var(--tx2)}
.ride-slot-actions{display:flex;gap:4px}
.ride-slot-actions .ib{width:34px;height:34px}
.ride-play-actions{display:flex;gap:7px;margin:8px 0}
#pop.quick{align-items:flex-start;justify-content:flex-end;background:rgba(0,0,0,.25);padding:58px 12px 12px}
#pop.quick>div{width:min(300px,calc(100vw - 24px));max-width:300px;margin:0;border-radius:14px;box-shadow:0 12px 35px #0009}
.quick-row{display:flex;align-items:center;gap:8px;margin:10px 0}
.quick-row input{flex:1;min-width:0}
.quick-toggle{width:100%;margin-top:4px}
#foot .apply-hint{margin:0;color:var(--tx2);font-size:10px;text-align:center}
@media(max-width:420px){.ride-toolbar.ride-toolbar-auto{grid-template-columns:minmax(0,1fr) minmax(90px,.8fr)}.ride-side-tabs,.ride-colors{gap:5px}.ride-slot{grid-template-columns:30px minmax(0,1fr) auto;gap:5px;padding:6px}.ride-slot-actions{gap:2px}.ride-slot-actions .ib{width:30px;height:30px}.ride-mini-btn{padding:0 7px}.ride-apply{padding:0 7px;font-size:10px}.ride-toolbar{grid-template-columns:minmax(0,1fr) minmax(0,1fr) minmax(90px,1fr);gap:4px}}

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
function jfetch(url,opt){if(url.indexOf('/json/')!==0)return jtry(url,opt,0);const task=jqTail.then(function(){return jtry(url,opt,0);});jqTail=task.catch(function(){});return task;}
function post(o){return jfetch('/json/state',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(o)}).catch(function(){});}
function debounce(fn,ms){let t;return function(){const a=arguments;clearTimeout(t);t=setTimeout(function(){fn.apply(null,a);},ms);};}
function throttleLatest(fn,ms){let last=0,t=null,args;return function(){args=arguments;const now=Date.now(),wait=ms-(now-last);if(wait<=0){clearTimeout(t);t=null;last=now;fn.apply(null,args);}else if(!t){t=setTimeout(function(){t=null;last=Date.now();fn.apply(null,args);},wait);}};}
const mpost=u=>fetch(u,{method:'POST',cache:'no-store'});

/* ===== konstanta ===== */
const SEG_COLOR_K=0,SEG_COLOR_L=1,SEG_K=2,SEG_L=3,SEG_LINK=4,SH=1000; // layer fisik: warna, efek per sisi, efek sambung
const FX_BLACKLIST=['frizzles','funky plank','game of life','geq','ghost rider','matrix','metaballs','tartan','polar lights','swirl','lissajous','rubix','invaders','maze','pulser','dna','plasma ball','hipnotic','black hole','cubes','fireworks','akemi','colored bursts','freqmatrix'];
const MZW_RE=/^(?:MZ\d\d|W\d\d) /;
const HIDE_AUDIO_FX=true;
const initialColorTab=location.pathname==='/color'||new URLSearchParams(location.search).get('t')==='warna';
const FX_PAL_DEFAULTS={'Fire 2012':'Fire','Fireworks 1D':'Rainbow','Fireworks':'Rainbow','Noise 1':'Pastel','Noise':'Pastel','Pride':'Pride','Aurora':'Aurora','Pacifica':'Pacifica'};
const SDN={all:'Semua',L:'Kiri',R:'Kanan'};
const DC=()=>({cs:'one',col:{h:39,s:100,v:100}});

/* ===== state ===== */
const S={tb:'fx',lastRideMode:'fx',fside:'all',sel:0,fs:{L:null,R:null},q:'',fo:false,
  fx:[],fxd:[],shows:[],showMs:[],list:[],palNames:[],palList:[],palRaw:{},palInfo:{cpal:0,cmax:10,um:[],major:0},palSig:'',
  fav:new Set(),fxs:{},fxsSaved:{},pm:{},modeSelected:{manual:null,manualLeft:null,'semi-auto':null,auto:null},cu:[],act:{L:null,R:null},lists:[] ,
  rideColors:{manual:{L:{cs:'pal',pal:2,col:{h:39,s:100,v:100}},R:{cs:'pal',pal:2,col:{h:39,s:100,v:100}}},'semi-auto':{L:{cs:'pal',pal:2,col:{h:39,s:100,v:100}},R:{cs:'pal',pal:2,col:{h:39,s:100,v:100}}},auto:{L:{cs:'pal',pal:2,col:{h:39,s:100,v:100}},R:{cs:'pal',pal:2,col:{h:39,s:100,v:100}}}},
  rideReverse:{manual:{R:false,L:false},semi:{R:false,L:false},auto:{R:false,L:false}},semiMirror:false,modeActive:'manual',
  pi:null,si:-1,se:null,dr:null,sp:0,qc:null,rideColorEdit:null,listsLoaded:false,
  pl:{use:-1,run:false,hold:false,list:0,slot:0,el:0,at:0,ui:false},showLive:{L:false,R:false},
  sideSwap:false,bri:180,net:true,sv:'ok',volt:null,pv:false,ready:false,popKind:null};
let legacyCu=[];
function loadLocal(){try{const d=JSON.parse(localStorage.getItem('mzRid4')||'null');if(d){S.fav=new Set(d.fav||[]);legacyCu=Array.isArray(d.cu)?d.cu:[];}}catch(e){}}
function saveLocal(){try{localStorage.setItem('mzRid4',JSON.stringify({fav:[...S.fav]}));}catch(e){}}
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
function pmKey(i,mode,side){const m=mode||rideMode(),sd=side||(m==='manual'?(S.fside==='L'?'L':'R'):'shared');return m+':'+sd+':'+i;}
function pm(i,mode,side){const key=pmKey(i,mode,side);if(!S.pm[key]){const d=fxDefs(i),g=function(k,df){return d[k]!=null?d[k]:df;};S.pm[key]={sx:g('sx',128),ix:g('ix',128),c1:g('c1',128),c2:g('c2',128),c3:g('c3',128),o1:g('o1',0),o2:g('o2',0),o3:g('o3',0)};}return S.pm[key];}
function pmDefault(i){const d=fxDefs(i),g=function(k,df){return d[k]!=null?d[k]:df;};return{sx:g('sx',128),ix:g('ix',128),c1:g('c1',128),c2:g('c2',128),c3:g('c3',128),o1:g('o1',0),o2:g('o2',0),o3:g('o3',0)};}
function palIdByName(n){return S.palNames.findIndex(function(x){return x===n;});}
function cfg(i){if(!S.fxs[i]){let c=DC();if(isShowId(i))c={cs:'pal',pal:0,col:{h:39,s:100,v:100}};else{const d=fxDefs(i);let pi=-1;if(d.pal!=null)pi=d.pal;else if(FX_PAL_DEFAULTS[fxName(i)])pi=palIdByName(FX_PAL_DEFAULTS[fxName(i)]);if(pi>0&&S.palNames[pi]&&S.palNames[pi].charAt(0)!=='*')c={cs:'pal',pal:pi,col:{h:39,s:100,v:100}};}S.fxs[i]=c;}return S.fxs[i];}
function rideColorModeKey(){const m=rideMode();return m==='semi'?'semi-auto':m==='auto'?'auto':'manual';}
function rideColor(k){const mode=rideColorModeKey(),set=S.rideColors[mode]||(S.rideColors[mode]={L:DC(),R:DC()});if(mode==='auto')return set.R||(set.R=DC());return set[k]||(set[k]=DC());}
function rideColorKeys(){return rideMode()==='auto'?['R']:sideKeys();}
function rideColorCurrent(){return rideColor(S.fside==='L'?'L':'R');}
function rideColorMode(){return rideMode()!=='pl';}
function rideColorPayload(c){const v=cfgCols(c,false),rgb=v.col[0];return{palette:c.cs==='one'?0:1,pal:c.cs==='one'?2:v.pal,r:rgb[0],g:rgb[1],b:rgb[2]};}
function rideColorStore(k){const mode=rideColorModeKey(),c=rideColor(k);return cuEnsure(c).then(function(){const v=rideColorPayload(c);
  return mpost('/mizuma/riding/colors?mode='+encodeURIComponent(mode)+'&side='+sideName(k)+'&palette='+v.palette+'&pal='+v.pal+'&r='+v.r+'&g='+v.g+'&b='+v.b);
}).then(function(r){if(!r.ok)throw new Error('riding color');return r.json();}).then(function(d){if(!d.ok)throw new Error(d.error||'riding color');});}
function rideColorFromApi(q){if(!q)return DC();const rgb=q.rgb||[255,255,255],hv=rgb2hsv(rgb[0],rgb[1],rgb[2]),skin=S.cu.find(function(k){return k.slot!=null&&cuId(k)===(q.pal|0);});
  return q.palette?(skin?{cs:'cus',cu:skin.id,col:{h:hv[0],s:hv[1],v:hv[2]}}:{cs:'pal',pal:q.pal|0,col:{h:hv[0],s:hv[1],v:hv[2]}}):{cs:'one',col:{h:hv[0],s:hv[1],v:hv[2]}};}
function loadRideColors(){return fetch('/mizuma/riding/colors').then(function(r){if(!r.ok)throw new Error('riding colors');return r.json();}).then(function(d){
  const modes=[['manual',d.manual||{right:d.right,left:d.left}],['semi-auto',d['semi-auto']],['auto',d.auto]];
  modes.forEach(function(m){if(!m[1])return;const q=m[1];S.rideColors[m[0]]={R:rideColorFromApi(q.right),L:rideColorFromApi(q.left||q.right)};});
  S.rideColors.auto.L=JSON.parse(JSON.stringify(S.rideColors.auto.R));
}).catch(function(){toast('Gagal memuat warna Riding dari perangkat');});}

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
  if(c.cs==='pal')return show&&c.pal===0?'Mizuma Asli':'Tema warna · '+(S.palNames[c.pal]||('Pilihan '+c.pal));
  if(c.cs==='cus'){const k=findCu(c.cu);return 'Warna buatan · '+(k?k.n:'-');}
  return 'Warna · '+rx(hsv(c.col.h,c.col.s,c.col.v));}
const ckn=(c,show)=>cfgName(c,show).replace(/^(Tema warna|Warna buatan|Warna) · /,'');

/* ===== sisi & segmen ===== */
function mapSegId(logical){const right=(logical==='kanan');return(right!==S.sideSwap)?SEG_K:SEG_L;}
function mapColorSegId(logical){const right=(logical==='kanan');return(right!==S.sideSwap)?SEG_COLOR_K:SEG_COLOR_L;}
const sideKeys=()=>S.fside==='all'?['R','L']:[S.fside];
const sideName=k=>k==='R'?'kanan':'kiri';
const sideArg=()=>S.fside==='all'?'both':sideName(S.fside);
function segIds(){return sideKeys().map(function(k){return mapSegId(sideName(k));});}
function rideMode(){const m=S.tb==='col'?S.lastRideMode:S.tb;return m==='fx'?'manual':m;}
function effectTargetIds(){const mode=rideMode();return mode==='semi'?[SEG_LINK]:mode==='auto'?[mapSegId('kanan'),mapSegId('kiri')]:segIds();}
function reverseValue(mode,k){const v=S.rideReverse[mode]||{};return mode==='semi'&&k==='R'?!!v.R!==!!v.L:!!v[k];}
function reverseForId(id){const mode=rideMode();if(mode==='semi'&&id===SEG_LINK)return reverseValue(mode,'R');const k=id===mapSegId('kanan')?'R':'L';return reverseValue(mode,k);}
function syncReverseControls(){document.querySelectorAll('[data-rev-mode][data-rev-side]').forEach(function(b){b.classList.toggle('on',reverseValue(b.dataset.revMode,b.dataset.revSide));b.setAttribute('aria-pressed',reverseValue(b.dataset.revMode,b.dataset.revSide)?'true':'false');});
  document.querySelectorAll('[data-mirror]').forEach(function(b){b.classList.toggle('on',S.semiMirror);b.setAttribute('aria-pressed',S.semiMirror?'true':'false');});}
function reverseButton(mode,k){if(mode==='semi')return'<button class="rev-side" data-rev-mode="semi" data-rev-side="R" aria-label="Balik arah efek untuk kedua sisi" title="Balik arah efek untuk kedua sisi" onclick="event.stopPropagation();toggleRideReverse(\'semi\',\'R\')">↶ Balik arah</button>';
  const label=k==='R'?'Kanan':'Kiri';return'<button class="rev-side" data-rev-mode="'+mode+'" data-rev-side="'+k+'" aria-label="Balik arah '+label.toLowerCase()+'" title="Balik arah '+label.toLowerCase()+'" onclick="event.stopPropagation();toggleRideReverse(\''+mode+'\',\''+k+'\')">↶ '+label+'</button>';}
function mirrorButton(){return'<button class="mirror-btn'+(S.semiMirror?' on':'')+'" data-mirror aria-pressed="'+(S.semiMirror?'true':'false')+'" onclick="toggleRideMirror()">Mirror</button>';}
function reverseControls(mode){if(['manual','semi'].indexOf(mode)<0)return'';if(mode==='semi')return'<div class="rev-pair">'+reverseButton('semi','R')+'</div>'+mirrorButton();
  return'<div class="rev-pair">'+reverseButton(mode,'L')+reverseButton(mode,'R')+'</div>';}
function toggleRideReverse(mode,k){if(!S.rideReverse[mode])return;const old=!!S.rideReverse[mode][k],oldL=!!S.rideReverse[mode].L,next=!reverseValue(mode,k);
  S.rideReverse[mode][k]=next;if(mode==='semi')S.rideReverse[mode].L=false;syncReverseControls();
  mpost('/mizuma/riding/reverse?mode='+(mode==='semi'?'semi-auto':mode)+'&side='+sideName(k)+'&enabled='+(next?'1':'0')).then(function(r){if(!r.ok)throw new Error('reverse');return r.json();}).then(function(d){if(!d.ok)throw new Error('reverse');}).catch(function(){S.rideReverse[mode][k]=old;if(mode==='semi')S.rideReverse[mode].L=oldL;syncReverseControls();toast('Gagal mengubah arah lampu');});}
function toggleRideMirror(){const old=S.semiMirror,next=!old;S.semiMirror=next;syncReverseControls();
  mpost('/mizuma/riding/mirror?enabled='+(next?'1':'0')).then(function(r){if(!r.ok)throw new Error('mirror');return r.json();}).then(function(d){if(!d.ok)throw new Error('mirror');}).catch(function(){S.semiMirror=old;syncReverseControls();toast('Gagal mengubah cerminan efek');});}
function colorTargets(){return sideKeys().map(function(k){return mapColorSegId(sideName(k));});}
function selOf(){if(rideMode()!=='manual'||S.fside==='all')return S.sel;const f=S.fs[S.fside];if(f!=null)return f;const a=S.act[S.fside];return a?a.fx:S.sel;}
const snap=i=>JSON.stringify({fx:i,c:cfg(i),p:isShowId(i)?0:pm(i)});
const effectSnap=(i,mode,side)=>JSON.stringify({fx:i,p:isShowId(i)?0:pm(i,mode,side)});
function isActive(i){return S.pl.use<0&&sideKeys().every(function(k){return S.act[k]&&S.act[k].fx===i;});}
function isSaved(i){const mode=rideMode();if(rideColorMode())return S.pl.use<0&&sideKeys().every(function(k){return S.act[k]&&S.act[k].fx===i&&S.act[k].s===effectSnap(i,mode,mode==='manual'?k:'shared');});
  const s=snap(i);return S.pl.use<0&&sideKeys().every(function(k){return S.act[k]&&S.act[k].s===s;});}

/* ===== lampu: pratinjau langsung ===== */
function plHold(){if(S.pl.run&&!S.pl.hold){S.pl.hold=true;S.pl.ui=true;mpost('/mizuma/pl/pause?on=1').catch(function(){});}}
function colorSegsFor(i,keys,overrides){return(keys||sideKeys()).map(function(k){
    const separate=rideColorMode(),c=overrides?overrides[k]:(separate?rideColor(k):cfg(i)),v=cfgCols(c,!separate&&isShowId(i));
    return{id:mapColorSegId(sideName(k)),on:true,fx:65,pal:separate&&c.cs==='one'?2:v.pal,sx:211,ix:77,c1:77,c2:0,c3:0,o1:0,o2:0,o3:0,col:v.col};
  });}
function segsFor(i,ids,keys){const p=pm(i),white=[[255,255,255],[0,0,0],[0,0,0]];
  const colors=colorSegsFor(i,keys);
  const effects=ids.map(function(id){return{id:id,on:true,fx:i,pal:0,rev:reverseForId(id),sx:p.sx,ix:p.ix,c1:p.c1,c2:p.c2,c3:p.c3,o1:p.o1,o2:p.o2,o3:p.o3,col:white};});
  return colors.concat(effects);}
function stopShowSides(keys){mpost('/mizuma/show/stop?side='+(keys.length===2?'both':sideName(keys[0]))).catch(function(){});}
function applyLive(i){
  const c=cfg(i),ks=sideKeys(),needs=[];
  if(c.cs==='cus'&&findCu(c.cu)&&cuNeed(findCu(c.cu)))needs.push(cuEnsure(c));
  if(rideColorMode())ks.forEach(function(k){const color=rideColor(k);if(color.cs==='cus'&&findCu(color.cu)&&cuNeed(findCu(color.cu)))needs.push(cuEnsure(color));});
  if(needs.length){Promise.all(needs).then(function(){applyLiveReady(i);}).catch(function(){toast('Gagal menerapkan warna');});return;}
  applyLiveReady(i);
}
function applyLiveReady(i){
  const ks=sideKeys(),mode=rideMode(),ids=effectTargetIds(),c=cfg(i);
  plHold();
  if(isShowId(i)){ks.forEach(function(k){S.showLive[k]=true;});const side=mode==='auto'||mode==='semi'?'both':sideArg();
    post({on:true,seg:segsFor(i,[],ks)}).then(function(){return mpost('/mizuma/show?index='+(i-SH)+'&side='+side);}).catch(function(){});}
  else{const st=ks.filter(function(k){return S.showLive[k];});if(st.length)stopShowSides(st);ks.forEach(function(k){S.showLive[k]=false;});
    const o={on:true,seg:segsFor(i,ids,ks)};if(mode==='auto')o.seg.push({id:SEG_LINK,on:false});if(S.bri>0)o.bri=S.bri;post(o);}
  S.pv=!isSaved(i);updInfo();}
const applyLiveD=throttleLatest(function(){applyLive(selOf());},120);
let rideColorWriteSeq=0;
const rideColorLiveD=throttleLatest(function(keys,seq){
  Promise.all(keys.map(rideColorStore)).then(function(){if(seq===rideColorWriteSeq){S.pv=!isSaved(selOf());updInfo();}})
    .catch(function(){S.pv=true;toast('Gagal menerapkan warna sisi ke lampu');});
},120);
function rideColorChanged(){
  const mode=rideColorModeKey();
  if(mode==='auto'){S.rideColors.auto.L=JSON.parse(JSON.stringify(rideColor('R')));}
  else if(S.fside==='all'){const v=JSON.parse(JSON.stringify(rideColor('R')));S.rideColors[mode].L=JSON.parse(JSON.stringify(v));S.rideColors[mode].R=v;}
  const keys=mode==='auto'?['R']:rideColorKeys(),seq=++rideColorWriteSeq;S.pv=true;rideColorLiveD(keys,seq);updInfo();
}
function rideColorQuickOpen(){
  const keys=rideMode()==='auto'?['R','L']:rideColorKeys(),base=S.fside==='L'?'L':'R',c=JSON.parse(JSON.stringify(rideColor(base))),colors={};
  keys.forEach(function(k){colors[k]=JSON.parse(JSON.stringify(S.fside==='all'?c:rideColor(k)));});
  S.rideColorEdit={keys:keys,colors:colors,revision:0};activeColorTab='wheel';rideColorQuickRender();
}
function rideColorQuickRender(){
  const e=S.rideColorEdit;if(!e)return;const c=e.colors[e.keys[0]],label=S.fside==='all'?'Kanan & kiri':SDN[S.fside];
  openSheet('<div style="font-weight:800;margin-bottom:8px">Warna sisi · '+label+'</div><div class="color-holder">'+colorEditorHTML(c,'rideColorQuickPal','rideColorQuickCu',false)+'</div><div class="row" style="margin-top:10px;flex:none"><button class="btn" onclick="rideColorQuickCancel()">Batal</button><button class="btn p" onclick="rideColorQuickApply()">Terapkan</button></div>',true);
  pkBind(c.col,function(){c.cs='one';rideColorQuickPreview();});refreshPalStrips();
}
function rideColorQuickPreview(){
  const e=S.rideColorEdit;if(!e)return;S.pv=true;const revision=++e.revision;
  if(S.fside==='all')e.colors.L=JSON.parse(JSON.stringify(e.colors.R));
  const colors=e.colors,ensure=e.keys.map(function(k){return cuEnsure(colors[k]);});
  Promise.all(ensure).then(function(){if(S.rideColorEdit!==e||e.revision!==revision)return;return post({seg:colorSegsFor(selOf(),e.keys,colors)});})
    .catch(function(){toast('Gagal menerapkan warna');});
}
function rideColorQuickPal(id){
  const e=S.rideColorEdit;if(!e)return;e.keys.forEach(function(k){e.colors[k].cs='pal';e.colors[k].pal=id;});
  rideColorQuickRender();rideColorQuickPreview();
}
function rideColorQuickCu(id){
  const e=S.rideColorEdit;if(!e)return;e.keys.forEach(function(k){e.colors[k].cs='cus';e.colors[k].cu=id;});
  rideColorQuickRender();rideColorQuickPreview();
}
function rideColorQuickCancel(){
  const e=S.rideColorEdit;if(!e)return;S.rideColorEdit=null;S.pv=!isSaved(selOf());closeSheet();
  post({seg:colorSegsFor(selOf(),e.keys)});
}
function rideColorQuickApply(){
  const e=S.rideColorEdit;if(!e)return;const mode=rideColorModeKey();if(mode==='auto'){S.rideColors.auto.R=JSON.parse(JSON.stringify(e.colors.R||e.colors.L));S.rideColors.auto.L=JSON.parse(JSON.stringify(S.rideColors.auto.R));}else e.keys.forEach(function(k){S.rideColors[mode][k]=JSON.parse(JSON.stringify(e.colors[k]));});
  S.rideColorEdit=null;closeSheet();rideColorChanged();toast('Warna sisi diterapkan dan disimpan di perangkat');
}
function postParam(k,v){post({seg:effectTargetIds().map(function(id){const o={id:id};o[k]=v;return o;})});}
const postParamD=throttleLatest(postParam,120);

/* ===== simpan ke engine ===== */
function ridingEffectModeName(){const m=rideMode();return m==='semi'?'semi-auto':m==='auto'?'auto':'manual';}
function ridingEffectQuery(i,side){const show=isShowId(i),mode=rideMode(),paramSide=mode==='manual'?side:'shared',p=show?pmDefault(i):pm(i,mode,paramSide);return new URLSearchParams({fx:show?0:i,isShow:show?1:0,showIndex:show?i-SH:0,sx:p.sx,ix:p.ix,c1:p.c1,c2:p.c2,c3:p.c3,o1:p.o1,o2:p.o2,o3:p.o3}).toString();}
function saveRidingEffect(i,k){const mode=ridingEffectModeName();return mpost('/mizuma/riding/effect?mode='+encodeURIComponent(mode)+'&side='+sideName(k)+'&'+ridingEffectQuery(i,k)).then(function(r){if(!r.ok)throw new Error('effect config');return r.json();}).then(function(d){if(!d.ok)throw new Error(d.error||'effect config');});}
function loadRidingEffects(){return fetch('/mizuma/riding/effects').then(function(r){if(!r.ok)throw new Error('effects');return r.json();}).then(function(d){
  const modes=[['manual',d.manual],['semi-auto',d['semi-auto']],['auto',d.auto]];
  modes.forEach(function(pair){const key=pair[0],group=pair[1];if(!group)return;const rows=key==='manual'?[['R',group.right],['L',group.left]]:[['R',group.shared]];
    rows.forEach(function(row){const side=row[0],e=row[1];if(!e||!e.valid)return;const fx=e.isShow?SH+(e.showIndex|0):(e.fx|0);const params={sx:e.sx,ix:e.ix,c1:e.c1,c2:e.c2,c3:e.c3,o1:e.o1?1:0,o2:e.o2?1:0,o3:e.o3?1:0};
      const uiMode=key==='semi-auto'?'semi':key;
      S.pm[uiMode+':'+(key==='manual'?side:'shared')+':'+fx]=params;
      if(key==='manual'){S.fs[side]=fx;S.modeSelected[side==='R'?'manual':'manualLeft']=fx;}else S.modeSelected[key]=fx;
    });
  });
  const active=S.modeActive==='semi'?'semi-auto':S.modeActive==='auto'?'auto':'manual';
  if(active==='manual'){
    [['R',d.manual&&d.manual.right],['L',d.manual&&d.manual.left]].forEach(function(row){const side=row[0],e=row[1];if(!e||!e.valid)return;const fx=e.isShow?SH+(e.showIndex|0):(e.fx|0);S.act[side]={fx:fx,s:effectSnap(fx,'manual',side)};S.showLive[side]=!!e.isShow;});
  }else if(d[active]&&d[active].shared&&d[active].shared.valid){
    const e=d[active].shared,fx=e.isShow?SH+(e.showIndex|0):(e.fx|0),snap=effectSnap(fx,active==='semi-auto'?'semi':'auto','shared');
    ['R','L'].forEach(function(side){S.act[side]={fx:fx,s:snap};S.showLive[side]=!!e.isShow;});
  }
}).catch(function(){/* perangkat firmware lama: gunakan preset WLED yang tersedia */});}
function restoreModeSelection(mode){const k=mode==='semi'?'semi-auto':mode==='auto'?'auto':'manual';
  if(k==='manual'){const side=S.fside==='L'?'L':'R';if(S.fs[side]!=null)S.sel=S.fs[side];}
  else if(S.modeSelected[k]!=null)S.sel=S.modeSelected[k];
  else if(k==='auto'&&S.shows.length)S.sel=SH;
  else if(k==='semi'){const f=S.list.find(function(x){return !x.show;});if(f)S.sel=f.i;}
}

function presetQuery(i,k){const show=isShowId(i),color=rideColorMode()?rideColor(k):cfg(i),c=cfgCols(color,show&&!rideColorMode()),p=show?{sx:128,ix:128,c1:128,c2:128,c3:128,o1:0,o2:0,o3:0}:pm(i,rideMode(),k);
  const q={fx:show?0:i,pal:c.pal,r:c.col[0][0],g:c.col[0][1],b:c.col[0][2],r1:c.col[1][0],g1:c.col[1][1],b1:c.col[1][2],r2:c.col[2][0],g2:c.col[2][1],b2:c.col[2][2],sx:p.sx,ix:p.ix,c1:p.c1,c2:p.c2,c3:p.c3,o1:p.o1,o2:p.o2,o3:p.o3};
  if(show){q.isShow=1;q.showIndex=i-SH;}else q.isShow=0;return new URLSearchParams(q).toString();}
function savePreset(k,i){return fetch('/mizuma/preset?slot=riding'+(k==='R'?'Kanan':'Kiri')+'&'+presetQuery(i,k),{method:'POST'}).then(function(r){if(!r.ok)throw new Error('HTTP '+r.status);return r.json();}).then(function(d){if(!d.ok)throw new Error('ditolak');});}
function persist(i,msg){const mode=rideMode(),ks=mode==='manual'?sideKeys():['R','L'];S.sv='saving';rs();
  const effectSides=mode==='manual'?ks:['R'],colorSides=mode==='auto'?['R']:ks;
  // Simpan snapshot warna dan efek ke ESP32 sebelum meminta engine mengaktifkan mode.
  return Promise.all(colorSides.map(function(k){return rideColorStore(k);}))
   .then(function(){return Promise.all(effectSides.map(function(k){return saveRidingEffect(i,k);}));})
   // Preset Riding lama hanya menjadi jembatan kompatibilitas Manual.
   // Menulisnya dari Semi/Auto akan menimpa konfigurasi Manual yang terpisah.
   .then(function(){return mode==='manual'?Promise.all(ks.map(function(k){return savePreset(k,i);})):Promise.resolve();})
   .then(function(){if(mode==='manual')S.modeSelected.manual=i;else if(mode==='semi')S.modeSelected['semi-auto']=i;else if(mode==='auto')S.modeSelected.auto=i;
     if(S.pl.use>=0)return mpost('/mizuma/pl/use?list=-1').then(function(r){if(!r.ok)throw new Error('playlist exit');return r.json();}).then(function(d){if(!d.ok)throw new Error('playlist exit');});})
   // Mode command memulihkan snapshot tersimpan melalui engine (termasuk Signature Show berulang).
   // Jangan jalankan applyLive() sesudahnya: endpoint preview sengaja bersifat satu kali.
   .then(function(){return setRidingMode(mode);})
   .then(function(){ks.forEach(function(k){S.act[k]={fx:i,s:effectSnap(i,mode,mode==='manual'?k:'shared')};S.showLive[k]=isShowId(i);});S.fxsSaved[i]=JSON.stringify(cfg(i));saveLocal();
     S.pl.use=-1;S.pl.run=false;S.pl.hold=false;S.pl.ui=false;S.pv=false;S.sv='ok';S.net=true;rs();toast(msg);R();})
   .catch(function(){S.sv='fail';rs();toast('Gagal menyimpan konfigurasi mode di perangkat.');});}
function useFx(i){if(rideMode()==='manual'&&S.fside!=='all')S.fs[S.fside]=i;S.sel=i;persist(i,fxName(i)+(rideMode()==='manual'&&S.fside!=='all'?' di LED '+SDN[S.fside].toLowerCase():'')+' dipakai untuk Riding');}
function restoreSaved(){const mode=rideMode();S.pv=false;S.pl.ui=false;setRidingMode(mode).then(loadSaved).then(loadRidingEffects).then(function(){restoreModeSelection(mode);S.sv='ok';R();}).catch(function(){toast('Gagal membatalkan pratinjau');});toast('Pratinjau dibatalkan');setTimeout(pollPl,500);R();}

/* ===== muat data ===== */
function cfgFromPreset(st){const c0=st.col[0],hv=rgb2hsv(c0[0],c0[1],c0[2]),col={h:hv[0],s:hv[1],v:hv[2]};
  if(st.pal===0)return{cs:'one',col:col};
  if(st.pal>palBase()-(S.palInfo.cmax||10)&&st.pal<=palBase()){const slot=palBase()-st.pal;let k=S.cu.find(function(z){return z.slot===slot;});
    if(!k&&S.palRaw[st.pal]&&S.palRaw[st.pal].length){const raw=S.palRaw[st.pal].filter(Array.isArray);let stops=raw.map(function(e){return[Math.round(e[0]/2.55),rx([e[1],e[2],e[3]])];});
      while(stops.length>8)stops.splice(1+Math.floor(Math.random()*(stops.length-2)),1);if(stops.length<2)stops=[[0,'#ff0000'],[100,'#0000ff']];
      k={id:'w'+slot,n:'Warna buatan '+(slot+1),st:stops,slot:slot,sig:null,ephemeral:true};S.cu.push(k);}
    if(k)return{cs:'cus',cu:k.id,col:col};}
  return{cs:'pal',pal:st.pal,col:col};}
function applyPresetState(d){['R','L'].forEach(function(k){const st=d&&d['riding'+(k==='R'?'Kanan':'Kiri')];if(!st||!st.valid)return;
  const fx=st.isShow?SH+(st.showIndex|0):(st.fx|0);
  if(st.isShow){
    const c0=st.col&&st.col[0]?st.col[0]:[255,165,0],hv=rgb2hsv(c0[0],c0[1],c0[2]);
    S.fxs[fx]=st.pal===0?{cs:'one',col:{h:hv[0],s:hv[1],v:hv[2]}}:{cs:'pal',pal:st.pal|0,col:{h:hv[0],s:hv[1],v:hv[2]}};
  }else S.fxs[fx]=cfgFromPreset(st);S.fxsSaved[fx]=JSON.stringify(S.fxs[fx]);
  if(!st.isShow)S.pm['manual:'+k+':'+fx]={sx:st.sx,ix:st.ix,c1:st.c1,c2:st.c2,c3:st.c3,o1:st.o1?1:0,o2:st.o2?1:0,o3:st.o3?1:0};S.fs[k]=fx;
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
  const verMatch=String(inf.ver||'').match(/^(\d+)\.(\d+)/);S.palInfo.major=verMatch?(parseInt(verMatch[1],10)||parseInt(verMatch[2],10)||0):0;
  if(!verMatch&&(inf.umpalnames!==undefined||inf.cpalmax!==undefined))S.palInfo.major=16;
  S.palInfo.cpal=inf.cpalcount||0;S.palInfo.cmax=inf.cpalmax||10;S.palInfo.um=Array.isArray(inf.umpalnames)?inf.umpalnames:[];
  S.cu.forEach(function(k){if(k.slot!=null&&k.slot>=S.palInfo.cpal)k.slot=null;});   // berkas palette hilang (mis. FS dibersihkan): pasang ulang saat dipakai
  S.palSig=names.length+'|'+S.palInfo.cpal+'|'+S.palInfo.um.length+'|'+S.palInfo.major;palXLoaded=palCacheLoad(S.palSig);S.palNames=names;buildPalList();palTry=0;return true;}).catch(function(){return false;});}
function loadSkinMeta(){return fetch('/mizuma/skins').then(function(r){if(!r.ok)throw new Error('skins');return r.json();}).then(function(rows){
  S.cu=Array.isArray(rows)?rows:[];S.cu.forEach(function(k){if(k.slot<0||k.slot>=S.palInfo.cpal)k.slot=null;k.sig=k.slot==null?null:cuFile(k);});const known={};S.cu.forEach(function(k){known[k.id]=true;});
  const pending=legacyCu.filter(function(k){return k&&k.id&&!known[k.id];});
  return Promise.all(pending.map(function(k){return saveCuMeta(k).then(function(){S.cu.push(k);});})).then(function(){legacyCu=[];saveLocal();});
}).catch(function(){toast('Gagal memuat warna buatan dari perangkat');});}

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
function frz(){const active=S.modeActive==='manual'?'fx':S.modeActive==='playlist'?'pl':S.modeActive,t=function(n,k){return[n,'',"goTb('"+k+"')",active===k];};
  if(S.tb==='col')return active==='pl'?'':sideBlock();
  return tbs([t('Manual','fx'),t('Semi-Auto','semi'),t('Auto','auto'),t('Playlist','pl')]);}
function setRidingMode(k){const m=k==='semi'?'semi-auto':k==='fx'?'manual':k;return mpost('/mizuma/riding/mode?mode='+encodeURIComponent(m)).then(function(r){if(!r.ok)throw new Error('riding mode');return r.json();}).then(function(d){if(!d.ok)throw new Error(d.error||'riding mode');return new Promise(function(resolve){setTimeout(resolve,90);});}).then(function(){S.modeActive=k==='fx'?'manual':k;syncRideModeBadge();});}
function goTb(k){S.tb=k;S.q='';S.pi=null;if(k!=='col')S.lastRideMode=k;
  if(k==='fx'&&S.fside==='all')S.fside='R';
  if(k==='semi'||k==='auto')S.fside='all';
  if(k==='pl'){R();$('#scr').scrollTop=0;ensureLists();return;}
  restoreModeSelection(k);S.pv=false;R();$('#scr').scrollTop=0;
  // Engine sudah menerapkan snapshot mode tersimpan. Preview UI tidak boleh menimpa runner Riding.
  setRidingMode(k==='fx'?'manual':k).then(function(){return loadRidingEffects();}).then(function(){restoreModeSelection(k==='fx'?'manual':k);R();}).catch(function(){toast('GPIO 0/1 belum siap atau mode gagal diaktifkan');});}

/* ===== Single Effect ===== */
function pvl(){
  if(S.tb==='pl'){const nm=S.pl.use>=0&&S.lists[S.pl.use]?S.lists[S.pl.use].n:(S.pl.run&&S.lists[S.pl.list]?S.lists[S.pl.list].n:'-');const on=S.pl.use>=0&&!S.pl.ui;
    return'<div class="pvl" id="pvl"><span>Di lampu · <b>'+esc(nm)+'</b></span><span class="bdg'+(on?' ok':'')+'">'+(on?'Menyala di motor':'Pratinjau saja')+'</span></div>';}
  const i=selOf(),sv=isSaved(i);return'<div class="pvl" id="pvl"><span>Di lampu · <b>'+esc(fxName(i))+'</b>'+(S.pv||S.pl.ui?' <button class="lnk" onclick="restoreSaved()">Batalkan</button>':'')+'</span><span class="bdg'+(sv?' ok':'')+'">'+(sv?'Menyala di motor':'Pratinjau saja')+'</span></div>';}
function fxList(){const sel=selOf(),mode=rideMode();let h='',n=0;
  S.list.forEach(function(f){const i=f.i;if(mode==='semi'&&f.show)return;if(mode==='auto'&&!f.show)return;if(S.fo&&!S.fav.has(i))return;n++;
    const on=sel===i,fv=S.fav.has(i),hid=S.q&&f.n.toLowerCase().indexOf(S.q.toLowerCase())<0;
    const activeMode=mode===S.modeActive;
    h+='<div class="fr ride-fx'+(on?' on':'')+'" data-n="'+esc(f.n.toLowerCase())+'" style="'+(hid?'display:none':'')+'" onclick="pick('+i+')"><span class="no">'+n+'</span><b>'+esc(f.n)+(f.show?' <em class="cb">✦ Mizuma</em>':'')+'</b>'+(on?'<span class="ride-mode-badge '+(activeMode?'on':'off')+'" data-ride-mode-badge>'+ (activeMode?'Mode Riding':'Mode Riding')+'</span>':'')+'<button class="star'+(fv?' on':'')+'" aria-label="Favorit" onclick="event.stopPropagation();tf('+i+')">'+(fv?'★':'☆')+'</button></div>';});
  if(!S.fx.length)return'<div class="emp">Memuat daftar efek…</div>';
  if(!h)return'<div class="emp">'+(S.fo?'Belum ada efek favorit. Ketuk efek lalu ★ untuk menambah.':'Tidak ada efek.')+'</div>';
  return h;}
function rideSideSelect(){return'<div class="ride-side-select-row"><div class="ride-side-tabs">'+['L','R'].map(function(k){const name=k==='R'?'Kanan':'Kiri',fx=S.fs[k]!=null?S.fs[k]:(S.act[k]?S.act[k].fx:S.sel);return'<button class="ride-side-tab'+(S.fside===k?' on':'')+'" onclick="selectRideSide(\''+k+'\')">'+name+' - '+esc(fxName(fx))+'</button>';}).join('')+'</div><button class="ride-apply" onclick="useFx(selOf())">Terapkan ke Riding</button></div>';}
function selectRideSide(k){S.fside=k;S.sel=selOf();R();}
function rideColorOpenFor(k){S.fside=k;rideColorQuickOpen();}
function rideColorButton(k,mode,withReverse){const c=rideColor(k),name=k==='R'?'Kanan':'Kiri';return'<div class="ride-color-cell"><button class="ride-color" onclick="rideColorOpenFor(\''+k+'\')"><i style="background:'+cfgGrad(c)+'"></i><span>Pilih Warna '+name+'</span></button>'+(withReverse?reverseButton(mode,k):'')+'</div>';}
function rideColorAllButton(){const c=rideColor('R');return'<div class="ride-color-cell"><button class="ride-color" onclick="rideColorOpenFor(\'all\')"><i style="background:'+cfgGrad(c)+'"></i><span>Pilih Warna / Skin</span></button></div>';}
function rideModeControls(mode){const apply='<button class="ride-apply" onclick="useFx(selOf())">Terapkan ke Riding</button>';
  if(mode==='semi')return'<div class="ride-toolbar">'+rideColorButton('L',mode,false)+rideColorButton('R',mode,false)+apply+'<div class="ride-shared-reverse">'+reverseButton('semi','R')+'</div>'+mirrorButton()+'</div>';
  return'<div class="ride-toolbar ride-toolbar-auto">'+rideColorAllButton()+apply+'</div>';}
function syncRideModeBadge(){const f=$('#frz');if(f)f.innerHTML=frz();document.querySelectorAll('[data-ride-mode-badge]').forEach(function(b){const active=S.modeActive===rideMode();b.classList.toggle('on',active);b.classList.toggle('off',!active);b.textContent='Mode Riding';});}
function vFx(){const mode=rideMode(),i=selOf(),name=fxName(i),isManual=mode==='manual',isSemi=mode==='semi',isAuto=mode==='auto';
  let h='<div class="ride-page"><div class="ride-intro">'+(isManual?'Pilih sisi, lalu atur efek dan warnanya.':isSemi?'Satu efek untuk kedua sisi. Warna kanan dan kiri diatur terpisah; arah dan mirror berlaku untuk keduanya.':'Pilih satu Signature Effect Mizuma, lalu pilih warna atau skin-nya.')+'</div>';
  if(isManual)h+=rideSideSelect();
  h+='<section class="ride-card"><div class="ride-card-head"><b>'+(isManual?'Efek sisi '+(S.fside==='L'?'kiri':'kanan'):isSemi?'Efek untuk kedua sisi':'Signature Effect')+'</b></div>'+
    '<div class="ride-current"><span class="ride-current-copy"><b>'+esc(name)+'</b><small>'+(isManual?'Sisi '+(S.fside==='L'?'kiri':'kanan'):isSemi?'Satu efek untuk kedua sisi':'Berjalan pada kedua sisi')+'</small></span>'+
    (isManual?'<div class="ride-color-cell"><button class="ride-mini-btn" onclick="rideColorQuickOpen()">Pilih Warna/Skin</button>'+reverseButton('manual',S.fside==='L'?'L':'R')+'</div>':'')+'</div>';
  if(isSemi||isAuto)h+=rideModeControls(mode);
  h+='<div class="ride-section-title"><b>'+(isSemi?'Pilih satu efek':isAuto?'Pilih Signature Effect':'Pilih efek')+'</b><button class="ride-mini-btn'+(S.fo?' active':'')+'" onclick="S.fo=!S.fo;R()">☆ Filter Efek Favorit</button></div><div class="ride-fx-list">'+fxList()+'</div></section>';
  h+='</div>';return h;}
function fqx(v){S.q=v;document.querySelectorAll('#scr .fr[data-n]').forEach(function(e){e.style.display=e.dataset.n.indexOf(v.toLowerCase())<0?'none':'';});}
function pick(i){if(rideMode()==='manual'&&S.fside!=='all'){S.fs[S.fside]=i;S.sel=i;}else{S.sel=i;S.fs={L:null,R:null};}applyLiveD(i);R();}
function tf(i){S.fav.has(i)?S.fav.delete(i):S.fav.add(i);saveLocal();R();}
function pbar(i){if(isShowId(i))return'<div class="m" style="width:100%;text-align:center">Animasi · '+fd(showSec(i))+' detik · diulang terus di Riding.</div>';
  const p=parseFxData(S.fxd[i]||''),v=pm(i);
  if(!p.sl.length&&!p.tg.length)return'<div class="m" style="width:100%;text-align:center">Efek ini tidak punya parameter tambahan.</div>';
  return'<div class="pb"><div class="pg2">'+p.sl.map(function(s){return'<label>'+esc(s.label)+'<input type="range" min="0" max="255" value="'+v[s.key]+'" oninput="prm(\''+s.key+'\',this.value)" onchange="updInfo()"></label>';}).join('')+'</div>'+
    (p.tg.length?'<div class="row" style="margin-top:6px;flex-wrap:wrap">'+p.tg.map(function(s){return'<button class="tgl'+(v[s.key]?' on':'')+'" onclick="tgp(\''+s.key+'\',this)">'+esc(s.label)+'</button>';}).join('')+'</div>':'')+'</div>';}
function prm(k,val){const i=selOf(),n=+val;if(rideMode()==='manual'&&S.fside==='all'){pm(i,'manual','R')[k]=n;pm(i,'manual','L')[k]=n;}else pm(i)[k]=n;plHold();postParamD(k,n);S.pv=!isSaved(i);}
function tgp(k,el){const i=selOf(),cur=pm(i),n=cur[k]?0:1;if(rideMode()==='manual'&&S.fside==='all'){pm(i,'manual','R')[k]=n;pm(i,'manual','L')[k]=n;}else cur[k]=n;el.classList.toggle('on',!!n);plHold();postParam(k,n);S.pv=!isSaved(i);updInfo();}
const saveLocalD=debounce(saveLocal,400);

/* ===== pemilih warna ===== */
function skinRows(c,fp,fc,show){ensurePalX();const cu=[...S.cu.keys()].reverse().map(function(i){const k=S.cu[i],tools=k.ephemeral?'':'<button class="ib" aria-label="Edit warna" onclick="event.stopPropagation();cpEdit('+i+')">✎</button><button class="ib" aria-label="Hapus warna" style="color:var(--bad)" onclick="event.stopPropagation();cpDel('+i+')">🗑</button>';return'<div class="pw cu'+(c.cs==='cus'&&c.cu===k.id?' on':'')+'" onclick="'+fc+'(\''+k.id+'\')"><span class="rd"></span><span class="pn" style="text-align:left">'+esc(k.n)+'<em class="cb">Buatan</em></span><span class="ps" style="background:'+cuG(k)+'"></span>'+tools+'</div>';}).join('');
  const bi=S.palList.map(function(e){return'<div class="pw'+(c.cs==='pal'&&c.pal===e.i?' on':'')+'" data-pid="'+e.i+'" onclick="'+fp+'('+e.i+')"><span class="rd"></span><span class="pn">'+esc(e.i===0?(show?'Mizuma Asli':'Default'):e.n)+'</span><span class="ps" style="background:'+(palGrad(e.i)||'var(--s3)')+'"></span></div>';}).join('');
  return'<div class="color-group">Tema warna</div><div class="pg">'+(bi||'<div class="m">Memuat pilihan warna…</div>')+'</div><div class="color-group">Warna buatanmu</div><div class="pg">'+(cu||'<div class="m">Belum ada warna buatan</div>')+'</div>';}
let activeColorTab='wheel';
function colorEditorHTML(c,fp,fc,show,collectionId){return'<div class="color-editor"><div class="color-tabs"><button class="color-tab'+(activeColorTab==='wheel'?' on':'')+'" data-color-tab="wheel" onclick="setColorTab(\'wheel\')">Warna</button><button class="color-tab'+(activeColorTab==='collection'?' on':'')+'" data-color-tab="collection" onclick="setColorTab(\'collection\')">Koleksi</button></div><div class="color-pane" data-color-pane="wheel"'+(activeColorTab==='wheel'?'':' hidden')+'>'+pkHTML(1)+'</div><div class="color-pane" data-color-pane="collection"'+(collectionId?' id="'+collectionId+'"':'')+(activeColorTab==='collection'?'':' hidden')+'>'+skinRows(c,fp,fc,show)+'</div></div>';}
function setColorTab(tab){activeColorTab=tab;document.querySelectorAll('.color-editor').forEach(function(e){e.querySelectorAll('[data-color-tab]').forEach(function(b){b.classList.toggle('on',b.dataset.colorTab===tab);});e.querySelectorAll('[data-color-pane]').forEach(function(p){p.hidden=p.dataset.colorPane!==tab;});});}
function vCol(){const i=selOf(),separate=rideColorMode(),c=separate?rideColorCurrent():cfg(i),show=!separate&&isShowId(i),note=!separate&&!fxUsesPal(i)?'<div class="m" style="margin:0 2px 8px">Efek ini tidak memakai pilihan warna.</div>':'';
  const head=separate?'<div class="m" style="margin:0 2px 8px">Warna sisi · <b style="color:var(--tx)">'+(S.fside==='all'?'Kanan & kiri':SDN[S.fside])+'</b>. Disimpan di perangkat.</div>':'<div class="m" style="margin:0 2px 8px">Warna untuk <b style="color:var(--tx)">'+esc(fxName(i))+'</b> · '+SDN[S.fside]+'</div>';
  return head+note+colorEditorHTML(c,'setPal','setCu',show);}
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
  if(['fx','semi'].indexOf(S.tb)>=0)return'<section class="ride-fixed-params ride-card"><div class="ride-card-head"><b>Atur gerakan efek</b><small>'+(S.tb==='fx'?'Untuk sisi ini':'Untuk kedua sisi')+'</small></div>'+pbar(i)+'</section>';
  if(S.tb==='col'){const d=rideColorMode()?true:(colDirty(i)||!isSaved(i));return'<div class="color-actions"><button class="btn fbtn sk" onclick="cpEdit(-1)">＋ Warna baru</button><button class="btn p fbtn" style="'+(d?'':'opacity:.55')+'" onclick="saveCol()">'+(d?'Simpan warna':'Warna tersimpan ✓')+'</button></div>';}
  return'';}
function wheel(){if(!$('#scr #pkw')||document.querySelector('#sheet.on'))return;const i=selOf(),separate=rideColorMode(),c=separate?rideColorCurrent():cfg(i);pkBind(c.col,function(){c.cs='one';if(separate)rideColorChanged();else colLive(i);});}
function colLive(i){const c=rideColorMode()?rideColorCurrent():cfg(i);document.querySelectorAll('.blk').forEach(function(b,k){b.classList.toggle('on',(k===0)===(c.cs==='one'));const r=b.querySelector('.rad');if(r)r.classList.toggle('on',(k===0)===(c.cs==='one'));});
  const m=document.querySelector('.blk .mini'),sp=document.querySelector('.blk .mt span');if(m)m.style.background='linear-gradient(90deg,'+css(hsv(c.col.h,c.col.s,c.col.v))+','+css(hsv(c.col.h,c.col.s,c.col.v))+')';if(sp)sp.textContent=rx(hsv(c.col.h,c.col.s,c.col.v));
  $('#foot').innerHTML=foot();if(rideColorMode())rideColorChanged();else applyLiveD();}

/* ===== sheet Warna cepat (tombol Warna di baris efek) ===== */
function qcOpen(i){const c=JSON.parse(JSON.stringify(cfg(i)));S.qc={i:i,c:c,orig:JSON.parse(JSON.stringify(c)),show:isShowId(i)};qcR();}
function qcPreview(){const d=S.qc;if(!d)return;S.fxs[d.i]=d.c;applyLive(d.i);}
function qcPal(id){S.qc.c.cs='pal';S.qc.c.pal=id;const q=$('#qcg'),b=q?q.scrollTop:0;qcR();const n=$('#qcg');if(n)n.scrollTop=b;qcPreview();}
function qcCu(id){S.qc.c.cs='cus';S.qc.c.cu=id;const q=$('#qcg'),b=q?q.scrollTop:0;qcR();const n=$('#qcg');if(n)n.scrollTop=b;qcPreview();}
function qcR(){const d=S.qc,c=d.c;
  openSheet('<div style="font-weight:800;margin-bottom:8px">Ubah warna · '+esc(fxName(d.i))+'</div><div class="color-holder">'+colorEditorHTML(c,'qcPal','qcCu',d.show,'qcg')+'</div><div class="row" style="margin-top:10px;flex:none"><button class="btn" onclick="qcCancel()">Batal</button><button class="btn p" onclick="qcApply()">Simpan warna</button></div>',true);
  pkBind(c.col,function(){c.cs='one';qcPreview();});refreshPalStrips();}
function qcCancel(){const d=S.qc;if(!d)return;S.fxs[d.i]=d.orig;S.qc=null;closeSheet();applyLive(d.i);R();}
function qcApply(){const d=S.qc;if(!d)return;S.fxs[d.i]=d.c;if(S.fside==='all')S.sel=d.i;else S.fs[S.fside]=d.i;S.qc=null;closeSheet();saveCol();}

/* ===== editor warna buatan ===== */
function cpEdit(i){if(i<0&&S.cu.length>=cuMax()){toast('Maksimal '+cuMax()+' warna buatan. Hapus salah satu untuk menambah.');return;}
  S.dr={i:i,from:S.qc?'qc':(S.se&&S.se.v==='col'?'se':null),c:i<0?{id:'c'+Date.now().toString(36),n:'',st:[[0,'#ff0000'],[100,'#0000ff']],slot:null,sig:null}:JSON.parse(JSON.stringify(S.cu[i]))};S.sp=0;cpR();}
function cpR(){const c=S.dr.c;openSheet('<div style="font-weight:800;margin-bottom:8px">'+(S.dr.i<0?'Warna baru':'Edit warna')+'</div><input class="srch" type="text" maxlength="24" placeholder="Nama warna" value="'+esc(c.n)+'" oninput="S.dr.c.n=this.value"><div class="gb" id="gbar" style="background:'+cuG(c)+'">'+c.st.map(function(s,j){return'<i class="stp'+(j===S.sp?' on':'')+'" style="left:'+s[0]+'%;background:'+s[1]+'"></i>';}).join('')+'</div><div class="row" style="margin-bottom:8px"><button class="btn sm" onclick="cpAdd()"'+(c.st.length>=8?' disabled':'')+'>＋ Titik</button><button class="btn sm" onclick="cpRm()"'+(c.st.length<=2?' disabled':'')+'>− Titik</button><button class="btn sm" onclick="cpRand()">🎲 Acak</button></div><div class="m" style="margin-bottom:6px">Geser titik pada bar untuk mengatur posisi · maks 8 titik</div>'+pkHTML(1)+'<div class="row cpf"><button class="btn" onclick="cpCancel()">Batal</button><button class="btn p" onclick="cpSave()">Simpan</button></div>');cpBind();}
function cpU(){const c=S.dr.c;$('#gbar').style.background=cuG(c);document.querySelectorAll('#gbar .stp').forEach(function(e,j){e.style.left=c.st[j][0]+'%';e.style.background=c.st[j][1];});}
function cpAdd(){const c=S.dr.c;if(c.st.length>=8)return;const s=srt(c);let bi=0,bg=0;for(let k=0;k<s.length-1;k++){const g=s[k+1][0]-s[k][0];if(g>bg){bg=g;bi=k;}}const a=s[bi],b=s[bi+1],pos=Math.round((a[0]+b[0])/2),ca=hex(a[1]),cb=hex(b[1]);c.st.push([pos,rx([(ca[0]+cb[0])/2,(ca[1]+cb[1])/2,(ca[2]+cb[2])/2])]);S.sp=c.st.length-1;cpR();}
function cpRm(){const c=S.dr.c;if(c.st.length<=2)return;c.st.splice(S.sp,1);S.sp=0;cpR();}
function cpRand(){const n=S.dr.c.st.length,b=Math.random()*360,m=Math.random()<.5;S.dr.c.st.forEach(function(s,k){s[1]=rx(hsv(m?(b+k*360/n+Math.random()*30)%360:Math.random()*360,65+Math.random()*35,70+Math.random()*30));});cpR();}
function cpBind(){const bar=$('#gbar'),sel=function(j){S.sp=j;bar.querySelectorAll('.stp').forEach(function(x,k){x.classList.toggle('on',k===j);});const a=rgb2hsv.apply(null,hex(S.dr.c.st[j][1]));S.dpk={h:a[0],s:a[1],v:a[2]};pkBind(S.dpk,function(){S.dr.c.st[S.sp][1]=rx(hsv(S.dpk.h,S.dpk.s,S.dpk.v));cpU();});};let dg=false;
  bar.onpointerdown=function(e){const b=bar.getBoundingClientRect(),x=(e.clientX-b.left)/b.width*100;let j=-1,m=1e9;S.dr.c.st.forEach(function(s,k){const d=Math.abs(s[0]-x);if(d<m){m=d;j=k;}});if(j<0||m*b.width/100>28)return;e.preventDefault();try{bar.setPointerCapture(e.pointerId);}catch(x2){}dg=true;if(S.sp!==j)sel(j);};
  bar.onpointermove=function(e){if(!dg)return;const b=bar.getBoundingClientRect();S.dr.c.st[S.sp][0]=Math.round(Math.max(0,Math.min(100,(e.clientX-b.left)/b.width*100)));cpU();};bar.onpointerup=bar.onpointercancel=function(){dg=false;};sel(S.sp);}
function cpCancel(){const f=S.dr&&S.dr.from;S.dr=null;if(f==='qc'&&S.qc)qcR();else if(f==='se'&&S.se)seR();else closeSheet();}
function cpSave(){const d=S.dr,c=d.c,old=d.i<0?null:JSON.parse(JSON.stringify(S.cu[d.i]));c.n=(c.n||'').trim()||('Warna '+(d.i<0?S.cu.length+1:d.i+1));
  if(d.i<0)S.cu.push(c);else{c.slot=S.cu[d.i].slot;S.cu[d.i]=c;}
  const f=d.from;S.sig=null;
  cuInstall(c).then(function(){S.dr=null;
    if(f==='qc'&&S.qc){S.qc.c.cs='cus';S.qc.c.cu=c.id;qcR();qcPreview();toast('Warna “'+esc(c.n)+'” tersimpan');}
    else if(f==='se'&&S.se){S.se.c={cs:'cus',cu:c.id,col:S.se.c.col||DC().col};seR();seColorPreview();toast('Warna tersimpan');}
    else{const i=selOf(),fc=rideColorMode()?rideColorCurrent():cfg(i);fc.cs='cus';fc.cu=c.id;closeSheet();if(rideColorMode())rideColorChanged();else applyLive(i);R();toast('Warna “'+esc(c.n)+'” tersimpan');}
    saveLocal();if(S.pi!=null)plSyncAll();})
   .catch(function(e){if(d.i<0)S.cu.pop();else S.cu[d.i]=old;toast(e&&e.message==='penuh'?'Ruang warna buatan penuh ('+cuMax()+').':'Gagal menerapkan warna.');});}
function cpDel0(i){const c=S.cu[i];fetch('/mizuma/skins/delete?id='+encodeURIComponent(c.id),{method:'POST'}).then(function(r){if(!r.ok)throw new Error('delete');return r.json();}).then(function(d){if(!d.ok)throw new Error('delete');S.cu.splice(i,1);let liveChanged=false;Object.keys(S.fxs).forEach(function(x){const f=S.fxs[x];if(f.cs==='cus'&&f.cu===c.id){if(+x===selOf())liveChanged=true;f.cs='pal';f.pal=0;}});
  if(S.qc&&S.qc.c.cs==='cus'&&S.qc.c.cu===c.id)S.qc.c={cs:'pal',pal:0,col:S.qc.c.col};
  S.lists.forEach(function(p,li){let changed=false;p.it.forEach(function(x){['L','R'].forEach(function(k){const sd=sideOf(x,k);if(sd.c.cs==='cus'&&sd.c.cu===c.id){sd.c={cs:'pal',pal:0,col:sd.c.col};if(x.man)x[k].c=sd.c;else x.c=sd.c;changed=true;}});});if(changed)plMark(li);});
  saveLocal();
  if(S.qc)qcR();else if(S.se&&S.se.v==='col'){const e=S.se;if(e.c.cs==='cus'&&e.c.cu===c.id)e.c={cs:'pal',pal:0,col:e.c.col};seR();}else{closeSheet();R();}if(liveChanged)applyLive(selOf());toast('Warna “'+esc(c.n)+'” dihapus');}
).catch(function(){toast('Gagal menghapus warna dari perangkat');});}
function cpDel(i){confirmDel('Hapus warna?','Warna “'+esc(S.cu[i].n)+'” akan dihapus. Efek yang memakainya kembali ke warna awal.',function(){cpDel0(i);});}

/* ================= Playlist Animasi (disimpan & dijalankan di engine) ================= */
const tot=p=>Math.round(p.it.reduce(function(z,x){return z+x.d;},0)*10)/10;
const sideOf=(x,k)=>x.man?x[k]:{f:x.f,c:x.c,p:x.p,rv:!!x.rv};
function fxDefId(){const e=S.list.find(function(z){return z.n==='Scan';})||S.list.find(function(z){return !z.show;})||S.list[0];return e?e.i:0;}
function newSide(f,c){return{f:f,c:c||(isShowId(f)?{cs:'pal',pal:0,col:DC().col}:cfg(f)?JSON.parse(JSON.stringify(cfg(f))):DC()),p:isShowId(f)?null:pmDefault(f),rv:false};}
function slotBg(x){const l=sideOf(x,'L'),r=sideOf(x,'R');if(!x.man||JSON.stringify(l.c)===JSON.stringify(r.c))return cfgGrad(l.c);return cfgGrad(l.c)+' top/100% 50% no-repeat,'+cfgGrad(r.c)+' bottom/100% 50% no-repeat';}
function slotTitle(x){const l=sideOf(x,'L'),r=sideOf(x,'R'),m=x.mode||'manual';return esc(m==='manual'&&l.f!==r.f?fxName(l.f)+' · '+fxName(r.f):fxName(r.f));}
function slotModeName(x){const m=x.mode||'manual';return m==='semi-auto'?'Semi Auto':m==='auto'?'Auto':'Manual';}
function thumb(p){return p.it.length?'<div class="th">'+p.it.map(function(x){return'<i style="background:'+slotBg(x)+'"></i>';}).join('')+'</div>':'<div class="th em2">Kosong</div>';}
/* konversi UI <-> engine */
function sideOut(sd){const f=sd.f,c=sd.c;if(isShowId(f)){const m=cfgCols(c,true);return{show:1,si:f-SH,fx:0,pal:m.pal,col:m.col,sx:128,ix:128,c1:128,c2:128,c3:128,o1:0,o2:0,o3:0,rv:sd.rv?1:0};}
  const m=cfgCols(c),p=sd.p||pm(f);return{show:0,si:0,fx:f,pal:m.pal,col:m.col,sx:p.sx,ix:p.ix,c1:p.c1,c2:p.c2,c3:p.c3,o1:p.o1,o2:p.o2,o3:p.o3,rv:sd.rv?1:0};}
function listOut(i){const p=S.lists[i];return{list:i,name:p.n,it:p.it.map(function(x){const mode=x.mode||'manual',R=sideOut(sideOf(x,'R')),L=sideOut(sideOf(x,'L'));if(mode==='auto'){const shared=JSON.parse(JSON.stringify(R));return{d:Math.round(x.d*10),mode:mode,mirror:false,R:shared,L:JSON.parse(JSON.stringify(shared))};}if(mode==='semi-auto'){L.show=R.show;L.si=R.si;L.fx=R.fx;L.sx=R.sx;L.ix=R.ix;L.c1=R.c1;L.c2=R.c2;L.c3=R.c3;L.o1=R.o1;L.o2=R.o2;L.o3=R.o3;L.rv=R.rv;return{d:Math.round(x.d*10),mode:mode,mirror:!!x.mirror,R:R,L:L};}return{d:Math.round(x.d*10),mode:mode,mirror:false,R:R,L:L};})};}
function sideIn(o,legacyRv){const f=o.show?SH+(o.si|0):(o.fx|0),c0=o.col[0],hv=rgb2hsv(c0[0],c0[1],c0[2]),col={h:hv[0],s:hv[1],v:hv[2]},rv=o.rv==null?!!legacyRv:!!o.rv;
  if(o.show)return{f:f,c:{cs:'pal',pal:o.pal|0,col:col},p:null,rv:rv};
  const st={pal:o.pal|0,col:o.col},c=cfgFromPreset(st);
  return{f:f,c:c,p:{sx:o.sx,ix:o.ix,c1:o.c1,c2:o.c2,c3:o.c3,o1:o.o1?1:0,o2:o.o2?1:0,o3:o.o3?1:0},rv:rv};}
function listIn(d){const it=(d.it||[]).map(function(s){const R=sideIn(s.R,s.rv),L=sideIn(s.L,s.rv),mode=['manual','semi-auto','auto'].indexOf(s.mode)>=0?s.mode:'manual';
  if(mode==='auto'){const shared=JSON.parse(JSON.stringify(R));return{d:s.d/10,mode:mode,mirror:false,rv:false,man:true,f:shared.f,c:shared.c,p:shared.p,L:JSON.parse(JSON.stringify(shared)),R:shared};}
  if(mode==='semi-auto'){L.f=R.f;L.p=R.p?Object.assign({},R.p):null;L.rv=!!R.rv;return{d:s.d/10,mode:mode,mirror:!!s.mirror,rv:false,man:true,f:R.f,c:R.c,p:R.p,L:L,R:R};}
  return{d:s.d/10,mode:mode,mirror:false,rv:false,man:true,f:R.f,c:R.c,p:R.p,L:L,R:R};});return{n:d.name||'Playlist',it:it};}
function loadLists(){return fetch('/mizuma/playlists').then(function(r){if(!r.ok)throw new Error('lists');return r.json();}).then(function(d){
  S.pl.use=d.use;const meta=Array.isArray(d.lists)?d.lists:[];
  S.lists=meta.map(function(l){return{n:l.n||'',it:[]};});
  const indexes=[];meta.forEach(function(l,i){if(l.n!==''||l.c>0)indexes.push(i);});
  return Promise.all(indexes.map(function(i){return fetch('/mizuma/pl/list?i='+i).then(function(r){if(!r.ok)throw new Error('list');return r.json();}).then(function(data){return{i:i,data:data};});}))
    .then(function(rows){rows.forEach(function(row){S.lists[row.i]=listIn(row.data);});S.listsLoaded=true;});
});}
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
  if(d.use>=0)S.modeActive='playlist';else if(S.modeActive==='playlist')S.modeActive='manual';
  if(ch){syncRideModeBadge();if(S.tb==='pl')R();}}).catch(function(){});}
setInterval(function(){if(!document.hidden&&(S.tb==='pl'||S.pl.use>=0))pollPl();},5000);
function plEl(){const p=S.pl;return p.run&&!p.hold?p.el+(Date.now()-p.at)/1000:p.el;}
setInterval(function(){if(S.tb!=='pl'||!S.pl.run||S.pl.hold)return;const l=S.lists[S.pl.list],x=l&&l.it[S.pl.slot];if(!x)return;let e=plEl();
  if(e>=x.d&&l.it.length>1){S.pl.el=e-x.d;S.pl.at=Date.now();S.pl.slot=(S.pl.slot+1)%l.it.length;if(!(SCD&&SCD.on)&&S.pi===S.pl.list)R();return;}
  const b=$('#pgb');if(b)b.style.width=Math.min(100,e/x.d*100)+'%';},200);

function plOpen(i){S.pi=i;S.si=-1;R();$('#scr').scrollTop=0;}
function plBack(){S.pi=null;S.si=-1;R();}
function plGallery(selected){let h='<div class="ride-playlist-grid">',count=0;
  S.lists.forEach(function(p,i){if(!p.n&&p.it.length===0)return;count++;h+='<button class="ride-playlist-card'+(i===selected?' on':'')+'" onclick="plOpen('+i+')" aria-label="Pilih playlist '+esc(p.n)+'"><b>'+esc(p.n||('Playlist '+(i+1)))+'</b><small>'+p.it.length+' dari 12 slot · '+fd(tot(p))+' detik</small></button>';});
  if(count<4)h+='<button class="ride-playlist-card add" onclick="newPl()">＋ Buat playlist</button>';
  if(count===0)h+='<div class="m" style="grid-column:1/-1">Belum ada playlist tersimpan.</div>';
  return h+'</div>'; }
function plFolder(i){const p=S.lists[i],run=S.pl.run&&S.pl.list===i&&!S.pl.hold,here=S.pl.run&&S.pl.list===i;
  let h='<section class="ride-card"><div class="ride-card-head"><b id="plHeading">'+esc(p.n)+'</b><small>'+p.it.length+' dari 12 slot</small></div><input class="srch" style="margin:0 0 8px;width:100%" type="text" maxlength="20" value="'+esc(p.n)+'" oninput="S.lists['+i+'].n=this.value;$(\'plHeading\').textContent=this.value;plMark('+i+')" aria-label="Nama playlist"><div class="m">Atur urutan dengan tombol panah. Buka slot untuk mengubah pengaturannya.</div><div class="ride-slot-list">';
  for(let k=0;k<12;k++){const x=p.it[k];if(!x){h+='<div class="ride-slot empty"><span class="ride-slot-num">'+(k+1)+'</span><span class="ride-slot-copy"><b>Slot belum diatur</b><small>Pilih efek untuk slot ini</small></span><div class="ride-slot-actions"><button class="ib" aria-label="Tambah slot '+(k+1)+'" onclick="slotAdd('+i+')">＋</button></div></div>';continue;}
    h+='<div class="ride-slot'+(here&&S.pl.slot===k?' on':'')+'"><span class="ride-slot-num">'+(k+1)+'</span><span class="ride-slot-copy"><b>'+slotTitle(x)+'</b><small>'+fd(x.d)+' detik · '+slotModeName(x)+'</small></span><div class="ride-slot-actions"><button class="ib" aria-label="Geser slot '+(k+1)+' ke atas" '+(k===0?'disabled':'')+' onclick="slotMove('+i+','+k+',-1)">↑</button><button class="ib" aria-label="Geser slot '+(k+1)+' ke bawah" '+(k===p.it.length-1?'disabled':'')+' onclick="slotMove('+i+','+k+',1)">↓</button><button class="ib" aria-label="Edit slot '+(k+1)+'" onclick="seOpen('+i+','+k+')">✎</button><button class="ib" aria-label="Hapus slot '+(k+1)+'" style="color:var(--bad)" onclick="slotDel('+i+','+k+')">×</button></div>'+(here&&S.pl.slot===k?'<i class="pgb" id="pgb" style="width:'+Math.min(100,plEl()/x.d*100)+'%"></i>':'')+'</div>';}
  return h+'</div><div class="ride-play-actions"><button class="btn'+(run?' p':'')+'" onclick="plPlay('+i+')">'+(run?'Jeda':'Putar playlist di Riding')+'</button><button class="btn" onclick="plStop('+i+')">Hentikan</button><button class="btn" style="color:var(--bad)" onclick="delPl('+i+')">Hapus playlist</button></div></section>';}
function slotMove(i,j,d){const to=j+d;if(to<0||to>=S.lists[i].it.length)return;slotReorder(i,j,to);}
function vPl(){if(!S.listsLoaded)return'<div class="emp">Memuat playlist dari perangkat…</div>';
  const existing=S.lists.findIndex(function(p){return !!p.n||p.it.length>0;});
  const idx=S.pi!=null&&S.lists[S.pi]?S.pi:existing;
  return'<div class="ride-page"><div class="ride-intro">Buat hingga 4 playlist, masing-masing berisi maksimal 12 slot.</div><section class="ride-card"><div class="ride-card-head"><b>Playlist saya</b><small>'+S.lists.filter(function(p){return !!p.n||p.it.length>0;}).length+' dari 4</small></div>'+plGallery(idx)+'</section>'+(idx>=0?plFolder(idx):'<section class="ride-card"><div class="emp">Buat playlist untuk mulai mengatur efek.</div><button class="btn p" onclick="newPl()">＋ Buat playlist</button></section>')+'</div>'; }
/* aksi playlist */
function plUseCall(i){return mpost('/mizuma/pl/use?list='+i).then(function(r){if(!r.ok)throw new Error('pl');});}
function plPlay(i){const l=S.lists[i];if(!l.it.length)return toast('Playlist kosong');
  if(S.pl.use===i&&S.pl.run){const on=!S.pl.hold;S.pl.hold=on;S.pl.at=Date.now();mpost('/mizuma/pl/pause?on='+(on?1:0)).catch(function(){});if(!on)S.pl.ui=false;R();return;}
  plFlush().then(function(){return plUseCall(i);}).then(function(){S.pl.use=i;S.pl.run=true;S.pl.hold=false;S.pl.list=i;S.pl.slot=0;S.pl.el=0;S.pl.at=Date.now();S.pl.ui=false;S.pv=false;S.modeActive='playlist';syncRideModeBadge();toast('Playlist “'+esc(l.n)+'” diputar dan dipakai untuk Riding');R();}).catch(function(){toast('Gagal memutar playlist');});}
function plStop(i){if(S.pl.use===i){mpost('/mizuma/pl/use?list=-1').then(function(){S.pl.use=-1;S.pl.run=false;S.pl.hold=false;S.pl.ui=false;S.modeActive='manual';syncRideModeBadge();toast('Playlist dihentikan, kembali ke Single Effect');setTimeout(function(){loadSaved().then(R);},400);R();}).catch(function(){});}
  else if(S.pl.run&&S.pl.list===i){mpost('/mizuma/pl/stop').then(function(){S.pl.run=false;S.pl.ui=false;R();});}}
function newPl(){const count=S.lists.filter(function(p){return !!p.n||p.it.length>0;}).length;if(count>=4)return toast('Maksimal 4 playlist');let i=S.lists.findIndex(function(p){return !p.n&&p.it.length===0;});if(i<0)return toast('Maksimal 4 playlist');S.lists[i]={n:'Playlist '+(i+1),it:[]};S.pi=i;S.si=-1;plMark(i);R();}
function delPl(i){confirmDel('Hapus playlist?','“'+esc(S.lists[i].n)+'” beserta '+S.lists[i].it.length+' slot di dalamnya akan dihapus.',function(){delPl0(i);});}
function delPl0(i){const p=S.lists[i];plFlush().then(function(){return mpost('/mizuma/pl/delete?list='+i);}).then(function(){S.pi=null;S.si=-1;S.pl.run=false;S.pl.hold=false;return loadLists();}).then(function(){closeSheet();R();toast('Playlist “'+esc(p.n)+'” dihapus');if(S.pl.use<0)setTimeout(function(){loadSaved().then(R);},400);}).catch(function(){toast('Gagal menghapus playlist');});}
function slotSel(i,j){S.si=j;plFlush().then(function(){return mpost('/mizuma/pl/play?list='+i+'&slot='+j+'&hold=1');}).then(function(){S.pl.run=true;S.pl.hold=true;S.pl.list=i;S.pl.slot=j;S.pl.el=0;S.pl.at=Date.now();S.pl.ui=true;R();}).catch(function(){toast('Gagal menampilkan slot');});}
function slotAdd(i){const l=S.lists[i];if(l.it.length>=12)return toast('Playlist penuh (12 slot)');const f=fxDefId(),s=newSide(f);l.it.push({d:5,mode:'manual',mirror:false,rv:false,man:false,f:f,c:s.c,p:s.p,L:s,R:s});S.pi=i;plMark(i);seOpen(i,l.it.length-1);}
function slotDel(i,j){confirmDel('Hapus efek ini?','“'+slotTitle(S.lists[i].it[j])+'” akan dihapus dari slot '+(j+1)+'.',function(){slotDel0(i,j);});}
function slotDel0(i,j){S.lists[i].it.splice(j,1);S.se=null;S.si=-1;closeSheet();plMark(i);R();toast('Slot dihapus');}
/* geser slot (sama dengan v19) */
let SCD=null,SCB=0;
const scTM=e=>{if(SCD&&SCD.on)e.preventDefault();};
function scDown(ev,i,k){if(ev.target.closest('.se,.sd')||(ev.pointerType==='mouse'&&ev.button!==0))return;const el=ev.currentTarget,st={i:i,k:k,el:el,x0:ev.clientX,y0:ev.clientY,on:false,to:k,id:ev.pointerId,touch:ev.pointerType!=='mouse'};SCD=st;
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
function seOpen(i,j){const x=S.lists[i].it[j];if(!x.man){x.L={f:x.f,c:JSON.parse(JSON.stringify(x.c)),p:x.p?Object.assign({},x.p):null,rv:!!x.rv};x.R={f:x.f,c:JSON.parse(JSON.stringify(x.c)),p:x.p?Object.assign({},x.p):null,rv:!!x.rv};x.man=true;}if(!x.mode)x.mode='manual';if(x.mirror==null)x.mirror=x.mode==='semi-auto'?!!S.semiMirror:false;
  S.pi=i;S.si=j;S.se={i:i,j:j,v:'main',dirty:false};R();seR();slotSelQuiet(i,j);}
function slotSelQuiet(i,j){plFlush().then(function(){return mpost('/mizuma/pl/play?list='+i+'&slot='+j+'&hold=1');}).then(function(){S.pl.run=true;S.pl.hold=true;S.pl.list=i;S.pl.slot=j;S.pl.ui=true;}).catch(function(){});}
function seClose(){const d=S.se&&S.se.dirty,i=S.se&&S.se.i;S.se=null;closeSheet();R();if(d){plFlush();toast('Slot tersimpan');}}
const favFirst=()=>{const a=S.list.map(function(e){return e.i;});return a.filter(function(f){return S.fav.has(f);}).concat(a.filter(function(f){return !S.fav.has(f);}));};
function seList(k){const x=seX(),cur=sideOf(x,k).f,mode=x.mode||'manual';const choices=favFirst().filter(function(f){return mode==='auto'?isShowId(f):(mode==='semi-auto'?!isShowId(f):true);});
  return'<div class="fsc" id="fsc_'+k+'">'+choices.map(function(f){return'<div class="fi'+(f===cur?' on':'')+'" onclick="seFx(\''+k+'\','+f+')">'+(S.fav.has(f)?'★ ':'')+esc(fxName(f))+(isShowId(f)?' <em class="cb">✦</em>':'')+'</div>';}).join('')+'</div>';}

function seColBtn(k){const x=seX(),sd=sideOf(x,k),c=sd.c,f=sd.f,mode=x.mode||'manual';return'<div class="se-color-row"><button class="skb" onclick="seColOpen(\''+k+'\')"><i style="background:'+cfgGrad(c)+'"></i><span>'+esc(ckn(c,isShowId(f)))+'</span></button>'+(mode==='manual'?'<button class="tgl'+(sd.rv?' on':'')+'" aria-label="Balik arah '+(k==='L'?'kiri':'kanan')+'" onclick="seRv(\''+k+'\')">↶</button>':'')+'</div>';}

function seParams(k){const sd=sideOf(seX(),k);if(isShowId(sd.f))return'<div class="m separam-hint">Parameter efek tidak tersedia pada animasi Mizuma.</div>';
  const m=parseFxData(S.fxd[sd.f]||'');if(!m.sl.length&&!m.tg.length)return'<div class="m separam-hint">Efek ini tidak memiliki parameter tambahan.</div>';
  const p=sd.p||(sd.p=pmDefault(sd.f));
  return'<div class="separams">'+m.sl.map(function(s){const v=p[s.key]!=null?p[s.key]:128;return'<div class="separam"><label><span>'+esc(s.label)+'</span><span class="sepv">'+v+'</span></label><input type="range" min="0" max="255" value="'+v+'" oninput="seParam(\''+k+'\',\''+s.key+'\',this.value,this)"></div>';}).join('')+
    m.tg.map(function(s){const on=!!p[s.key];return'<button class="tgl'+(on?' on':'')+'" data-label="'+esc(s.label)+'" style="margin:2px" onclick="seParamToggle(\''+k+'\',\''+s.key+'\',this)">'+esc(s.label)+': '+(on?'On':'Off')+'</button>';}).join('')+'</div>';}
function seColumn(k,t){return'<div class="sec"><div class="sect">'+t+'</div>'+seList(k)+seParams(k)+seColBtn(k)+'</div>';}
const tgl=(on,onc,l)=>'<button class="tgl'+(on?' on':'')+'" onclick="'+onc+'">'+l+'</button>';
function seModeChange(mode){const x=seX(),old=x.mode||'manual';if(['manual','semi-auto','auto'].indexOf(mode)<0)return;
  if(mode==='semi-auto'){let shared=JSON.parse(JSON.stringify(x.R||x.L));if(isShowId(shared.f)){const q=S.list.find(function(z){return !z.show;});if(q)shared=newSide(q.i,shared.c);}x.R.f=shared.f;x.R.p=shared.p?Object.assign({},shared.p):null;x.R.rv=!!shared.rv;x.L.f=shared.f;x.L.p=shared.p?Object.assign({},shared.p):null;x.L.rv=!!shared.rv;}
  else if(mode==='auto'){let shared=JSON.parse(JSON.stringify(x.R||x.L));if(!isShowId(shared.f)){const q=S.list.find(function(z){return z.show;});if(q)shared=newSide(q.i,{cs:'pal',pal:0,col:DC().col});}x.R.f=shared.f;x.L.f=shared.f;x.R.p=x.L.p=null;x.R.rv=x.L.rv=false;x.L.c=JSON.parse(JSON.stringify(x.R.c));}
  else if(old==='auto'){const f=fxDefId();x.R=newSide(f,x.R.c);x.L=newSide(f,x.L.c);}
  x.mode=mode;x.mirror=mode==='semi-auto'?!!S.semiMirror:false;S.se.dirty=true;plMark(S.se.i);R();seR();seLive();}
function seModePicker(){const m=seX().mode||'manual';return'<label class="m" style="display:block;margin:0 0 8px">Mode slot<select style="width:100%;margin-top:4px" onchange="seModeChange(this.value)"><option value="manual"'+(m==='manual'?' selected':'')+'>Manual · efek kanan-kiri</option><option value="semi-auto"'+(m==='semi-auto'?' selected':'')+'>Semi Auto · satu efek, dua warna</option><option value="auto"'+(m==='auto'?' selected':'')+'>Auto · Signature Effect</option></select></label>';}
function seSharedBody(mode){const x=seX();let h='<div class="sec"><div class="sect">'+(mode==='auto'?'Signature Effect Mizuma':'Efek bersama')+'</div>'+seList('R')+seParams('R')+'</div>';
  if(mode==='semi-auto')h+='<div class="cols2">'+seColBtn('L')+seColBtn('R')+'</div><div class="rev-pair"><button class="rev-side'+(x.R.rv?' on':'')+'" aria-label="Balik arah efek bersama pada slot ini" onclick="seRv(\'R\')">↶ Arah bersama</button><button class="tgl'+(x.mirror?' on':'')+'" aria-pressed="'+(x.mirror?'true':'false')+'" onclick="seMirror()">Mirror</button></div>';
  else h+='<div class="sec"><div class="sect">Warna / Skin bersama</div>'+seColBtn('R')+'</div>';
  return h;}
function seMain(){const x=seX(),e=S.se,mode=x.mode||'manual';
  return'<div class="seh"><b>Edit slot '+(e.j+1)+'</b><button class="ib" aria-label="Tutup" onclick="seClose()">✕</button></div>'+seModePicker()+
  (mode==='manual'?'<div class="se3"><div class="seb"><div class="sbt">Efek sisi</div><div class="cols2">'+seColumn('L','Kiri')+seColumn('R','Kanan')+'</div></div></div>':seSharedBody(mode))+
  '<div class="seb dur"><label for="sedur">Durasi (detik)</label><input id="sedur" type="number" inputmode="decimal" min="0.1" max="999" step="0.5" value="'+fd(x.d)+'" onchange="seDur(this)"></div>'+
  '<div class="row cpf"><button class="btn" style="color:var(--bad)" onclick="slotDel('+e.i+','+e.j+')">🗑 Hapus</button><button class="btn p" onclick="seClose()">Selesai</button></div>';}
function seColView(){const e=S.se,c=e.c,show=isShowId(sideOf(seX(),e.tg).f),nm={L:'Kiri',R:'Kanan'}[e.tg];
  return'<div class="seh"><button class="ib" aria-label="Kembali" onclick="seColBack()">‹</button><b style="flex:1;margin-left:8px">Warna · '+nm+'</b></div><div id="ieg" class="color-holder">'+colorEditorHTML(c,'seColPal','seColCu',show)+'</div><div class="row cpf"><button class="btn" onclick="seColBack()">Batal</button><button class="btn p" onclick="seColSet()">Pilih warna</button></div>';}
function seR(){const e=S.se;if(!e)return;const sh=$('#sh'),keep={};sh.querySelectorAll('.fsc').forEach(function(a){keep[a.id]=a.scrollTop;});const top=sh.scrollTop;
  openSheet(e.v==='col'?seColView():seMain());$('#sheet').classList.add('sxe');$('#sheet').classList.toggle('sev',e.v==='col');
  if(e.v==='main'){sh.querySelectorAll('.fsc').forEach(function(a){if(keep[a.id]!=null)a.scrollTop=keep[a.id];else{const o=a.querySelector('.fi.on');if(o)a.scrollTop=Math.max(0,o.offsetTop-a.offsetTop-50);}});sh.scrollTop=top;}
  else if(e.v==='col'){const o=e.c.col;pkBind(o,function(){e.c.cs='one';const ch=$('#iech'),n=$('#iecn');if(ch)ch.style.background=cfgGrad(e.c);if(n)n.textContent=cfgName(e.c);seColorPreview();});}
  refreshPalStrips();}
function seTouch(){S.se.dirty=true;plMark(S.se.i);R();seR();seLive();}
function seLive(){const e=S.se;if(!e)return;plFlush().then(function(){return mpost('/mizuma/pl/play?list='+e.i+'&slot='+e.j+'&hold=1');}).catch(function(){});}
const seLiveD=debounce(seLive,180);
function seParam(k,key,val,el){const x=seX(),mode=x.mode||'manual',sd=sideOf(x,k);if(!sd.p)sd.p=pmDefault(sd.f);sd.p[key]=+val;
  if(mode==='semi-auto'){['L','R'].forEach(function(side){if(!x[side].p)x[side].p=Object.assign({},sd.p);x[side].p[key]=+val;});}
  if(el&&el.tagName==='INPUT'){const value=el.parentNode.querySelector('.sepv');if(value)value.textContent=val;}
  if(el&&el.tagName==='BUTTON'){const on=!!(+val);el.classList.toggle('on',on);el.textContent=(el.dataset.label||'Opsi')+': '+(on?'On':'Off');}
  S.se.dirty=true;plMark(S.se.i);seLiveD();}
function seParamToggle(k,key,el){const sd=sideOf(seX(),k);if(!sd.p)sd.p=pmDefault(sd.f);seParam(k,key,sd.p[key]?0:1,el);}
function seColorPreview(){const e=S.se;if(!e||e.v!=='col')return;sideOf(seX(),e.tg).c=JSON.parse(JSON.stringify(e.c));e.dirty=true;plMark(e.i);seLiveD();}
function seFx(k,f){const x=seX(),mode=x.mode||'manual',old=sideOf(x,k),s=newSide(f,isShowId(f)&&!isShowId(old.f)?null:(isShowId(old.f)&&!isShowId(f)?null:JSON.parse(JSON.stringify(old.c))));s.rv=!!old.rv;if(mode==='manual')x[k]=s;else if(mode==='semi-auto'){['L','R'].forEach(function(side){const c=x[side].c;x[side]=JSON.parse(JSON.stringify(s));x[side].c=c;});}else{const c=JSON.parse(JSON.stringify(s.c));['L','R'].forEach(function(side){x[side]=JSON.parse(JSON.stringify(s));x[side].c=JSON.parse(JSON.stringify(c));});}seTouch();}
function seRv(k){const x=seX(),mode=x.mode||'manual';sideOf(x,k).rv=!sideOf(x,k).rv;if(mode==='semi-auto')x.L.rv=x.R.rv=!!x.R.rv;seTouch();}
function seMirror(){const x=seX();if((x.mode||'manual')!=='semi-auto')return;x.mirror=!x.mirror;S.se.dirty=true;plMark(S.se.i);seR();seLiveD();}
function seDur(el){const v=Math.max(.1,Math.min(999,Math.round((+el.value||5)*10)/10));seX().d=v;el.value=fd(v);S.se.dirty=true;plMark(S.se.i);R();}
function seColOpen(k){const e=S.se,sd=sideOf(seX(),k);e.v='col';e.tg=k;e.orig=JSON.parse(JSON.stringify(sd.c));e.c=JSON.parse(JSON.stringify(sd.c));e.mode=e.c.cs==='one'&&!isShowId(sd.f)?'one':'skin';activeColorTab='wheel';seR();}
function seColBack(){const e=S.se;if(e&&e.orig){e.c=e.orig;sideOf(seX(),e.tg).c=JSON.parse(JSON.stringify(e.orig));e.orig=null;e.dirty=true;plMark(e.i);seLiveD();}S.se.v='main';seR();}
function seColMode(m){const e=S.se;if(m==='one')e.c.cs='one';else if(e.c.cs==='one'){e.c.cs='pal';e.c.pal=S.palList.length>1?S.palList[1].i:0;}e.mode=m;seR();seColorPreview();}
function seColPal(id){const e=S.se;e.c.cs='pal';e.c.pal=id;const b=$('#ieg').scrollTop;seR();$('#ieg').scrollTop=b;seColorPreview();}
function seColCu(id){const e=S.se;e.c.cs='cus';e.c.cu=id;const b=$('#ieg').scrollTop;seR();$('#ieg').scrollTop=b;seColorPreview();}
function seColSet(){const e=S.se,x=seX(),color=JSON.parse(JSON.stringify(e.c));sideOf(x,e.tg).c=color;if(x.mode==='auto'){x.L.c=JSON.parse(JSON.stringify(color));x.R.c=JSON.parse(JSON.stringify(color));}e.orig=null;e.v='main';seTouch();}

/* ===== pengaturan global ===== */
function briOpen(){$('#pop').innerHTML='<div><div style="font-weight:800;margin-bottom:10px">Pengaturan cepat</div><div class="quick-row"><span>☀</span><input id="gbi" type="range" min="1" max="255" value="'+S.bri+'"><b id="gbv" style="min-width:40px;text-align:right">'+Math.round(S.bri/2.55)+'%</b></div><button class="btn quick-toggle'+(S.sideSwap?' p':'')+'" onclick="reqSwap()">⇄ Tukar sisi · '+(S.sideSwap?'Aktif':'Tidak aktif')+'</button></div>';$('#pop').className='on quick';S.popKind='other';
  $('#gbi').oninput=function(e){S.bri=+e.target.value;$('#gbv').textContent=Math.round(S.bri/2.55)+'%';briSend(S.bri);};}
const briSend=debounce(function(v){post({bri:v});mpost('/mizuma/bri?v='+v).catch(function(){});},80);
function reqSwap(){const next=!S.sideSwap;jfetch('/mizuma/side-swap?enabled='+(next?'1':'0'),{method:'POST',cache:'no-store'}).then(function(r){if(!r.ok)throw 0;return r.json();}).then(function(d){if(!d.ok)throw 0;S.sideSwap=!!d.enabled;toast(S.sideSwap?'⇄ Output kanan-kiri ditukar':'Pemetaan kanan-kiri dikembalikan');setTimeout(function(){loadSaved().then(R);},400);briOpen();}).catch(function(){toast('Gagal mengubah posisi');});}

/* ===== navigasi bawah ===== */
const IC={dash:'<rect x="3" y="3" width="7" height="9" rx="1"/><rect x="14" y="3" width="7" height="5" rx="1"/><rect x="14" y="12" width="7" height="9" rx="1"/><rect x="3" y="16" width="7" height="5" rx="1"/>',
 wel:'<path d="M12 3l2.4 5.2 5.6.7-4.1 3.9 1 5.6L12 15.6 7.1 18.4l1-5.6L4 8.9l5.6-.7z"/>',rid:'<path d="M9 18h6M10 21h4M12 3a6 6 0 0 0-3.6 10.8c.7.6 1.1 1.4 1.3 2.2h4.6c.2-.8.6-1.6 1.3-2.2A6 6 0 0 0 12 3z"/>',
 col:'<path d="M12 3s6 6.5 6 11a6 6 0 0 1-12 0c0-4.5 6-11 6-11z"/>',rem:'<path d="M3 12h13M11 6l6 6-6 6M20 6v12"/>',set:'<circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.7 1.7 0 0 0 .3 1.8l.1.1a2 2 0 1 1-2.8 2.8l-.1-.1a1.7 1.7 0 0 0-1.8-.3 1.7 1.7 0 0 0-1 1.5V21a2 2 0 1 1-4 0v-.1a1.7 1.7 0 0 0-1.1-1.5 1.7 1.7 0 0 0-1.8.3l-.1.1a2 2 0 1 1-2.8-2.8l.1-.1a1.7 1.7 0 0 0 .3-1.8 1.7 1.7 0 0 0-1.5-1H3a2 2 0 1 1 0-4h.1a1.7 1.7 0 0 0 1.5-1.1 1.7 1.7 0 0 0-.3-1.8l-.1-.1a2 2 0 1 1 2.8-2.8l.1.1a1.7 1.7 0 0 0 1.8.3H9a1.7 1.7 0 0 0 1-1.5V3a2 2 0 1 1 4 0v.1a1.7 1.7 0 0 0 1 1.5 1.7 1.7 0 0 0 1.8-.3l.1-.1a2 2 0 1 1 2.8 2.8l-.1.1a1.7 1.7 0 0 0-.3 1.8V9a1.7 1.7 0 0 0 1.5 1H21a2 2 0 1 1 0 4h-.1a1.7 1.7 0 0 0-1.5 1z"/>'};
/* Tab utama: Dashboard · Color · Welcoming · Riding · Sein-Rem · Pengaturan. Color dan Riding berada di halaman ini; lainnya halaman terpisah. */
const TABS=[['dash','Dashboard',"location.href='/app'"],['col','Color',"goNav('col')"],['wel','Welcoming',"location.href='/welcoming'"],['rid','Riding',"goNav('fx')"],['rem','Sein-Rem',"location.href='/sein-rem'"],['set','Pengaturan',"location.href='/pengaturan'"]];
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
  $('#nav').innerHTML=navHTML();$('#scr').scrollTop=top;wheel();refreshPalStrips();syncReverseControls();}

/* ===== keluar halaman: kembalikan lampu ke Riding tersimpan bila masih pratinjau ===== */
window.addEventListener('pagehide',function(){try{
  if(S.pl.ui)navigator.sendBeacon('/mizuma/pl/stop','');
  else if(S.pv||S.rideColorEdit){const mode=rideMode(),wire=mode==='semi'?'semi-auto':mode;navigator.sendBeacon('/mizuma/riding/mode?mode='+encodeURIComponent(wire),'');}
  navigator.sendBeacon('/mizuma/riding/context?active=0','');}catch(e){}});

/* ===== init ===== */
function boot(){
  Promise.all([loadFxData(),loadPalettes()]).then(function(r){
    if(!r[0]){setTimeout(boot,1500);return null;}
    return Promise.all([jfetch('/mizuma/side-swap').then(function(x){return x.json();}).then(function(d){S.sideSwap=!!d.enabled;}).catch(function(){}),
      jfetch('/mizuma/riding/reverse').then(function(x){return x.json();}).then(function(d){['manual','semi-auto','auto'].forEach(function(m){const key=m==='semi-auto'?'semi':m;if(d[m]){S.rideReverse[key].R=!!d[m].kanan;S.rideReverse[key].L=!!d[m].kiri;}});}).catch(function(){}),
      jfetch('/mizuma/riding/mirror').then(function(x){return x.json();}).then(function(d){S.semiMirror=!!d.enabled;}).catch(function(){}),
      jfetch('/json/state').then(function(x){return x.json();}).then(function(j){if(j&&j.bri)S.bri=j.bri;}).catch(function(){}),loadSkinMeta().then(loadRideColors),loadSolidColorSlots(),
      fetch('/mizuma/riding/mode').then(function(x){return x.json();}).then(function(d){
        const m=d.mode==='semi-auto'?'semi':d.mode==='auto'?'auto':d.mode==='playlist'?'pl':'fx';S.modeActive=m==='pl'?'playlist':m==='fx'?'manual':m;S.lastRideMode=m;S.tb=initialColorTab?'col':m;S.fside=m==='fx'?'R':'all';if(m==='pl')ensureLists();
        return mpost('/mizuma/riding/context?active=1');
      }).catch(function(){})]).then(function(){return Promise.all([loadSaved(),loadRidingEffects()]);}).then(function(){restoreModeSelection(S.tb==='fx'?'manual':S.tb);});
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
<title>Sein-Rem - Mizuma</title>
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
<button data-tab="sein" class="active">Sein</button>
<button data-tab="rem">Rem</button>
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
let activeTab=new URLSearchParams(location.search).get('t')||localStorage.getItem('mzActiveTab')||'sein';
if(['sein','rem'].indexOf(activeTab)<0)activeTab='sein';
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
const HUE_RULES={sein:[25,60],rem:[345,15]};
const RESTRICTED_COLORS={sein:[{n:'Amber',h:'FFA500'},{n:'Kuning Tua',h:'FF8C00'},{n:'Amber Muda',h:'FFB347'},{n:'Kuning',h:'FFD700'}],rem:[{n:'Merah',h:'FF0000'},{n:'Merah Tua',h:'CC0000'},{n:'Merah Terang',h:'FF3333'},{n:'Merah Gelap',h:'8B0000'}]};
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
function throttleLatest(fn,ms){let last=0,t=null,args;return function(){args=arguments;const now=Date.now(),wait=ms-(now-last);if(wait<=0){clearTimeout(t);t=null;last=now;fn.apply(null,args);}else if(!t){t=setTimeout(function(){t=null;last=Date.now();fn.apply(null,args);},wait);}};}
function post(o){jfetch('/json/state',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(o)}).catch(function(){});}
function debounce(fn,ms){let t;return function(){const a=arguments;clearTimeout(t);t=setTimeout(function(){fn.apply(null,a);},ms);};}
function segIds(){if(activeSide==='kanan')return[mapSegId('kanan')];if(activeSide==='kiri')return[mapSegId('kiri')];return[mapSegId('kanan'),mapSegId('kiri')];}
function isRestrictedTab(){return activeTab==='sein'||activeTab==='rem';}
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
function palVersion(inf){const m=String(inf.ver||'').match(/^(\d+)\.(\d+)/);return m?(parseInt(m[1],10)||parseInt(m[2],10)||0):((inf.umpalnames!==undefined||inf.cpalmax!==undefined)?16:0);}
function palIdBase(){return palInfo.major>=16?200:255;}
function isPlaceholderPal(id){const d=palRaw[id];if(!d||!d.length||!Array.isArray(d[0]))return false;const f=d[0];if(!(f[1]===f[2]&&f[2]===f[3]))return false;return d.every(function(e){return Array.isArray(e)&&e[1]===f[1]&&e[2]===f[2]&&e[3]===f[3];});}
function palCacheLoad(sig){try{const c=JSON.parse(localStorage.getItem('mzPalCache')||'null');if(c&&c.sig===sig&&c.raw){palRaw=c.raw;Object.keys(palRaw).forEach(function(k){palSlot[k]=palRaw[k].some(function(x){return typeof x==='string'&&x!=='r';});});}}catch(e){}}
function palCacheSave(){try{localStorage.setItem('mzPalCache',JSON.stringify({sig:palSig,raw:palRaw}));}catch(e){}}
function loadPalettes(force){Promise.all([jfetch('/json/pal').then(function(r){return r.json();}),jfetch('/json/info').then(function(r){return r.json();})]).then(function(v){const names=v[0],inf=v[1]||{};palInfo.major=palVersion(inf);palInfo.cpal=inf.cpalcount||0;palInfo.um=Array.isArray(inf.umpalnames)?inf.umpalnames:[];const sig=names.length+'|'+palInfo.cpal+'|'+palInfo.um.length+'|'+palInfo.major;if(!force&&sig===palSig)return;const first=(palSig==='');palSig=sig;if(first)palCacheLoad(sig);palNamesRaw=names;const arr=[];names.forEach(function(n,i){if(n!=='r')arr.push({n:n,i:i});});arr.sort(function(a,b){if(a.i===0)return -1;if(b.i===0)return 1;const sa=a.n.charAt(0)==='*',sb=b.n.charAt(0)==='*';if(sa&&sb)return a.i-b.i;return a.n.localeCompare(b.n);});for(let k=0;k<palInfo.cpal;k++)arr.push({n:'~ Custom '+(k+1)+' ~',i:palIdBase()-k,custom:true});if(palInfo.major>=16)palInfo.um.forEach(function(n,k){arr.push({n:n||('Usermod '+(k+1)),i:255-k,custom:true});});palList=arr;renderPaletteRows(document.getElementById('searchBox').value.toLowerCase());renderColorRow();palTry=0;updateSideInfo();}).catch(function(){setTimeout(function(){loadPalettes(force);},1500);});}
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
  /* Sein dan Rem memakai penyaringan bawaan. */
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
function tabLabel(){return{sein:'Sein',rem:'Rem'}[activeTab];}
function subLabel(){return{template:'Warna Template',custom:'Warna Custom',efek:'Pilih Efek',simpan:'Riwayat Simpan'}[activeSub];}
function updateCtx(){document.getElementById('fxSavedName')?0:0;}
function updateModeAktif(){if(isRestrictedTab()){document.getElementById('modeAktif').textContent='Terbatas \u2014 '+restrictedName+' ('+tabLabel()+')';return;}if(activeTab==='welcoming'&&cur.isShow){document.getElementById('modeAktif').textContent='Animasi Mizuma \u2014 '+(showNames[cur.showIndex]||('MZ'+String(cur.showIndex+1).padStart(2,'0')));return;}if(activeSub==='custom'){document.getElementById('modeAktif').textContent='Custom \u2014 warna manual';}else{document.getElementById('modeAktif').textContent='Template \u2014 '+(cur.palName||'-')+(fxUsesPal(cur.fx)?'':' (efek ini tidak memakai palette)');}}
function refreshSavedInfo(){updateSideInfo();}
/* ===== SAVE ===== */
function doSave(){if(isRestrictedTab())normalizeRestrictedState(false);const sides=activeSide==='both'?['Kanan','Kiri']:[cap(activeSide)];const c0=segColors[0]||[255,255,255],c1=isRestrictedTab()?[0,0,0]:(segColors[1]||[0,0,0]),c2=isRestrictedTab()?[0,0,0]:(segColors[2]||[0,0,0]);const params=new URLSearchParams({fx:cur.fx,pal:isRestrictedTab()?0:cur.pal,isShow:(activeTab==='welcoming'&&cur.isShow)?1:0,showIndex:(activeTab==='welcoming'&&cur.isShow)?cur.showIndex:0,r:c0[0],g:c0[1],b:c0[2],r1:c1[0],g1:c1[1],b1:c1[2],r2:c2[0],g2:c2[1],b2:c2[2],sx:cur.params.sx!=null?cur.params.sx:128,ix:cur.params.ix!=null?cur.params.ix:128,c1:cur.params.c1!=null?cur.params.c1:128,c2:cur.params.c2!=null?cur.params.c2:128,c3:cur.params.c3!=null?cur.params.c3:128,o1:cur.params.o1!=null?cur.params.o1:0,o2:cur.params.o2!=null?cur.params.o2:0,o3:cur.params.o3!=null?cur.params.o3:0});sides.forEach(function(sd){fetch('/mizuma/preset?slot='+encodeURIComponent(activeTab+sd)+'&'+params.toString(),{method:'POST'}).catch(function(){});});clearDirty();const d=new Date();toast('\u2713 Tersimpan '+('0'+d.getHours()).slice(-2)+':'+('0'+d.getMinutes()).slice(-2));renderSavedList();refreshSavedInfo();}
function renderSavedList(){const box=document.getElementById('savedList');box.innerHTML='Memuat...';fetch('/mizuma/presets').then(function(r){return r.json();}).then(function(d){box.innerHTML='';['sein','rem'].forEach(function(t){['Kanan','Kiri'].forEach(function(s){const st=d[t+s];const row=document.createElement('div');row.className='saved-row';const on=!!(st&&st.valid);const txt=on?'Tersimpan':'Default';row.innerHTML='<span class="sk">'+cap(t)+' \u2014 '+s+'</span><span class="st'+(on?' on':'')+'">'+txt+'</span>';box.appendChild(row);});});}).catch(function(){box.innerHTML='Gagal memuat status.';});}
document.getElementById('btnSaveSub').addEventListener('click',doSave);
/* ===== MODAL ===== */
function showModal(action){pendingAction=action;document.getElementById('modalCtx').textContent=tabLabel()+' \u2014 '+sideLabel();document.getElementById('saveModal').style.display='flex';}
document.getElementById('mSave').addEventListener('click',function(){doSave();document.getElementById('saveModal').style.display='none';if(pendingAction)pendingAction();pendingAction=null;});
document.getElementById('mDiscard').addEventListener('click',function(){clearDirty();document.getElementById('saveModal').style.display='none';if(pendingAction)pendingAction();pendingAction=null;});
document.getElementById('mCancel').addEventListener('click',function(){document.getElementById('saveModal').style.display='none';pendingAction=null;});
/* ===== PRESET APPLY & UI UPDATE ===== */
function updateControlsFromPreset(tab,st){if(!st)return;cur.isShow=(tab==='welcoming'&&!!st.isShow);cur.showIndex=cur.isShow?Math.max(0,Math.min(showNames.length-1,Number(st.showIndex)||0)):0;cur.fx=cur.isShow?0:(st.fx!=null?st.fx:0);cur.pal=(tab==='sein'||tab==='rem')?0:(st.pal!=null?st.pal:0);const np=paramsFor(cur.fx);np.sx=st.sx!=null?st.sx:128;np.ix=st.ix!=null?st.ix:128;np.c1=st.c1!=null?st.c1:128;np.c2=st.c2!=null?st.c2:128;np.c3=st.c3!=null?st.c3:128;np.o1=st.o1!=null?st.o1:0;np.o2=st.o2!=null?st.o2:0;np.o3=st.o3!=null?st.o3:0;cur.params=np;if(st.col&&st.col.length){const cols=Array.isArray(st.col[0])?st.col:[st.col];for(let i=0;i<3;i++){if(cols[i])segColors[i]=[cols[i][0],cols[i][1],cols[i][2]];}slotCount=cols.length;}const pName=palNameById(cur.pal);cur.palName=pName;if(isRestrictedTab())normalizeRestrictedState(false);renderColorRow();if(cur.isShow)renderShowInfo();else renderParams(cur.fx);document.querySelectorAll('.fx-item').forEach(function(el){const match=cur.isShow?(Number(el.dataset.showIndex)===cur.showIndex):(el.dataset.fxIndex!=null&&Number(el.dataset.fxIndex)===cur.fx);el.classList.toggle('active',match);});document.querySelectorAll('.pal-row').forEach(function(el){el.classList.toggle('active',el.dataset.palname===cur.palName);});updateModeAktif();if(cur.isShow)document.getElementById('fxSavedName').textContent=showNames[cur.showIndex]||'Animasi Mizuma';}
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
document.getElementById('tabbar').addEventListener('click',function(e){if(e.target.tagName!=='BUTTON')return;const t=e.target.dataset.tab;if(t===activeTab)return;if(dirty){showModal(function(){switchTab(t);});}else{switchTab(t);}});
document.getElementById('controlPanel').addEventListener('click',function(e){const btn=e.target.closest('button');if(!btn)return;/* Posisi dan arah adalah kontrol langsung, bukan perubahan preset: jangan ditahan dialog Simpan. */if(btn.dataset.action==='swap'){requestSideSwap();return;}if(btn.dataset.action==='reverse'){requestShowReverse();return;}if(!btn.dataset.side)return;const side=btn.dataset.side;if(side===activeSide)return;if(dirty){showModal(function(){commitSide(side);});}else{commitSide(side);}});
document.getElementById('subnav').addEventListener('click',function(e){const btn=e.target.closest('button.nav-b');if(!btn)return;document.querySelectorAll('#subnav .nav-b').forEach(function(b){b.classList.remove('active');});btn.classList.add('active');setSub(btn.dataset.sub);});
document.getElementById('brightSlider').addEventListener('input',function(e){document.getElementById('brightVal').textContent=e.target.value;sendBriD(+e.target.value);});
/* ===== INIT ===== */
document.querySelectorAll('#tabbar button').forEach(function(b){b.classList.toggle('active',b.dataset.tab===activeTab);});
/* Halaman ini khusus Sein-Rem (Sein · Rem). Welcoming dan Riding punya halaman sendiri. */
const TAB_GRP=['sein','rem'];
document.querySelectorAll('#tabbar button').forEach(function(b){if(TAB_GRP.indexOf(b.dataset.tab)<0)b.style.display='none';});
updateDirectionButton();
setSub(activeSub);renderSavedList();
loadFxData(function(ok){if(!ok){setTimeout(function(){loadFxData(function(ok2){if(ok2)initFromState();else buildEffects();});},1500);}else{initFromState();}});
function initFromState(){loadSideSwap().then(loadShowReverse).then(function(){return jfetch('/json/state');}).then(function(r){return r.json();}).then(function(j){if(j.bri!=null){document.getElementById('brightSlider').value=j.bri;document.getElementById('brightVal').textContent=j.bri;paintRange(document.getElementById('brightSlider'));}const s=segById(j,mapSegId('kanan'));if(s){if(s.fx!=null)cur.fx=s.fx;if(s.pal!=null)cur.pal=s.pal;cur.params=paramsFor(cur.fx);captureParams(s);if(s.col&&s.col.length){for(let i=0;i<3;i++){if(s.col[i])segColors[i]=[s.col[i][0],s.col[i][1],s.col[i][2]];}slotCount=s.col.length;}}}).catch(function(){}).then(function(){setSub(activeSub);buildEffects();renderColorRow();document.querySelectorAll('input[type=range]').forEach(function(el){if(!el.classList.contains('kelvin'))paintRange(el);});setTimeout(function(){applySaved(activeTab);},400);});}
</script>
<script>
(function(){
function inj(url,cb){fetch(url).then(function(r){return r.text();}).then(cb).catch(function(){});}
inj('/mizuma/frag/header',function(t){const el=document.getElementById('mzH');if(el)el.outerHTML=t;
inj('/mizuma/frag/nav?a=sein',function(t2){const el2=document.getElementById('mzN');if(el2)el2.outerHTML=t2;
var sc=document.createElement('script');sc.src='/mizuma.js';document.body.appendChild(sc);});});
window.addEventListener('pagehide',function(){if(activeSide==='both')return;try{navigator.sendBeacon('/json/state',new Blob([JSON.stringify({seg:[{id:SEG_K,on:true},{id:SEG_L,on:true}]})],{type:'application/json'}));}catch(e){}});
})();
</script>
</body>
</html>
)rawliteral";

// --------------------------------------------------------------------------------------------------------------------------------------
// Perender halaman + pendaftaran rute UI
// --------------------------------------------------------------------------------------------------------------------------------------
inline String mizumaEsc(const String &s) {
  String o; o.reserve(s.length() + 8);
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    switch (c) {
      case '&': o += "&amp;"; break;
      case '<': o += "&lt;"; break;
      case '>': o += "&gt;"; break;
      case '"': o += "&quot;"; break;
      case '\'': o += "&#39;"; break;
      default: o += c;
    }
  }
  return o;
}

// activeKey: dash | color | welcoming | riding | sein | pengaturan
// Halaman Composer baru (T2), dibangun dari kerangka HTML/CSS desain pengguna. Dibuka lewat /riding2 selama dua halaman Riding hidup berdampingan.
const char MIZUMA_COMPOSER_HTML[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="id">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<title>Composer</title>
<style>
:root{box-sizing:border-box;--bg:#f5f4ef;--tx:#1f1e1d;--mu:#6b6a68;padding-top:env(safe-area-inset-top,0px);padding-bottom:env(safe-area-inset-bottom,0px)}
@media (prefers-color-scheme:dark){:root:not([data-theme="light"]){--bg:#1c1b19;--tx:#ece9e2;--mu:#a19e97}}
:root[data-theme="dark"]{--bg:#1c1b19;--tx:#ece9e2;--mu:#a19e97}
html{scroll-padding-top:env(safe-area-inset-top,0px)}
body{margin:0;padding:32px 16px;background:var(--bg);color:var(--tx);font:14px/1.4 system-ui,-apple-system,"Segoe UI",sans-serif;display:flex;flex-wrap:wrap;gap:40px;justify-content:center;align-items:flex-start}
.board h2{margin:0 0 12px;font-size:13px;font-weight:500;color:var(--mu)}



.fx{scrollbar-width:thin;scrollbar-color:#4A5166 transparent}
.fx::-webkit-scrollbar{width:4px}
.fx::-webkit-scrollbar-thumb{background:#4A5166;border-radius:2px}



.frame.tall{height:auto}

/* ===== MIZUMA COMPOSER — status dan interaksi yang disepakati ===== */
.frame{position:relative}
.native-color-label{margin-left:auto;flex:none;color:#D6A85E;font-size:10px;font-weight:700;letter-spacing:.1px;white-space:nowrap}
.saved-markers{margin-left:auto;flex:none;display:inline-flex;align-items:center;gap:7px}
.saved-state{position:relative;display:inline-flex;align-items:center;justify-content:center;width:22px;height:22px;flex:none;box-sizing:border-box;border-radius:4px}
.saved-state--inactive{background:transparent;border:1.7px solid #FFB547}
.saved-state--inactive svg{width:14px;height:14px;fill:none;stroke:#FFB547;stroke-width:1.9;stroke-linejoin:round;stroke-linecap:round}
.saved-state--active{width:auto;min-width:42px;height:22px;padding:0 6px;gap:4px;background:#FFB547;border:1px solid #FFB547;border-radius:4px;color:#111111;white-space:nowrap}
.saved-state--active .saved-play{width:12px;height:12px;fill:#111111;stroke:none}
.saved-state-label{font-size:10px;line-height:1;font-weight:800;color:#111111;letter-spacing:.1px}
.preview-eye{display:inline-flex;align-items:center;justify-content:center;color:#C3C8D6;flex:none}
.preview-eye svg{width:16px;height:16px;fill:none;stroke:currentColor;stroke-width:2;stroke-linecap:round;stroke-linejoin:round}
.copy-confirm{position:absolute;z-index:80;left:12px;right:12px;bottom:72px;padding:12px;background:#252B38;border:1px solid #596174;border-radius:13px;box-shadow:0 8px 26px rgba(0,0,0,.42);color:#E8EAF0}
.copy-confirm strong{display:block;font-size:13px;line-height:1.35;margin-bottom:4px}
.copy-confirm p{margin:0;color:#B8C0D0;font-size:11px;line-height:1.4}
.copy-confirm-actions{display:flex;justify-content:flex-end;gap:8px;margin-top:10px}
.copy-confirm-actions button{min-height:34px;padding:0 12px;border-radius:8px;font-size:12px;font-weight:700;border:1px solid #596174;background:transparent;color:#E8EAF0}
.copy-confirm-actions .confirm-copy{background:#FFB547;color:#1A1405;border-color:#FFB547}
.mz-toast{position:absolute;z-index:90;left:12px;right:12px;bottom:72px;padding:11px 13px;border-radius:11px;background:#252B38;border:1px solid #3DDC84;color:#E8EAF0;font-size:12px;font-weight:600;box-shadow:0 8px 24px rgba(0,0,0,.38);animation:mzToastIn .18s ease-out}
@keyframes mzToastIn{from{opacity:0;transform:translateY(6px)}to{opacity:1;transform:translateY(0)}}
/* Floating parameter hint: overlays the panel and never changes slider layout. */
.param-row{position:relative;overflow:visible}
.param-row [data-param-icon]{cursor:pointer;touch-action:manipulation}
.param-tooltip{
  position:absolute;z-index:140;left:0;bottom:calc(100% + 8px);
  max-width:min(220px,calc(100vw - 48px));width:max-content;
  padding:7px 10px;border-radius:8px;
  background:#090B10;border:1px solid #596174;color:#F0F2F7;
  font-size:11px;font-weight:600;line-height:1.35;white-space:normal;
  box-shadow:0 6px 18px rgba(0,0,0,.42);
  opacity:0;visibility:hidden;transform:translateY(4px);
  transition:opacity .12s ease,transform .12s ease,visibility .12s;
  pointer-events:none;
}
.param-tooltip::after{
  content:"";position:absolute;left:11px;top:100%;
  border:5px solid transparent;border-top-color:#596174;
}
.param-row.tooltip-visible .param-tooltip{
  opacity:1;visibility:visible;transform:translateY(0);
}


/* ===== Header device controls ===== */
.mz-demo-header{overflow:visible!important}
.header-actions{z-index:35}
.header-icon-btn{cursor:pointer;touch-action:manipulation;transition:background .16s ease,border-color .16s ease,color .16s ease,transform .12s ease}
.header-icon-btn:hover,.header-icon-btn:focus-visible{background:#252B38!important;border-color:#657087!important}
.connection-indicator:hover{background:#1C1F27!important}
.connection-dot{position:absolute;width:6px;height:6px;border-radius:50%;right:4px;bottom:5px;background:#3DDC84;box-shadow:0 0 0 2px #0A0A0A}
.header-icon-btn[data-tip]::after,.composer-icon-toggle[data-tip]::after{
  content:attr(data-tip);position:absolute;z-index:160;left:50%;top:calc(100% + 8px);transform:translate(-50%,-3px);
  padding:6px 9px;border-radius:7px;background:#090B10;border:1px solid #596174;color:#F0F2F7;
  font-size:10.5px;font-weight:600;line-height:1.3;white-space:nowrap;box-shadow:0 5px 16px rgba(0,0,0,.42);
  opacity:0;visibility:hidden;pointer-events:none;transition:opacity .12s ease,transform .12s ease,visibility .12s ease
}
.header-icon-btn[data-tip]:hover::after,.header-icon-btn[data-tip]:focus-visible::after,.header-icon-btn.tip-open::after,
.composer-icon-toggle[data-tip]:hover::after,.composer-icon-toggle[data-tip]:focus-visible::after,.composer-icon-toggle.tip-open::after{
  opacity:1;visibility:visible;transform:translate(-50%,0)
}
.brightness-popover{position:absolute;right:0;top:calc(100% + 9px);z-index:180;width:232px;padding:13px;background:#202530;border:1px solid #465066;border-radius:13px;color:#E8EAF0;box-shadow:0 12px 30px rgba(0,0,0,.5)}
.brightness-popover[hidden]{display:none}
.popover-heading{display:flex;align-items:center;justify-content:space-between;font-size:12px;font-weight:650;margin-bottom:8px}
.popover-heading strong{font-size:12px;color:#FFCB78;font-variant-numeric:tabular-nums}
.brightness-slider{width:100%;margin:0;accent-color:#FFB547;cursor:pointer;touch-action:pan-y}
.brightness-scale{display:flex;justify-content:space-between;color:#929BAD;font-size:10px;margin-top:3px}
.popover-divider{height:1px;background:#3A4253;margin:12px 0 9px}
.physical-swap-btn{width:100%;display:flex;align-items:center;gap:9px;text-align:left;padding:7px 3px;border:0;background:transparent;color:#E8EAF0;border-radius:8px;cursor:pointer}
.physical-swap-btn:hover{background:#2B3241}
.swap-icon{width:28px;height:28px;display:flex;align-items:center;justify-content:center;border-radius:8px;background:#302A20;color:#F0BD68;flex:none}
.swap-copy{display:flex;flex:1;min-width:0;flex-direction:column;gap:2px}
.swap-copy strong{font-size:11.5px;font-weight:700}
.swap-copy small{font-size:9.5px;color:#A0A9BA;white-space:nowrap}
.swap-switch{width:30px;height:17px;border-radius:12px;background:#454D5E;padding:2px;flex:none;transition:background .16s ease}
.swap-switch i{display:block;width:13px;height:13px;border-radius:50%;background:#D3D8E2;transition:transform .16s ease}
.physical-swap-btn[aria-pressed="true"] .swap-switch{background:#D99A36}
.physical-swap-btn[aria-pressed="true"] .swap-switch i{transform:translateX(13px);background:#FFF4DF}
/* Thumbnail warna/skin sekarang menjadi bagian dari identitas efek yang dipratinjau. */
.effect-color-thumb{
  display:block;flex:none;width:30px;height:30px;border-radius:7px;
  background:var(--preview-swatch, var(--preview-color,#FFB547));
  border:1px solid #657087;box-sizing:border-box;
  box-shadow:inset 0 0 0 1px rgba(255,255,255,.12),0 1px 3px rgba(0,0,0,.28);
}
.effect-heading{min-width:0;flex:1 1 auto;line-height:1.25}
.effect-heading .effect-number{display:flex;align-items:center;gap:4px;font:400 11px ui-monospace,Menlo,Consolas,monospace;color:#A0A6B5}
.effect-heading .effect-name{display:block;font-size:15px;font-weight:700;color:#E8EAF0;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
/* Tombol Edit Warna/Skin: roda warna dan pipet menyatu dengan kapsul. Adaptasi ukuran untuk baris Composer. */
.color-action-btn{appearance:none;display:inline-flex;align-items:center;justify-content:flex-start;gap:0;height:36px;min-width:0;flex:none;padding:0;border:0;background:transparent;color:#F2F4F8;cursor:pointer;position:relative;outline:none;-webkit-tap-highlight-color:transparent;white-space:nowrap;transition:transform .15s ease,filter .2s ease}
.icon-wheel{width:36px;height:36px;border-radius:50%;position:relative;z-index:2;display:flex;align-items:center;justify-content:center;flex:none;background:conic-gradient(#FFA726 0deg 45deg,#FF5252 45deg 90deg,#EC407A 90deg 135deg,#AB47BC 135deg 180deg,#42A5F5 180deg 225deg,#26C6DA 225deg 270deg,#4DB6AC 270deg 315deg,#9CCC65 315deg 360deg);border:1.5px solid #FFFFFF;box-shadow:inset 0 0 0 1.5px #16191E;transition:border-color .25s ease,box-shadow .25s ease,transform .15s ease}
.icon-wheel::before{content:"";width:10px;height:10px;background:#16191E;border-radius:50%;position:absolute;z-index:1}
.icon-wheel svg{width:26px;height:26px;z-index:2;filter:drop-shadow(0 1px 2px rgba(0,0,0,.48));overflow:visible}
.capsule-body{height:36px;box-sizing:border-box;display:flex;flex-direction:column;justify-content:center;text-align:left;margin-left:-18px;padding:0 12px 0 25px;background:transparent;border:1.5px solid #FFFFFF;border-left:none;border-radius:0 50px 50px 0;z-index:1;line-height:1.12;min-width:73px;transition:background-color .25s ease,border-color .25s ease,box-shadow .25s ease,transform .15s ease}
.btn-title{color:#FFFFFF;font-size:11.5px;font-weight:750;letter-spacing:.1px;line-height:1.1}
.btn-subtitle{color:#C5A358;font-size:8.5px;font-weight:550;line-height:1.1;white-space:nowrap;transition:color .25s ease}
.color-action-btn:hover .icon-wheel,.color-action-btn:focus-visible .icon-wheel{border-color:rgba(245,158,11,.9);box-shadow:inset 0 0 0 1.5px #16191E,0 0 8px rgba(245,158,11,.18)}
.color-action-btn:hover .capsule-body,.color-action-btn:focus-visible .capsule-body{border-color:rgba(245,158,11,.9);background-color:rgba(245,158,11,.12);box-shadow:0 0 8px rgba(245,158,11,.13)}
.color-action-btn:hover .btn-subtitle,.color-action-btn:focus-visible .btn-subtitle{color:#FBBF24}
.color-action-btn:active .icon-wheel{border-color:#F59E0B;transform:scale(.985)}
.color-action-btn:active .capsule-body{border-color:#F59E0B;background-color:rgba(245,158,11,.22);transform:scale(.985)}
.color-action-btn:focus-visible{outline:2px solid #F59E0B;outline-offset:3px;border-radius:999px}
@media(max-width:360px){.color-action-btn,.icon-wheel,.capsule-body{height:34px}.icon-wheel{width:34px}.icon-wheel svg{width:24px;height:24px}.capsule-body{margin-left:-17px;padding-left:23px;padding-right:9px;min-width:68px}.btn-title{font-size:10.8px}.btn-subtitle{font-size:8px}}

.composer-icon-toggle{position:relative;flex:none;width:34px;height:34px;padding:0;display:flex;align-items:center;justify-content:center;border-radius:9px;border:1px solid #3A4052;background:transparent;color:#BBC3D2;cursor:pointer;touch-action:manipulation;transition:background .16s ease,border-color .16s ease,color .16s ease,transform .12s ease}
.composer-icon-toggle:hover,.composer-icon-toggle:focus-visible{background:#252B38;border-color:#68738A;color:#F1F3F8}
.composer-icon-toggle[aria-pressed="true"],.composer-icon-toggle.is-active{background:#3A2C19;border-color:#C58B35;color:#FFC56D;box-shadow:inset 0 0 0 1px rgba(255,181,71,.08)}
.composer-icon-toggle:active{transform:scale(.94)}
.composer-icon-toggle[data-tip]::after{left:auto;right:0;top:calc(100% + 7px);transform:translateY(-3px)}
.composer-icon-toggle[data-tip]:hover::after,.composer-icon-toggle[data-tip]:focus-visible::after,.composer-icon-toggle.tip-open::after{transform:translateY(0)}
.copy-composition-btn{transition:background .16s ease,border-color .16s ease,transform .12s ease}
.copy-composition-btn:hover,.copy-composition-btn:focus-visible{background:#252B38!important;border-color:#A5AEC0!important;color:#FFFFFF!important}
.copy-composition-btn:active{transform:scale(.96)}
@media(max-width:390px){.brightness-popover{right:-2px;width:min(232px,calc(100vw - 48px))}}


/* ===== Navbar bawah enam tab ===== */
.mz-bottom-nav{flex:none;height:62px;display:flex;align-items:center;justify-content:space-around;border-top:1px solid #262626;background:#121212;padding:3px 2px max(3px,env(safe-area-inset-bottom));gap:0}
.mz-nav-item{appearance:none;min-width:0;flex:1 1 0;height:100%;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:3px;padding:2px 0 0;border:0;border-radius:9px;background:transparent;color:#8E95A8;font:600 9.5px/1.1 system-ui,Segoe UI,Roboto,sans-serif;white-space:nowrap;cursor:pointer;touch-action:manipulation}
.mz-nav-item svg{width:19px;height:19px;fill:none;stroke:currentColor;stroke-width:1.75;stroke-linecap:round;stroke-linejoin:round;flex:none}
.mz-nav-item.active{color:#FFB547}
.mz-nav-item:active{background:rgba(255,181,71,.08);transform:scale(.97)}
@media(max-width:360px){.mz-nav-item{font-size:8.7px}.mz-nav-item svg{width:18px;height:18px}}
/* ===== Tab Warna: color editor utama + Koleksi ===== */
.color-manager{width:100%;height:100%;min-height:0;display:flex;flex-direction:column;background:#0B0C10;color:#E8EAF0;font-family:system-ui,Segoe UI,Roboto,sans-serif;position:relative}
.color-manager-scroll{flex:1;min-height:0;overflow:auto;padding:9px 12px 12px;display:flex;flex-direction:column;gap:10px;scrollbar-width:thin;scrollbar-color:#4A5166 transparent}
.cm-page-title{display:flex;align-items:center;justify-content:space-between;gap:8px;flex:none}
.cm-page-title h3{margin:0;font-size:17px;line-height:1.2;font-weight:760;letter-spacing:-.2px}
.cm-page-title p{margin:3px 0 0;color:#929BAD;font-size:10px}
.cm-tabs{display:grid;grid-template-columns:1fr 1fr;gap:4px;padding:3px;background:#1B1E27;border:1px solid #2B3040;border-radius:11px;flex:none}
.cm-tab{height:33px;border:0;border-radius:8px;background:transparent;color:#A0A6B5;font-size:12px;font-weight:700;cursor:pointer}
.cm-tab.active{background:#303746;color:#FFBD63;box-shadow:inset 0 0 0 1px rgba(255,181,71,.18)}
.cm-pane{display:flex;flex-direction:column;gap:10px;min-height:0}
.cm-pane[hidden]{display:none}
.cm-wheel-area{display:flex;flex-direction:column;align-items:center;gap:6px;padding:1px 0 0;flex:none}
.cm-wheel-wrap{width:min(258px,65vw,30vh);aspect-ratio:1;position:relative;flex:none}
#cmColorWheel{display:block;width:100%;height:100%;touch-action:none;border-radius:50%}
.cm-brightness-row{display:grid;grid-template-columns:58px minmax(0,1fr) 42px;gap:8px;align-items:end;min-height:54px;flex:none}
.cm-rgb-readout{display:flex;flex-direction:column;gap:2px;padding:3px 4px;border-radius:8px;background:#1A1D24;border:1px solid #303642;align-self:center}
.cm-rgb-channel{display:flex;align-items:center;gap:4px;height:16px}
.cm-rgb-channel b{width:20px;height:15px;border-radius:4px;color:#fff;font-size:9px;line-height:15px;text-align:center;font-weight:800}
.cm-rgb-channel em{font-style:normal;flex:1;text-align:center;font-size:10px;line-height:15px;color:#F0F2F7;background:rgba(255,255,255,.08);border-radius:4px;font-variant-numeric:tabular-nums}
.cm-brightness{min-width:0;display:flex;flex-direction:column;gap:5px;align-self:center;padding-bottom:1px}
.cm-brightness-label{text-align:center;font-size:12px;font-weight:750;color:#F1F3F7;font-variant-numeric:tabular-nums}
.cm-brightness input{width:100%;height:16px;margin:0;appearance:none;-webkit-appearance:none;background:var(--brightness-gradient,linear-gradient(90deg,#111,#FFB547));border-radius:0;outline:none;cursor:pointer;touch-action:pan-y}
.cm-brightness input::-webkit-slider-thumb{appearance:none;-webkit-appearance:none;width:21px;height:21px;border:3px solid #F5F6FA;border-radius:50%;background:rgba(255,255,255,.22);box-shadow:0 1px 5px rgba(0,0,0,.7)}
.cm-brightness input::-moz-range-thumb{width:15px;height:15px;border:3px solid #F5F6FA;border-radius:50%;background:rgba(255,255,255,.22);box-shadow:0 1px 5px rgba(0,0,0,.7)}
.cm-random{width:42px;height:42px;border:1px solid #343B4A;border-radius:50%;background:#1B1E25;color:#F2F4F8;display:flex;align-items:center;justify-content:center;font-size:18px;cursor:pointer;box-shadow:0 2px 7px rgba(0,0,0,.24)}
.cm-random:hover{border-color:#D69A3E;background:#272A32;color:#FFCB78}
.cm-swatch-group{display:flex;flex-direction:column;gap:5px;flex:none}
.cm-swatch-heading{display:flex;align-items:center;justify-content:space-between;gap:8px;padding:0 1px}
.cm-swatch-heading strong{font-size:10.5px;color:#C8CEDA;font-weight:750;letter-spacing:.15px}
.cm-swatch-heading small{font-size:9px;color:#778195}
.cm-swatch-grid{display:grid;grid-template-columns:repeat(6,minmax(0,1fr));gap:6px}
.cm-color-tile{height:auto;aspect-ratio:1/1;min-width:0;min-height:0;position:relative;border-radius:9px;border:1px solid rgba(255,255,255,.32);box-shadow:inset 0 0 0 1px rgba(0,0,0,.08);cursor:pointer;padding:0;overflow:visible}
.cm-color-tile.selected{outline:2px solid #F3F5FA;outline-offset:2px;z-index:1}
.cm-color-tile:active{transform:scale(.97)}
.cm-tile-edit{position:absolute;right:-4px;top:-5px;width:23px;height:23px;border-radius:50%;border:1px solid rgba(255,255,255,.72);background:#090A0D;color:#F3F5FA;display:flex;align-items:center;justify-content:center;padding:0;cursor:pointer;box-shadow:0 1px 4px rgba(0,0,0,.55)}
.cm-tile-edit svg{width:12px;height:12px;stroke:currentColor;fill:none;stroke-width:1.8;stroke-linecap:round;stroke-linejoin:round}
.cm-editor-hint{font-size:9px;color:#778195;line-height:1.35;margin:0}
/* Koleksi: Skin Bawaan ditampilkan lebih dahulu, disusul Warna Bawaan. */
.cm-collection-scroll{display:flex;flex-direction:column;gap:13px}
.cm-section{display:flex;flex-direction:column;gap:8px}
.cm-section-head{display:flex;align-items:center;justify-content:space-between;gap:8px}
.cm-section-head strong{font-size:12px;font-weight:780;color:#F0F2F7}
.cm-section-head small{font-size:9.5px;color:#929BAD}
.cm-primary{display:inline-flex;align-items:center;justify-content:center;gap:5px;min-height:31px;padding:0 9px;border:0;border-radius:8px;background:#FFB547;color:#1A1405;font-size:10px;font-weight:760;white-space:nowrap;cursor:pointer}
.cm-primary:disabled{opacity:.42;cursor:not-allowed}
.cm-limit{font-size:9.5px;color:#929BAD;font-variant-numeric:tabular-nums}
.cm-skin-list{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:6px}
.cm-skin{height:46px;min-width:0;padding:6px;display:flex;flex-direction:row;align-items:center;gap:6px;border:1px solid #303849;border-radius:9px;background:#171B24;box-sizing:border-box}
.cm-skin-gradient{display:block;width:48px;height:30px;flex:none;border-radius:6px;border:1px solid rgba(255,255,255,.2);box-shadow:inset 0 0 0 1px rgba(255,255,255,.08)}
.cm-skin-info{flex:1;min-width:0;display:flex;align-items:center}
.cm-skin-info b{display:block;width:100%;font-size:10px;line-height:1.2;color:#F0F2F7;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.cm-skin-actions{display:flex;align-items:center;justify-content:flex-end;gap:3px;flex:none}
.cm-mini-action{width:22px;height:25px;padding:0;display:flex;align-items:center;justify-content:center;border:1px solid #3A4052;border-radius:6px;background:#202632;color:#BFC7D5;cursor:pointer;flex:none}
.cm-mini-action:hover{border-color:#D49A42;color:#FFCB78;background:#272A31}
.cm-mini-action.danger:hover{border-color:#B65C5C;color:#FF9292}
.cm-mini-action svg{width:12px;height:12px;fill:none;stroke:currentColor;stroke-width:1.8;stroke-linecap:round;stroke-linejoin:round}
.cm-search{width:100%;height:34px;padding:0 10px;border:1px solid #394253;border-radius:8px;background:#151922;color:#E8EAF0;font-size:11px;outline:none}
.cm-built-in-grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:6px}
.cm-built-in-item{height:46px;min-width:0;box-sizing:border-box;display:flex;align-items:center;gap:6px;padding:6px;background:#171B24;border:1px solid #303849;border-radius:9px;text-align:left;color:#E8EAF0;cursor:pointer}
.cm-built-in-item.selected{border-color:#D69A3E;background:rgba(245,165,36,.08)}
.cm-built-in-strip{width:48px;height:30px;border-radius:6px;flex:none;border:1px solid rgba(255,255,255,.2)}
.cm-built-in-copy{min-width:0;display:flex;flex-direction:column;justify-content:center;gap:2px}
.cm-built-in-copy b{font-size:10px;line-height:1.2;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.cm-built-in-copy small{display:none}
.cm-note{font-size:9px;line-height:1.4;color:#8993A6;margin:0}
/* RGB popup mengikuti pola pkRgbOpen pada mizuma_ui.h: preview, HEX, slider dan angka R/G/B. */
.cm-rgb-backdrop,.cm-modal-backdrop{position:absolute;inset:0;z-index:210;background:rgba(0,0,0,.68);display:flex;align-items:flex-end;justify-content:center;padding:10px}
.cm-rgb-backdrop[hidden],.cm-modal-backdrop[hidden]{display:none}
.cm-rgb-modal,.cm-modal{width:100%;max-height:88%;overflow:auto;background:#202530;border:1px solid #465066;border-radius:17px;padding:14px;color:#E8EAF0;box-shadow:0 15px 45px rgba(0,0,0,.55)}
.cm-rgb-modal h4,.cm-modal h4{margin:0 0 4px;font-size:15px;font-weight:800}
.cm-rgb-modal p,.cm-modal p{margin:0 0 11px;color:#A0A9BA;font-size:10px;line-height:1.4}
.cm-rgb-preview-row{display:flex;align-items:center;gap:8px;margin:8px 0 11px}
.cm-rgb-preview{width:48px;height:40px;border-radius:8px;border:1px solid rgba(255,255,255,.3);flex:none}
.cm-rgb-preview-row label{font-size:10px;color:#A0A9BA}
.cm-rgb-preview-row input{flex:1;min-width:0;height:36px;padding:0 9px;background:#151922;border:1px solid #465066;border-radius:8px;color:#F0F2F7;font:12px ui-monospace,monospace}
.cm-rgb-fields{display:flex;flex-direction:column;gap:8px}
.cm-rgb-row{display:grid;grid-template-columns:24px minmax(0,1fr) 54px;gap:8px;align-items:center}
.cm-rgb-row b{height:25px;border-radius:5px;color:#fff;font-size:11px;display:flex;align-items:center;justify-content:center}
.cm-rgb-row input[type=range]{width:100%;margin:0;accent-color:#FFB547}
.cm-rgb-row input[type=number]{width:54px;height:32px;padding:0 5px;border:1px solid #465066;border-radius:7px;background:#151922;color:#F0F2F7;text-align:center;font:11px ui-monospace,monospace}
.cm-modal h4{margin:0 0 4px;font-size:16px}
.cm-field{display:flex;flex-direction:column;gap:5px;margin:10px 0}
.cm-field label{font-size:10px;color:#BFC7D5;font-weight:650}
.cm-field input[type=text]{width:100%;height:38px;padding:0 10px;background:#151922;border:1px solid #465066;border-radius:8px;color:#F0F2F7;font:13px system-ui;box-sizing:border-box}
.cm-color-edit-row{display:flex;align-items:center;gap:7px;padding:7px 0;border-bottom:1px solid #343B4C}
.cm-color-edit-row input[type=color]{width:36px;height:32px;padding:2px;border:1px solid #465066;border-radius:7px;background:#151922}
.cm-color-edit-row span{flex:1;font-size:11px;color:#C8CFDA}
.cm-color-edit-row code{font-size:10px;color:#A0A9BA}
.cm-modal-actions{display:flex;gap:8px;margin-top:13px}
.cm-modal-actions button{flex:1;min-height:39px;border-radius:9px;font-size:11px;font-weight:700;cursor:pointer}
.cm-secondary{border:1px solid #465066;background:transparent;color:#E8EAF0}
.cm-save{border:0;background:#FFB547;color:#1A1405}
.cm-danger-confirm{display:flex;align-items:center;gap:8px;margin-top:10px;padding:9px;border:1px solid #674447;background:#2D2026;border-radius:9px}
.cm-danger-confirm span{flex:1;font-size:10px;color:#E8C6C6}

/* Skin Editor: susunan mengikuti cpR() + pkHTML()/pkBind() pada mizuma_ui.h. */
.cm-skin-editor{display:flex;flex-direction:column;gap:8px}
.cm-skin-gradient-track{position:relative;height:42px;border:1px solid rgba(255,255,255,.34);border-radius:8px;box-shadow:inset 0 0 0 1px rgba(0,0,0,.16);touch-action:pan-y;overflow:visible;margin:4px 2px 14px}
.cm-skin-stop{position:absolute;top:-7px;width:20px;height:56px;margin-left:-10px;border-radius:6px;border:2px solid #F5F6FA;box-shadow:0 0 0 1px rgba(0,0,0,.72);padding:0;cursor:grab;touch-action:none;background:transparent}
.cm-skin-stop::after{content:'';position:absolute;left:50%;bottom:-8px;transform:translateX(-50%);width:5px;height:5px;border-radius:50%;background:#F5F6FA;box-shadow:0 0 0 1px #101319}
.cm-skin-stop.selected{top:-10px;height:62px;border:3px solid #FFB547;box-shadow:0 0 0 1px #08090D,0 0 0 3px rgba(255,181,71,.24);z-index:2}
.cm-skin-tools{display:flex;gap:6px}
.cm-skin-tool{flex:1;min-width:0;height:32px;border:1px solid #465066;border-radius:8px;background:#252B36;color:#E8EAF0;font-size:10px;font-weight:720;cursor:pointer}
.cm-skin-tool:disabled{opacity:.35;cursor:not-allowed}
.cm-skin-hint{margin:-2px 0 0;color:#929BAD;font-size:9px;line-height:1.4}
.cm-skin-picker{display:flex;flex-direction:column;gap:6px;margin-top:1px}
.cm-skin-wheel-wrap{width:min(290px,76vw,31vh);aspect-ratio:1;margin:0 auto;position:relative}
#cmSkinWheel{display:block;width:100%;height:100%;touch-action:none}
.cm-skin-ctl{display:grid;grid-template-columns:auto minmax(0,1fr) auto;gap:9px;align-items:center;margin-top:2px}
.cm-skin-rgb-open{display:flex;flex-direction:column;gap:2px;padding:4px;background:#1B1E27;border:1px solid #394253;border-radius:8px;color:#F2F4F8;cursor:pointer}
.cm-skin-rgb-open span{display:flex;gap:3px;align-items:center}
.cm-skin-rgb-open b{width:16px;height:15px;line-height:15px;text-align:center;border-radius:3px;color:#fff;font-size:9px}
.cm-skin-rgb-open em{min-width:30px;font-style:normal;font-size:10px;text-align:center;color:#F2F4F8;background:rgba(255,255,255,.09);border-radius:3px;font-variant-numeric:tabular-nums}
.cm-skin-brightness{min-width:0;display:flex;flex-direction:column;gap:4px}
.cm-skin-brightness label{text-align:center;font-size:12px;font-weight:750;color:#F1F3F7;font-variant-numeric:tabular-nums}
.cm-skin-brightness input{width:100%;height:15px;margin:0;appearance:none;-webkit-appearance:none;background:var(--skin-brightness-gradient,linear-gradient(90deg,#08090D,#FFB547));border-radius:0;outline:none;cursor:pointer;touch-action:pan-y}
.cm-skin-brightness input::-webkit-slider-thumb{appearance:none;-webkit-appearance:none;width:21px;height:21px;border:2px solid rgba(255,255,255,.96);border-radius:50%;background:rgba(255,255,255,.28);box-shadow:0 1px 4px rgba(0,0,0,.55)}
.cm-skin-brightness input::-moz-range-thumb{width:16px;height:16px;border:2px solid rgba(255,255,255,.96);border-radius:50%;background:rgba(255,255,255,.28);box-shadow:0 1px 4px rgba(0,0,0,.55)}
.cm-skin-random-stop{width:42px;height:42px;border:1px solid #394253;border-radius:50%;background:#1B1E25;color:#F2F4F8;display:flex;align-items:center;justify-content:center;font-size:17px;cursor:pointer}
.cm-skin-color-quick,.cm-skin-saved-colors{display:grid;grid-template-columns:repeat(6,minmax(0,1fr));gap:7px;margin-top:7px}
.cm-skin-color-quick button,.cm-skin-saved-colors button{position:relative;aspect-ratio:1.2;min-width:0;border:2px solid rgba(255,255,255,.24);border-radius:9px;padding:0;cursor:pointer;overflow:visible}
.cm-skin-saved-colors button{border-radius:9px}
.cm-skin-saved-colors .skin-slot-edit{position:absolute;right:-5px;top:-6px;width:19px;height:19px;border:1px solid #777;border-radius:50%;background:#050608;color:#fff;font-size:10px;line-height:17px;text-align:center;box-shadow:0 1px 3px rgba(0,0,0,.5)}
.cm-skin-rgb-backdrop{position:fixed;inset:0;z-index:240;background:rgba(0,0,0,.72);display:flex;align-items:flex-end;justify-content:center;padding:12px}
.cm-skin-rgb-backdrop[hidden]{display:none}
.cm-skin-rgb-modal{width:100%;max-width:500px;background:#202530;border:1px solid #465066;border-radius:16px;padding:14px;color:#E8EAF0;box-shadow:0 15px 45px rgba(0,0,0,.55)}
.cm-skin-rgb-modal h5{margin:0 0 10px;font-size:14px;font-weight:800}
.cm-skin-rgb-preview{display:flex;align-items:center;gap:8px;margin-bottom:10px}
.cm-skin-rgb-preview i{width:48px;height:38px;flex:none;border:1px solid rgba(255,255,255,.3);border-radius:8px}
.cm-skin-rgb-preview label{font-size:10px;color:#A0A9BA}
.cm-skin-rgb-preview input{height:36px;min-width:0;flex:1;padding:0 9px;background:#151922;border:1px solid #465066;border-radius:8px;color:#F0F2F7;font:12px ui-monospace,monospace}
.cm-skin-rgb-row{display:grid;grid-template-columns:24px minmax(0,1fr) 54px;gap:8px;align-items:center;margin:8px 0}
.cm-skin-rgb-row b{height:25px;border-radius:5px;color:#fff;font-size:11px;display:flex;align-items:center;justify-content:center}
.cm-skin-rgb-row input[type=range]{width:100%;margin:0;accent-color:#FFB547}
.cm-skin-rgb-row input[type=number]{width:54px;height:32px;padding:0 5px;border:1px solid #465066;border-radius:7px;background:#151922;color:#F0F2F7;text-align:center;font:11px ui-monospace,monospace}
.cm-skin-rgb-done{width:100%;min-height:38px;margin-top:9px;border:0;border-radius:9px;background:#FFB547;color:#1A1405;font-size:11px;font-weight:800}
.cm-skin-editor-footer{display:flex;gap:8px;margin-top:10px}
.cm-skin-editor-footer button{flex:1;min-height:40px;border-radius:9px;font-size:11px;font-weight:750}
.cm-modal.skin-editor-modal{max-height:94%;padding-bottom:12px}
.cm-modal.skin-editor-modal #cmModalBody{overflow:auto;min-height:0;padding-bottom:3px}
@media(max-height:700px){.cm-skin-wheel-wrap{width:min(228px,66vw,27vh)}.cm-skin-gradient-track{height:38px}.cm-skin-stop{height:52px}.cm-skin-stop.selected{height:58px}}
@media(max-width:360px){.cm-skin-list,.cm-built-in-grid{gap:5px}.cm-skin,.cm-built-in-item{height:46px;padding:5px;gap:5px}.cm-skin-gradient,.cm-built-in-strip{width:44px;height:30px}.cm-skin-actions .cm-mini-action{width:21px;height:25px}.cm-skin-wheel-wrap{width:min(250px,74vw,28vh)}.cm-skin-color-quick,.cm-skin-saved-colors{gap:5px}}
@media(max-height:700px){.cm-skin-wheel-wrap{width:min(156px,42vw)}.cm-skin-brightness{height:120px}.cm-skin-gradient-track{height:40px}.cm-skin-stop{height:54px}.cm-skin-stop.selected{height:60px}}
@media(max-width:360px){.color-manager-scroll{padding-left:9px;padding-right:9px}.cm-color-tile{height:auto;aspect-ratio:1/1}.cm-swatch-grid{gap:4px}.cm-wheel-wrap{width:min(224px,62vw,28vh)}.cm-brightness-row{grid-template-columns:54px minmax(0,1fr) 38px;gap:6px}.cm-random{width:38px;height:38px}}


/* ===== Header Mizuma: wordmark, mode Riding, tegangan, koneksi ===== */
.mz-brand-lockup{display:flex;align-items:center;gap:7px;min-width:0;flex:none}
.mz-brand-mark{width:29px;height:29px;border-radius:9px;background:linear-gradient(135deg,#FFB547 0%,#E88A21 58%,#B85A13 100%);display:flex;align-items:center;justify-content:center;color:#111;font-size:17px;font-weight:900;line-height:1;box-shadow:0 2px 9px rgba(245,165,36,.22)}
.mz-brand-words{display:flex;flex-direction:column;gap:1px;line-height:1}
.mz-brand-words strong{font-size:12px;letter-spacing:1.25px;font-weight:850;color:#F5F6F8}
.mz-brand-words small{font-size:7px;letter-spacing:1.4px;font-weight:700;color:#DDA44D}
.riding-mode-indicator{display:flex;flex-direction:column;align-items:center;justify-content:center;gap:2px;min-width:57px;padding:4px 8px;border:1px solid #343B49;border-radius:8px;background:#171B22;line-height:1.05;flex:none}
.riding-mode-indicator small{font-size:7px;font-weight:800;letter-spacing:1.05px;color:#A0A6B5}
.riding-mode-indicator strong{font-size:10px;font-weight:800;letter-spacing:.45px;color:#F3C16C}
.voltage-readout{position:relative;display:inline-flex;align-items:center;justify-content:center;gap:5px;min-width:51px;height:28px;padding:0 7px;border-radius:8px;border:1px solid #343B49;background:#171B22;color:#DCE2EC;white-space:nowrap;flex:none}
.voltage-readout i{width:6px;height:6px;border-radius:50%;background:#3DDC84;box-shadow:0 0 5px rgba(61,220,132,.6)}
.voltage-readout strong{font-size:10px;font-weight:750;font-variant-numeric:tabular-nums}
.voltage-readout[data-tip]::after{content:attr(data-tip);position:absolute;z-index:160;right:0;top:calc(100% + 7px);padding:6px 8px;border-radius:7px;background:#090B10;border:1px solid #596174;color:#F0F2F7;font-size:9.5px;font-weight:600;line-height:1.25;white-space:nowrap;box-shadow:0 5px 16px rgba(0,0,0,.42);opacity:0;visibility:hidden;pointer-events:none;transition:opacity .12s ease,visibility .12s ease}
.voltage-readout:hover::after,.voltage-readout:focus-within::after{opacity:1;visibility:visible}
.header-actions{gap:6px!important}
.save-to-button{font-size:11px!important;letter-spacing:-.12px}
.save-to-button svg{width:16px!important;height:16px!important}
.save-to-button .save-to-label{font-weight:650;color:inherit;white-space:nowrap}
@media(max-width:360px){
  .mz-brand-lockup{gap:5px}.mz-brand-mark{width:26px;height:26px;font-size:15px;border-radius:8px}
  .mz-brand-words strong{font-size:10px;letter-spacing:1px}.mz-brand-words small{font-size:6px;letter-spacing:1px}
  .riding-mode-indicator{min-width:49px;padding:4px 5px}.riding-mode-indicator small{font-size:6px}.riding-mode-indicator strong{font-size:9px}
  .voltage-readout{min-width:44px;height:26px;padding:0 5px;gap:3px}.voltage-readout strong{font-size:9px}
  .header-actions{gap:3px!important}.connection-indicator,.brightness-menu-btn{width:30px!important;height:30px!important}
  .save-to-button{font-size:10px!important;padding-left:4px!important;padding-right:4px!important;gap:4px!important}
  .save-to-button svg{width:14px!important;height:14px!important}
}


/* Revisi sinkronisasi Composer Color Editor dengan Tab Warna */
.cm-color-tile{aspect-ratio:1/1;height:auto}
.cm-swatch-grid>.cm-color-tile{width:100%;height:auto;aspect-ratio:1/1}
.ccs-wheel-canvas{display:block;width:100%;height:100%;border-radius:50%;touch-action:none}
.ccs-body .cm-wheel-area{position:relative;padding:1px 0 0;gap:6px;width:100%}
.ccs-body .cm-wheel-wrap{width:min(258px,65vw,30vh);}
.ccs-current-swatch{position:absolute;top:1px;right:4px;width:36px;height:36px;box-sizing:border-box;border:1px solid rgba(255,255,255,.55);border-radius:8px;box-shadow:0 2px 8px rgba(0,0,0,.4)}
.ccs-body .cm-brightness-row{grid-template-columns:58px minmax(0,1fr) 42px;gap:8px;min-height:54px}
.ccs-body .cm-rgb-readout.ccs-rgb{width:58px;box-sizing:border-box;cursor:pointer;color:#E8EAF0;font-family:inherit;text-align:left}
.ccs-grabber{width:38px;height:4px;flex:none;margin:8px auto 0;border-radius:3px;background:#667084}
.ccs-head{flex:none;display:flex;align-items:center;gap:11px;min-height:77px;padding:10px 14px;border-bottom:1px solid #303746;box-sizing:border-box}
.ccs-head-icon{width:42px;height:42px;flex:none;display:flex;align-items:center;justify-content:center;border:1px solid rgba(255,255,255,.56);border-radius:50%;background:conic-gradient(#FF4C78,#FFCA43,#36D7B5,#478BFF,#C65BFF,#FF4C78)}
.ccs-head-icon svg{width:25px;height:25px}
.ccs-head-copy{flex:1;min-width:0;display:flex;flex-direction:column;gap:2px}
.ccs-eyebrow{font-size:8px;font-weight:800;letter-spacing:1.2px;color:#FFB547}
.ccs-title{font-size:15px;font-weight:800;line-height:1.15;color:#F1F3F7}
.ccs-context{font-size:10px;color:#A0A6B5;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.ccs-close{width:34px;height:34px;flex:none;display:flex;align-items:center;justify-content:center;border:1px solid #3A4052;border-radius:9px;background:#1B1E27;color:#C8CEDA}
.ccs-close svg{width:18px;height:18px;fill:none;stroke:currentColor;stroke-width:1.8;stroke-linecap:round}
.ccs-footer{flex:none;display:flex;justify-content:center;gap:8px;width:100%;max-width:360px;box-sizing:border-box;padding:10px 12px calc(10px + env(safe-area-inset-bottom));margin:0 auto;border-top:1px solid #303746;background:#0B0C10}
.ccs-footer button{flex:1;min-width:0;height:40px;border-radius:10px;font-size:11px;font-weight:750}
.ccs-footer .ccs-cancel{border:1px solid #465066;background:transparent;color:#E8EAF0}
.ccs-footer .ccs-use{border:0;background:#FFB547;color:#1A1405}
.ccs-body .cm-skin.selected{border-color:#D69A3E;background:rgba(245,165,36,.08);box-shadow:inset 0 0 0 1px rgba(245,165,36,.14)}
.ccs-body .cm-swatch-grid{grid-template-columns:repeat(6,minmax(0,1fr));gap:6px}
.ccs-body .cm-color-tile{width:100%;height:auto;aspect-ratio:1/1}
.ccs-body .cm-swatch-group{gap:5px}
.ccs-body .cm-swatch-heading{display:flex;justify-content:space-between;align-items:center}
.ccs-body .cm-brightness input{background:var(--brightness-gradient,linear-gradient(90deg,#08090D,#FFB547))}
.ccs-body .cm-pane{gap:10px}
.ccs-body .cm-collection-scroll{gap:13px}
.ccs-body .cm-section{gap:8px}
.ccs-body .cm-skin-list,.ccs-body .cm-built-in-grid{grid-template-columns:repeat(2,minmax(0,1fr));gap:6px}
.ccs-body .cm-skin,.ccs-body .cm-built-in-item{height:46px;min-width:0;padding:6px;gap:6px}
.ccs-body .cm-skin-gradient,.ccs-body .cm-built-in-strip{width:48px;height:30px;flex:none}
.ccs-body .cm-skin-info{display:flex;flex-direction:column;align-items:flex-start;justify-content:center;gap:2px}
.ccs-body .cm-skin-info b,.ccs-body .cm-built-in-copy b{font-size:10px}
.cm-item-num{font:500 8px ui-monospace,Menlo,Consolas,monospace;color:#929BAD;font-style:normal;line-height:1}
.ccs-rgb-backdrop{z-index:225;align-items:center}
.ccs-rgb-backdrop .cm-rgb-modal{max-height:88%;}
@media(max-width:360px){.ccs-body .cm-wheel-wrap{width:min(224px,62vw,28vh)}.ccs-body .cm-swatch-grid{gap:4px}}
.sx-row{flex:none;display:flex;align-items:stretch;gap:8px;padding:8px 10px;border-radius:16px;background:#1C1F27;box-sizing:border-box}
.sx-row[hidden]{display:none}
.sx-strip{position:relative;flex:1 1 auto;min-width:0;display:flex;height:44px;border-radius:12px;overflow:hidden;border:1px solid #4A5060;box-sizing:border-box;background:#12141A}
.sx-half{appearance:none;flex:1 1 0;min-width:0;padding:0 6px;border:0;background:var(--c);color:#F3F4F8;cursor:pointer;position:relative;z-index:0;display:flex;align-items:center}
.sx-half:focus-visible,.sx-link:focus-visible{outline:2px solid #FFB547;outline-offset:-2px}
.sx-strip:after{content:"";position:absolute;inset:0;z-index:1;pointer-events:none;background:repeating-linear-gradient(115deg,rgba(255,255,255,0) 0 15px,rgba(255,255,255,.16) 15px 19px)}
.sx-seam{position:absolute;z-index:2;left:50%;top:0;bottom:0;width:3px;margin-left:-1.5px;background:#12141A;pointer-events:none}
.sx-strip[data-joined="true"] .sx-seam{display:none}
.sx-side-label{min-width:0;max-width:100%;padding:3px 7px;border-radius:7px;background:rgba(12,14,19,.78);text-align:left;line-height:1.15}
.sx-side-label small,.sx-side-label b{display:block;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.sx-side-label small{font-size:9px;color:#AEB5C6}
.sx-side-label b{font-size:11px}
.sx-half-shared{justify-content:center;text-align:center}
.sx-half-shared .sx-side-label{text-align:center}
.sx-native-label{position:absolute;z-index:4;inset:0;display:none;align-items:center;justify-content:center;background:rgba(12,14,19,.68);color:#E9C98F;font-size:10px;font-weight:750;letter-spacing:.2px}
.sx-row.is-native .sx-half{filter:saturate(.35) brightness(.55);cursor:default}
.sx-row.is-native .sx-strip:after{background:repeating-linear-gradient(135deg,rgba(18,20,26,0) 0 7px,rgba(18,20,26,.55) 7px 10px)}
.sx-row.is-native .sx-native-label{display:flex}
.sx-link{appearance:none;flex:none;width:78px;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:4px;padding:0 6px;border-radius:12px;border:1px solid #3A4052;background:transparent;color:#C3C8D6;font:600 11px system-ui,Segoe UI,Roboto,sans-serif;cursor:pointer;box-sizing:border-box;white-space:nowrap}
.sx-link-top{display:inline-flex;align-items:center;gap:4px}
.sx-link svg{width:14px;height:14px;flex:none}
.sx-switch{position:relative;width:28px;height:16px;border-radius:9px;background:#394252;border:1px solid #596174;box-sizing:border-box;flex:none}
.sx-switch:after{content:"";position:absolute;top:2px;left:2px;width:10px;height:10px;border-radius:50%;background:#8E95A8;transition:all .15s}
.sx-link[aria-checked="true"]{border-color:#8E6735;background:#24231F;color:#F3C16C}
.sx-link[aria-checked="true"] .sx-switch{background:#D99A36;border-color:#D99A36}
.sx-link[aria-checked="true"] .sx-switch:after{left:14px;background:#FFF4DF}
.sx-link:disabled{opacity:.45;cursor:default}
.sx-scope{display:inline-flex;align-items:center;gap:6px;flex:none;height:34px;padding:0 7px;border-radius:9px;background:#252B36;border:1px solid #3A4052;box-sizing:border-box}
.sx-scope[hidden]{display:none}
.sx-scope svg{width:16px;height:16px;flex:none;color:#F2C477}
.sx-scope-copy{line-height:1.15;white-space:nowrap}
.sx-scope-copy small{display:block;font-size:9px;color:#8E95A8}
.sx-scope-copy b{display:block;font-size:10px;color:#F2C477}
.effect-color-thumb.is-native{filter:saturate(.45) brightness(.65)}
@media(max-width:360px){.sx-row{gap:6px;padding:7px 8px}.sx-link{width:72px}.sx-half{padding:0 4px}.sx-side-label{padding:3px 5px}}
.cp-loading{display:flex;align-items:center;justify-content:center;gap:8px;min-height:30px;color:#929BAD;font-size:11px}
.cp-loading i{width:13px;height:13px;border:2px solid #596174;border-top-color:#FFB547;border-radius:50%;animation:cpSpin .75s linear infinite}
.cp-loading--fill{flex:1;min-height:80px}
.cp-side-loading{height:40px;flex:1;border:1px solid #303746;border-radius:12px;background:linear-gradient(90deg,#1C1F27 25%,#252B36 45%,#1C1F27 65%);background-size:220% 100%;animation:cpShimmer 1.2s ease-in-out infinite}
@keyframes cpSpin{to{transform:rotate(360deg)}}
@keyframes cpShimmer{to{background-position-x:-220%}}

html,body{margin:0!important;padding:0!important;display:block!important;height:100%;background:#0A0A0A;color:#E8EAF0;font-family:system-ui,Segoe UI,Roboto,sans-serif}
#frame>div:first-child{width:100%!important;max-width:100%!important;height:100%!important}
#frame{position:relative;width:100%;height:100vh;height:100dvh;border-radius:0;box-shadow:none;overflow:hidden;display:flex;flex-direction:column;background:#12141A}
#cpParams button:disabled,#frame button:disabled{cursor:default}
.mz-toast{position:fixed}
</style>
</head>
<body>
<div class="frame" id="frame" style=""><div style="width:390px;height:760px;display:flex;flex-direction:column;background:#12141A;color:#E8EAF0;font-family:system-ui,Segoe UI,Roboto,sans-serif">
<div class="mz-demo-header" style="flex:none;height:52px;display:flex;align-items:center;justify-content:space-between;padding:0 16px;border-bottom:1px solid #262626;background:#0A0A0A;position:relative;z-index:30;overflow:visible">
<div aria-label="Mizuma Performance" class="mz-brand-lockup">
<span aria-hidden="true" class="mz-brand-mark">M</span>
<span class="mz-brand-words"><strong>MIZUMA</strong><small>PERFORMANCE</small></span>
</div>
<div aria-label="Mode Riding aktif: Manual" class="riding-mode-indicator">
<small>RIDING</small><strong data-riding-mode="">MANUAL</strong>
</div>
<div class="header-actions" style="display:flex;align-items:center;gap:8px;position:relative">

<button aria-label="Koneksi perangkat: terhubung" class="connection-indicator header-icon-btn" data-tip="Terhubung ke perangkat Mizuma" style="position:relative;width:34px;height:34px;border:0;border-radius:9px;background:transparent;color:#3DDC84;display:flex;align-items:center;justify-content:center;padding:0" type="button">
<svg aria-hidden="true" fill="none" height="20" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="1.8" viewbox="0 0 24 24" width="20"><path d="M2.5 8.8a15 15 0 0 1 19 0"></path><path d="M5.7 12a10 10 0 0 1 12.6 0"></path><path d="M9 15.3a5 5 0 0 1 6 0"></path><circle cx="12" cy="19" fill="currentColor" r="1.2" stroke="none"></circle></svg>
<span aria-hidden="true" class="connection-dot"></span>
</button>
<button aria-expanded="false" aria-label="Kecerahan dan pengaturan sisi LED" class="brightness-menu-btn header-icon-btn" data-tip="Kecerahan &amp; sisi LED" style="width:34px;height:34px;border:1px solid #3A4052;border-radius:9px;background:#1C1F27;color:#F3C16C;display:flex;align-items:center;justify-content:center;padding:0" type="button">
<svg aria-hidden="true" fill="none" height="19" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="1.8" viewbox="0 0 24 24" width="19"><circle cx="12" cy="12" r="3.5"></circle><path d="M12 2v2M12 20v2M4.93 4.93l1.42 1.42M17.65 17.65l1.42 1.42M2 12h2M20 12h2M4.93 19.07l1.42-1.42M17.65 6.35l1.42-1.42"></path></svg>
</button>
<div aria-label="Kecerahan dan pemetaan sisi fisik" class="brightness-popover" hidden="" role="group">
<div class="popover-heading"><span>Kecerahan LED</span><strong class="brightness-value">180</strong></div>
<input aria-label="Kecerahan LED" class="brightness-slider" max="255" min="1" type="range" value="180"/>
<div class="brightness-scale"><span>Redup</span><span>Terang</span></div>
<div class="popover-divider"></div>
<button aria-pressed="false" class="physical-swap-btn" type="button">
<span aria-hidden="true" class="swap-icon"><svg fill="none" height="18" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="1.7" viewbox="0 0 24 24" width="18"><path d="M16 3l4 4-4 4"></path><path d="M4 7h16"></path><path d="M8 21l-4-4 4-4"></path><path d="M20 17H4"></path></svg></span>
<span class="swap-copy"><strong>Tukar sisi fisik</strong><small class="swap-state">Normal · kanan tetap kanan</small></span>
<span aria-hidden="true" class="swap-switch"><i></i></span>
</button>
</div>
</div>
</div>
<div style="flex:none;height:42px;display:flex;border-bottom:1px solid #262626"><span style="flex:1;text-align:center;padding:10px 0;font-size:13px;font-weight:600;color:#FFB547;border-bottom:2px solid #FFB547">Composer</span><span style="flex:1;text-align:center;padding:10px 0;font-size:13px;font-weight:600;color:#A0A6B5;border-bottom:2px solid transparent">Favorit</span><span style="flex:1;text-align:center;padding:10px 0;font-size:13px;font-weight:600;color:#A0A6B5;border-bottom:2px solid transparent">Playlist</span><span style="flex:1;text-align:center;padding:10px 0;font-size:13px;font-weight:600;color:#A0A6B5;border-bottom:2px solid transparent">Player</span></div>
<div style="flex:1;min-height:0;display:flex;flex-direction:column"><div style="width:100%;flex:1;min-height:0;box-sizing:border-box;background:#12141A;color:#E8EAF0;font-family:system-ui,Segoe UI,Roboto,sans-serif;padding:8px 12px 8px;display:flex;flex-direction:column;gap:8px">
<div id="cpModes" aria-busy="true" style="flex:none;display:flex;gap:3px;background:#1C1F27;border-radius:10px;padding:2px"><div class="cp-loading" style="flex:1"><i></i><span>Memuat mode…</span></div></div>
<div id="cpSemiColors" class="sx-row" aria-label="Warna kiri dan kanan Semi-auto" hidden><div id="cpSemiStrip" class="sx-strip"><span class="sx-seam" aria-hidden="true"></span><span class="sx-native-label">Warna bawaan efek</span></div><button id="cpSemiJoin" type="button" class="sx-link" role="switch" aria-checked="false" aria-label="Sambungkan warna kiri dan kanan"><span class="sx-link-top"><svg aria-hidden="true" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="m18.84 12.25 1.72-1.71a5 5 0 0 0-.12-7.07 5 5 0 0 0-6.95 0l-1.72 1.71"></path><path d="m5.17 11.75-1.71 1.71a5 5 0 0 0 .12 7.07 5 5 0 0 0 6.95 0l1.71-1.71"></path><path d="M8 2v3M2 8h3M16 19v3M19 16h3"></path></svg><span>Sambung</span></span><span class="sx-switch" aria-hidden="true"></span></button></div>
<div id="cpSides" aria-busy="true" style="display:flex;gap:8px;align-items:stretch"><div class="cp-side-loading"></div><div class="cp-side-loading"></div></div>
<div style="flex:1;min-height:0;background:#1C1F27;border-radius:16px;padding:8px 10px;display:flex;flex-direction:column;gap:6px;width:100%;box-sizing:border-box">
<div style="display:flex;align-items:center;justify-content:space-between;gap:7px;min-width:0"><div style="display:flex;align-items:center;gap:7px;min-width:0;flex:1 1 auto"><span aria-label="Warna atau skin yang sedang dipratinjau" class="effect-color-thumb" style="--preview-color:#FFB547;--preview-swatch:linear-gradient(135deg,#E8B64A 0 20%,#DCE4F1 20% 34%,#4969A7 34% 58%,#D9A94B 58% 78%,#F1DDAA 78% 100%)" title="Warna atau skin yang sedang dipratinjau"></span><div class="effect-heading"><span class="effect-number"><span style="display:flex"><svg aria-hidden="true" fill="none" height="14" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" viewbox="0 0 24 24" width="14"><path d="M2 12s3.6-7 10-7 10 7 10 7-3.6 7-10 7S2 12 2 12z"></path><circle cx="12" cy="12" r="3"></circle></svg></span>040</span><span class="effect-name">Scanner</span></div></div><div style="display:flex;gap:3px;flex:none;align-items:center"><span id="cpSemiScope" class="sx-scope" aria-label="Satu efek untuk kedua sisi" hidden><svg aria-hidden="true" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="2" y="8" width="7" height="8" rx="2"></rect><rect x="15" y="8" width="7" height="8" rx="2"></rect><path d="M9 12h6"></path></svg><span class="sx-scope-copy"><small>Satu efek</small><b>Kiri + Kanan</b></span></span><button aria-label="Edit Warna/Skin" class="color-action-btn" title="Edit warna atau skin untuk efek ini" type="button">
<span aria-hidden="true" class="icon-wheel">
<svg fill="none" viewbox="0 0 24 24" xmlns="http://www.w3.org/2000/svg">
<path d="M14.5 6.5L17.5 9.5L8.2 18.8C7.6 19.4 6.7 19.6 5.9 19.4L4.2 19.8L4.6 18.1C4.4 17.3 4.6 16.4 5.2 15.8L14.5 6.5Z" fill="#FFFFFF" stroke="#111317" stroke-linejoin="round" stroke-width="1.8"></path>
<path d="M19.6 4.4C20.5 5.3 20.5 6.8 19.6 7.7L17.8 9.5L14.5 6.2L16.3 4.4C17.2 3.5 18.7 3.5 19.6 4.4Z" fill="#1E2229" stroke="#111317" stroke-linejoin="round" stroke-width="1.8"></path>
<line stroke="#111317" stroke-linecap="round" stroke-width="2" x1="13.8" x2="17.1" y1="6.9" y2="10.2"></line>
</svg>
</span>
<span class="capsule-body"><span class="btn-title">Edit</span><span class="btn-subtitle">Warna/Skin</span></span>
</button><button aria-label="Balik arah" aria-pressed="false" class="composer-icon-toggle reverse-toggle" data-tip="Balik arah efek" title="Balik arah efek" type="button"><svg aria-hidden="true" fill="none" height="18" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="1.7" viewbox="0 0 24 24" width="18"><path d="M8 3 4 7l4 4"></path><path d="M4 7h16"></path><path d="m16 21 4-4-4-4"></path><path d="M20 17H4"></path></svg></button><button aria-label="Mirror" aria-pressed="false" class="composer-icon-toggle mirror-toggle" data-tip="Mirror efek" title="Mirror efek" type="button"><svg aria-hidden="true" fill="none" height="18" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="1.7" viewbox="0 0 24 24" width="18"><path d="M8 3H5a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h3"></path><path d="M16 3h3a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2h-3"></path><path d="M12 2v2M12 8v2M12 14v2M12 20v2"></path></svg></button></div></div>
<div style="display:flex;gap:8px"><div style="position:relative;flex:1;min-width:0"><span style="position:absolute;left:10px;top:8px;color:#8E95A8;display:flex"><svg aria-hidden="true" fill="none" height="16" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" viewbox="0 0 24 24" width="16"><circle cx="11" cy="11" r="7"></circle><path d="m21 21-4.3-4.3"></path></svg></span><input aria-label="Cari nama atau nomor efek" id="q" placeholder="Cari dari 187 efek" style="width:100%;box-sizing:border-box;height:34px;border-radius:10px;border:1px solid #3A4052;background:#12141A;color:#E8EAF0;padding:0 10px 0 32px;font-size:13px"/></div>
<button aria-label="Filter efek" style="flex:none;height:34px;padding:0 12px;display:flex;align-items:center;gap:6px;border-radius:10px;border:1px solid #3A4052;background:transparent;color:#E8EAF0;font-size:13px;font-weight:600"><svg aria-hidden="true" fill="none" height="16" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" viewbox="0 0 24 24" width="16"><path d="M22 3H2l8 9.46V19l4 2v-8.54z"></path></svg>Filter</button></div>
<div class="fx" aria-busy="true" style="flex:1;min-height:0;display:flex;flex-direction:column;overflow-y:auto"><div class="cp-loading cp-loading--fill"><i></i><span>Memuat daftar efek…</span></div></div>
<div style="flex:none;border-top:1px solid #2B3040;padding-top:4px;display:flex;flex-direction:column;gap:0">
<div id="cpParams" style="display:grid;grid-template-columns:minmax(0,1fr) minmax(0,1fr);gap:0 16px"></div></div></div>
<div style="display:flex;gap:8px">
<button aria-label="Simpan ke Favorit" class="save-to-button" style="flex:1 1 auto;min-width:0;height:38px;display:flex;align-items:center;justify-content:center;gap:5px;padding:0 6px;border-radius:9px;font-size:12px;font-weight:600;white-space:nowrap;border:1px solid #3A4052;background:transparent;color:#E8EAF0" title="Simpan ke Favorit"><svg aria-hidden="true" fill="none" height="16" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="1.8" style="flex:none" viewbox="0 0 24 24" width="16"><path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"></path><path d="M17 21v-8H7v8"></path><path d="M7 3v5h8"></path></svg><span class="save-to-label">Simpan ke Favorit</span></button>
<button aria-disabled="true" aria-label="Simpan ke Playlist (segera)" class="save-to-button" style="flex:1 1 auto;min-width:0;height:38px;display:flex;align-items:center;justify-content:center;gap:5px;padding:0 6px;border-radius:9px;font-size:12px;font-weight:600;white-space:nowrap;border:1px dashed #3A4052;background:transparent;color:#8E95A8" title="Simpan ke Playlist"><svg aria-hidden="true" fill="none" height="16" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="1.8" style="flex:none" viewbox="0 0 24 24" width="16"><path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"></path><path d="M17 21v-8H7v8"></path><path d="M7 3v5h8"></path></svg><span class="save-to-label">Simpan ke Playlist</span></button>
<button aria-label="Play di Riding" class="play-riding-btn" style="flex:1 1 auto;min-width:0;height:38px;display:flex;align-items:center;justify-content:center;gap:7px;padding:0 6px;border-radius:9px;font-size:12.5px;font-weight:700;white-space:nowrap;border:0;background:#FFB547;color:#1A1405" title="Play di Riding"><svg aria-hidden="true" fill="none" height="18" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round" stroke-width="2" style="flex:none" viewbox="0 0 24 24" width="18"><path d="M6 4l14 8-14 8z" fill="currentColor"></path></svg>Play di Riding</button>
</div>
</div></div>
<nav aria-label="Navigasi utama" class="mz-bottom-nav"><button aria-current="false" class="mz-nav-item" data-nav="dash" type="button"><svg aria-hidden="true" viewbox="0 0 24 24"><path d="m3 10 9-7 9 7v10a1 1 0 0 1-1 1h-5v-7H9v7H4a1 1 0 0 1-1-1z"></path></svg><span>Dashboard</span></button><button aria-current="false" class="mz-nav-item" data-nav="color" type="button"><svg aria-hidden="true" viewbox="0 0 24 24"><path d="M12 3a9 9 0 1 0 0 18h1.2a2.3 2.3 0 0 0 1.4-4.1 1.7 1.7 0 0 1 1-3h1.1A4.3 4.3 0 0 0 21 9.6C21 5.9 17 3 12 3Z"></path><circle cx="7.5" cy="10" r="1"></circle><circle cx="11" cy="7" r="1"></circle><circle cx="15.5" cy="8" r="1"></circle></svg><span>Warna</span></button><button aria-current="false" class="mz-nav-item" data-nav="welcoming" type="button"><svg aria-hidden="true" viewbox="0 0 24 24"><circle cx="12" cy="12" r="4"></circle><path d="M12 2v2m0 16v2M4.93 4.93l1.42 1.42m11.3 11.3 1.42 1.42M2 12h2m16 0h2M4.93 19.07l1.42-1.42m11.3-11.3 1.42-1.42"></path></svg><span>Welcoming</span></button><button aria-current="false" class="mz-nav-item" data-nav="sein" type="button"><svg aria-hidden="true" viewbox="0 0 24 24"><path d="M9 18h6m-5 3h4m-5-6a6 6 0 1 1 6 0c-.8.6-1 1.2-1 2H10c0-.8-.2-1.4-1-2Z"></path></svg><span>Sein-Rem</span></button><button aria-current="page" class="mz-nav-item active" data-nav="riding" type="button"><svg aria-hidden="true" viewbox="0 0 24 24"><circle cx="6" cy="17" r="3.2"></circle><circle cx="18" cy="17" r="3.2"></circle><path d="m6 17 4-8h4l4 8M9 9h-3m7 0 2-3h3m-8 3 4 8"></path></svg><span>Riding</span></button><button aria-current="false" class="mz-nav-item" data-nav="settings" type="button"><svg aria-hidden="true" viewbox="0 0 24 24"><circle cx="12" cy="12" r="3"></circle><path d="m19.4 15 .1.1 1.4 1.1-1.4 2.4-1.7-.7a8 8 0 0 1-1.7 1l-.3 1.8h-2.8l-.3-1.8a8 8 0 0 1-1.7-1l-1.7.7-1.4-2.4 1.4-1.1a7 7 0 0 1 0-2l-1.4-1.1 1.4-2.4 1.7.7a8 8 0 0 1 1.7-1l.3-1.8h2.8l.3 1.8a8 8 0 0 1 1.7 1l1.7-.7 1.4 2.4-1.4 1.1a7 7 0 0 1-.1 2Z"></path></svg><span>Pengaturan</span></button></nav></div></div>
<script>window.__TPL={"rowOff": "flex:none;height:34px;display:flex;align-items:center;gap:10px;padding:0 10px;border:0;border-radius:10px;background:transparent;color:#E8EAF0;font-size:14px;text-align:left", "rowOn": "flex:none;height:34px;display:flex;align-items:center;gap:10px;padding:0 10px;border:0;border-radius:10px;background:#2B3040;color:#E8EAF0;font-size:14px;text-align:left", "rowN": "width:28px;font:400 11px ui-monospace,Menlo,Consolas,monospace;color:#8E95A8", "rowName": "flex:1", "native": "<span class=\"native-color-label\" title=\"Efek ini menggunakan warna bawaan\">Warna Bawaan</span>", "stAct": "<span aria-label=\"Tersimpan dan aktif di Riding\" class=\"saved-state saved-state--active\" role=\"img\" title=\"Tersimpan dan sedang diterapkan di Riding\"><svg aria-hidden=\"true\" class=\"saved-play\" viewbox=\"0 0 24 24\"><path d=\"M6 4.5v15L19 12z\"></path></svg><span class=\"saved-state-label\">Play</span></span>", "eyeEl": "<span aria-label=\"Sedang dipratinjau\" class=\"preview-eye\" title=\"Sedang dipratinjau\"><svg aria-hidden=\"true\" viewbox=\"0 0 24 24\"><path d=\"M2 12s3.6-7 10-7 10 7 10 7-3.6 7-10 7S2 12 2 12z\"></path><circle cx=\"12\" cy=\"12\" r=\"3\"></circle></svg></span>", "stIn": "<span aria-label=\"Tersimpan, tidak aktif di Riding\" class=\"saved-state saved-state--inactive\" role=\"img\" title=\"Tersimpan, tidak aktif di Riding\"><svg aria-hidden=\"true\" viewbox=\"0 0 24 24\"><path d=\"M7 4.5v15l12-7.5z\"></path></svg></span>", "modeOff": "flex:1;height:30px;display:flex;align-items:center;justify-content:center;gap:5px;border:0;border-radius:8px;background:transparent;color:#C3C8D6;font-size:13px;font-weight:600", "modeOn": "flex:1;height:30px;display:flex;align-items:center;justify-content:center;gap:5px;border:0;border-radius:8px;background:#2B3040;color:#E8EAF0;font-size:13px;font-weight:600", "playSvg": "<svg aria-hidden=\"true\" fill=\"#FFB547\" height=\"12\" style=\"opacity:1;flex:none\" viewbox=\"0 0 24 24\" width=\"12\"><path d=\"M6 4l14 8-14 8z\"></path></svg>", "sideOn": "flex:1 1 0;min-width:0;height:40px;display:flex;align-items:center;gap:6px;padding:0 10px;border-radius:12px;border:2px solid #FFB547;background:#1C1F27;color:#E8EAF0;text-align:left", "sideOff": "flex:1 1 0;min-width:0;height:40px;display:flex;align-items:center;gap:6px;padding:0 10px;border-radius:12px;border:2px solid transparent;background:#1C1F27;color:#E8EAF0;text-align:left", "sideTx": "flex:1;min-width:0;line-height:1.15", "sideLab": "display:block;font:400 11px ui-monospace,Menlo,Consolas,monospace;color:#8E95A8", "sideName": "display:block;font-size:14px;font-weight:700;font-family:system-ui,Segoe UI,Roboto,sans-serif", "dot": "<span style=\"flex:none;width:8px;height:8px;border-radius:50%;background:#FFB547;box-shadow:0 0 6px #FFB547\"></span>", "copyBtn": "<button aria-label=\"Salin komposisi dari kanan ke kiri\" class=\"copy-composition-btn\" data-effect=\"Fireworks\" data-overwrite=\"Chase\" data-source=\"Kanan\" data-target=\"Kiri\" style=\"width:58px;flex:none;display:flex;align-items:center;justify-content:center;gap:5px;padding:0;border-radius:18px;border:1px solid #697386;background:transparent;color:#E2E6EF\" title=\"Salin komposisi dari kanan ke kiri\"><svg aria-hidden=\"true\" fill=\"none\" height=\"17\" stroke=\"currentColor\" stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-width=\"1.55\" style=\"flex:none\" viewbox=\"0 0 24 24\" width=\"17\"><path d=\"M19 12H5M12 19l-7-7 7-7\"></path></svg><svg aria-hidden=\"true\" fill=\"none\" height=\"17\" stroke=\"currentColor\" stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-width=\"1.55\" style=\"flex:none\" viewbox=\"0 0 24 24\" width=\"17\"><rect height=\"11\" rx=\"2\" width=\"11\" x=\"8.5\" y=\"8.5\"></rect><path d=\"M15.5 8.5V6.8A2.3 2.3 0 0 0 13.2 4.5H6.8a2.3 2.3 0 0 0-2.3 2.3v6.4a2.3 2.3 0 0 0 2.3 2.3h1.7\"></path></svg></button>", "enumHtml": "<span style=\"display:flex\"><svg aria-hidden=\"true\" fill=\"none\" height=\"14\" stroke=\"currentColor\" stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-width=\"2\" viewbox=\"0 0 24 24\" width=\"14\"><path d=\"M2 12s3.6-7 10-7 10 7 10 7-3.6 7-10 7S2 12 2 12z\"></path><circle cx=\"12\" cy=\"12\" r=\"3\"></circle></svg></span>{{n}}", "icSx": "<svg aria-hidden=\"true\" fill=\"none\" height=\"18\" stroke=\"currentColor\" stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-width=\"2\" viewbox=\"0 0 24 24\" width=\"18\"><path d=\"m12 14 4-4\"></path><path d=\"M3.3 19a10 10 0 1 1 17.4 0\"></path></svg>", "icIx": "<svg aria-hidden=\"true\" fill=\"none\" height=\"18\" stroke=\"currentColor\" stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-width=\"2\" viewbox=\"0 0 24 24\" width=\"18\"><circle cx=\"12\" cy=\"12\" r=\"9\"></circle><path d=\"M12 3v18a9 9 0 0 0 0-18z\" fill=\"currentColor\"></path></svg>", "icC": "<svg aria-hidden=\"true\" fill=\"none\" height=\"18\" stroke=\"currentColor\" stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-width=\"2\" viewbox=\"0 0 24 24\" width=\"18\"><path d=\"m12 3 1.9 5.1L19 10l-5.1 1.9L12 17l-1.9-5.1L5 10l5.1-1.9z\"></path></svg>", "slRow": "<div style=\"display:flex;align-items:center;gap:6px;height:28px;min-width:0\"><span style=\"color:#A0A6B5;display:flex\" title=\"Kecepatan\"><svg aria-hidden=\"true\" fill=\"none\" height=\"18\" stroke=\"currentColor\" stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-width=\"2\" viewbox=\"0 0 24 24\" width=\"18\"><path d=\"m12 14 4-4\"></path><path d=\"M3.3 19a10 10 0 1 1 17.4 0\"></path></svg></span><input aria-label=\"Kecepatan\" id=\"sx\" max=\"255\" min=\"0\" style=\"flex:1;min-width:0;margin:0;accent-color:#FFB547\" type=\"range\" value=\"128\"/><span style=\"width:24px;text-align:right;font-size:11px;color:#A0A6B5\">128</span></div>", "swRow": "<div style=\"display:flex;align-items:center;gap:8px;height:28px\"><button aria-checked=\"false\" aria-label=\"Opsi 1\" role=\"switch\" style=\"flex:none;width:44px;height:22px;border-radius:11px;border:1px solid #3A4052;background:#2B3040;padding:0;position:relative\"><span style=\"position:absolute;left:2px;top:2px;width:16px;height:16px;border-radius:50%;background:#8E95A8\"></span></button><span style=\"font-size:12px;color:#A0A6B5\">Opsi 1</span><button aria-label=\"Kembalikan ke bawaan efek\" style=\"margin-left:auto;flex:none;width:28px;height:28px;padding:0;display:flex;align-items:center;justify-content:center;border-radius:8px;border:1px solid #3A4052;background:transparent;color:#E8EAF0\" title=\"Kembalikan ke bawaan efek\"><svg aria-hidden=\"true\" fill=\"none\" height=\"14\" stroke=\"currentColor\" stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-width=\"2\" viewbox=\"0 0 24 24\" width=\"14\"><path d=\"M3 12a9 9 0 1 0 9-9 9.8 9.8 0 0 0-6.7 2.7L3 8\"></path><path d=\"M3 3v5h5\"></path></svg></button></div>"};</script>
<script>
(function(){
'use strict';
const SH=1000;
const HIDE_AUDIO_FX=true;
const FX_BLACKLIST=['frizzles','funky plank','game of life','geq','ghost rider','matrix','metaballs','tartan','polar lights','swirl','lissajous','rubix','invaders','maze','pulser','dna','plasma ball','hipnotic','black hole','cubes','fireworks','akemi','colored bursts','freqmatrix'];
const MZW_RE=/^(?:MZ\d\d|W\d\d) /;
function fxPart(i,k){if(isShowId(i))return null;const m=(S.fxd[i]||'').split(';');return m.length>k?m[k]:null;}
function fxDefs(i){const d={},s=fxPart(i,4);if(s)s.split(',').forEach(function(kv){const p=kv.split('=');if(p.length===2){const v=parseInt(p[1],10);if(!isNaN(v))d[p[0].trim()]=v;}});return d;}
function isRsvd(n){const t=(n||'').toLowerCase();return t===''||t==='rsvd'||t==='-';}
function fxOk(i,name){if(isRsvd(name)||MZW_RE.test(name))return false;const t=name.toLowerCase();if(name.indexOf('2D')===0||FX_BLACKLIST.indexOf(t)>=0)return false;const f=fxPart(i,3)||'';if(f.indexOf('2')>=0&&f.indexOf('1')<0&&f.indexOf('0')<0)return false;if(HIDE_AUDIO_FX&&(f.indexOf('v')>=0||f.indexOf('f')>=0))return false;return true;}
function parseFxData(meta){const parts=(meta||'').split(';'),labels=(parts[0]||'').split(','),def=['Kecepatan','Intensitas','Custom 1','Custom 2','Custom 3'],keys=['sx','ix','c1','c2','c3'],sl=[];for(let i=0;i<5;i++){let l=labels[i];if(l===undefined||l==='')continue;if(l==='!')l=def[i];sl.push({key:keys[i],label:l});}const tg=[];for(let i=0;i<3;i++){let l=labels[5+i];if(l===undefined||l==='')continue;if(l==='!')l='Opsi '+(i+1);tg.push({key:['o1','o2','o3'][i],label:l});}return{sl:sl,tg:tg};}
function pmDefault(i){const d=fxDefs(i),g=function(k,df){return d[k]!=null?d[k]:df;};return{sx:g('sx',128),ix:g('ix',128),c1:g('c1',128),c2:g('c2',128),c3:g('c3',128),o1:g('o1',0),o2:g('o2',0),o3:g('o3',0)};}
const T=window.__TPL;
const $=(s,r)=>(r||document).querySelector(s);
const esc=s=>String(s).replace(/[&<>"]/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;'}[c]));
const clone=o=>JSON.parse(JSON.stringify(o));
const MODES=[['auto','Auto'],['semi-auto','Semi-auto'],['manual','Manual']];
const S={fx:[],fxd:[],shows:[],showMs:[],list:[],order:{},palRaw:{},palPromise:null,palNames:[],skins:[],solidColors:Array(6).fill(null),colorDataPromise:null,colorDataErrors:[],palBase:null,palCount:0,saved:null,riding:'manual',mode:'manual',side:'R',colorSide:'R',q:'',D:{},PM:{},mirror:false,previewing:false,net:true,ready:false,err:'',bri:180,swap:false};
const SIDE_DISP=$('#cpSides').style.display;const F=$('#frame'),modesBox=$('#cpModes'),semiColors=$('#cpSemiColors'),semiStrip=$('#cpSemiStrip'),semiJoin=$('#cpSemiJoin'),sidesBox=$('#cpSides'),listEl=$('.fx'),qEl=$('#q'),paramsEl=$('#cpParams');
const playBtn=$('.play-riding-btn'),revBtn=$('.reverse-toggle'),mirBtn=$('.mirror-toggle'),thumb=$('.effect-color-thumb'),colorBtn=$('.color-action-btn'),semiScope=$('#cpSemiScope');
const isShowId=i=>i>=SH,isShow=isShowId;
function fxName(i){return isShow(i)?(S.shows[i-SH]||('Show '+(i-SH))):(S.fx[i]||('Efek '+i));}
function fxPartS(i,k){return isShow(i)?null:fxPart(i,k);}
function selfColored(i){return !isShow(i)&&(fxPartS(i,1)||'')===''&&(fxPartS(i,2)||'')==='';}
function numLabel(i){return isShow(i)?('S'+String(i-SH+1).padStart(2,'0')):String(i).padStart(3,'0');}
function buildList(){const a=S.fx.map((n,i)=>({n:n,i:i})).filter(e=>fxOk(e.i,e.n));S.shows.forEach((n,k)=>a.push({n:n,i:SH+k}));a.sort((x,y)=>x.n.localeCompare(y.n,'id',{sensitivity:'base',numeric:true})||x.i-y.i);S.list=a;
 S.order={manual:{},'semi-auto':{},auto:{}};['manual','semi-auto','auto'].forEach(mode=>{let n=0;a.forEach(e=>{if(modeAllows(mode,e.i))S.order[mode][e.i]=String(++n).padStart(3,'0');});});}
function orderLabel(i,mode){return S.order[mode]&&S.order[mode][i]||numLabel(i);}
function modeAllows(mode,i){return mode==='manual'?true:mode==='auto'?isShow(i):!isShow(i);}
const jget=(u,n)=>fetch(u).then(r=>{if(!r.ok)throw new Error(u+' → HTTP '+r.status);return r.json();}).catch(e=>(n===undefined?2:n)>0?new Promise(res=>setTimeout(res,350)).then(()=>jget(u,(n===undefined?2:n)-1)):Promise.reject(e)); // coba ulang 2x; galat menyebut URL yang gagal
function loadFx(){return Promise.all([jget('/json/eff'),jget('/json/fxdata'),jget('/mizuma/shows').catch(()=>[]),jget('/mizuma/show-durations').catch(()=>[])]).then(v=>{S.fx=v[0];S.fxd=v[1];S.shows=v[2]||[];S.showMs=v[3]||[];buildList();});}
function loadPalX(page){return jget('/json/palx?page='+(page||0)).then(function(d){const map=d&&d.p;if(!map||typeof map!=='object')throw new Error('Data palet tidak valid');Object.keys(map).forEach(function(k){if(Array.isArray(map[k]))S.palRaw[parseInt(k,10)]=map[k];});if(typeof d.m==='number'&&(page||0)<d.m)return loadPalX((page||0)+1);});}
function ensurePalX(){if(!S.palPromise)S.palPromise=loadPalX(0);return S.palPromise;}
function effOf(e){if(!e||!e.valid)return null;const fx=e.isShow?SH+(e.showIndex|0):(e.fx|0);return{fx:fx,p:{sx:e.sx,ix:e.ix,c1:e.c1,c2:e.c2,c3:e.c3,o1:e.o1?1:0,o2:e.o2?1:0,o3:e.o3?1:0},mirror:!!e.mirror};}
function colOf(c){return c?{pm:!!c.palette,pn:c.pal|0,r:c.rgb[0],g:c.rgb[1],b:c.rgb[2]}:{pm:false,pn:2,r:255,g:255,b:255};}
function loadSaved(){return Promise.all([jget('/mizuma/riding/effects'),jget('/mizuma/riding/colors'),jget('/mizuma/riding/reverse').catch(()=>({})),jget('/mizuma/riding/mirror').catch(()=>({enabled:false})),jget('/mizuma/riding/mode')]).then(v=>{const e=v[0],c=v[1],rv=v[2];
 const sv={manual:{R:effOf(e.manual&&e.manual.right),L:effOf(e.manual&&e.manual.left)},'semi-auto':{R:effOf(e['semi-auto']&&e['semi-auto'].shared)},auto:{R:effOf(e.auto&&e.auto.shared)}};
 const ext=(o,x)=>o&&Object.assign(o,x);
 ext(sv.manual.R,{rev:!!(rv.manual&&rv.manual.kanan),col:colOf(c.manual&&c.manual.right)});ext(sv.manual.L,{rev:!!(rv.manual&&rv.manual.kiri),col:colOf(c.manual&&c.manual.left)});
 ext(sv['semi-auto'].R,{rev:!!(rv['semi-auto']&&rv['semi-auto'].kanan),col:colOf(c['semi-auto']&&c['semi-auto'].right)});sv['semi-auto'].L={col:colOf(c['semi-auto']&&c['semi-auto'].left)};
 sv['semi-auto'].join=!!(c['semi-auto']&&c['semi-auto'].join);
 ext(sv.auto.R,{rev:false,col:colOf(c.auto&&c.auto.right)});
 S.saved=sv;S.mirror=!!v[3].enabled;S.riding=v[4].mode||'manual';S.ready=v[4].ready!==false;});}
function defaultFx(mode){const e=S.list.find(x=>modeAllows(mode,x.i));return e?e.i:0;}
function initDraft(){MODES.forEach(m=>{const mode=m[0],sv=S.saved[mode],d={};const sides=mode==='manual'?['R','L']:['R'];
 sides.forEach(k=>{let s=sv[k];if(!s||!modeAllows(mode,s.fx)){const fx=defaultFx(mode);s={fx:fx,p:pmDefault(fx),rev:false,col:s&&s.col||colOf()};}d[k]=clone(s);});
 if(mode==='semi-auto'){d.Lcol=clone(sv.L.col);d.mirror=S.mirror;d.join=!!sv.join;}S.D[mode]=d;});}
const drf=()=>S.D[S.mode]||{};
const cur=()=>drf()[S.mode==='manual'?S.side:'R']||{fx:0,p:{},rev:false,col:null}; // objek cadangan saat data belum termuat
const savedCur=()=>{const sv=S.saved&&S.saved[S.mode];return sv&&sv[S.mode==='manual'?S.side:'R']||null;};
const strip=s=>s?{fx:s.fx,p:s.p,rev:s.rev,col:s.col,mirror:!!s.mirror}:null;
function draftSig(mode){const d=S.D[mode];return JSON.stringify(mode==='manual'?[strip(d.R),strip(d.L)]:mode==='semi-auto'?[strip(d.R),d.Lcol,d.mirror,d.join]:[strip(d.R)]);}
function savedSig(mode){const sv=S.saved[mode];return JSON.stringify(mode==='manual'?[strip(sv.R),strip(sv.L)]:mode==='semi-auto'?[strip(sv.R),sv.L&&sv.L.col,S.mirror,sv.join]:[strip(sv.R)]);}
const changed=mode=>draftSig(mode)!==savedSig(mode);
function qs(mode){const d=S.D[mode],q=new URLSearchParams();q.set('mode',mode);
 const add=(pre,s)=>{if(isShow(s.fx)){q.set(pre+'show','1');q.set(pre+'idx',s.fx-SH);}else q.set(pre+'fx',s.fx);
  ['sx','ix','c1','c2','c3'].forEach(k=>q.set(pre+k,s.p[k]));['o1','o2','o3'].forEach(k=>{if(s.p[k])q.set(pre+k,'1');});if(s.rev)q.set(pre+'rev','1');if(mode==='manual'&&s.mirror)q.set(pre+'mirror','1');};
 const col=(pre,c)=>{q.set(pre+'pm',c.pm?'1':'0');q.set(pre+'pn',c.pn);q.set(pre+'r',c.r);q.set(pre+'g',c.g);q.set(pre+'b',c.b);};
 if(mode==='manual'){add('kanan_',d.R);col('kanan_',d.R.col);add('kiri_',d.L);col('kiri_',d.L.col);}
 else if(mode==='semi-auto'){add('kanan_',d.R);col('kanan_',d.R.col);col('kiri_',d.Lcol);q.set('mirror',d.mirror?'1':'0');q.set('join',d.join?'1':'0');}
 else{add('kanan_',d.R);col('kanan_',d.R.col);}
 return q.toString();}
/* ===== pratinjau lewat engine (tidak menyimpan) ===== */
let pvT=0,pvBusy=false,pvAgain=false;
function schedulePreview(){if(!S.saved)return;S.previewing=true;clearTimeout(pvT);pvT=setTimeout(sendPreview,120);}
function sendPreview(){if(pvBusy){pvAgain=true;return;}pvBusy=true;
 fetch('/mizuma/comp/preview?'+qs(S.mode),{method:'POST'}).then(r=>{if(r.status===503)throw 0;if(!r.ok)return r.json().then(j=>{toast('Pratinjau ditolak: '+(j.error||r.status));});S.net=true;})
 .catch(()=>{S.net=false;pvAgain=true;}).finally(()=>{pvBusy=false;if(pvAgain){pvAgain=false;setTimeout(sendPreview,250);}renderTop();});}
setInterval(function(){if(S.previewing)fetch('/mizuma/status').then(()=>{S.net=true;renderTop();}).catch(()=>{S.net=false;renderTop();});},5000);
function endPreview(){if(!S.previewing)return;try{navigator.sendBeacon('/mizuma/comp/end');}catch(e){fetch('/mizuma/comp/end',{method:'POST',keepalive:true});}}
window.addEventListener('pagehide',endPreview);
document.addEventListener('visibilitychange',function(){if(!document.hidden&&S.previewing)sendPreview();});
/* ===== aksi ===== */
let TT;function toast(m){const old=$('.mz-toast',F);if(old)old.remove();const t=document.createElement('div');t.className='mz-toast';t.setAttribute('role','status');t.textContent=m;F.appendChild(t);clearTimeout(TT);TT=setTimeout(()=>{if(t.isConnected)t.remove();},2600);}
function play(){const mode=S.mode;playBtn.disabled=true;
 fetch('/mizuma/comp/play?'+qs(mode),{method:'POST'}).then(r=>{if(!r.ok)throw 0;return r.json();}).then(()=>{clearTimeout(pvT);S.previewing=false;return loadSaved();})
 .then(()=>toast('Komposisi berhasil dijalankan di Riding')).catch(()=>toast('Gagal menjalankan di Riding')).finally(render);}
function setMode(m){if(S.mode===m)return;S.mode=m;S.side='R';S.q='';qEl.value='';render();listEl.scrollTop=0;schedulePreview();}
function setSide(k){S.side=k;render();scrollSel();}
function pickFx(i){const s=cur();if(s.fx===i)return;const key=S.mode+':'+S.side+':'+i;s.fx=i;s.p=S.PM[key]?clone(S.PM[key]):(savedCur()&&savedCur().fx===i?clone(savedCur().p):pmDefault(i));schedulePreview();}
function setParam(k,v){const s=cur();s.p[k]=v;S.PM[S.mode+':'+S.side+':'+s.fx]=clone(s.p);schedulePreview();}
function resetParams(){const s=cur();s.p=pmDefault(s.fx);S.PM[S.mode+':'+S.side+':'+s.fx]=clone(s.p);schedulePreview();}
function copySide(){const dd=drf(),from=S.side,to=from==='R'?'L':'R';dd[to]=clone(dd[from]);schedulePreview();}
/* ===== render (memakai markup dan gaya dari berkas desain) ===== */
function renderTop(){const lab=({'manual':'MANUAL','semi-auto':'SEMI-AUTO','auto':'AUTO','playlist':'PLAYLIST'})[S.riding]||String(S.riding).toUpperCase();
 $('[data-riding-mode]').textContent=lab;$('.riding-mode-indicator').setAttribute('aria-label','Mode Riding aktif: '+lab);
 const ci=$('.connection-indicator');ci.setAttribute('aria-label','Koneksi perangkat: '+(S.net?'terhubung':'terputus'));$('.connection-dot').style.background=S.net?'':'#6B7280';}
function renderModes(){modesBox.removeAttribute('aria-busy');modesBox.innerHTML=MODES.map(m=>'<button type="button" data-m="'+m[0]+'" aria-pressed="'+(S.mode===m[0])+'" style="'+(S.mode===m[0]?T.modeOn:T.modeOff)+'">'+(S.riding===m[0]?T.playSvg:'')+m[1]+'</button>').join('');}
function renderSides(){sidesBox.removeAttribute('aria-busy');if(S.mode!=='manual'){sidesBox.style.display='none';return;}sidesBox.style.display=SIDE_DISP;
 const card=k=>{const s=S.saved.manual[k],on=S.side===k,lab=(k==='L'?'Kiri':'Kanan')+(s?' · '+orderLabel(s.fx,'manual'):''),nm=s?fxName(s.fx):'Belum diatur';
  return '<button type="button" data-s="'+k+'" aria-pressed="'+on+'" aria-label="Sisi '+(k==='L'?'kiri':'kanan')+', '+esc((s?orderLabel(s.fx,'manual')+' ':'')+nm)+(S.riding==='manual'?', tersimpan di Riding':'')+'" style="'+(on?T.sideOn:T.sideOff)+'"><span style="'+T.sideTx+'"><span style="'+T.sideLab+'">'+esc(lab)+'</span><span style="'+T.sideName+'">'+esc(nm)+'</span></span>'+(on&&S.riding==='manual'?T.dot:'')+'</button>';};
 const to=S.side==='R'?'Kiri':'Kanan',from=S.side==='R'?'Kanan':'Kiri';
 sidesBox.innerHTML=card('L')+T.copyBtn.replace('data-source="Kanan"','data-source="'+from+'"').replace('data-target="Kiri"','data-target="'+to+'"').replace(/aria-label="Salin komposisi dari kanan ke kiri"/,'aria-label="Salin komposisi dari '+from.toLowerCase()+' ke '+to.toLowerCase()+'"')+card('R');
 const cb=$('.copy-composition-btn',sidesBox);if(cb&&S.side==='L'){const sv=cb.querySelectorAll('svg');if(sv[0])sv[0].style.transform='scaleX(-1)';}}
function colorCss(c){const clamp=v=>Math.max(0,Math.min(255,parseInt(v,10)||0));return'rgb('+clamp(c.r)+','+clamp(c.g)+','+clamp(c.b)+')';}
function paletteSwatch(c){const fallback=colorCss(c);if(!c.pm)return fallback;const d=S.palRaw[c.pn];if(!Array.isArray(d)||!d.length)return fallback;
 const clamp=v=>Math.max(0,Math.min(255,parseInt(v,10)||0));
 const stops=d.map(e=>{if(Array.isArray(e)&&e.length>=4){const pos=Math.max(0,Math.min(255,parseInt(e[0],10)||0))/255*100;return' rgb('+clamp(e[1])+','+clamp(e[2])+','+clamp(e[3])+') '+pos+'%';}return fallback;});
 if(stops.length===1)stops.push(stops[0]);return'linear-gradient(90deg,'+stops.join(',')+')';}
function colorName(c){if(!c)return'Warna';if(c.pm){const skin=S.skins.find(x=>x.slot!=null&&S.palBase!=null&&S.palBase-(+x.slot)===(c.pn|0));return skin?String(skin.n||skin.name||'Skin'):S.palNames[c.pn]||('Palet '+c.pn);}return rgbHex([c.r,c.g,c.b]);}
function semiColor(side){const d=drf();return d.join?d.R.col:(side==='L'?d.Lcol:d.R.col);}
function editorColor(){return S.mode==='semi-auto'?semiColor(S.colorSide):cur().col;}
function setEditorColor(color){if(S.mode==='semi-auto'){const d=drf();if(!d.join&&S.colorSide==='L')d.Lcol=clone(color);else d.R.col=clone(color);}else cur().col=clone(color);}
function colorEditorTarget(){if(S.mode==='semi-auto')return drf().join?'Semua sisi':'Sisi '+(S.colorSide==='L'?'Kiri':'Kanan');return'Sisi '+(S.side==='L'?'Kiri':'Kanan');}
function renderSemiColors(){semiColors.hidden=S.mode!=='semi-auto';if(semiColors.hidden)return;
 const d=drf(),joined=!!d.join,native=selfColored(d.R.fx),left=joined?d.R.col:d.Lcol,right=d.R.col;
 semiColors.classList.toggle('is-native',native);semiStrip.dataset.joined=String(joined);
 semiStrip.innerHTML=joined
  ?'<button type="button" class="sx-half sx-half-shared" data-semi-color="R"><span class="sx-side-label"><small>Semua sisi</small><b>'+esc(colorName(right))+'</b></span></button><span class="sx-seam" aria-hidden="true"></span><span class="sx-native-label">Warna bawaan efek</span>'
  :'<button type="button" class="sx-half" data-semi-color="L"><span class="sx-side-label"><small>Kiri</small><b>'+esc(colorName(left))+'</b></span></button><button type="button" class="sx-half" data-semi-color="R"><span class="sx-side-label"><small>Kanan</small><b>'+esc(colorName(right))+'</b></span></button><span class="sx-seam" aria-hidden="true"></span><span class="sx-native-label">Warna bawaan efek</span>';
 semiStrip.querySelectorAll('.sx-half').forEach(button=>{const side=button.dataset.semiColor,color=side==='L'?left:right;button.style.setProperty('--c',paletteSwatch(color));button.disabled=native;button.setAttribute('aria-label','Edit warna sisi '+(joined?'kiri dan kanan':side==='L'?'kiri':'kanan')+', '+colorName(color));});
 semiJoin.setAttribute('aria-checked',String(joined));semiJoin.disabled=native;semiJoin.setAttribute('aria-disabled',String(native));
}
function renderPrev(){const s=cur(),d=drf();
 const c=s.col||{r:255,g:255,b:255,pm:false},solid=colorCss(c);thumb.style.setProperty('--preview-color',solid);
 const semi=S.mode==='semi-auto',native=semi&&selfColored(s.fx);
 if(semi){const left=d.join?d.R.col:d.Lcol,right=d.R.col;thumb.style.setProperty('--preview-swatch',d.join?paletteSwatch(right):paletteSwatch(left)+' left center / 50% 100% no-repeat, '+paletteSwatch(right)+' right center / 50% 100% no-repeat');thumb.title=native?'Warna ditentukan oleh efek':(d.join?'Warna bersama · '+colorName(right):'Kiri · '+colorName(left)+' / Kanan · '+colorName(right));}
 else{thumb.style.setProperty('--preview-swatch',paletteSwatch(c));thumb.title=c.pm?'Palet WLED '+c.pn:'RGB '+c.r+', '+c.g+', '+c.b;}
 thumb.classList.toggle('is-native',native);
 $('.effect-number').innerHTML=T.enumHtml.replace('{{n}}',orderLabel(s.fx,S.mode));$('.effect-name').textContent=fxName(s.fx);
 const auto=S.mode==='auto';
 semiScope.hidden=S.mode!=='semi-auto';
 revBtn.setAttribute('aria-pressed',String(!!s.rev));revBtn.classList.toggle('is-active',!!s.rev);revBtn.disabled=auto;revBtn.style.opacity=auto?.4:'';
 revBtn.title=semi?'Balik arah efek Semi-auto di kedua sisi':'Balik arah efek';
 const mirrorAllowed=!auto,mirrorActive=mirrorAllowed&&(S.mode==='manual'?!!s.mirror:!!d.mirror);
 mirBtn.setAttribute('aria-pressed',String(mirrorActive));mirBtn.classList.toggle('is-active',mirrorActive);mirBtn.disabled=!mirrorAllowed;mirBtn.style.opacity=mirrorAllowed?'':.4;mirBtn.title=auto?'Mirror tidak tersedia pada Auto':'Mirror '+(S.mode==='manual'?'efek '+(S.side==='R'?'Kanan':'Kiri'):'efek Semi-auto');
 colorBtn.disabled=false;colorBtn.style.display=semi?'none':'';colorBtn.style.opacity='';colorBtn.title='Edit warna atau skin untuk efek ini';}
function marks(i){const sv=savedCur(),s=cur();let h='',m='';
 if(selfColored(i))h+=T.native;
 if(sv&&sv.fx===i)m+=(S.riding===S.mode)?T.stAct:T.stIn;
 if(s.fx===i)m+=T.eyeEl;
 return h+(m?'<span class="saved-markers">'+m+'</span>':'');}
function renderList(){listEl.removeAttribute('aria-busy');if(S.err){listEl.innerHTML='<div style="padding:16px;color:#A0A6B5;text-align:center">'+esc(S.err)+'</div>';return;}
 const q=S.q.trim().toLowerCase(),s=cur();const rows=S.list.filter(e=>modeAllows(S.mode,e.i)&&(!q||e.n.toLowerCase().indexOf(q)>=0||orderLabel(e.i,S.mode).toLowerCase().indexOf(q)>=0||numLabel(e.i).toLowerCase().indexOf(q)>=0));
 listEl.innerHTML=rows.length?rows.map(e=>{const on=s.fx===e.i;return '<button type="button" data-i="'+e.i+'"'+(on?' aria-pressed="true"':'')+' style="'+(on?T.rowOn:T.rowOff)+'"><span style="'+T.rowN+'" title="Urutan alfabetis; ID efek WLED '+numLabel(e.i)+'">'+orderLabel(e.i,S.mode)+'</span><span style="'+T.rowName+'">'+esc(e.n)+'</span>'+marks(e.i)+'</button>';}).join(''):'<div style="padding:16px;color:#A0A6B5;text-align:center">Tidak ada efek yang cocok</div>';}
function mk(html){const t=document.createElement('div');t.innerHTML=html;return t.firstElementChild;}
function renderParams(){const s=cur();paramsEl.innerHTML='';
 if(isShow(s.fx)){const ms=S.showMs[s.fx-SH];paramsEl.appendChild(mk('<div style="grid-column:1/3;font-size:12px;color:#A0A6B5;padding:6px 0">Show Mizuma'+(ms?' · '+(Math.round(ms/100)/10)+' detik':'')+' · diulang terus di Riding.</div>'));return;}
 const meta=parseFxData(S.fxd[s.fx]||''),ic={sx:T.icSx,ix:T.icIx};
 meta.sl.forEach(x=>{const r=mk(T.slRow);r.firstElementChild.title=x.label;r.firstElementChild.innerHTML=ic[x.key]||T.icC;const inp=r.querySelector('input');inp.id='p_'+x.key;inp.dataset.k=x.key;inp.setAttribute('aria-label',x.label);inp.value=s.p[x.key]|0;r.lastElementChild.textContent=s.p[x.key]|0;paramsEl.appendChild(r);});
 const tg=meta.tg.length?meta.tg:[null];
 tg.forEach((x,n)=>{const r=mk(T.swRow);const sw=r.querySelector('[role=switch]'),lab=sw.nextElementSibling,rs=r.lastElementChild;
  if(x){const on=!!s.p[x.key];sw.dataset.t=x.key;sw.setAttribute('aria-checked',String(on));sw.setAttribute('aria-label',x.label);lab.textContent=x.label;if(on){sw.style.background='#D99A36';sw.style.borderColor='#D99A36';sw.firstElementChild.style.left='24px';sw.firstElementChild.style.background='#FFF4DF';}}
  else{sw.style.display='none';lab.style.display='none';}
  if(n===0){rs.id='cpReset';}else rs.style.display='none';paramsEl.appendChild(r);});}
function renderPlay(){const same=!changed(S.mode)&&S.riding===S.mode;playBtn.disabled=same||!S.ready;playBtn.style.opacity=(same||!S.ready)?.45:'';}
function render(){if(!S.saved)return;renderTop();renderModes();renderSemiColors();renderSides();renderPrev();renderList();renderParams();renderPlay();}
function scrollSel(){const r=listEl.querySelector('[aria-pressed="true"]');if(r)listEl.scrollTop=Math.max(0,r.offsetTop-listEl.clientHeight/2+r.offsetHeight/2);}
/* ===== editor warna Composer ===== */
let colorSheet=null,colorTab='color',colorDraft=null,colorOriginal=null,colorHSV=[32,70,100],colorEscapeHandler=null,rgbModalOriginal=null,rgbModalDraft=null;
const QUICK_COLORS=['#FF3B30','#FFB547','#FFE600','#32D74B','#0A84FF','#BF5AF2'];
function rgbHex(rgb){return'#'+rgb.map(v=>Math.max(0,Math.min(255,Math.round(v))).toString(16).padStart(2,'0')).join('').toUpperCase();}
function hexRgb(hex){const m=/^#?([0-9a-f]{6})$/i.exec(hex||'');return m?[parseInt(m[1].slice(0,2),16),parseInt(m[1].slice(2,4),16),parseInt(m[1].slice(4,6),16)]:[255,255,255];}
function rgbHsv(rgb){const r=rgb[0]/255,g=rgb[1]/255,b=rgb[2]/255,mx=Math.max(r,g,b),mn=Math.min(r,g,b),d=mx-mn;let h=0;
 if(d){h=mx===r?((g-b)/d)%6:mx===g?(b-r)/d+2:(r-g)/d+4;h=(h*60+360)%360;}return[h,mx?d/mx*100:0,mx*100];}
function hsvRgb(hsv){const h=((hsv[0]%360)+360)%360,s=Math.max(0,Math.min(100,hsv[1]))/100,v=Math.max(0,Math.min(100,hsv[2]))/100;
 const f=n=>{const k=(n+h/60)%6;return Math.round((v-v*s*Math.max(0,Math.min(k,4-k,1)))*255);};return[f(5),f(3),f(1)];}
function ensureColorData(){if(S.colorDataPromise)return S.colorDataPromise;
 S.colorDataErrors=[];S.colorDataPromise=Promise.all([
  jget('/json/pal').then(d=>{S.palNames=Array.isArray(d)?d:Object.keys(d||{}).map(k=>d[k]);}).catch(()=>S.colorDataErrors.push('Palet WLED tidak dapat dimuat.')),
  jget('/json/info').then(d=>{const major=parseInt(String(d.ver||'').split('.')[0],10);if(!Number.isFinite(major))throw new Error('Versi WLED tidak valid');S.palBase=major>=16?200:255;S.palCount=parseInt(d.cpalcount,10)||0;}).catch(()=>S.colorDataErrors.push('Informasi versi WLED tidak dapat dimuat; skin custom disembunyikan.')),
  jget('/mizuma/skins').then(d=>{S.skins=Array.isArray(d)?d:(d&&Array.isArray(d.skins)?d.skins:[]);}).catch(()=>S.colorDataErrors.push('Koleksi skin perangkat tidak dapat dimuat.')),
  jget('/mizuma/colors').then(d=>{if(d&&d.initialized&&Array.isArray(d.slots)&&d.slots.length===6)S.solidColors=d.slots.map(c=>/^#[0-9a-f]{6}$/i.test(c||'')?c.toUpperCase():null);}).catch(()=>S.colorDataErrors.push('Warna tersimpan tidak dapat dimuat.')),
  ensurePalX().catch(()=>S.colorDataErrors.push('Pratinjau gradasi palet tidak tersedia.'))
 ]);return S.colorDataPromise;}
function drawColorWheel(){if(!colorSheet)return;const canvas=$('#cmColorWheel',colorSheet),ctx=canvas&&canvas.getContext('2d');if(!ctx)return;
 const n=canvas.width,c=n/2,r=c-1;ctx.clearRect(0,0,n,n);for(let i=0;i<360;i++){ctx.beginPath();ctx.moveTo(c,c);ctx.arc(c,c,r,(i-1)*Math.PI/180,i*Math.PI/180);ctx.closePath();ctx.fillStyle='hsl('+i+',100%,50%)';ctx.fill();}
 const g=ctx.createRadialGradient(c,c,0,c,c,r);g.addColorStop(0,'rgba(255,255,255,1)');g.addColorStop(1,'rgba(255,255,255,0)');ctx.fillStyle=g;ctx.beginPath();ctx.arc(c,c,r,0,Math.PI*2);ctx.fill();
 const a=colorHSV[0]*Math.PI/180,dist=r*colorHSV[1]/100;ctx.beginPath();ctx.arc(c+Math.cos(a)*dist,c+Math.sin(a)*dist,6,0,Math.PI*2);ctx.fillStyle='#fff';ctx.fill();ctx.lineWidth=2;ctx.strokeStyle='#171923';ctx.stroke();}
function setEditorRgb(rgb,draw){colorHSV=rgbHsv(rgb);const v=$('#cmBrightness',colorSheet);if(v)v.value=Math.round(colorHSV[2]);if(draw)drawColorWheel();renderEditorColor();}
function renderEditorColor(){if(!colorSheet)return;const rgb=hsvRgb(colorHSV),hex=rgbHex(rgb),tile=$('#cmCurrent',colorSheet);if(tile)tile.style.background=hex;
 ['r','g','b'].forEach((k,i)=>{const el=$('#cm'+k.toUpperCase(),colorSheet);if(el)el.textContent=rgb[i];});
 const slider=$('#cmBrightness',colorSheet);if(slider){slider.value=Math.round(colorHSV[2]);slider.style.setProperty('--brightness-gradient','linear-gradient(90deg,#08090D,'+hex+')');}
 const label=$('#cmBrightnessValue',colorSheet);if(label)label.textContent=Math.round(colorHSV[2])+'%';}
function previewEditorDraft(){if(!colorDraft)return;if(!colorDraft.pm){const rgb=hsvRgb(colorHSV);colorDraft.r=rgb[0];colorDraft.g=rgb[1];colorDraft.b=rgb[2];colorDraft.pn=2;}
 setEditorColor(colorDraft);schedulePreview();}
function syncRgbModal(rgb){if(!colorSheet)return;const hex=rgbHex(rgb);$('#cmRgbPreview',colorSheet).style.background=hex;$('#cmHex',colorSheet).value=hex;
 ['Red','Green','Blue'].forEach((name,i)=>{$('#cm'+name+'Range',colorSheet).value=rgb[i];$('#cm'+name+'Number',colorSheet).value=rgb[i];});}
function openRgbModal(){const rgb=hsvRgb(colorHSV);rgbModalOriginal=colorHSV.slice();rgbModalDraft=clone(colorDraft);syncRgbModal(rgb);$('#cmRgbBackdrop',colorSheet).hidden=false;}
function closeRgbModal(save){const popup=$('#cmRgbBackdrop',colorSheet);if(!popup)return;
 if(!save&&rgbModalOriginal){colorHSV=rgbModalOriginal.slice();colorDraft=clone(rgbModalDraft);drawColorWheel();renderEditorColor();setEditorColor(colorDraft);schedulePreview();render();}
 popup.hidden=true;rgbModalOriginal=null;rgbModalDraft=null;}
function updateRgbModal(rgb){colorDraft.pm=false;setEditorRgb(rgb,true);previewEditorDraft();syncRgbModal(hsvRgb(colorHSV));}
function renderSolidGrid(){if(!colorSheet)return;const grid=$('#cmSavedColors',colorSheet);if(!grid)return;
 grid.innerHTML=S.solidColors.map((c,i)=>'<button type="button" class="cm-color-tile'+(c&&c===rgbHex(hsvRgb(colorHSV))?' selected':'')+'" data-slot="'+i+'" aria-label="'+(c?'Pilih warna tersimpan '+(i+1):'Slot warna kosong '+(i+1))+'" title="'+(c||'Slot warna kosong')+'" style="background:'+(c||'#202530')+'"></button>').join('');}
function renderCollection(){if(!colorSheet)return;const skinGrid=$('#cmSkinGrid',colorSheet),palGrid=$('#cmPaletteGrid',colorSheet);
 const skins=S.skins.filter(x=>S.palBase!=null&&x.slot!=null&&x.slot!==''&&Number.isInteger(+x.slot)&&(+x.slot>=0)&&(!S.palCount||+x.slot<S.palCount))
  .map(x=>({item:x,name:String(x.n||x.name||'Skin'),id:S.palBase-(+x.slot)}))
  .sort((a,b)=>a.name.localeCompare(b.name,'id',{sensitivity:'base',numeric:true}));
 skinGrid.innerHTML=skins.length?skins.map((x,i)=>{const sel=colorDraft.pm&&colorDraft.pn===x.id,raw=S.palRaw[x.id],hasPalette=Array.isArray(raw)&&raw.length>0;
  const swatch=hasPalette?paletteSwatch({pm:true,pn:x.id,r:0,g:0,b:0}):'#303746',swatchTitle=hasPalette?'Palet WLED aktual':'Data palet WLED tidak tersedia';
  return'<button type="button" class="cm-skin'+(sel?' selected':'')+'" data-palette="'+x.id+'" data-kind="skin" aria-pressed="'+sel+'"><span class="cm-skin-gradient" title="'+swatchTitle+'" style="background:'+swatch+'"></span><span class="cm-skin-info"><i class="cm-item-num">'+String(i+1).padStart(3,'0')+'</i><b>'+esc(x.name)+'</b></span></button>';}).join(''):'<p class="cm-note">'+(S.colorDataErrors.some(x=>x.indexOf('skin')>=0)?esc(S.colorDataErrors.find(x=>x.indexOf('skin')>=0)):'Belum ada skin bawaan yang tersedia.')+'</p>';
 const pals=S.palNames.map((name,id)=>({name:String(name),id:id})).filter(x=>x.name&&x.name.charAt(0)!=='*')
  .sort((a,b)=>a.name.localeCompare(b.name,'id',{sensitivity:'base',numeric:true}));
 palGrid.innerHTML=pals.length?pals.map((x,i)=>{const c={pm:true,pn:x.id,r:255,g:181,b:71},sel=colorDraft.pm&&colorDraft.pn===x.id;
  return'<button type="button" class="cm-built-in-item'+(sel?' selected':'')+'" data-palette="'+x.id+'" data-kind="palette" aria-pressed="'+sel+'"><span class="cm-built-in-strip" style="background:'+paletteSwatch(c)+'"></span><span class="cm-built-in-copy"><i class="cm-item-num">'+String(i+1).padStart(3,'0')+'</i><b>'+esc(x.name)+'</b></span></button>';}).join(''):'<p class="cm-note">Palet WLED tidak tersedia.</p>';
 const err=$('#cmDataError',colorSheet);if(err)err.textContent=S.colorDataErrors.filter(x=>x.indexOf('skin')<0).join(' ');renderSolidGrid();}
function setColorTab(tab){colorTab=tab;const c=$('#cmTabColor',colorSheet),k=$('#cmTabCollection',colorSheet);c.classList.toggle('active',tab==='color');k.classList.toggle('active',tab==='collection');
 $('#cmPaneColor',colorSheet).hidden=tab!=='color';$('#cmPaneCollection',colorSheet).hidden=tab!=='collection';if(tab==='collection')renderCollection();}
function closeColorEditor(commit){if(!commit&&colorOriginal&&colorSheet){setEditorColor(colorOriginal);schedulePreview();render();}
 if(colorSheet){colorSheet.remove();colorSheet=null;}if(colorEscapeHandler){document.removeEventListener('keydown',colorEscapeHandler);colorEscapeHandler=null;}colorDraft=null;colorOriginal=null;}
function openColorEditor(){if(colorSheet)return;const c=editorColor()||{pm:false,pn:2,r:255,g:255,b:255},rgb=[c.r,c.g,c.b],target=colorEditorTarget();
 colorOriginal=clone(c);colorDraft={pm:!!c.pm,pn:c.pn|0,r:rgb[0],g:rgb[1],b:rgb[2]};colorHSV=rgbHsv(rgb);colorTab='color';
 colorSheet=mk('<div class="ccs-overlay" style="position:fixed;inset:0;z-index:200;background:rgba(0,0,0,.76);display:flex;justify-content:center;align-items:stretch"><section class="color-manager ccs-body" role="dialog" aria-modal="true" aria-label="Edit Warna/Skin untuk '+esc(target)+'" style="max-width:430px;width:100%;height:100%;max-height:100dvh">'+
 '<div class="ccs-grabber" aria-hidden="true"></div><header class="ccs-head"><span class="ccs-head-icon" aria-hidden="true"><svg viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg"><path d="M14.5 6.5L17.5 9.5L8.2 18.8C7.6 19.4 6.7 19.6 5.9 19.4L4.2 19.8L4.6 18.1C4.4 17.3 4.6 16.4 5.2 15.8L14.5 6.5Z" fill="#fff" stroke="#111317" stroke-width="1.8" stroke-linejoin="round"/><path d="M19.6 4.4C20.5 5.3 20.5 6.8 19.6 7.7L17.8 9.5L14.5 6.2L16.3 4.4C17.2 3.5 18.7 3.5 19.6 4.4Z" fill="#1e2229" stroke="#111317" stroke-width="1.8" stroke-linejoin="round"/><line x1="13.8" y1="6.9" x2="17.1" y2="10.2" stroke="#111317" stroke-width="2" stroke-linecap="round"/></svg></span><div class="ccs-head-copy"><span class="ccs-eyebrow">COMPOSER · COLOR EDITOR</span><strong class="ccs-title">Edit Warna/Skin</strong><span class="ccs-context">'+esc(target)+' · '+esc(fxName(cur().fx))+'</span></div><button type="button" class="ccs-close" data-close aria-label="Tutup editor"><svg viewBox="0 0 24 24"><path d="m6 6 12 12M18 6 6 18"/></svg></button></header>'+
 '<div class="color-manager-scroll">'+
 '<div class="cm-tabs"><button id="cmTabColor" class="cm-tab active" type="button">Warna</button><button id="cmTabCollection" class="cm-tab" type="button">Koleksi</button></div>'+
 '<div id="cmPaneColor" class="cm-pane"><div class="cm-wheel-area"><div class="cm-wheel-wrap"><canvas id="cmColorWheel" width="220" height="220" aria-label="Pilih warna"></canvas></div>'+
 '<span id="cmCurrent" class="ccs-current-swatch" aria-label="Contoh warna saat ini"></span></div>'+
 '<div class="cm-brightness-row"><button type="button" class="cm-rgb-readout ccs-rgb" id="cmRgbOpen" aria-label="Buka editor RGB manual" title="Atur RGB manual"><span class="cm-rgb-channel"><b style="background:#E5332B">R</b><em id="cmR"></em></span><span class="cm-rgb-channel"><b style="background:#1FA84A">G</b><em id="cmG"></em></span><span class="cm-rgb-channel"><b style="background:#2F6BFF">B</b><em id="cmB"></em></span></button><div class="cm-brightness"><div class="cm-brightness-label">Kecerahan: <span id="cmBrightnessValue"></span></div><input id="cmBrightness" type="range" min="0" max="100" value="100" aria-label="Kecerahan warna"></div><button type="button" class="cm-random" aria-label="Warna acak" title="Warna acak">🎲</button></div>'+
 '<div class="cm-swatch-group"><div class="cm-swatch-heading"><strong>Warna cepat</strong></div><div id="cmQuickColors" class="cm-swatch-grid"></div></div>'+
 '<div class="cm-swatch-group"><div class="cm-swatch-heading"><strong>Warna tersimpan</strong></div><div id="cmSavedColors" class="cm-swatch-grid"></div></div></div>'+
 '<div id="cmPaneCollection" class="cm-pane cm-collection-scroll" hidden><div id="cmDataError" class="cm-note"></div><div class="cm-section"><div class="cm-section-head"><strong>Skin Buatan</strong></div><div id="cmSkinGrid" class="cm-skin-list"><p class="cm-note">Memuat koleksi skin…</p></div></div><div class="cm-section"><div class="cm-section-head"><strong>Skin Bawaan</strong></div><div id="cmPaletteGrid" class="cm-built-in-grid"><p class="cm-note">Memuat palet WLED…</p></div></div></div></div>'+
 '<footer class="ccs-footer"><button type="button" class="ccs-cancel" data-close>Tutup</button><button type="button" class="ccs-use" data-apply>Gunakan untuk '+esc(target)+'</button></footer>'+
 '<div class="cm-rgb-backdrop ccs-rgb-backdrop" id="cmRgbBackdrop" hidden><div class="cm-rgb-modal" role="dialog" aria-modal="true" aria-labelledby="cmRgbTitle"><h4 id="cmRgbTitle">Warna manual</h4><p>Masukkan warna secara presisi menggunakan HEX atau kanal RGB.</p><div class="cm-rgb-preview-row"><i class="cm-rgb-preview" id="cmRgbPreview"></i><label for="cmHex">Hex</label><input id="cmHex" type="text" maxlength="7" value="#FFFFFF" autocapitalize="off" spellcheck="false" placeholder="#RRGGBB"></div><div class="cm-rgb-fields"><label class="cm-rgb-row"><b style="background:#E5332B">R</b><input type="range" min="0" max="255" id="cmRedRange"><input type="number" min="0" max="255" inputmode="numeric" id="cmRedNumber"></label><label class="cm-rgb-row"><b style="background:#1FA84A">G</b><input type="range" min="0" max="255" id="cmGreenRange"><input type="number" min="0" max="255" inputmode="numeric" id="cmGreenNumber"></label><label class="cm-rgb-row"><b style="background:#2F6BFF">B</b><input type="range" min="0" max="255" id="cmBlueRange"><input type="number" min="0" max="255" inputmode="numeric" id="cmBlueNumber"></label></div><div class="cm-modal-actions"><button type="button" class="cm-secondary" data-rgb-cancel>Batal</button><button type="button" class="cm-save" data-rgb-done>Selesai</button></div></div></div></section></div>');
 document.body.appendChild(colorSheet);const sheet=colorSheet;
 $('#cmQuickColors',sheet).innerHTML=QUICK_COLORS.map((x,i)=>'<button type="button" class="cm-color-tile" data-quick="'+i+'" title="'+x+'" style="background:'+x+'"></button>').join('');
 renderSolidGrid();drawColorWheel();renderEditorColor();setColorTab('color');
 const canvas=$('#cmColorWheel',sheet);let dragging=false;function wheelPick(ev){const b=canvas.getBoundingClientRect(),x=(ev.clientX-b.left)/b.width-.5,y=(ev.clientY-b.top)/b.height-.5,rad=Math.min(1,Math.sqrt(x*x+y*y)*2);
  colorHSV[0]=(Math.atan2(y,x)*180/Math.PI+360)%360;colorHSV[1]=Math.min(100,rad*100);colorDraft.pm=false;drawColorWheel();renderEditorColor();previewEditorDraft();}
 canvas.addEventListener('pointerdown',e=>{dragging=true;canvas.setPointerCapture(e.pointerId);wheelPick(e);});canvas.addEventListener('pointermove',e=>{if(dragging)wheelPick(e);});canvas.addEventListener('pointerup',()=>{dragging=false;});canvas.addEventListener('pointercancel',()=>{dragging=false;});
 $('#cmBrightness',sheet).addEventListener('input',e=>{colorHSV[2]=+e.target.value;colorDraft.pm=false;drawColorWheel();renderEditorColor();previewEditorDraft();});
 $('#cmRgbOpen',sheet).addEventListener('click',openRgbModal);
 $('#cmHex',sheet).addEventListener('input',e=>{if(/^#?[0-9a-f]{6}$/i.test(e.target.value))updateRgbModal(hexRgb(e.target.value));});
 [['Red',0],['Green',1],['Blue',2]].forEach(([name,index])=>{
  const range=$('#cm'+name+'Range',sheet),number=$('#cm'+name+'Number',sheet);
  const update=value=>{const rgb=hsvRgb(colorHSV);rgb[index]=Math.max(0,Math.min(255,parseInt(value,10)||0));updateRgbModal(rgb);};
  range.addEventListener('input',()=>update(range.value));number.addEventListener('input',()=>{if(number.value!=='')update(number.value);});
  number.addEventListener('change',()=>{if(number.value==='')number.value=hsvRgb(colorHSV)[index];});
 });
 sheet.addEventListener('click',e=>{const close=e.target.closest('[data-close]');if(close){closeColorEditor();return;}
  if(e.target===sheet){closeColorEditor();return;}
  if(e.target.id==='cmRgbBackdrop'){closeRgbModal(false);return;}
  if(e.target.closest('#cmTabColor')){setColorTab('color');return;}if(e.target.closest('#cmTabCollection')){setColorTab('collection');return;}
  const q=e.target.closest('[data-quick]');if(q){setEditorRgb(hexRgb(QUICK_COLORS[+q.dataset.quick]),true);colorDraft.pm=false;previewEditorDraft();return;}
  const tile=e.target.closest('[data-slot]');if(tile){const saved=S.solidColors[+tile.dataset.slot];if(saved){setEditorRgb(hexRgb(saved),true);colorDraft.pm=false;previewEditorDraft();}else toast('Slot warna ini masih kosong');return;}
  const item=e.target.closest('[data-palette]');if(item){colorDraft.pm=true;colorDraft.pn=+item.dataset.palette;previewEditorDraft();renderCollection();return;}
  if(e.target.closest('.cm-random')){colorHSV=[Math.random()*360,65+Math.random()*35,70+Math.random()*30];colorDraft.pm=false;drawColorWheel();renderEditorColor();previewEditorDraft();return;}
  if(e.target.closest('[data-rgb-cancel]')){closeRgbModal(false);return;}
  if(e.target.closest('[data-rgb-done]')){closeRgbModal(true);return;}
  if(e.target.closest('[data-apply]')){previewEditorDraft();closeColorEditor(true);render();toast('Warna pratinjau diperbarui');}
 });
 colorEscapeHandler=function(e){if(e.key==='Escape'&&colorSheet){if(!$('#cmRgbBackdrop',colorSheet).hidden)closeRgbModal(false);else closeColorEditor();}};document.addEventListener('keydown',colorEscapeHandler);
 ensureColorData().then(()=>{if(colorSheet===sheet){renderCollection();renderSemiColors();}});
}
/* ===== event ===== */
modesBox.addEventListener('click',e=>{const b=e.target.closest('button[data-m]');if(b)setMode(b.dataset.m);});
semiColors.addEventListener('click',e=>{const join=e.target.closest('#cpSemiJoin');if(join){if(join.disabled)return;drf().join=!drf().join;S.colorSide='R';schedulePreview();render();return;}
 const color=e.target.closest('[data-semi-color]');if(color&&!color.disabled){S.colorSide=color.dataset.semiColor;openColorEditor();}});
sidesBox.addEventListener('click',e=>{const c=e.target.closest('.copy-composition-btn');
 if(c){const old=$('.copy-confirm',F);if(old)old.remove();const from=c.dataset.source||'Kanan',to=c.dataset.target||'Kiri',dd=drf(),k=to==='Kiri'?'L':'R';
  const eff=fxName(cur().fx),over=dd[k]?fxName(dd[k].fx):'komposisi sebelumnya';
  const p=document.createElement('div');p.className='copy-confirm';p.setAttribute('role','dialog');p.setAttribute('aria-label','Konfirmasi salin komposisi');
  p.innerHTML='<strong>Salin komposisi ke sisi '+to.toLowerCase()+'?</strong><p>'+esc(eff)+' dari sisi '+from.toLowerCase()+' akan menggantikan '+esc(over)+' di sisi '+to.toLowerCase()+'.</p><div class="copy-confirm-actions"><button type="button" class="cancel-copy">Batal</button><button type="button" class="confirm-copy">Ya, salin</button></div>';
  F.appendChild(p);p.querySelector('.cancel-copy').onclick=()=>p.remove();p.querySelector('.confirm-copy').onclick=()=>{p.remove();copySide();render();toast('Komposisi berhasil disalin ke sisi '+to.toLowerCase());};return;}
 const b=e.target.closest('button[data-s]');if(b)setSide(b.dataset.s);});
listEl.addEventListener('click',e=>{const b=e.target.closest('button[data-i]');if(b){const t=listEl.scrollTop;pickFx(parseInt(b.dataset.i,10));render();listEl.scrollTop=t;}});
qEl.addEventListener('input',e=>{S.q=e.target.value;renderList();});
revBtn.addEventListener('click',()=>{const s=cur();s.rev=!s.rev;schedulePreview();render();});
mirBtn.addEventListener('click',()=>{if(S.mode==='manual')cur().mirror=!cur().mirror;else drf().mirror=!drf().mirror;schedulePreview();render();});
paramsEl.addEventListener('input',e=>{const r=e.target;if(r.dataset&&r.dataset.k){const v=parseInt(r.value,10);r.nextElementSibling.textContent=v;setParam(r.dataset.k,v);renderPlay();}});
paramsEl.addEventListener('click',e=>{const sw=e.target.closest('[role=switch]');if(sw&&sw.dataset.t){const s=cur(),k=sw.dataset.t;s.p[k]=s.p[k]?0:1;S.PM[S.mode+':'+S.side+':'+s.fx]=clone(s.p);schedulePreview();render();return;}if(e.target.closest('#cpReset')){resetParams();render();}});
playBtn.addEventListener('click',play);
colorBtn.addEventListener('click',openColorEditor);
/* header: kecerahan dan tukar sisi fisik */
const pop=$('.brightness-popover'),mbtn=$('.brightness-menu-btn'),bs=$('.brightness-slider'),bv=$('.brightness-value'),sw=$('.physical-swap-btn');
mbtn.addEventListener('click',e=>{e.stopPropagation();const o=pop.hidden;pop.hidden=!o;mbtn.setAttribute('aria-expanded',String(o));});
document.addEventListener('click',e=>{if(!pop.hidden&&!pop.contains(e.target)&&!mbtn.contains(e.target)){pop.hidden=true;mbtn.setAttribute('aria-expanded','false');}});
let briT;bs.addEventListener('input',()=>{bv.textContent=bs.value;clearTimeout(briT);briT=setTimeout(()=>{const v=parseInt(bs.value,10);fetch('/json/state',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({bri:v})}).catch(()=>{});fetch('/mizuma/bri?v='+v,{method:'POST'}).catch(()=>{});},80);});
function renderSwap(){sw.setAttribute('aria-pressed',String(S.swap));const st=$('.swap-state',sw);if(st)st.textContent=S.swap?'Ditukar · kanan menjadi kiri':'Normal · kanan tetap kanan';}
sw.addEventListener('click',()=>{const next=!S.swap;fetch('/mizuma/side-swap?enabled='+(next?'1':'0'),{method:'POST'}).then(r=>{if(!r.ok)throw 0;S.swap=next;renderSwap();return loadSaved();}).then(()=>render()).catch(()=>toast('Gagal menukar sisi'));});
/* tombol yang belum tersambung: nonaktif dan berlabel (tidak ada tombol hantu) */
[['[aria-label="Filter efek"]','Filter efek (segera)'],['.save-to-button[aria-label="Simpan ke Favorit"]','Simpan ke Favorit (segera)'],['.save-to-button[aria-label^="Simpan ke Playlist"]','Simpan ke Playlist (segera)']].forEach(x=>{const b=$(x[0]);if(b){b.setAttribute('aria-disabled','true');b.disabled=true;b.title=x[1];b.setAttribute('aria-label',x[1]);b.style.opacity=.55;}});
const NAV={dash:'/app',color:'/color',welcoming:'/welcoming',sein:'/sein-rem',riding:'/riding',pengaturan:'/pengaturan'};
document.querySelectorAll('.mz-nav-item').forEach(b=>b.addEventListener('click',()=>{const u=NAV[b.dataset.nav];if(u&&b.dataset.nav!=='riding')location.href=u;}));
/* mulai */
function boot(n){const saved=loadSaved().then(()=>{renderTop();renderModes();renderSides();});Promise.all([loadFx(),saved]).then(()=>{S.err='';initDraft();render();listEl.scrollTop=0;ensurePalX().then(()=>renderPrev()).catch(e=>console.warn('Composer palette thumbnails:',e));}).catch(e=>{console.error('Composer:',e&&e.stack||e);S.err='Gagal memuat data dari perangkat ('+(e&&e.message||e)+').'+(n<5?' Mencoba lagi…':' Muat ulang halaman.');S.saved=null;S.D={};renderTop();[modesBox,sidesBox,listEl].forEach(el=>{el.removeAttribute('aria-busy');el.innerHTML='<div style="padding:12px;color:#A0A6B5;text-align:center;font-size:11px">'+esc(S.err)+'</div>';});if(n<5)setTimeout(()=>boot(n+1),2000);});}
boot(0);
jget('/json/state').then(j=>{if(j&&j.bri){S.bri=j.bri;bs.value=j.bri;bv.textContent=j.bri;}}).catch(()=>{});
jget('/mizuma/side-swap').then(j=>{S.swap=!!j.enabled;renderSwap();}).catch(()=>{});
})();

</script>
</body>
</html>
)rawliteral";

inline String mizumaNav(const char *activeKey) {
  String nav = FPSTR(MIZUMA_BOTTOMNAV_HTML);
  static const char *const keys[] = {"dash", "color", "welcoming", "riding", "sein", "pengaturan"};
  for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
    nav.replace(String("__A_") + keys[i] + "__", (activeKey && strcmp(activeKey, keys[i]) == 0) ? "active" : "");
  }
  return nav;
}

inline void mizumaApplyCommon(String &html, const char *activeKey, const MizumaVehicleInfo &v) {
  html.replace("%HEADER%", FPSTR(MIZUMA_HEADER_HTML));
  html.replace("%BOTTOMNAV%", mizumaNav(activeKey));
  html.replace("%VEHICLE_NAME%",  mizumaEsc(v.name.length()  ? v.name  : String("Motor Anda")));
  html.replace("%VEHICLE_BRAND%", mizumaEsc(v.brand.length() ? v.brand : String("-")));
  html.replace("%VEHICLE_YEAR%",  v.year ? String(v.year) : String("-"));
  html.replace("%VEHICLE_PLATE%", mizumaEsc(v.plate.length() ? v.plate : String("-")));
  html.replace("%FIRMWARE%", MZ_FW_VERSION);
}
inline String mizumaRenderPage(const char *pageTemplate, const char *activeKey, const MizumaVehicleInfo &v) {
  String html = FPSTR(pageTemplate);
  mizumaApplyCommon(html, activeKey, v);
  return html;
}

// css/js: selalu divalidasi ulang (ETag = stempel build) -> setelah firmware baru, HP otomatis memakai versi terbaru.
inline void mizumaSendAsset(AsyncWebServerRequest *req, const char *type, const char *data) {
  const String etag = String("\"") + MZ_UI_BUILD + "\"";
  if (req->hasHeader("If-None-Match") && req->header("If-None-Match") == etag) { req->send(304); return; }
  AsyncWebServerResponse *res = req->beginResponse_P(200, type, data);
  res->addHeader("ETag", etag);
  res->addHeader("Cache-Control", "no-cache");
  req->send(res);
}

// Satu-satunya pintu masuk UI dari engine. getVehicle dipanggil setiap halaman diminta (selalu data terbaru).
// Rute: /app /color /welcoming /riding /sein-rem /pengaturan   (/led lama dialihkan ke rute baru)
inline void mizumaUiRegister(AsyncWebServer &server, std::function<MizumaVehicleInfo()> getVehicle) {
  server.on("/mizuma.css", HTTP_GET, [](AsyncWebServerRequest *req){ mizumaSendAsset(req, "text/css", MIZUMA_SHARED_CSS); });
  server.on("/mizuma.js",  HTTP_GET, [](AsyncWebServerRequest *req){ mizumaSendAsset(req, "text/javascript", MIZUMA_HEADER_SCRIPT); });
  server.on("/riding", HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/html", MIZUMA_RIDING_HTML); });
  server.on("/color",  HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/html", MIZUMA_RIDING_HTML); }); // halaman yang sama, membuka tab Color
  server.on("/riding2", HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/html", MIZUMA_COMPOSER_HTML); }); // Composer baru (dev)
  server.on("/sein-rem", HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/html", MIZUMA_LED_HTML); });
  server.on("/led", HTTP_GET, [](AsyncWebServerRequest *req){ // kompatibilitas tautan lama
    String t = req->hasParam("t") ? req->getParam("t")->value() : String("");
    if (t == "welcoming") req->redirect("/welcoming");
    else if (t == "sein" || t == "rem") req->redirect("/sein-rem?t=" + t);
    else if (t == "warna") req->redirect("/color");
    else req->redirect("/riding"); });
  server.on("/app", HTTP_GET, [getVehicle](AsyncWebServerRequest *req){
    req->send(200, "text/html", mizumaRenderPage(MIZUMA_HOME_HTML, "dash", getVehicle())); });
  server.on("/welcoming", HTTP_GET, [getVehicle](AsyncWebServerRequest *req){
    req->send(200, "text/html", mizumaRenderPage(MIZUMA_WELCOMING_HTML, "welcoming", getVehicle())); });
  server.on("/pengaturan", HTTP_GET, [getVehicle](AsyncWebServerRequest *req){
    req->send(200, "text/html", mizumaRenderPage(MIZUMA_SETTINGS_HTML, "pengaturan", getVehicle())); });
  server.on("/mizuma/frag/header", HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/html", MIZUMA_HEADER_HTML); });
  server.on("/mizuma/frag/script", HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/javascript", MIZUMA_HEADER_SCRIPT); });
  server.on("/mizuma/frag/nav",    HTTP_GET, [](AsyncWebServerRequest *req){ // ?a=<kunci tab aktif>
    String a = req->hasParam("a") ? req->getParam("a")->value() : String("");
    req->send(200, "text/html", mizumaNav(a.c_str())); });
}