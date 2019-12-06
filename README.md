# MAE157_Group3
Compressed Air Rocket Project
TODO: Flush this out since it'll be reference material
## Branches
---
1. SampleMax is the branch with adjustments geared towards sampling rate optimization
2. Master is the original base program
## Tasks
Recording pressure transducer readings while simultaneously communicating with a remote master module in charge of signaling when the launch is supposed to happen. All utilizing an Elgoo uno which can only handle single serial port communication.
### SampleMax Sampling Rate (I'll probably make this look better later)
Sampling Rate of the Arduino is heavily dependent on data recording speed(the ADC by itself can operate at ~9600hz). Depending on your medium (for example: SD Card or Serial monitor) this itself varies quite a bit. The structure of the program itself also matters a lot. You can look into big-O notation for more details, but basically you want to reduce the lines of code executed in a program to speed it up. For us, we isolated each notable event (data collection, data dumping, display signaling) into 3 separate 'loops' Where each function responsible for each task was only called as often as really needed. Naturally our sampling rate is the fastest, then comes the rate at which data is dumped to the serial monitor, then the rate at which data is sent for parsing to the master. This helps the arduino operate at an overall faster rate and helps us speed up sampling rate significantly. 
