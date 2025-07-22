# PerfectAudioWorks

## About this program
### An opensource media player
Uses portaudio to initialize connection to output buffer,<br>
libsndfile to write to output buffer from file (for now only .wav files are supported by this program),<br>
Qt6 for rendering ui <br>
**Consider that it only works on linux (so far)!** <br>
![PAW_SCREENSHOT](https://cdn.discordapp.com/attachments/870025078828589098/1397171995749253290/image.png?ex=6880c151&is=687f6fd1&hm=2a290aae639565955cb64e222306e1d07ef7de7bec80cd7e8573336bcbaa49fe&)
## How to compile this program?
ensure that you have these libraries installed:
```
portaudio
libsndfile
Qt6
```
then create build directory:
``` 
mkdir build
```
and compile it:
```
cmake ..
make
```
## Known bugs
### 1) No sound
since this program choses default output for playback, and you are using other audio device that not have been asssigned as default.

