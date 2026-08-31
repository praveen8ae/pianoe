# Architecture & Design Guide

## Overview

The Piano application follows a modular, event-driven architecture designed for extensibility and maintainability. The codebase emphasizes clean separation of concerns, making it easy to replace or extend individual components.

## Design Principles

### 1. **Separation of Concerns**
Each component has a single responsibility:
- **Piano**: Music logic and note management
- **InputHandler**: Event translation from X11 to musical notes
- **Recorder**: Recording/playback sequencing
- **AudioEngine**: Sound synthesis abstraction
- **UIRenderer**: Graphics rendering
- **PianoApplication**: Coordination and orchestration

### 2. **Dependency Injection**
Components receive their dependencies through constructors, making them testable and replaceable:

```cpp
Piano piano;
InputHandler inputHandler(&piano);  // Piano injected
Recorder recorder(&piano);           // Piano injected
```

### 3. **Interface-Based Design**
The `AudioSynthesizer` is an abstract base class allowing multiple implementations:

```cpp
class AudioSynthesizer {
    virtual bool initialize() = 0;
    virtual void noteOn(double frequency, double velocity) = 0;
    // ...
};
```

### 4. **Smart Memory Management**
All components use `std::unique_ptr` for automatic cleanup:

```cpp
std::unique_ptr<Piano> piano_;
std::unique_ptr<Recorder> recorder_;
```

## Component Deep Dive

### Piano Engine

**Responsibility**: Manage 88-key range, note frequencies, and key states

**Key Classes**:
- `Note`: Represents a musical note with frequency
- `PianoKey`: Represents a physical key on the piano
- `Piano`: Collection of 88 keys with lookup and layout

**Note Frequency Calculation**:
```
F(n) = F₀ × 2^((n-n₀)/12)
where F₀ = 440Hz (A4), using equal temperament
```

**Key Features**:
- Accurate frequency calculation for all 88 keys
- White/black key differentiation
- Screen coordinate management for rendering
- Press/release state tracking

### Input Handler

**Responsibility**: Translate X11 events to musical note events

**Event Flow**:
```
X11 Event → handleXEvent() → KeySym mapping → Piano::pressKey()/releaseKey()
```

**Key Features**:
- Keyboard-to-note mapping (customizable)
- Key repeat detection
- Mouse-to-piano translation
- Event callback system for extensibility

**Keyboard Mapping Example**:
```cpp
mapKeyToNote(XK_a, Note::NoteName::C, 4);  // 'A' plays C4
mapKeyToNote(XK_w, Note::NoteName::CSharp, 4);  // 'W' plays C#4
```

### Recorder

**Responsibility**: Record and playback note sequences with timing

**Data Structures**:
```cpp
struct NoteEvent {
    Type type;              // NoteOn or NoteOff
    Note::NoteName name;    // Which note
    int octave;             // Which octave
    double timestamp;       // When (seconds from start)
    double velocity;        // How loud (0.0-1.0)
};
```

**Recording Process**:
1. `startRecording()` - Initialize timing
2. `recordNoteOn()` / `recordNoteOff()` - Store events with timestamps
3. `stopRecording()` - Finalize recording

**Playback Process**:
1. `startPlayback()` - Initialize playback timer
2. `updatePlayback()` - Check current time and trigger events
3. Events trigger `piano_->pressKey()` and `piano_->releaseKey()`

**File Format** (custom text-based):
```
PIANO_REC
Title: My Recording
Author: John Doe
Tempo: 120
Duration: 5.234
Events: 42
---
ON 0 4 0.000 1.0
OFF 0 4 0.200 1.0
ON 2 4 0.250 0.95
...
```

### Audio Engine

**Responsibility**: Abstract audio synthesis and provide plugin interface

**Architecture**:
```
Application → AudioEngine → AudioSynthesizer (Interface)
                                ↓
                    ┌───────────┴───────────┐
                    ↓                       ↓
            SilentSynthesizer      YourCustomSynthesizer
```

**Synthesizer Interface**:
```cpp
class AudioSynthesizer {
    virtual bool initialize() = 0;
    virtual void noteOn(double frequency, double velocity) = 0;
    virtual void noteOff(double frequency) = 0;
    virtual void setVolume(double volume) = 0;
    virtual void setWaveform(int waveform) = 0;
};
```

**Adding a New Synthesizer**:
```cpp
class MyCustomSynthesizer : public AudioSynthesizer {
    bool initialize() override {
        // Your initialization code
        return true;
    }
    
    void noteOn(double frequency, double velocity) override {
        // Generate sound at frequency with velocity
    }
    
    void noteOff(double frequency) override {
        // Stop sound at frequency
    }
    
    // ... implement other methods
};

// Usage in main.cpp
audioEngine_->initialize(std::make_unique<MyCustomSynthesizer>());
```

### UI Renderer

**Responsibility**: Render piano keys and status information using X11

**Rendering Pipeline**:
1. `clear()` - Clear window
2. `renderPiano()` - Draw all keys
3. `renderStatus()` - Draw text overlays
4. `flush()` - Update display

