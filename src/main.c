#define _POSIX_C_SOURCE 199309L 

#include <stdio.h>
#include <time.h>

#include "ecu.h"
#include "can.h"


void log_telemetry_csv(FILE *file, const ECUState *s, uint32_t timestamp_ms) {
    fprintf(file, "%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n",
            timestamp_ms,
            s->rpm,
            s->speed,
            s->throttle,
            s->brake_pressure,
            s->gear,
            s->oil_pressure,
            s->engine_temp,
            s->battery_voltage,
            s->engine_on);
}

void startFile(FILE *f) {
    fprintf(f, "Timestamp,RPM,Speed,Throttle,BrakePressure,Gear,OilPressure,EngineTemp,BatteryVoltage,EngineOn\n");
}

void sleep_ms(int ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

int main() {
    uint32_t timestamp = 0;

    ECUState ecu_tx = {0};
    ECUState ecu_rx = {0};
    CANFrame frame;
    CANBus bus = {0};

    ecu_tx.engine_on = 1;

    FILE *csv = fopen("telemetry_log.csv", "w");
    if (csv == NULL) {
        fprintf(stderr, "Erro ao criar CSV!\n");
        return 1;
    }
    startFile(csv);
    
    

    while (1) {
        // Brake in 1s
        if (timestamp % 1000 == 0 && timestamp > 0) {
            ecu_tx.brake = !ecu_tx.brake;
        }

        simulate_sensors(&ecu_tx);
        simulate_diagnostics(&ecu_tx);

        encode_powertrain_frame(&ecu_tx, &frame);
        can_bus_send(&bus, &frame);

        encode_dynamics_frame(&ecu_tx, &frame);
        can_bus_send(&bus, &frame);

        encode_sensors_frame(&ecu_tx, &frame);
        can_bus_send(&bus, &frame);

        // Ordenation
        can_bus_arbitrate(&bus);
        can_bus_deliver(&bus, &ecu_rx);
        
        print_ecu_state(&ecu_rx);
        log_telemetry_csv(csv, &ecu_rx, timestamp);

        bus.count = 0;
        
        timestamp += 10;
        sleep_ms(10);
    }
    
    return 0;
}
