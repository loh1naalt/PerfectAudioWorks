# PerfectAudioWorks

## About this program
### An opensource media player
Uses portaudio to initialize connection to output buffer,<br>
libsndfile to write to output buffer from file (for now only .wav files are supported by this program),<br>
Qt6 for rendering ui <br>
**Consider that it only works on linux (so far)!** <br>
### Which file formats can it open?
it can open uncompressed file formats such as .wav, .ogg, .opus, .flac(?) etc... <br>
it also able to open mp3 files, **BUT** it's highly not recommended to open them, because libsndfile cannot normally handle mp3 alike codecs. <br>
### Screenshot of program
![PAW_SCREENSHOT](https://cdn.discordapp.com/attachments/870025078828589098/1397171995749253290/image.png?ex=6880c151&is=687f6fd1&hm=2a290aae639565955cb64e222306e1d07ef7de7bec80cd7e8573336bcbaa49fe&)
## How to compile this program?
ensure that you have these libraries installed:
```
portaudio-dev
libsndfile-dev
Qt6-dev
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

