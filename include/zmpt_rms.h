#pragma once
#include <Arduino.h>
class ZmptRms{public: bool begin(uint8_t pin,float vps,uint32_t fs=2000,uint16_t win=20); void setVPerStep(float v){v_per_step_=v;} void setHPFAlpha(float a){a_hpf_=constrain(a,0.0f,0.9999f);} void setLPFAlpha(float a){a_lpf_=constrain(a,0.0f,1.0f);} void tick(); float vrms()const{return vrms_lpf_;} uint16_t windowSize()const{return N_;}
private: uint8_t pin_=35; float v_per_step_=0.08f; uint32_t fs_=2000; uint16_t win_ms_=20; uint32_t Ts_us_=500,next_us_=0; static const uint16_t MAXN=2048; int32_t ring_[MAXN]; uint16_t N_=0,head_=0; bool filled_=false; double sum_=0,sum2_=0; float a_hpf_=0.995f; int32_t x_prev_=0,y_prev_=0; float a_lpf_=0.20f, vrms_lpf_=0.0f; void push(int32_t y); };
