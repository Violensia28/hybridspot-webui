export function initOTA(){
  const el = document.getElementById('view-ota');
  el.innerHTML = `<div class=card><h3>OTA Upload</h3><p>Firmware OTA endpoint will be added in the next push. For now, flash via USB or Web Flasher.</p></div>`;
}
