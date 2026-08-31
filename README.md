# 🎹 Interactive Piano

A full-featured interactive piano with both GUI and console interfaces.

## Features

### GUI Version (Interactive Web Piano)
- **Visual Piano Keyboard**: 88-key piano with white and black keys
- **Multiple Waveforms**: Sine, Square, Sawtooth, Triangle
- **Octave Control**: Select between octaves 2-6
- **Volume Control**: Adjustable volume slider
- **Recording & Playback**: Record your playing and playback recordings
- **Mouse Support**: Click any key to play
- **Keyboard Support**: Use your computer keyboard to play notes
- **Real-time Audio**: Instant sound synthesis using Web Audio API
- **Responsive Design**: Works on desktop and tablet devices

### Console Version (Original C++)
- Interactive keyboard input using piano-like keys
- Generates piano notes as WAV audio files
- Demo melody generation
- Builds with a simple Makefile

## Quick Start - GUI Piano

### Option 1: Run with Python (Recommended)
```bash
python3 run_piano.py
```
This will start a local web server and automatically open the piano in your browser at `http://localhost:8000/piano.html`

### Option 2: Open Directly in Browser
Simply open `piano.html` in your web browser:
```bash
firefox piano.html    # or any other browser
```

## Keyboard Shortcuts

| Key | Note | Key | Note |
|-----|------|-----|------|
| **A** | C4 | **K** | C5 |
| **W** | C#4 | **O** | C#5 |
| **S** | D4 | **L** | D5 |
| **E** | D#4 | | |
| **D** | E4 | **Z** | Lower Octave |
| **F** | F4 | **X** | Raise Octave |
| **T** | F#4 | | |
| **G** | G4 | | |
| **Y** | G#4 | | |
| **H** | A4 | | |
| **U** | A#4 | | |
| **J** | B4 | | |

## GUI Features

### Controls
- **Volume Slider**: Adjust output volume (0-100%)
- **Waveform Selector**: Choose between different sound waveforms
- **Octave Selector**: Select the starting octave (2-6)
- **Record Button**: Start/stop recording your performance
- **Playback Button**: Replay your recorded performance
- **Clear Button**: Stop all notes and clear recording

### Recording & Playback
1. Click the "Record" button to start recording
2. Play some notes using keyboard or mouse
3. Click "Stop Recording" to finish
4. Click "Playback" to hear your performance again

## Console Version - Build & Run

```bash
make
./piano
```

You can also generate a demo file directly:
```bash
./piano demo
./piano sequence
```

The program writes WAV files such as `demo_piano.wav` and `sequence_piano.wav` in the project folder.

## Requirements

### GUI Version
- Modern web browser with Web Audio API support (Chrome, Firefox, Safari, Edge)
- Python 3 (optional, only if using `run_piano.py`)

### Console Version
- C++17 compatible compiler (GCC, Clang)
- Standard C++ library

## Architecture

### GUI (piano.html)
- Pure HTML5/CSS3/JavaScript
- Uses Web Audio API for real-time synthesis
- No external dependencies required
- ~500 lines of well-documented code

### Console (main.cpp)
- Pure C++ implementation
- Generates WAV files programmatically
- ~150 lines of code with frequency tables and envelope

## License

MIT License - Feel free to use and modify!

