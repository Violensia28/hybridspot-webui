#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <RotaryEncoder.h>
#include <Bounce2.h>
#include <ACS712.h>
#include <stdarg.h>
#include "pinmap.h"
#include "settings_store.h"
#include "BuildInfo.h"
#include "zmpt_rms.h"
#include "weld_fsm.h"
#include "ssr_logger.h"
#include "webui.h"
#if defined(USE_OLED)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
#endif
#if defined(USE_ENCODER)
RotaryEncoder encoder(PIN_ENC_A, PIN_ENC_B, RotaryEncoder::LatchMode::FOUR3);
Bounce2::Button encBtn;
#endif
#if defined(USE_ACS712)
ACS712 acs(PIN_ACS, 3.3f, 4095, 66.0f);
#endif
ZmptRms zmpt; WeldFSM fsm; SettingsStore::Settings gSettings; static char lbuf[160];
static inline void logf(const char* fmt,...){ va_list ap; va_start(ap,fmt); vsnprintf(lbuf,sizeof(lbuf),fmt,ap); va_end(ap); Serial.println(lbuf);} 
static inline void buzz(uint16_t ms=40){ pinMode(PIN_BUZZ,OUTPUT); digitalWrite(PIN_BUZZ,HIGH); delay(ms); digitalWrite(PIN_BUZZ,LOW);} 
struct Pres{uint16_t pre_ms,pause_ms,main_ms; bool dual;}; static const Pres PRESETS[]={ {0,0,60,false},{20,100,70,true},{30,120,80,true},{0,0,90,false},{40,150,100,true}}; static uint8_t pcount(){return sizeof(PRESETS)/sizeof(PRESETS[0]);}
static float gI=0,gV=0,gI0=0,gV0=0; static bool gActive=false; static void SSR(bool on){ digitalWrite(PIN_SSR,on?HIGH:LOW);} static void FLOG(const char* e){logf("[FSM] %s",e);} 
static void draw(){#if defined(USE_OLED) u8g2.clearBuffer(); u8g2.setFont(u8g2_font_6x12_tf); u8g2.drawStr(0,10,APP_NAME); char ln[48]; u8g2.setFont(u8g2_font_7x13B_tf); snprintf(ln,sizeof(ln),"I: %.1f mA",gI); u8g2.drawStr(0,28,ln); snprintf(ln,sizeof(ln),"V: %.1f V",gV); u8g2.drawStr(0,44,ln); auto&p=PRESETS[gSettings.preset_idx-1]; snprintf(ln,sizeof(ln),"Preset %u: %s %u/%u/%u",gSettings.preset_idx,p.dual?"x2":"x1",p.pre_ms,p.pause_ms,p.main_ms); u8g2.drawStr(0,60,ln); u8g2.sendBuffer(); #endif }
static void btn(){#if defined(USE_ENCODER) encBtn.update(); static bool d=false; static uint32_t t0=0; if(encBtn.pressed()){d=true;t0=millis();} if(encBtn.released()){uint32_t h=millis()-t0; d=false; if(h<=600){ if(fsm.isIdle()){ auto&px=PRESETS[gSettings.preset_idx-1]; WeldPreset p{px.pre_ms,px.pause_ms,px.main_ms,px.dual}; gI0=gI; gV0=gV; gActive=true; fsm.start(p);} else { fsm.abort(); } } } #endif }
static void enc(){#if defined(USE_ENCODER) static long lst=0; encoder.tick(); long pos=encoder.getPosition(); long d=pos-lst; if(!d) return; lst=pos; int idx=(int)gSettings.preset_idx+(d>0?1:-1); if(idx<1)idx=1; if(idx>(int)pcount()) idx=pcount(); if(idx!=gSettings.preset_idx){ gSettings.preset_idx=(uint8_t)idx; SettingsStore::save(gSettings); buzz(20);} #endif }
static String dumpCSV(){ return SSRLog::dumpCSV(); }
void setup(){ Serial.begin(115200); delay(150); #if defined(USE_OLED) Wire.begin(PIN_I2C_SDA,PIN_I2C_SCL); u8g2.begin(); #endif pinMode(PIN_SSR,OUTPUT); digitalWrite(PIN_SSR,LOW); #if defined(USE_ENCODER) pinMode(PIN_ENC_A,INPUT_PULLUP); pinMode(PIN_ENC_B,INPUT_PULLUP); encoder.setPosition(0); encBtn.attach(PIN_ENC_BTN,INPUT_PULLUP); encBtn.interval(5); encBtn.setPressedState(LOW); #endif SettingsStore::begin(); SettingsStore::load(gSettings); #if defined(USE_ACS712) acs.setmVperAmp(gSettings.acs_mV_per_A*gSettings.acs_divider); (void)acs.autoMidPointDC(100); #endif zmpt.begin(PIN_ZMPT,gSettings.zmpt_v_per_step,2000,20); zmpt.setHPFAlpha(0.995f); zmpt.setLPFAlpha(0.20f); fsm.begin(SSR,FLOG); SSRLog::begin(); WiFi.mode(WIFI_AP); WiFi.softAP("HYBRIDSPOT","hybridspot"); logf("AP IP: %s", WiFi.softAPIP().toString().c_str()); WebUI::begin(); WebUI::setBuild({APP_NAME,APP_VERSION,(String(GIT_REF)+"/"+String(GIT_SHA)).c_str()}); WebUI::onSelectPreset([](uint8_t idx){ if(idx<1)idx=1; if(idx>pcount()) idx=pcount(); gSettings.preset_idx=idx; SettingsStore::save(gSettings); }); WebUI::onWeldStart([](){ if(fsm.isIdle()){ auto&px=PRESETS[gSettings.preset_idx-1]; WeldPreset p{px.pre_ms,px.pause_ms,px.main_ms,px.dual}; gI0=gI; gV0=gV; gActive=true; fsm.start(p);} }); WebUI::onWeldStop([](){ fsm.abort(); gActive=false; }); WebUI::onCalibSave([](const WebUI::Calib& c){ gSettings.acs_mV_per_A=c.acs_mva; gSettings.acs_divider=c.acs_div; gSettings.zmpt_v_per_step=c.zmpt_vps; #if defined(USE_ACS712) acs.setmVperAmp(gSettings.acs_mV_per_A*gSettings.acs_divider); #endif zmpt.setVPerStep(gSettings.zmpt_v_per_step); SettingsStore::save(gSettings); }); WebUI::onCalibReset([](){ SettingsStore::resetDefaults(gSettings); #if defined(USE_ACS712) acs.setmVperAmp(gSettings.acs_mV_per_A*gSettings.acs_divider); #endif zmpt.setVPerStep(gSettings.zmpt_v_per_step); }); WebUI::onLogsClear([](){ SSRLog::clear(); }); WebUI::onLogsDump([](){ return dumpCSV(); }); logf("BOOT %s %s | preset=%u", APP_NAME, APP_VERSION, gSettings.preset_idx); buzz(60);} 
void loop(){ zmpt.tick(); static uint32_t tACS=0; if(millis()-tACS>=5){ tACS=millis(); #if defined(USE_ACS712) gI=acs.mA_DC(1); #else gI=0; #endif } gV=zmpt.vrms(); fsm.tick(); if(fsm.isDone() && gActive){ auto&px=PRESETS[gSettings.preset_idx-1]; SSRPulseRec r{}; r.t0_ms=millis(); r.preset_idx=gSettings.preset_idx; r.dual=px.dual; r.pre_ms=px.pre_ms; r.pause_ms=px.pause_ms; r.main_ms=px.main_ms; r.i_start_mA=gI0; r.v_start_V=gV0; delay(5); r.i_post_mA=gI; r.v_post_V=gV; SSRLog::append(r); buzz(40); gActive=false; fsm.abort(); } enc(); btn(); static uint32_t tUI=0; if(millis()-tUI>=200){ tUI=millis(); draw(); } WebUI::Status s{}; auto st=fsm.state(); s.fsm = st==WeldFSM::IDLE? WebUI::FsmState::IDLE : (st==WeldFSM::PRE?WebUI::FsmState::PRE:(st==WeldFSM::PAUSE?WebUI::FsmState::PAUSE:(st==WeldFSM::MAIN?WebUI::FsmState::MAIN:WebUI::FsmState::DONE))); s.i_mA=gI; s.v_rms=gV; s.preset=gSettings.preset_idx; auto&p=PRESETS[gSettings.preset_idx-1]; s.dual=p.dual; s.pre=p.pre_ms; s.pause=p.pause_ms; s.main=p.main_ms; WebUI::setStatus(s); WebUI::Calib c{}; c.acs_mva=gSettings.acs_mV_per_A; c.acs_div=gSettings.acs_divider; c.zmpt_vps=gSettings.zmpt_v_per_step; c.hpf=0.995f; c.lpf=0.20f; WebUI::setCalib(c); WebUI::loop(); }
