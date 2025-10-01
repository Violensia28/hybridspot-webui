import{getStatus}from"/js/api.js";export function initStatus(){const e=document.getElementById("view-status");e.innerHTML=`<div class="card"><div class="row">
<div class="col"><b>Mode:</b> <span id="st-mode">-</span></div>
<div class="col"><b>V<sub>RMS</sub>:</b> <span id="st-v">-</span> V</div>
<div class="col"><b>I<sub>RMS</sub>:</b> <span id="st-i">-</span> A</div>
<div class="col"><b>Guards:</b> <span id="st-g">-</span></div>
</div></div>`;async function t(){const t=await getStatus();if(t.offline){return void(document.getElementById("st-mode").textContent="Offline");}document.getElementById("st-mode").textContent=t.mode||"-",document.getElementById("st-v").textContent=(null!=t.vrms?t.vrms:0).toFixed(1),document.getElementById("st-i").textContent=(null!=t.irms?t.irms:0).toFixed(1);const e=t.guards||{};document.getElementById("st-g").textContent=`V:${e.v?"!":"OK"} I:${e.i?"!":"OK"} MCB:${e.mcb?"!":"OK"}`}t(),setInterval(t,1e3)}