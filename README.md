# CUBIC SYNTHESIS

A rubik's cube-controlled synthesizer + music performance interface, built with MaxMSP and Arduino! 

## Video Demo
[![Cubic Synthesis Video Demo](https://img.youtube.com/vi/c7BzYmt0XyI/maxresdefault.jpg)](https://www.youtube.com/watch?v=c7BzYmt0XyI "Everything Is AWESOME")

## Concept     

Turns the process of solving a rubik's cube into music, from three perspectives:
1. Physical:   
      - accelerometers attached to fingers control the frequency of a continuous, 'lead' synth sound.
2. Mechanical:   
      - noises made during scrambling are recorded and played back through random granular synthesis at every turn.
4. Conceptual:   
      - each facelet is assigned a note and synthesized voice. Each turn of the cube plays chords moving from discordant (scrambled) to harmonious (solved). 

## Tech Specs
- Arduino Uno (connected to accelerometers)
- Arduino Feather ESP32 (connected to Rubik's cube through Bluetooth)
- Giiker Bluetooth Rubik's Cube
- 2 x Adafruit LSM6DSO32 6-DoF Accelerometer and Gyroscope (STEMMA QT / Qwiic)

## Sources
- Arduino code adapted from https://github.com/playfultechnology/esp32-smartcube and https://learn.adafruit.com/lsm6dsox-and-ism330dhc-6-dof-imu/arduino.  
- Disjoint cycle notation generators for faceturns (and other theoretical tidbits) from http://www.permutationpuzzles.org/rubik/webnotes/rubik.pdf.
- MaxMSP synth engine designed and built with great inspiration from the wonderful Oliver Thurley, https://www.youtube.com/channel/UCBNjKsD4ZfV1htmgbcgZduQ.

