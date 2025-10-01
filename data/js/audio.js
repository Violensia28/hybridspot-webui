let ctx; const resume=()=>{ if(!ctx) ctx=new (window.AudioContext||window.webkitAudioContext)(); if(ctx.state==='suspended') ctx.resume(); };
export function beep(f=1000, ms=120, v=0.15, type='square'){
  resume(); const o=ctx.createOscillator(), g=ctx.createGain(); o.type=type; o.frequency.value=f; g.gain.value=v; o.connect(g); g.connect(ctx.destination); o.start(); setTimeout(()=>o.stop(), ms);
}
export const tones = { start:()=>beep(880,120,0.15,'square'), ok:()=>beep(1200,80,0.12,'square'), error:()=>beep(440,200,0.2,'sawtooth') };
