import { getStatus } from '/js/api.js';
import { toast } from '/js/ui.js';
export function initStatus(){
  const el = document.getElementById('view-status');
  el.innerHTML = `<div class="card"><div class="row">
    <div class="col"><b>Mode:</b> <span id="st-mode">-</span></div>
    <div class="col"><b>V<sub>RMS</sub>:</b> <span id="st-v">-</span> V</div>
    <div class="col"><b>I<sub>RMS</sub>:</b> <span id="st-i">-</span> A</div>
    <div class="col"><b>Guards:</b> <span id="st-g">-</span></div>
  </div></div>`;
  async function tick(){
    const s = await getStatus();
    if(s.offline){ document.getElementById('st-mode').textContent='Offline'; return; }
    document.getElementById('st-mode').textContent=s.mode||'-';
    document.getElementById('st-v').textContent=(s.vrms??0).toFixed(1);
    document.getElementById('st-i').textContent=(s.irms??0).toFixed(1);
    const g = s.guards||{}; document.getElementById('st-g').textContent = `V:${g.v?'!':'OK'} I:${g.i?'!':'OK'} MCB:${g.mcb?'!':'OK'}`;
  }
  tick(); setInterval(tick, 1000);
}
