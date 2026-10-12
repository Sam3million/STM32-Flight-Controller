# STM32 Flight Controller and Graphic Display

![Main Photo](/docs/primitives.jpg)

This is an embedded flight controller and display project I did using an STM32H753ZI, an MPU-6050 IMU, and an ILI9341 Display.

The wiring is as follows:
Display
- SPI SCK	-> PA5 (D13 on the Nucleo's ST ZIO connectors)
- CS		-> PD4 (D54)
- DC		-> PD5 (D53)
- RST		-> PD6 (D52)
- SPI MOSI	-> PD7 (D51)
- GND		-> GND
- VCC		-> 3.3V

MPU-6050
-TBD

## Status
Currently done: display driver, primitive and font rendering, measurements)
Planned: IMU, filter, PID, FreeRTOS)

## Results
-TBD

## Build
This project was built using Cube STM32CubeMX and STM32CubeIDE 2.20 on macOS.

## Third party
- font8x8
- spleen

## License
MIT License

## Blog Post
Read a detailed post about the project: LINK TBD.