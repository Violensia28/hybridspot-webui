import { getSensorCfg, setSensorCfg } from '/js/api.js';
import { toast } from '/js/ui.js';
export function initSettings(){
  const el = document.getElementById('view-settings');
  el.innerHTML = `<div class="card"><h3>Settings</h3>
  <div class="row">
    <div class="col"><label>Current Sensor</label>
      <select id="sv-ctype">
        <option value="ACS712_5A">ACS712-5A</option>
        <option value="ACS712_20A">ACS712-20A</option>
        <option value="ACS712_30A" selected>ACS712-30A</option>
      </select>
    </div>
    <div class="col"><label>mV per A</label><input id="sv-mva" type="number" step="0.1"/></div>
    <div class="col"><label>ADC Midpoint</label><input id="sv-mid" type="number"/></div>
  </div>
  <div class="row">
    <div class="col"><label>V scale (V/ADC)</label><input id="sv-vs" type="number" step="0.00001"/></div>
    <div class="col"><label>Phase Corr (deg)</label><input id="sv-ph" type="number" step="0.1"/></div>
  </div>
  <div class="row">
    <div class="col"><button class="btn btn-primary" id="sv-save">Save</button></div>
  </div>
  </div>`;
  const mvaDefault = { 'ACS712_5A':185, 'ACS712_20A':100, 'ACS712_30A':66 };
  function applyDefaults(){ sv_mva.value = mvaDefault[sv_ctype.value]||66; }
  sv_ctype.onchange = applyDefaults; applyDefaults();
  getSensorCfg().then(c=>{
    try{
      sv_ctype.value = c.current?.type || 'ACS712_30A';
      sv_mva.value = c.current?.mV_per_A ?? mvaDefault[sv_ctype.value];
      sv_mid.value = c.current?.adc_midpoint ?? 2048;
      sv_vs.value = c.voltage?.scale_V_per_ADC ?? 0.122;
      sv_ph.value = c.voltage?.phase_corr_deg ?? 2;
    }catch(e){}
  });
  document.getElementById('sv-save').onclick = async ()=>{
    const ok = await setSensorCfg({
      current:{ type: sv_ctype.value, mV_per_A:+sv_mva.value, adc_midpoint:+sv_mid.value },
      voltage:{ scale_V_per_ADC:+sv_vs.value, phase_corr_deg:+sv_ph.value }
    });
    toast(ok? 'Saved' : 'Save failed');
  };
}
