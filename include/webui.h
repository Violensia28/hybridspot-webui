#pragma once
#include <Arduino.h>
#include <functional>
namespace WebUI{
  enum class FsmState:uint8_t{IDLE,PRE,PAUSE,MAIN,DONE};
  struct Status{ FsmState fsm=FsmState::IDLE; float i_mA=0.0f; float v_rms=0.0f; uint8_t preset=1; bool dual=false; uint16_t pre=0,pause=0,main=60; };
  struct BuildInfo{ const char* name; const char* ver; const char* git; };
  struct Calib{ float acs_mva=66.0f; float acs_div=1.0f; float zmpt_vps=0.08f; float hpf=0.995f; float lpf=0.20f; };
  void begin(); void loop(); void setBuild(const BuildInfo&); void setStatus(const Status&); void setCalib(const Calib&);
  void onSelectPreset(std::function<void(uint8_t)>); void onWeldStart(std::function<void()>); void onWeldStop(std::function<void()>);
  void onCalibSave(std::function<void(const Calib&)>); void onCalibReset(std::function<void()>);
  void onLogsClear(std::function<void()>); void onLogsDump(std::function<String()>);
}
