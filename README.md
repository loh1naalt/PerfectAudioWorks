# PerfectAudioWorks
## About this program
### An opensource media player
Uses portaudio to initialize connection to output buffer,
libsndfile to write to output buffer from file (for now only .wav files are supported by this program),
Qt6 for rendering ui
**Consider that it only works on linux (so far)!**
![Paw_Screenshot]([https://cdn.discordapp.com/attachments/870025078828589098/1396640852608024577/image.png?ex=687ed2a7&is=687d8127&hm=f39b6c296c1180f6194177bb0d0ee04b4c2e456b866a90ccd10758a77d737e03&)
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
