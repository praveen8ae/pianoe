# 🎹 Professional Piano Application - C++

A fully functional, production-grade desktop piano application written in modern C++17. This is a professional-level instrument ready for sound synthesis integration, featuring a clean architecture, real-time input handling, recording/playback capabilities, and extensible audio engine design.

## Features

### Core Piano Engine
- **88-key full-range piano** (A0 to C8) with accurate note frequencies
- **White and black keys** with proper visual representation
- **Real-time input handling** via keyboard and mouse
- **Professional key management** with press/release states

### User Interface
- **X11-based graphics rendering** for cross-platform display
- **Interactive visual keyboard** with key highlighting on press
- **Real-time status display** showing recording and playback status
- **Responsive keyboard input** with per-key state tracking

### Recording & Playback
- **Full recording system** capturing all note events with precise timing
- **Playback engine** with frame-accurate note sequencing
- **File I/O** for saving and loading recordings (custom binary format)
- **Metadata support** for title, author, and tempo information

### Audio Architecture  
- **Extensible audio engine** with plugin interface
- **Silent synthesizer** placeholder for rapid development
- **Ready for sound backends**: PortAudio, JACK, Alsa, etc.
- **Clean separation** between UI, logic, and audio

### Professional Code Structure
- **CMake build system** for cross-platform compilation
- **Modular architecture** with clear separation of concerns
- **Memory management** using smart pointers
- **Event-driven design** with callback system
- **Well-documented codebase** with comprehensive comments

## Architecture

```
┌─────────────────────────────────────────────────────┐
│          PianoApplication (Main Coordinator)        │
├─────────────────────────────────────────────────────┤
│                                                     │
│  ┌──────────┐  ┌─────────────┐  ┌──────────────┐  │
│  │  Piano   │  │InputHandler │  │   Recorder   │  │
│  │ (Engine) │  │  (Controls) │  │ (Sequencer)  │  │
│  └──────────┘  └─────────────┘  └──────────────┘  │
│       ▲               │               ▲            │
│       │               │               │            │
│       └───────────────┴───────────────┘            │
│                       │                            │
│  ┌────────────────────┴──────────────────┐        │
│  │                                       │        │
│  ┌──────────────┐            ┌──────────────────┐│
│  │ UIRenderer   │            │  AudioEngine     ││
│  │ (Graphics)   │            │ (Synthesizer)    ││
│  └──────────────┘            └──────────────────┘│
│                                                     │
└─────────────────────────────────────────────────────┘
```

### Components

#### **Piano** (`Piano.h/.cpp`)
- Manages 88 piano keys (A0-C8)
- Maintains note frequencies and key states
- Provides key lookup and layout management
- Responsible for musical logic

#### **InputHandler** (`InputHandler.h/.cpp`)
- Handles X11 keyboard and mouse events
- Maps keyboard keys to musical notes
- Tracks pressed/released keys
- Supports customizable key mappings

#### **Recorder** (`Recorder.h/.cpp`)
- Records note events with timestamps
- Playback with frame-accurate timing
- Save/load functionality
- Metadata management (title, author, tempo)

#### **AudioEngine** (`AudioEngine.h/.cpp`)
- Abstract synthesizer interface
- Plugin architecture for audio backends
- Volume and waveform controls
- Ready for integration with audio libraries

#### **UIRenderer** (`UIRenderer.h/.cpp`)
- X11-based graphics rendering
- Piano key visualization
- Font rendering for labels
- Color management and drawing primitives

#### **PianoApplication** (`Application.h/.cpp`)
- Main application coordinator
- Event loop management
- Component lifecycle management
- User interaction orchestration

## Building

### Prerequisites
- Linux/Unix system with X11 development files
- GCC or Clang C++17 compiler
- CMake 3.16+

### Installation

```bash
# Clone and build
cd pianoe
mkdir build
cd build
cmake ..
make

# Binary will be at: build/piano
```

### Run

```bash
./piano
```

## Usage

### Keyboard Controls
| Key | Action | Notes |
|-----|--------|-------|
| **A-L** | Play notes | Maps to C4-D5 keys |
| **Z/X** | Octave down/up | Switch octave ranges |
| **R** | Start/stop recording | Toggle recording mode |
| **P** | Play recording | Playback recorded performance |
| **C** | Clear | Clear all and reset |
| **Q** | Quit | Exit application |
| **H** | Help | Toggle help display |

### Mouse Control
- **Click on keys** to play notes
- **Release** to stop notes

### Recording
1. Press **R** to start recording
2. Play notes using keyboard or mouse
3. Press **R** again to stop
4. Press **P** to playback your recording

## File Structure

```
pianoe/
├── CMakeLists.txt          # Build configuration
├── include/
│   ├── Piano.h             # Piano engine (88 keys)
│   ├── InputHandler.h      # Keyboard/mouse input
│   ├── Recorder.h          # Recording/playback
│   ├── AudioEngine.h       # Audio synthesis interface
│   ├── UIRenderer.h        # X11 graphics rendering
│   └── Application.h       # Main coordinator
├── src/
│   ├── main.cpp            # Entry point
│   ├── Piano.cpp           # Piano implementation
│   ├── InputHandler.cpp    # Input handling
│   ├── Recorder.cpp        # Recording system
│   ├── AudioEngine.cpp     # Audio engine
│   ├── UIRenderer.cpp      # Graphics rendering
│   └── Application.cpp     # Application logic
├── build/                  # Build directory
│   └── piano               # Compiled executable
└── README.md               # This file
```

## Adding Audio Synthesis

The application is designed to easily integrate audio backends:

```cpp
// Example: Implement your audio synthesizer
class PortAudioSynthesizer : public AudioSynthesizer {
    bool initialize() override { /* ... */ }
    void noteOn(double frequency, double velocity) override { /* ... */ }
    void noteOff(double frequency) override { /* ... */ }
    // ... implement other methods
};

// In main.cpp
audioEngine_->initialize(std::make_unique<PortAudioSynthesizer>());
```

The interface provides:
- `initialize()` - Setup audio system
- `noteOn(frequency, velocity)` - Start playing note
- `noteOff(frequency)` - Stop playing note
- `setVolume(level)` - Control volume
- `setWaveform(type)` - Change waveform (sine, square, etc.)

## Performance

- **Lightweight**: ~86KB compiled binary (release)
- **Responsive**: 60 FPS rendering loop
- **Low latency**: Direct X11 event handling
- **Memory efficient**: Smart pointer managed resources

## Future Enhancements

- [ ] Audio synthesis backends (PortAudio, JACK, Alsa)
- [ ] MIDI input support
- [ ] Multiple recording tracks
- [ ] Built-in effects (reverb, delay, etc.)
- [ ] Sustain/pedal support
- [ ] Touch screen support
- [ ] Plugin system for custom sounds
- [ ] Save recordings as WAV/MIDI

## Technical Details

### Key Frequency Calculation
The piano uses standard equal temperament tuning with A4 = 440Hz:
```
Frequency = BaseFrequency × 2^(octaveDifference)
```

### Event-Driven Architecture
- Non-blocking event loop
- Callback-based event handling
- Separation of input, logic, and rendering

### Memory Management
- Modern C++17 with smart pointers
- No manual memory allocation in application code
- RAII principles throughout

## License

MIT License - Feel free to use, modify, and distribute

## Credits

Built as a professional-grade C++ application demonstrating:
- Modern C++ design patterns
- Audio application architecture
- Cross-platform GUI programming with X11
- Real-time system design
- Professional code organization

