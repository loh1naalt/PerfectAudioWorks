# PerfectAudioWorks

## About this program
**PerfectAudioWorks** is an open-source media player inspired by the classic **Winamp design**.  
It is built with performance and simplicity in mind, while remaining modular for future expansion.  

Currently, the player:  
- Uses **PortAudio** to initialize the audio output buffer.  
- Uses **libsndfile** to decode and write audio data (supports several formats out of the box).  
- Uses **Qt6** for rendering the UI.  

> ⚠️ At this stage, PerfectAudioWorks works only on **Linux**. Windows support is planned.  

---

## Supported File Formats

### Supported via **libsndfile**
- `.wav` (uncompressed)  
- `.flac` (Free Lossless Audio Codec)  
- `.opus` (Opus codec in Ogg container)  
- `.ogg` (Ogg Vorbis)  

### Planned via **separate codecs**
- `.mp3` (via `mpg123` or `minimp3`)  
- Other compressed/streaming formats (future plugin system)  

---

## Codecs Table

| Format | Handled by     | Status       |
|--------|----------------|--------------|
| WAV    | libsndfile     | ✅ Works     |
| FLAC   | libsndfile     | ✅ Works     |
| OGG    | libsndfile     | ✅ Works     |
| Opus   | libsndfile     | ✅ Works     |
| MP3    | mpg123/minimp3 | 🔜 Planned   |
| AAC    | FFmpeg     | 🔜 Planned |

---

## Screenshot
![PAW_SCREENSHOT](https://cdn.discordapp.com/attachments/870025078828589098/1397171995749253290/image.png?ex=6880c151&is=687f6fd1&hm=2a290aae639565955cb64e222306e1d07ef7de7bec80cd7e8573336bcbaa49fe&)  

---

## Installation & Compilation

### Dependencies
- `portaudio`  
- `libsndfile`  
- `Qt6`  

### Ubuntu / Debian
```bash
sudo apt update
sudo apt install portaudio19-dev libsndfile1-dev qt6-base-dev cmake build-essential
```

### Arch Linux
```bash
sudo pacman -S portaudio libsndfile qt6-base cmake make gcc
```

### Build Instructions
```bash
git clone https://github.com/loh1naalt/PerfectAudioWorks.git
cd PerfectAudioWorks
mkdir build && cd build
cmake ..
make
```

---

## Usage
Run the player by passing an audio file as argument:  
```bash
./PerfectAudioWorks ~/Music/example.flac
```


---

## Known Bugs
~~1. **No sound output** if the program selects the default output, but your audio device is not set as system default.~~<br>
**fixed** you can chose device to play on. But consider that it initially will play on default device 
2. **Crash on progress bar seek**: spamming left/right arrow keys after making the progress bar active may cause a crash.  

---


## License
⚠️ No license has been set yet. Currently, the project is **source-available**, but not under an open-source license.  
