/**
 * @file filament_sensors_handler_no_side_sensor.cpp
 * @brief this file contains dummy side sensor code for printers without it
 */

#include "filament_sensors_handler.hpp"
#include "filament_sensor_types.hpp"
#include <cassert>

// function returning abstract sensor - used in higher level api
IFSensor *GetSideFSensor([[maybe_unused]] uint8_t index) {
    return nullptr;
}

void FilamentSensors::AdcSide_FilteredIRQ([[maybe_unused]] int32_t val, [[maybe_unused]] uint8_t tool_index) {
    bsod("no adc sensor");
}

void side_fs_process_sample([[maybe_unused]] int32_t fs_raw_value, [[maybe_unused]] uint8_t tool_index) {
    bsod("no adc sensor");
}