**Key Drawing**:
- White keys: RGBcolor(255, 255, 255)
- Black keys: RGB(0, 0, 0)
- Pressed keys: Different shade to show state
- Labels: Note names on white keys

**Coordinate System**:
- Origin at top-left
- X increases rightward
- Y increases downward
- Keys positioned during `piano_->layoutKeys(width, height)`

### Application Coordinator

**Responsibility**: Orchestrate all components and manage application lifecycle

**Main Loop**:
```cpp
while (isRunning_) {
    handleEvents();    // Process X11 events
    update();          // Update playback state
    render();          // Draw current state
    sleep(16ms);       // ~60 FPS
}
```

**Event Handling Chain**:
```
X11 Event → handleEvents()
    ├→ Check for global commands (Q/R/P/etc)
    ├→ Pass to InputHandler for note events
    ├→ Record if recording is active
    └→ Render updated state
```

## Data Flow Examples

### Playing a Note via Keyboard

```
User presses 'A' key
    ↓
X11 KeyPress event
    ↓
InputHandler::handleXEvent()
    ↓
XLookupKeysym() → XK_a
    ↓
getKeyNote() → Note::NoteName::C
getKeyOctave() → 4
    ↓
Piano::pressKey(C, 4)
    ↓
PianoKey::setPressed(true)
    ↓
[Rendering] DrawKey highlights pressed state
    ↓
[Audio] AudioEngine::playNote(frequency=261.63Hz)
    ↓
[Recording] if recording: Recorder::recordNoteOn(C, 4, timestamp)
```

### Playback Sequence

```
User presses 'P'
    ↓
startPlayback() - Initialize playback timer
    ↓
Main loop continues...
    ↓
updatePlayback() called each frame
    ↓
Check: current_time >= next_event_time?
    ↓
YES: Trigger event (press/release key)
    ↓
NO: Wait for next frame
    ↓
All events processed? Stop playback
```

## Extension Points

### 1. **Adding Audio Backends**
Implement `AudioSynthesizer` interface:
- PortAudio for cross-platform audio
- JACK for professional audio routing
- Alsa for Linux-specific audio
- Web Audio API via WebAssembly bridge

### 2. **Adding MIDI Support**
Extend `InputHandler`:
```cpp
void handleMidiNote(int midiNote, int velocity);
void handleMidiControl(int controller, int value);
```

### 3. **Adding Effects**
Extend `AudioEngine`:
```cpp
void addEffect(std::unique_ptr<AudioEffect> effect);
```

### 4. **Multi-Track Recording**
Extend `Recorder`:
```cpp
class MultiTrackRecorder {
    std::vector<Track> tracks_;
    void recordToTrack(int trackIndex, ...);
};
```

### 5. **Waveform Selection**
Already in audio engine interface:
```cpp
setWaveform(0);  // Sine
setWaveform(1);  // Square
setWaveform(2);  // Sawtooth
setWaveform(3);  // Triangle
```

## Potential Issues & Solutions

### Issue: X11 Macro Conflicts
**Problem**: `KeyPress` and `KeyRelease` are X11 macros
**Solution**: Use numeric event type values (2, 3) instead of macro names

### Issue: Memory Leaks
**Solution**: All resources use `std::unique_ptr` with RAII

### Issue: Audio Latency
**Solution**: Implement synthesizer with real-time priority threads

### Issue: Input Lag
**Solution**: Non-blocking X11 event queue with dedicated input thread

## Performance Considerations

1. **60 FPS Rendering**: Target 16ms per frame
2. **Event Latency**: <5ms from key press to audio start
3. **Memory**: ~86KB binary, minimal runtime memory
4. **CPU**: Single-threaded main loop, can be parallelized if needed

## Build System

**CMake Configuration**:
- Automatic dependency detection
- X11 library linking
- Optimization flags for release builds
- Platform-specific settings

**Compile Commands**:
```bash
cmake ..      # Configure
make          # Compile
make install  # Install binary
```

## Testing Considerations

The modular design makes testing straightforward:

```cpp
// Test Piano independently
Piano piano;
piano.pressKey(Note::NoteName::C, 4);
assert(piano.getPianoKey(Note::NoteName::C, 4)->isPressed());

// Test Recorder independently
Recorder recorder(&piano);
recorder.recordNoteOn(Note::NoteName::C, 4, 1.0);
assert(recorder.getRecordedEvents().size() == 1);

// Mock AudioEngine for testing
class TestSynthesizer : public AudioSynthesizer { /*...*/ };
```

## Thread Safety

Current implementation is single-threaded. For multi-threaded audio:
1. Use lock-free queues for event communication
2. Audio thread updates synthesizer state
3. Main thread handles UI and input
4. Synchronize via atomic variables

## Future Refactoring Ideas

1. **Event System**: Replace callbacks with event queue
2. **State Machine**: Explicit states for recording/playing
3. **Plugin Architecture**: Dynamic library loading for synthesizers
4. **Async I/O**: Non-blocking file operations for recording save/load
5. **Logging System**: Debug/release logging infrastructure

---

For more details on specific components, see the source code comments and header files.
