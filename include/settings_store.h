#pragma once
#include <Arduino.h>
namespace SettingsStore{
struct Settings{ float acs_mV_per_A=66.0f; float acs_divider=1.0f; float zmpt_v_per_step=0.080f; uint8_t preset_idx=1; }; 
bool begin(const char* ns="hybridspot"); bool load(Settings& s); bool save(const Settings& s); bool resetDefaults(Settings& s);
}
