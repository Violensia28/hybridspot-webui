#include <Arduino.h>
#include "ssr_logger.h"

String SSRLog::dumpCSV() {
    String csv;
    csv.reserve(this->count() * 64 + 64);
    csv = F("ts_ms,preset,mode,pre_ms,pause_ms,main_ms,i_start,i_post,v_start,v_post\n");

    const size_t cnt = this->count();
    for (size_t i = 0; i < cnt; ++i) {
        SSRPulseRec r;
        this->get(i, r);

        csv += String(r.t0_ms);                  csv += ',';
        csv += String(r.preset_idx);             csv += ',';
        csv += (r.dual ? F("dual") : F("single")); csv += ',';
        csv += String(r.pre_ms);                 csv += ',';
        csv += String(r.pause_ms);               csv += ',';
        csv += String(r.main_ms);                csv += ',';
        csv += String(r.i_start_mA, 1);          csv += ',';
        csv += String(r.i_post_mA, 1);           csv += ',';
        csv += String(r.v_start_V, 2);           csv += ',';
        csv += String(r.v_post_V, 2);
        csv += '\n';
    }
    return csv;
}
