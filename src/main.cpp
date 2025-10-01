#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

#ifndef DEFAULT_PIN_SSR
#define DEFAULT_PIN_SSR 26
#endif

// Build flags
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

struct SensorConfig {
  String current_type; // e.g., "ACS712_30A"
  float mv_per_A;      // 66.0 for 30A, 100.0 for 20A, 185.0 for 5A
  int adc_mid;         // midpoint, e.g., 2048
  float v_scale;       // ZMPT scale V per ADC unit
  float phase_deg;
};

SensorConfig cfg;

String jsonEscape(const String &s){
  String o; o.reserve(s.length()+8);
  for(char c: s){
    if(c=='"' || c=='\\') { o += '\\'; o += c; }
    else if(c=='\n') o += "\\n";
    else o += c;
  }
  return o;
}

String makeStatusJson(){
  // Stub values for Build #1
  float vrms = 230.0;
  float irms = 0.0;
  bool welding = false;
  String j = "{\"mode\":\"manual\",\"welding\":";
  j += (welding?"true":"false");
  j += ",\"vrms\":" + String(vrms,1) + ",\"irms\":" + String(irms,1) + ",\"guards\":{\"v\":false,\"i\":false,\"mcb\":false}}";
  return j;
}

void handleRoot(){
  #include "ui_assets.h"
  server.send_P(200, "text/html; charset=utf-8", INDEX_HTML);
}

void handleStatic(const char* mime, const char* data){
  server.send_P(200, mime, data);
}

void handleStatus(){
  String j = makeStatusJson();
  server.send(200, "application/json", j);
}

void handleNotFound(){
  server.send(404, "application/json", "{\"error\":\"not found\"}");
}

void handleStart(){
  // Build #1: stub acknowledge
  server.send(202, "application/json", "{\"ok\":true}");
}
void handleStop(){ server.send(200, "application/json", "{\"ok\":true}"); }

void handleGetSensor(){
  String j = "{\"current\":{\"type\":\"" + cfg.current_type + "\",\"mV_per_A\":" + String(cfg.mv_per_A,1) + ",\"adc_midpoint\":" + String(cfg.adc_mid) + "},";
  j += "\"voltage\":{\"scale_V_per_ADC\":" + String(cfg.v_scale,5) + ",\"phase_corr_deg\":" + String(cfg.phase_deg,2) + "}}";
  server.send(200, "application/json", j);
}

void handleSetSensor(){
  // Very simple parser: expects query params for Build #1 to avoid JSON lib
  // e.g.: /api/sensor/config?current=ACS712_30A&mvA=66&mid=2048&vscale=0.122&phase=2
  if(server.hasArg("current")) cfg.current_type = server.arg("current");
  if(server.hasArg("mvA"))    cfg.mv_per_A = server.arg("mvA").toFloat();
  if(server.hasArg("mid"))    cfg.adc_mid = server.arg("mid").toInt();
  if(server.hasArg("vscale")) cfg.v_scale = server.arg("vscale").toFloat();
  if(server.hasArg("phase"))  cfg.phase_deg = server.arg("phase").toFloat();
  // persist
  prefs.begin("sensor", false);
  prefs.putString("ctype", cfg.current_type);
  prefs.putFloat("mvpA", cfg.mv_per_A);
  prefs.putInt("mid", cfg.adc_mid);
  prefs.putFloat("vscale", cfg.v_scale);
  prefs.putFloat("phase", cfg.phase_deg);
  prefs.end();
  server.send(200, "application/json", "{\"ok\":true}");
}

void handleOTA(){
  // Placeholder for OTA upload page content
  #include "ui_assets.h"
  server.send_P(200, "text/html; charset=utf-8", OTA_HTML);
}

void setupRoutes(){
  // UI
  server.on("/", handleRoot);
  server.on("/css/main.css", []{ #include "ui_assets.h" handleStatic("text/css", CSS_MAIN); });
  server.on("/css/theme.css", []{ #include "ui_assets.h" handleStatic("text/css", CSS_THEME); });
  server.on("/js/app.js", []{ #include "ui_assets.h" handleStatic("application/javascript", JS_APP); });
  server.on("/js/api.js", []{ #include "ui_assets.h" handleStatic("application/javascript", JS_API); });
  server.on("/js/audio.js", []{ #include "ui_assets.h" handleStatic("application/javascript", JS_AUDIO); });
  server.on("/js/ui.js", []{ #include "ui_assets.h" handleStatic("application/javascript", JS_UI); });
  server.on("/js/preset.js", []{ #include "ui_assets.h" handleStatic("application/javascript", JS_PRESET); });
  server.on("/js/manual.js", []{ #include "ui_assets.h" handleStatic("application/javascript", JS_MANUAL); });
  server.on("/js/status.js", []{ #include "ui_assets.h" handleStatic("application/javascript", JS_STATUS); });
  server.on("/js/logs.js", []{ #include "ui_assets.h" handleStatic("application/javascript", JS_LOGS); });
  server.on("/js/settings.js", []{ #include "ui_assets.h" handleStatic("application/javascript", JS_SETTINGS); });
  server.on("/js/ota.js", []{ #include "ui_assets.h" handleStatic("application/javascript", JS_OTA); });
  server.on("/assets/logo.svg", []{ #include "ui_assets.h" handleStatic("image/svg+xml", SVG_LOGO); });

  // API
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/weld/start", HTTP_POST, handleStart);
  server.on("/api/weld/stop", HTTP_POST, handleStop);
  server.on("/api/sensor/config", HTTP_GET, handleGetSensor);
  server.on("/api/sensor/config", HTTP_POST, handleSetSensor);

  // OTA (placeholder page; firmware OTA endpoint can be added next step)
  server.on("/ota", HTTP_GET, handleOTA);

  server.onNotFound(handleNotFound);
}

void setup(){
  Serial.begin(115200);
  pinMode(DEFAULT_PIN_SSR, OUTPUT);
  digitalWrite(DEFAULT_PIN_SSR, LOW);

  // Load sensor defaults
  cfg.current_type = "ACS712_30A";
  cfg.mv_per_A = 66.0f;
  cfg.adc_mid = 2048;
  cfg.v_scale = 0.122f;
  cfg.phase_deg = 2.0f;

  // Load from NVS if exists
  prefs.begin("sensor", true);
  cfg.current_type = prefs.getString("ctype", cfg.current_type);
  cfg.mv_per_A     = prefs.getFloat("mvpA", cfg.mv_per_A);
  cfg.adc_mid      = prefs.getInt("mid", cfg.adc_mid);
  cfg.v_scale      = prefs.getFloat("vscale", cfg.v_scale);
  cfg.phase_deg    = prefs.getFloat("phase", cfg.phase_deg);
  prefs.end();

#if AP_ONLY
  WiFi.mode(WIFI_AP);
  WiFi.softAP("SPOTWELD+", "weld12345");
  Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());
#else
  WiFi.mode(WIFI_AP_STA);
#endif
  setupRoutes();
  server.begin();
}

void loop(){
  server.handleClient();
}
