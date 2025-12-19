#include <stdio.h>
#include "can.h"
#include "ecu.h"

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

    // RPM -> BIG-END
    f->data[0] = (s->rpm >> CAN_RPM_SHIFT_MSB) & CAN_BYTE_MASK;
    f->data[1] = s->rpm & CAN_BYTE_MASK;

    f->data[2] = s->gear;

    // 00 00 00 01
    // 00 00 00 10
    // 00 00 00 11
    //          xx
    f->data[3] =
        (s->engine_on << 1) |
        (s->brake << 0);
}

void encode_sensors_frame(const ECUState *s, CANFrame *f) {
    f->id = CAN_ID_SENSORS;
    f->dlc = CAN_DLC_SENSORS;

    f->data[0] = s->engine_temp;
    f->data[1] = s->oil_pressure;
    f->data[2] = s->battery_voltage;
    f->data[3] = s->error_code;
    f->data[4] = 0;
}

void encode_dynamics_frame(const ECUState *s, CANFrame *f) {
    f->id = CAN_ID_DYNAMICS;
    f->dlc = CAN_DLC_DYNAMICS;

    f->data[0] = s->speed;
    f->data[1] = s->brake_pressure;
    f->data[2] = s->throttle;
    f->data[3] = 0;
}

void decode_dynamics_frame(const CANFrame *f, ECUState *s) {
    s->speed = f->data[0];
    s->brake_pressure = f->data[1];
    s->throttle = f->data[2];
};


void decode_sensors_frame(const CANFrame *f, ECUState *s) {
    s->engine_temp = f->data[0];
    s->oil_pressure = f->data[1];
    s->battery_voltage = f->data[2];
    s->error_code = f->data[3];
}

void decode_powertrain_frame(const CANFrame *f, ECUState *s) {
    s->rpm = (f->data[0] << 8) | f->data[1];
    s->gear = f->data[2];
    s->engine_on = (f->data[3] & CAN_ENGINE_ON_BIT) != 0;
    s->brake     = (f->data[3] & CAN_BRAKE_BIT) != 0;
}