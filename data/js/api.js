const J=(u,opt={})=>fetch(u,{headers:{"Cache-Control":"no-cache"},...opt});
export async function getStatus(){try{const r=await J("/api/status");if(!r.ok)throw 0;return await r.json()}catch(e){return{offline:true}}}
export async function postStart(p){const r=await J("/api/weld/start",{method:"POST",body:JSON.stringify(p)});return r.ok}
export async function postStop(){const r=await J("/api/weld/stop",{method:"POST"});return r.ok}
export async function getSensorCfg(){const r=await J("/api/sensor/config");if(!r.ok)throw 0;return await r.json()}
export async function setSensorCfg(o){const q=new URLSearchParams({current:o.current?.type||"ACS712_30A",mvA:String(o.current?.mV_per_A||66),mid:String(o.current?.adc_midpoint||2048),vscale:String(o.voltage?.scale_V_per_ADC||0.122),phase:String(o.voltage?.phase_corr_deg||2)}).toString();const r=await J("/api/sensor/config?"+q,{method:"POST"});return r.ok}