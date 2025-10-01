#pragma once
#include <Arduino.h>
struct WeldPreset{ uint16_t pre_ms; uint16_t pause_ms; uint16_t main_ms; bool dual; };
class WeldFSM{public: enum State:uint8_t{IDLE,PRE,PAUSE,MAIN,DONE}; using FnSSR=void(*)(bool); using FnLog=void(*)(const char*);
void begin(FnSSR s, FnLog l=nullptr){ssr_=s; log_=l; st_=IDLE;} bool start(const WeldPreset&p){ if(st_!=IDLE) return false; pr_=p; t0_=millis(); if(pr_.dual && pr_.pre_ms>0){ ssr_(true); tn_=t0_+pr_.pre_ms; st_=PRE; if(log_)log_("PRE_START"); } else { ssr_(true); tn_=t0_+pr_.main_ms; st_=MAIN; if(log_)log_("MAIN_START"); } return true; }
void abort(){ if(ssr_) ssr_(false); st_=IDLE;} void tick(){ uint32_t now=millis(); switch(st_){ case IDLE: break; case PRE: if((int32_t)(now-tn_)>=0){ ssr_(false); if(log_)log_("PRE_END"); tn_=now+pr_.pause_ms; st_=PAUSE;} break; case PAUSE: if((int32_t)(now-tn_)>=0){ ssr_(true); if(log_)log_("MAIN_START"); tn_=now+pr_.main_ms; st_=MAIN;} break; case MAIN: if((int32_t)(now-tn_)>=0){ ssr_(false); st_=DONE; if(log_)log_("MAIN_END"); } break; case DONE: break; }}
bool isIdle()const{return st_==IDLE;} bool isDone()const{return st_==DONE;} State state()const{return st_;}
private: FnSSR ssr_=nullptr; FnLog log_=nullptr; WeldPreset pr_{}; State st_=IDLE; uint32_t t0_=0, tn_=0; };
