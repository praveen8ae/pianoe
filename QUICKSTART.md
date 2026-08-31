# Quick Start Guide

## Installation & First Run (5 minutes)

### Prerequisites
- Linux/Unix with X11
- GCC/Clang C++17
- CMake 3.16+

On Ubuntu/Debian:
```bash
sudo apt-get install build-essential cmake libx11-dev
```

### Build
```bash
cd pianoe
mkdir build
cd build
cmake ..
make
```

### Run
```bash
./piano
```

You should see a window with an 88-key piano keyboard.

---

## Basic Usage

### Play Notes

**Keyboard** (arranged like a piano):
```
Row 1: A W S E D F T G Y H U J K O L
       C C# D D# E F F# G G# A A# B C C#
```

**Mouse**: Click any key to play

### Try Recording

1. Press **R** to start recording (status shows "Recording...")
2. Play some notes
3. Press **R** again to stop
4. Press **P** to playback

### Controls Summary
- **Q** - Quit
- **R** - Record/Stop recording
- **P** - Playback
- **C** - Clear (stop playing and reset)
- **H** - Help
- **Z/X** - Change octave (if you implement octave switching)

---

## What's Implemented

✅ **Fully Working**:
- 88-key piano with accurate frequencies
- Real-time keyboard and mouse input
- Visual key display
- Recording and playback of performances
- Save/load recordings to disk

⏳ **Ready for Implementation**:
- Audio synthesis (architecture ready, currently silent)
- MIDI support
- Effects and filters
- Multiple tracks
- More UI features

---

## Adding Audio (Next Step)

The audio system is designed for easy integration. Create your synthesizer:

```cpp
// my_synthesizer.h
#include "AudioEngine.h"
#include <cmath>

class MySynthesizer : public AudioSynthesizer {
public:
    bool initialize() override { return true; }
    void shutdown() override {}
    void noteOn(double frequency, double velocity) override {
        // Generate sound at frequency
    }
    void noteOff(double frequency) override {
        // Stop sound
    }
    void setVolume(double volume) override { volume_ = volume; }
    void setWaveform(int waveform) override { waveform_ = waveform; }
    std::string getStatus() const override { return "My Synthesizer"; }
    bool isInitialized() const override { return true; }
private:
    double volume_ = 1.0;
    int waveform_ = 0;
};
```

Then in `src/main.cpp`:
```cpp
audioEngine_->initialize(std::make_unique<MySynthesizer>());
```

See [ARCHITECTURE.md](ARCHITECTURE.md) for detailed audio integration guide.

---

## Project Structure

```
pianoe/
├── build/            ← Compiled binary here after make
│   └── piano
├── include/          ← Header files (.h)
├── src/              ← Source files (.cpp)
├── CMakeLists.txt    ← Build configuration
├── README.md         ← Full documentation
└── ARCHITECTURE.md   ← Detailed architecture guide
```

---

## Troubleshooting

### "Failed to open X11 display"
Make sure X11 is available:
```bash
echo $DISPLAY
# Should show something like :0
```

### Compilation errors
Check C++ compiler version (need C++17):
```bash
g++ --version
# Should be 7.0 or later
```

### Window doesn't appear
Try running with debug output:
```bash
./piano 2>&1 | head -20
```

---

## Next Steps

1. **Explore the code**: Start with `include/Application.h`
2. **Add audio**: Implement your audio synthesizer
3. **Extend features**: Add MIDI, effects, multiple tracks
4. **Contribute**: Send PRs for improvements

---

## Resources

- [C++ Documentation](https://cppreference.com)
- [X11 Programming](https://www.x.org/wiki/)
- [Audio Processing](https://www.dsprelated.com/)
- [Project Architecture](ARCHITECTURE.md)

---

Happy playing! 🎹
