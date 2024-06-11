#include "gcode_info_scan.hpp"

#include "gcode_info.hpp"
#include <common/async_job/async_job.hpp>

static AsyncJob gcode_scan_async_job;

static std::atomic<gcode_info_scan::ScanStartResult> scan_start_result_ = gcode_info_scan::ScanStartResult::not_started;

LOG_COMPONENT_REF(MarlinServer);

static void gcode_info_scan_callback(AsyncJobExecutionControl &control) {
    auto &gcode_info = GCodeInfo::getInstance();

    gcode_info.reset_info();

    AnyGcodeFormatReader file_reader(gcode_info.GetGcodeFilepath());
    if (!file_reader.is_open()) {
        log_error(MarlinServer, "Media prefetch GCodeInfo: fail to open");
        gcode_info.set_error(N_("Failed to open file"));
        control.with_synchronized([] {
            scan_start_result_ = gcode_info_scan::ScanStartResult::failed;
        });
        return;
    }

    control.with_synchronized([] {
        scan_start_result_ = gcode_info_scan::ScanStartResult::started;
    });

    // Wait for gcode to be valid
    while (!gcode_info.check_valid_for_print(file_reader)) {
        if (gcode_info.has_error()) {
            log_error(MarlinServer, "Media prefetch GCodeInfo: not valid: %s", gcode_info.error_str());
            return;
        }

        osDelay(500);

        if (control.is_discarded()) {
            return;
        }
    }

    log_info(MarlinServer, "Media prefetch GCodeInfo: loading");
    gcode_info.load(*file_reader);
}

namespace gcode_info_scan {

ScanStartResult scan_start_result() {
    return scan_start_result_.load();
}

void start_scan() {
    scan_start_result_ = ScanStartResult::not_started;
    gcode_scan_async_job.issue(&gcode_info_scan_callback);
}

void cancel_scan() {
    gcode_scan_async_job.discard();
}

} // namespace gcode_info_scan
