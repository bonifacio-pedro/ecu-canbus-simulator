# 🚗 CAN Bus ECU Simulator

![CAN Bus](https://img.shields.io/badge/Protocol-CAN%20Bus-blue)
![Language](https://img.shields.io/badge/Language-C99-orange)
![Status](https://img.shields.io/badge/Status-Production%20Ready-success)

![TelemetrySim](ecu_telemetry_analysis.png)

> **Professional-grade automotive ECU simulator implementing ISO 11898 CAN protocol with multi-frame arbitration, realistic physics, and diagnostic system.**

## 🎯 Features

- ✅ **Real CAN Bus Implementation**: Message arbitration, priority handling, broadcast
- ✅ **3 CAN Frames**: Powertrain (0x100), Dynamics (0x200), Sensors (0x300)
- ✅ **Realistic Physics**: Engine, throttle, brake, temperature simulation
- ✅ **Diagnostic System**: Error codes (DTC-like), multi-sensor monitoring
- ✅ **Telemetry Export**: CSV logging for post-analysis
- ✅ **Python Visualization**: Automated plotting and analysis
  
## 🔒 Safety Features (NEW!)1. XOR Checksum Validation
1. XOR Checksum Validation
    Every CAN frame includes an XOR checksum to detect data corruption:
    ```c
    uint8_t can_checksum(const uint8_t *data, uint8_t len) {
        uint8_t cs = 0;
        for (uint8_t i = 0; i < len; i++) {
            cs ^= data[i];
        }
        return cs;
    }
    ``` 
    Protection against:
- ✅ Bit flips during transmission
- ✅ Electrical noise interference
- ✅ Hardware failures

2. Sequence Counter Validation
    Each frame type has an independent rolling counter (0-255):
    ```c
    static uint8_t seq_powertrain = 0;  // Increments with each transmission
    static uint8_t last_seq_powertrain = 0xFF;  // Tracks expected sequence
    ```
Detection of:
- ✅ Lost messages (gap in sequence)
- ✅ Duplicated messages (same sequence twice)
- ✅ Out-of-order messages (sequence jump)

3. Error Code System
When validation fails, specific error flags are set:
```c
typedef enum {
    ECU_ERR_NONE          = 0x00,
    ECU_ERR_CAN_CHECKSUM  = 0x01,
    ECU_ERR_CAN_SEQ       = 0x02,
    ECU_ERR_CAN_TIMEOUT   = 0x04,
    ECU_ERR_OVER_RPM      = 0x08,
    ECU_ERR_OVER_TEMP     = 0x10,
} ECUErrorCode;
```
## 🏗️ Architecture

```
┌─────────────────────────────────────────────┐
│         CAN BUS SIMULATOR                   │
└─────────────────────────────────────────────┘

ECU_TX (Transmitter)
  │
  ├─→ encode_powertrain_frame()  → CAN ID 0x100
  │    ├─ Data encoding
  │    ├─ Sequence counter++
  │    └─ XOR checksum
  │
  ├─→ encode_dynamics_frame()    → CAN ID 0x200
  │    ├─ Data encoding
  │    ├─ Sequence counter++
  │    └─ XOR checksum
  │
  └─→ encode_sensors_frame()     → CAN ID 0x300
       ├─ Data encoding
       ├─ Sequence counter++
       └─ XOR checksum
                │
                ↓
   ┌────────────────────────────────────┐
   │          CAN BUS                   │
   │  • send()                          │
   │  • arbitrate() ← 🔥 (by priority)  │
   │  • deliver()                       │
   └────────────────────────────────────┘
                │
                ↓
ECU_RX (Receiver)
  │
  ├─→ decode_powertrain_frame()
  │    ├─ ✓ Checksum validation
  │    ├─ ✓ Sequence validation
  │    └─ Data extraction
  │
  ├─→ decode_dynamics_frame()
  │    ├─ ✓ Checksum validation
  │    ├─ ✓ Sequence validation
  │    └─ Data extraction
  │
  └─→ decode_sensors_frame()
       ├─ ✓ Checksum validation
       ├─ ✓ Sequence validation
       └─ Data extraction
```

## 🚀 Quick Start
### Build
```bash
make
```
### Run
```bash
make run
```
### Analyze
```bash
jupyter notebook analyze_telemetry.ipynb
```

## 📊 CAN Frame Structure

### Frame 1: Powertrain (ID 0x100, DLC 4)
| Byte | Data | Format | Range |Notes
|------|------|--------|-------|----
| 0-1  | RPM  | uint16_t (Big-Endian) | 0-8000 |MSB first
| 2    | Gear | uint8_t | 0-6 |Automatic calculation
| 3    | Flags | Bit 1: Engine On<br>Bit 0: Brake | 0x00-0x03 |Bitwise packed
|4|Sequence|uint8_t|0-255|Rolling counter
|5|Checksum|uint8_t|0-255|XOR of bytes 0-4

### Frame 2: Dynamics (ID 0x200, DLC 4)
| Byte | Data | Format | Range |Notes
|------|------|--------|-------| ---
| 0 | Speed | uint8_t | 0-250 km/h |From RPM calculation
| 1 | Brake Pressure | uint8_t | 0-100% |Gradual increase
| 2 | Throttle | uint8_t | 0-100% |Input simulation
|3|Reserved|uint8_t| -|Future use
|4|Sequence|uint8_t|0-255|Rolling counter
|5|Checksum|uint8_t|0-255|XOR of bytes 0-4

### Frame 3: Sensors (ID 0x300, DLC 5)
| Byte | Data | Format | Range |Notes
|------|------|--------|-------|---
| 0 | Engine Temp | uint8_t | 0-255°C |Physics simulation
| 1 | Oil Pressure | uint8_t | 0-100 bar |RPM-based
| 2 | Battery Voltage | uint8_t | 0-255 (×0.1V) |12.4V = 124
| 3 | Error Code | uint8_t | DTC codes |Multi-bit flags
| 3 | Error Code | uint8_t | DTC codes |Multi-bit flags
| 4 | Reserved |  | - |Future use
|5|Sequence|uint8_t|0-255|Rolling counter
|6|Checksum|uint8_t|0-255|XOR of bytes 0-4



## 📈 Sample Output
```
=== ACTUAL STATE ==
RPM: 4523
SPEED: 145
THROTTLE: 78
BRAKE: 0
GEAR: 4
ENGINE TEMP: 92
OIL PRESSURE: 65
BATTERY_VOLTAGE: 140
ERROR CODE: 0
===================
```

## 🎯 Technical Highlights

**For Automotive Engineers:**
- ISO 11898 compliant message structure
- Multi-priority arbitration system
- Realistic sensor simulation with physics
- DTC-like error code system

**For Software Engineers:**
- Clean architecture (CAN protocol ↔ ECU logic separation)
- Modular design (easy to add new frames)
- Production-quality code (error handling, documentation)
- Multi-language stack (C + Python)

## 📚 Documentation

- [CAN.md](docs/CAN.md) - Complete CAN protocol explanation
- [analyze_telemetry.ipynb](analyze_telemetry.ipynb) - Data analysis notebook
## 🛠️ Technologies

- **C99**: Core simulation
- **Python 3**: Data analysis
- **Jupyter**: Visualization
- **Make**: Build system

## 👤 Author

**Pedro Henrique Bonifácio da Rosa**  
Computer Engineering Student @ UNISINOS  
Focused on: Automotive Embedded Systems | CAN Protocol | ECU Development

📧 pedrorosa.rb@gmail.com  
💼 [LinkedIn](https://www.linkedin.com/in/pedro-bonifácio-9869a9263/)  

## 📄 License

MIT License - Feel free to use for learning and research!

---

⭐ **If this project helped you understand CAN Bus, give it a star!**

*Built with precision for the automotive software community* 🚗💨