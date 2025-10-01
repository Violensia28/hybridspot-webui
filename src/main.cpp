#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

#ifndef DEFAULT_PIN_SSR
#define DEFAULT_PIN_SSR 26
#endif
#ifndef AP_ONLY
#define AP_ONLY 1
#endif
#ifndef WEB_BEEP_ENABLE
#define WEB_BEEP_ENABLE 1
#endif
#ifndef SIM_MODE
#define SIM_MODE 1
#endif

WebServer server(80);
Preferences prefs;

struct SensorConfig { String current_type; float mv_per_A; int adc_mid; float v_scale; float phase_deg; };
SensorConfig cfg;

String makeStatusJson(){
  float vrms = 230.0, irms = 0.0; bool welding = false;
  String j = "{\"mode\":\"manual\",\"welding\":"; j += (welding?"true":"false");
  j += ",\"vrms\":" + String(vrms,1) + ",\"irms\":" + String(irms,1) + ",\"guards\":{\"v\":false,\"i\":false,\"mcb\":false}}"; return j;
}

#include "ui_assets.h"

void handleRoot(){ server.send_P(200, "text/html; charset=utf-8", INDEX_HTML); }
void handleStatic(const char* mime, const char* data){ server.send_P(200, mime, data); }
void handleStatus(){ server.send(200, "application/json", makeStatusJson()); }
void handleNotFound(){ server.send(404, "application/json", "{\"error\":\"not found\"}"); }
void handleStart(){ server.send(202, "application/json", "{\"ok\":true}"); }
void handleStop(){ server.send(200, "application/json", "{\"ok\":true}"); }

void handleGetSensor(){
  String j = "{\"current\":{\"type\":\"" + cfg.current_type + "\",\"mV_per_A\":" + String(cfg.mv_per_A,1) + ",\"adc_midpoint\":" + String(cfg.adc_mid) + "},";
  j += "\"voltage\":{\"scale_V_per_ADC\":" + String(cfg.v_scale,5) + ",\"phase_corr_deg\":" + String(cfg.phase_deg,2) + "}}";
  server.send(200, "application/json", j);
}
void handleSetSensor(){
  if(server.hasArg("current")) cfg.current_type = server.arg("current");
  if(server.hasArg("mvA"))    cfg.mv_per_A = server.arg("mvA").toFloat();
  if(server.hasArg("mid"))    cfg.adc_mid = server.arg("mid").toInt();
  if(server.hasArg("vscale")) cfg.v_scale = server.arg("vscale").toFloat();
  if(server.hasArg("phase"))  cfg.phase_deg = server.arg("phase").toFloat();
  prefs.begin("sensor", false);
  prefs.putString("ctype", cfg.current_type);
  prefs.putFloat("mvpA", cfg.mv_per_A);
  prefs.putInt("mid", cfg.adc_mid);
  prefs.putFloat("vscale", cfg.v_scale);
  prefs.putFloat("phase", cfg.phase_deg);
  prefs.end();
  server.send(200, "application/json", "{\"ok\":true}");
}

void handleOTA(){ server.send_P(200, "text/html; charset=utf-8", OTA_HTML); }

void setupRoutes(){
  server.on("/", handleRoot);
  server.on("/css/main.css", []{ handleStatic("text/css", CSS_MAIN); });
  server.on("/css/theme.css", []{ handleStatic("text/css", CSS_THEME); });
  server.on("/js/app.js", []{ handleStatic("application/javascript", JS_APP); });
  server.on("/js/api.js", []{ handleStatic("application/javascript", JS_API); });
  server.on("/js/audio.js", []{ handleStatic("application/javascript", JS_AUDIO); });
  server.on("/js/ui.js", []{ handleStatic("application/javascript", JS_UI); });
  server.on("/js/preset.js", []{ handleStatic("application/javascript", JS_PRESET); });
  server.on("/js/manual.js", []{ handleStatic("application/javascript", JS_MANUAL); });
  server.on("/js/status.js", []{ handleStatic("application/javascript", JS_STATUS); });
  server.on("/js/logs.js", []{ handleStatic("application/javascript", JS_LOGS); });
  server.on("/js/settings.js", []{ handleStatic("application/javascript", JS_SETTINGS); });
  server.on("/js/ota.js", []{ handleStatic("application/javascript", JS_OTA); });
  server.on("/assets/logo.svg", []{ handleStatic("image/svg+xml", SVG_LOGO); });
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/weld/start", HTTP_POST, handleStart);
  server.on("/api/weld/stop", HTTP_POST, handleStop);
  server.on("/api/sensor/config", HTTP_GET, handleGetSensor);
  server.on("/api/sensor/config", HTTP_POST, handleSetSensor);
  server.on("/ota", HTTP_GET, handleOTA);
  server.onNotFound(handleNotFound);
}

void setup(){
  Serial.begin(115200);
  pinMode(DEFAULT_PIN_SSR, OUTPUT); digitalWrite(DEFAULT_PIN_SSR, LOW);
  cfg.current_type = "ACS712_30A"; cfg.mv_per_A=66.0f; cfg.adc_mid=2048; cfg.v_scale=0.122f; cfg.phase_deg=2.0f;
  prefs.begin("sensor", true);
  cfg.current_type = prefs.getString("ctype", cfg.current_type);
  cfg.mv_per_A     = prefs.getFloat("mvpA", cfg.mv_per_A);
  cfg.adc_mid      = prefs.getInt("mid", cfg.adc_mid);
  cfg.v_scale      = prefs.getFloat("vscale", cfg.v_scale);
  cfg.phase_deg    = prefs.getFloat("phase", cfg.phase_deg);
  prefs.end();
  WiFi.mode(WIFI_AP); WiFi.softAP("SPOTWELD+", "weld12345");
  setupRoutes(); server.begin();
}

void loop(){ server.handleClient(); }
