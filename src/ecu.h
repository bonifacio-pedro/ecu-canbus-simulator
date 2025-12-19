#ifndef ECU_MAN_H 
#define ECU_MAN_H

#include <stdint.h>

#define MIN_CURVE_SPEED         20 
#define BRAKE_CURVE_FACTOR      0.3f 
#define BRAKE_RELEASE_DELAY     10  

typedef enum {
    ECU_ERR_NONE          = 0x00,
    ECU_ERR_CAN_CHECKSUM  = 0x01,
    ECU_ERR_CAN_SEQ       = 0x02,
    ECU_ERR_CAN_TIMEOUT   = 0x04,
    ECU_ERR_OVER_RPM      = 0x08,
    ECU_ERR_OVER_TEMP     = 0x10,
} ECUErrorCode;

typedef struct {
    // POWERTRAINS
    uint16_t rpm;
    uint8_t gear;
    uint8_t engine_on;
    uint8_t brake;
    

    // DYNAMICS
    uint8_t speed;
    uint8_t brake_pressure;
    uint8_t throttle;


    // SENSORS
    uint8_t engine_temp;      // 0-255°C
    uint8_t oil_pressure;     // 0-100 bar
    uint8_t battery_voltage;  // 0-255 (12.8V = 128)
    uint8_t error_code;  

    uint16_t rsv;
} ECUState;

void simulate_sensors(ECUState *s);
void simulate_diagnostics(ECUState *s);
void print_ecu_state(const ECUState *s);

#endif