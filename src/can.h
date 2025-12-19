#ifndef CAN_MAN_H 
#define CAN_MAN_H

// == CAN BUS
#define CAN_BUS_MAX_FRAMES      3

// == BIT OPERATIONS
#define CAN_RPM_SHIFT_MSB       8
#define CAN_BYTE_MASK           0xFF

// == MASKS
#define CAN_ENGINE_ON_BIT       (1 << 1)
#define CAN_BRAKE_BIT           (1 << 0)

// == IDS
#define CAN_ID_POWERTRAIN       0x100
#define CAN_ID_DYNAMICS         0x200
#define CAN_ID_SENSORS          0x300

// == DLC
#define CAN_DLC_POWERTRAIN 6
#define CAN_DLC_SENSORS    7
#define CAN_DLC_DYNAMICS   6

#include <stdint.h> 
#include "ecu.h"

typedef struct {
    uint16_t id;     // 11-bit standard CAN ID
    uint8_t  dlc;    // 0–8 bytes
    uint8_t  data[8];
} CANFrame;

typedef struct {
    CANFrame frames[CAN_BUS_MAX_FRAMES];
    uint8_t  count;
} CANBus;

uint8_t can_checksum(const uint8_t *data, uint8_t len);

void can_bus_send(CANBus *bus, const CANFrame *frame);
void can_bus_arbitrate(CANBus *bus);
void can_bus_deliver(const CANBus *bus, ECUState *ecu);

void encode_powertrain_frame(const ECUState *s, CANFrame *f);
int decode_powertrain_frame(const CANFrame *f, ECUState *s);
void encode_sensors_frame(const ECUState *s, CANFrame *f);
int decode_sensors_frame(const CANFrame *f, ECUState *s);
void encode_dynamics_frame(const ECUState *s, CANFrame *f);
int decode_dynamics_frame(const CANFrame *f, ECUState *s);


#endif