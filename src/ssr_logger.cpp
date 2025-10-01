#include "ssr_logger.h"
namespace{ SSRPulseRec rb[SSR_LOG_CAP]; size_t head=0,cnt=0; char line[192]; }
void SSRLog::begin(){ head=0; cnt=0; }
void SSRLog::clear(){ head=0; cnt=0; Serial.println("[SSRLOG] cleared"); }
bool SSRLog::append(const SSRPulseRec& r){ rb[head]=r; head=(head+1)%SSR_LOG_CAP; if(cnt<SSR_LOG_CAP) cnt++; snprintf(line,sizeof(line),"[SSRLOG] t0=%lu preset=%u %s %u/%u/%u I=%.1f->%.1f V=%.2f->%.2f",(unsigned long)r.t0_ms,r.preset_idx,r.dual?"dual":"single",r.pre_ms,r.pause_ms,r.main_ms,r.i_start_mA,r.i_post_mA,r.v_start_V,r.v_post_V); Serial.println(line); return true; }
size_t SSRLog::size(){ return cnt; }
bool SSRLog::get(size_t i,SSRPulseRec& out){ if(i>=cnt) return false; size_t st=(head+SSR_LOG_CAP-cnt)%SSR_LOG_CAP; out=rb[(st+i)%SSR_LOG_CAP]; return true; }
String SSRLog::dumpCSV(){ String csv="ts_ms,preset,mode,pre_ms,pause_ms,main_ms,i_start,i_post,v_start,v_post
"; for(size_t i=0;i<cnt;i++){ SSRPulseRec r; get(i,r); csv+=String(r.t0_ms)+","+String(r.preset_idx)+","+(r.dual?"dual":"single")+","+String(r.pre_ms)+","+String(r.pause_ms)+","+String(r.main_ms)+","+String(r.i_start_mA,1)+","+String(r.i_post_mA,1)+","+String(r.v_start_V,2)+","+String(r.v_post_V,2)+"
";} return csv; }
