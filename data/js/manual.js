import{postStart,postStop}from"/js/api.js";import{tones}from"/js/audio.js";export function initManual(){const e=document.getElementById("view-manual");e.innerHTML=`<div class="card">
<h3>Manual Mode</h3>
<div class="row">
  <div class="col"><label>Pulse</label><select id="mn-pulse"><option>single</option><option>double</option></select></div>
  <div class="col"><label>Duration ms</label><input id="mn-dur" type="number" min="1" max="1000" value="120"/></div>
  <div class="col"><label>Cycles</label><input id="mn-cyc" type="number" min="1" max="10" value="2"/></div>
  <div class="col"><label>Gap ms (double)</label><input id="mn-gap" type="number" min="20" max="500" value="120"/></div>
</div>
<div class="row">
  <div class="col"><button class="btn btn-primary" id="mn-start">Start</button></div>
  <div class="col"><button class="btn btn-danger" id="mn-stop">Stop</button></div>
</div>
</div>`;document.getElementById("mn-start").onclick=async()=>{tones.start();const e=await postStart({mode:"manual",params:{pulse:mn_pulse.value,dur_ms:+mn_dur.value,cycles:+mn_cyc.value,gap_ms:+mn_gap.value}});e?tones.ok():tones.error()},document.getElementById("mn-stop").onclick=async()=>{const e=await postStop();e?tones.ok():tones.error()}}