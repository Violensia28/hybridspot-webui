import { initTabs } from '/js/ui.js';
import { getStatus } from '/js/api.js';
import { initStatus } from '/js/status.js';
import { initPreset } from '/js/preset.js';
import { initManual } from '/js/manual.js';
import { initSettings } from '/js/settings.js';
import { initLogs } from '/js/logs.js';
import { initOTA } from '/js/ota.js';

window.addEventListener('DOMContentLoaded', async ()=>{
  initTabs();
  initStatus();
  initPreset();
  initManual();
  initSettings();
  initLogs();
  initOTA();
  // default open status
  document.querySelector('nav#tabs button[data-tab="status"]').click();
});
