import { postStart } from '/js/api.js';
import { tones } from '/js/audio.js';
export function initPreset(){
  const el = document.getElementById('view-preset');
  el.innerHTML = `<div class="card">
  <h3>Preset Mode</h3>
  <div class="row">
    <div class="col"><label>Material</label><select id="pr-mat"><option>Ni</option><option>Al</option><option>Cu</option></select></div>
    <div class="col"><label>Thickness</label><select id="pr-thk"><option>0.10</option><option>0.12</option><option>0.15</option><option>0.20</option><option>0.25</option></select></div>
    <div class="col"><label>Pulse</label><select id="pr-pulse"><option>single</option><option>double</option></select></div>
  </div>
  <div class="row">
    <div class="col"><button class="btn btn-primary" id="pr-apply">Apply & Start</button></div>
  </div>
  <div class="row"><div class="col"><small>Recent:</small><div id="pr-recent"></div></div></div>
  </div>`;
  document.getElementById('pr-apply').onclick = async ()=>{
    tones.start();
    const ok = await postStart({mode:'preset', params:{ material:pr_mat.value, thickness:pr_thk.value, pulse:pr_pulse.value }});
    if(ok) tones.ok(); else tones.error();
  };
}
