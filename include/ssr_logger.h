#pragma once
#include <Arduino.h>
#ifndef SSR_LOG_CAP
#define SSR_LOG_CAP 64
#endif
struct SSRPulseRec{ uint32_t t0_ms; uint8_t preset_idx; bool dual; uint16_t pre_ms,pause_ms,main_ms; float i_start_mA,v_start_V,i_post_mA,v_post_V; };
namespace SSRLog{ void begin(); void clear(); bool append(const SSRPulseRec& r); size_t size(); bool get(size_t idx,SSRPulseRec& out); String dumpCSV(); }
