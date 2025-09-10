# PerfectAudioWorks

## About this program

**PerfectAudioWorks** is an open-source media player inspired by the classic **Winamp design**.
It is built with performance and simplicity in mind, while remaining modular for future expansion.

Currently, the player:

* Uses **PortAudio** to initialize the audio output buffer.
* Uses **Qt6** for rendering the UI.

> ⚠️ At this stage, PerfectAudioWorks works only on **Linux**. Windows support is planned.

---

## Supported File Formats

* `.wav` (uncompressed)
* `.flac` (Free Lossless Audio Codec)
* `.opus` (Opus codec in Ogg container)
* `.ogg` (Ogg Vorbis)
* `.mp3` (via mpg123)

### Planned via **separate codecs**

* `.aac` (via FFmpeg)
* Other compressed/streaming formats (future plugin system)

---

## Codecs Table

| Format | Handled by | Status     |
| ------ | ---------- | ---------- |
| WAV    | libsndfile | ✅ Works    |
| FLAC   | libsndfile | ✅ Works    |
| OGG    | libsndfile | ✅ Works    |
| Opus   | libsndfile | ✅ Works    |
| MP3    | mpg123     | ✅ Works    |
| AAC    | FFmpeg     | 🔜 Planned |

---

## Optional Dependencies & Codec Flags

| Codec      | Optional | CMake Flag       | Notes                                    |
| ---------- | -------- | ---------------- | ---------------------------------------- |
| libsndfile | ✅        | `-DENABLE_SNDFILE` | Handles WAV, FLAC, OGG, Opus             |
| mpg123     | ✅        | `-DENABLE_MPG123`  | Handles MP3 playback                     |
| FFmpeg     | ✅        | `-DENABLE_FFMPEG`  | Handles AAC and other compressed formats |

---

## Screenshot

![PAW_SCREENSHOT](https://cdn.discordapp.com/attachments/882608057979383869/1415279069297115248/image.png?ex=68c2a0d8&is=68c14f58&hm=171a8497ac1ca9192c9510b5120a5acfecdaad43dacb8565cb3c9cbd8bf36596&)

---

## Installation & Compilation

### Dependencies

* `portaudio`
* `Qt6`
* Optional codecs:

  * `libsndfile` (WAV, FLAC, OGG, Opus)
  * `mpg123` (MP3)
  * `FFmpeg` (AAC and other formats)

### Ubuntu / Debian

```
sudo apt update
sudo apt install portaudio19-dev qt6-base-dev cmake build-essential \
                 libsndfile1-dev mpg123-dev \
                 libavcodec-dev libavformat-dev libavutil-dev libswresample-dev
```

### Arch Linux

```
sudo pacman -S portaudio qt6-base cmake make gcc \
               libsndfile mpg123 ffmpeg
```

### Build Instructions

By default, all codecs are **enabled**. You can disable a codec by passing `-D<FLAG>=OFF` to CMake:

```
git clone https://github.com/loh1naalt/PerfectAudioWorks.git
cd PerfectAudioWorks
mkdir build && cd build
cmake .. -DENABLE_SNDFILE=ON -DENABLE_MPG123=OFF -DENABLE_FFMPEG=ON
make
```

* The example above **disables MP3 support via mpg123**, while keeping libsndfile and FFmpeg enabled.
* CMake automatically skips building source files for any disabled codecs.

---

## Usage

Run the player:

```
./PAW 
```

 Or pass an audio file as argument:
 
```
./PAW  <audio_file_path>
```

---

## Known Bugs

1. ~~**No sound output** if the program selects the default output, but your audio device is not set as system default.~~
   ✅ Fixed: you can now choose the device to play on, but playback still defaults to the system’s default device.

2. **Crash on progress bar seek**: spamming left/right arrow keys after making the progress bar active may cause a crash.

3. ~~**Playing through mpg123 would result in white noise**~~
   ✅ Fixed: sample format conversion from int16 to float32 is now handled correctly.

---

## License

⚠️ No license has been set yet. Currently, the project is **source-available**, but not under an open-source license.
