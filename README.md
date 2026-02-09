# Flight Software Prototype – Single-Axis AOCS (ESP8266 + FreeRTOS)

## Overview

This repository contains a **flight-software–inspired prototype** implementing a **single-axis attitude control system** using:

- **ESP8266** running **FreeRTOS** (ESP8266_RTOS_SDK)
- **IMU (MPU6050)** for angular rate sensing
- **Reaction wheel / brushless motor** driven via **ESC + PWM**
- A **D attitude controller with deadband**
- A **modular, cFS-like software architecture**

The goal of this project is **architectural clarity and correctness**, not performance optimization or production readiness.

---

## Architecture Philosophy

The design intentionally mirrors **spacecraft flight software patterns**:

- Clear separation of responsibilities
- Deterministic, cyclic execution
- Message-based data exchange
- No tight coupling between control logic and hardware

### Key Principles

- Each module exposes **only**:
  ```cpp
  void init();
  void step(float dt);
  ```
- No dynamic memory allocation
- No STL usage
- No inheritance (for now)
- All inter-module data flows through a **software bus**

---

## High-Level Architecture

```
                    +----------------------+
                    |       FreeRTOS       |
                    |      Scheduler       |
                    +----------+-----------+
                               |
        -------------------------------------------------
        |                    |                         |
        v                    v                         v
+---------------+    +---------------+         +---------------+
|    IO_MGR     |    |   HealthMgr   |         |    AOCSMgr    |
|               |    |               |         |               |
| - IMU driver  |    | Fault monitor |         | Attitude ctrl |
| - ESC output  |    | Limits check  |         | Wheel command |
+-------+-------+    +-------+-------+         +-------+-------+
        ^                    ^                         ^
        |                    |                         |
        v                    |                         v  
                 +-----------------------------------+
                 |           Software Bus            |
                 |         (FreeRTOS Queues)         |
                 |                                   |
                 |  IMU data   → subscribers         |
                 |  Motor cmd  → IO manager          |
                 +-----------------------------------+


```

## Modules

### 1. IO_MGR (`io_mgr.*`)

**Responsibilities**
- Owns all hardware interaction
- Reads IMU data
- Drives reaction wheel via PWM
- Publishes sensor data to the software bus
- Consumes actuator commands from the software bus

---

### 2. AOCSMgr (`aocs_mgr.*`)

**Responsibilities**
- Implements single-axis **D attitude controller**
- dampens the oscillation with gyro rate input `θ_dot`
- Applies a small deadband to avoid drifts and noises
- Outputs wheel speed commands

---

### 3. HealthMgr (`health_mgr.*`)

**Responsibilities**
- Monitors system signals
- Performs threshold and rate-of-change checks
- Flags unhealthy conditions

This module performs **monitoring only**, not isolation or recovery.

---

### 4. Software Bus (`software_bus.*`)

A lightweight analogue of the **cFS Software Bus**, implemented using FreeRTOS queues.

**Data structures**
- `ImuData` – IMU → AOCS
- `MotorCmd` – AOCS → IO

Queues use **latest-value semantics** (`xQueueOverwrite`).

---

## Control Law

### Controller Type

**Single-Axis D Rate damping Controller with Deadband**

---

### Attitude Estimation

\[
\theta_{est}(k+1) = \theta_{est}(k) + \omega \cdot \Delta t
\]

---

### Control Law

\[
\tau = -K_p (\theta_{est} - \theta_{ref})_{db} - K_d \omega
\]

---

### Deadband

A small attitude deadband (default ≈ **0.5°**) prevents constant micro-corrections caused by noise.

---

### Actuator Dynamics

\[
\omega_{rw}(k+1) =
\omega_{rw}(k) + \frac{\tau}{J} \Delta t
\]

---

## Task Rates

| Task | Rate |
|---|---|
| IO_MGR | 100 Hz |
| AOCS_MGR | 100 Hz |
| HealthMgr | 5 Hz |

---

## File Structure

```
.
├── app_main.cpp
├── tasks.cpp / tasks.h
├── software_bus.cpp / software_bus.h
├── io_mgr.cpp / io_mgr.h
├── imu.cpp / imu.h
├── reaction_wheel.cpp / reaction_wheel.h
├── aocs_mgr.cpp / aocs_mgr.h
├── health_mgr.cpp / health_mgr.h
└── README.md
```

