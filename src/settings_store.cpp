#include "settings_store.h"
#include <Preferences.h>
namespace{ Preferences prefs; const char* K1="acs_mva"; const char* K2="acs_div"; const char* K3="zmpt_vps"; const char* K4="preset"; }
namespace SettingsStore{
bool begin(const char* ns){ return prefs.begin(ns,false);} 
bool load(Settings& s){ if(!prefs.isKey(K1)) return save(s); s.acs_mV_per_A=prefs.getFloat(K1,s.acs_mV_per_A); s.acs_divider=prefs.getFloat(K2,s.acs_divider); s.zmpt_v_per_step=prefs.getFloat(K3,s.zmpt_v_per_step); s.preset_idx=prefs.getUChar(K4,s.preset_idx); return true; }
bool save(const Settings& s){ prefs.putFloat(K1,s.acs_mV_per_A); prefs.putFloat(K2,s.acs_divider); prefs.putFloat(K3,s.zmpt_v_per_step); prefs.putUChar(K4,s.preset_idx); return true; }
bool resetDefaults(Settings& s){ s=Settings(); return save(s);} }
