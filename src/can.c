#include <stdio.h>
#include "can.h"
#include "ecu.h"

// TO EACH FRAME, ONE SEQUENCE REGISTRY
static uint8_t seq_powertrain = 0;
static uint8_t seq_sensors    = 0;
static uint8_t seq_dynamics   = 0;

static uint8_t last_seq_powertrain = 0xFF;
static uint8_t last_seq_sensors = 0xFF;
static uint8_t last_seq_dynamics = 0xFF;

uint8_t can_checksum(const uint8_t *data, uint8_t len) {
    uint8_t cs = 0;
    for (uint8_t i = 0; i < len; i++) {
        cs ^= data[i];
    }
    return cs;
}

int vld_checksum(const uint8_t *data, uint8_t len, ECUState *s) {
    uint8_t expected_cs = can_checksum(data, len);
    uint8_t received_cs = data[len];

    if (expected_cs != received_cs) {
        s->error_code |= ECU_ERR_CAN_CHECKSUM;
        return 0;
    }
    return 1;
}

int vld_sequence(uint8_t received, uint8_t *expected, ECUState *s) {
    uint8_t next = (*expected + 1) & 0xFF;

    if (received != next) {
        s->error_code |= ECU_ERR_CAN_SEQ;
        return 0;
    }

    *expected = received;
    return 1;
}


void can_bus_send(CANBus *bus, const CANFrame *f) {
    if (bus->count < CAN_BUS_MAX_FRAMES) {
        bus->frames[bus->count] = *f;
        bus->count++;
    }
}

void can_bus_arbitrate(CANBus *bus) {
     for (int i = 0; i < bus->count - 1; i++) {
        for (int j = i + 1; j < bus->count; j++) {
            if (bus->frames[j].id < bus->frames[i].id) {
                CANFrame tmp = bus->frames[i];
                bus->frames[i] = bus->frames[j];
                bus->frames[j] = tmp;
            }
        }
    }
}

void can_bus_deliver(const CANBus *bus, ECUState *ecu) {
    for (int i = 0; i < bus->count; i++) {
        const CANFrame *frame = &bus->frames[i];
        
        switch (frame->id) {
            case CAN_ID_POWERTRAIN:
                decode_powertrain_frame(frame, ecu);
                break;
            case CAN_ID_DYNAMICS:
                decode_dynamics_frame(frame, ecu);
                break;
            case CAN_ID_SENSORS:
                decode_sensors_frame(frame, ecu);
                break;
            default:
                break;
        }
    }
}

void encode_powertrain_frame(const ECUState *s, CANFrame *f) {
    f->id = CAN_ID_POWERTRAIN;
    f->dlc = CAN_DLC_POWERTRAIN;
    f->data[0] = (s->rpm >> CAN_RPM_SHIFT_MSB) & CAN_BYTE_MASK;
    f->data[1] = s->rpm & CAN_BYTE_MASK;
    f->data[2] = s->gear;
    f->data[3] =
        (s->engine_on << 1) |
        (s->brake << 0);

    // SEQUENCE COUNTER IDX
    f->data[4] = seq_powertrain++;

    // CHECKSUM
    f->data[5] = can_checksum(f->data, CAN_DLC_POWERTRAIN-1);
}

void encode_sensors_frame(const ECUState *s, CANFrame *f) {
    f->id = CAN_ID_SENSORS;
    f->dlc = CAN_DLC_SENSORS;

    f->data[0] = s->engine_temp;
    f->data[1] = s->oil_pressure;
    f->data[2] = s->battery_voltage;
    f->data[3] = s->error_code;
    f->data[4] = 0;
    f->data[5] = seq_sensors++;

    // CHECKSUM
    f->data[6] = can_checksum(f->data, CAN_DLC_SENSORS-1);
}

void encode_dynamics_frame(const ECUState *s, CANFrame *f) {
    f->id = CAN_ID_DYNAMICS;
    f->dlc = CAN_DLC_DYNAMICS;

    f->data[0] = s->speed;
    f->data[1] = s->brake_pressure;
    f->data[2] = s->throttle;
    f->data[3] = 0;
    f->data[4] = seq_dynamics++;

    // CHECKSUM
    f->data[5] = can_checksum(f->data, CAN_DLC_DYNAMICS-1);
}

int decode_dynamics_frame(const CANFrame *f, ECUState *s) {
    if (!vld_checksum(f->data, f->dlc-1, s))
        return -1;

    if (!vld_sequence(f->data[4], &last_seq_dynamics, s))
        return -1;


    s->speed = f->data[0];
    s->brake_pressure = f->data[1];
    s->throttle = f->data[2];

    return 0;
};


int decode_sensors_frame(const CANFrame *f, ECUState *s) {
    if (!vld_checksum(f->data, f->dlc-1, s))
        return -1;

    if (!vld_sequence(f->data[5], &last_seq_sensors, s))
        return -1;


    s->engine_temp = f->data[0];
    s->oil_pressure = f->data[1];
    s->battery_voltage = f->data[2];
    s->error_code = f->data[3];

    return 0;
}

int decode_powertrain_frame(const CANFrame *f, ECUState *s) {
    if (!vld_checksum(f->data, f->dlc-1, s))
        return -1;

    if (!vld_sequence(f->data[4], &last_seq_powertrain, s))
        return -1;


    s->rpm = (f->data[0] << 8) | f->data[1];
    s->gear = f->data[2];
    s->engine_on = (f->data[3] & CAN_ENGINE_ON_BIT) != 0;
    s->brake     = (f->data[3] & CAN_BRAKE_BIT) != 0;

    return 0;
}