# Inverted Pendulum

Self-balancing two-wheeled robot built as the final project for the **Control Theory** course at the Universidade de Santiago de Compostela (2023/2024).

The robot keeps itself upright on two wheels by reading its tilt with an IMU and driving the motors through a PID controller. It also exposes a small Wi-Fi web interface that can be used to move it around or switch the controller on and off.

## How it works

1. **Sensing** - A MPU6050 IMU is read over I2C. The accelerometer gives an absolute (but noisy) tilt angle, while the gyroscope gives a fast (but drifting) angular rate.
2. **Angle estimation** - Both measurements are fused with a complementary filter:
   `angle = 0.98 * (angle + gyro_rate * dt) + 0.02 * accelerometer_angle`
   This yields a smooth and stable estimate of the tilt around the X axis.
3. **PID control** - The error is the difference between the desired angle (upright, 0 deg) and the measured angle. The output is:
   `u = Kp * error + Ki * integral + Kd * derivative`
   and is saturated to the PWM range (-255..255).
4. **Actuation** - The control signal is applied to both DC motors through a TB6612FNG driver. When the robot leans forward the wheels drive forward, and vice versa, producing the correction torque that brings it back to the vertical position.
5. **Tuning** - Gains are adjusted in the classic order: first `Kp`, then `Ki`, and finally `Kd` (in this build `Kd` was left at 0 because it did not improve the response).

The PID loop runs as fast as possible (small `delay`), since a high sampling rate is essential for balancing.

## Repository contents

| Path | Description |
| --- | --- |
| `Base_FreeCAD/` | FreeCAD models and STL files for the 3D-printed chassis. |
| `PCB/` | KiCad project (schematic, board layout, custom symbols/footprints), Gerber files and fabrication backups. |
| `Memoria/` | LaTeX report (`Memoria.tex` / `Memoria.pdf`) documenting the design, hardware and software. |
| `Programas/PID/` | Arduino sketches for the balancing controller: basic PID, low-pass filtered PID, IIR-filtered PID and PID with Wi-Fi position control. |
| `Programas/Programas_auxiliares/` | Helper sketches for motor testing, IMU measurements and sensor calibration. |
| `Prueba_esp32/` | Minimal sketch used to verify that the ESP32 board and toolchain work. |
| `Guion_proyecto.txt` | Short project log with the first hardware tests. |

## Hardware

- **ESP32** DevKit-C (main microcontroller, 5 V logic, built-in Wi-Fi).
- **MPU6050** (GY-521) accelerometer + gyroscope, I2C.
- **TB6612FNG** dual H-bridge motor driver.
- **2x DC gear motors** with wheels (ROB-14450).
- **2x 4xAA battery holders in series** (12 V for the motors) and an **LM7805** linear regulator (5 V for the ESP32).
- Custom PCB designed in KiCad and a 3D-printed base designed in FreeCAD.

> Note: the LM7805 needs at least ~7 V at its input, which is why the two 6 V battery packs are connected in series. The motors receive the full 12 V while the ESP32 is fed with regulated 5 V.

## Building and running

### Software

1. Install the **Arduino IDE** and add ESP32 board support through the Boards Manager.
2. Install the **MPU6050** and **I2Cdev** libraries.
3. Open one of the sketches in `Programas/` and select the ESP32 Dev Module board and the correct COM port.
4. Upload the sketch.

Recommended order for a first run:

1. `Prueba_esp32/` - check the board and serial connection.
2. `Programas/Programas_auxiliares/Programa_calibracion_sensor/` - calibrate the IMU offsets with the robot perfectly still.
3. `Programas/Programas_auxiliares/Marcha_motores/` - verify motor direction and wiring.
4. `Programas/PID/Control_PID/` - test and tune the PID.
5. `Programas/PID/PID_posicion/` - final version with Wi-Fi control.

### Wi-Fi control

`PID_posicion.ino` starts the ESP32 as a Wi-Fi access point. Connect to the network shown in the serial monitor and open the IP address (`192.168.4.1` by default) in a browser. The page provides forward/backward/left/right buttons, a stop button and buttons to enable or disable the PID.

The AP credentials are defined at the top of the sketch; change them before publishing or deploying the robot.

## Authors

- Marcos Fontenlos Ferreiro
