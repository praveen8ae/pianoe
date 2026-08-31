#include "Piano.h"
#include <iostream>
#include <sstream>

// Note implementation
Note::Note(NoteName name, int octave, double frequency, double duration)
    : name_(name), octave_(octave), frequency_(frequency), duration_(duration) {
}

double Note::calculateFrequency(NoteName name, int octave) {
    // A4 = 440 Hz
    // Frequencies go: C4 = 261.63 Hz
    static const double noteFrequencies[] = {
        261.63,  // C
        277.18,  // C#
        293.66,  // D
        311.13,  // D#
        329.63,  // E
        349.23,  // F
        369.99,  // F#
        392.00,  // G
        415.30,  // G#
        440.00,  // A
        466.16,  // A#
        493.88   // B
    };
    
    int noteIndex = static_cast<int>(name);
    double baseFreq = noteFrequencies[noteIndex];
    
    // Adjust for octave relative to octave 4
    int octaveDifference = octave - 4;
    return baseFreq * std::pow(2.0, octaveDifference);
}

std::string Note::getLabel() const {
    static const std::string noteNames[] = {
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    };
    
    std::ostringstream oss;
    oss << noteNames[static_cast<int>(name_)] << octave_;
    return oss.str();
}

// PianoKey implementation
PianoKey::PianoKey(const Note& note, KeyColor color, int visualIndex)
    : note_(note), color_(color), visualIndex_(visualIndex), isPressed_(false) {
}

void PianoKey::setScreenPosition(int x, int y) {
    screenX_ = x;
    screenY_ = y;
}

void PianoKey::setDimensions(int width, int height) {
    width_ = width;
    height_ = height;
}

// Piano implementation
Piano::Piano() {
    initializeKeys();
}

Piano::~Piano() = default;

void Piano::initializeKeys() {
    // Standard 88-key piano: A0 to C8
    int visualWhiteIndex = 0;
    int visualBlackIndex = 0;
    
    // Create all notes from A0 to C8
    for (int octave = 0; octave <= 8; ++octave) {
        for (int noteIdx = 0; noteIdx < 12; ++noteIdx) {
            Note::NoteName name = static_cast<Note::NoteName>(noteIdx);
            
            // Skip notes before A0 and after C8
            if (octave == 0 && name != Note::NoteName::A && name != Note::NoteName::ASharp && name != Note::NoteName::B) {
                continue;
            }
            if (octave == 8 && name != Note::NoteName::C) {
                break;
            }
            
            double frequency = Note::calculateFrequency(name, octave);
            Note note(name, octave, frequency);
            
            PianoKey::KeyColor color = isBlackKey(name) ? PianoKey::KeyColor::Black : PianoKey::KeyColor::White;
            int visualIndex = isBlackKey(name) ? visualBlackIndex++ : visualWhiteIndex++;
            
            auto key = std::make_unique<PianoKey>(note, color, visualIndex);
            
            // Store in lookup map
            std::ostringstream oss;
            oss << note.getLabel();
            keyLookup_[oss.str()] = key.get();
            
            keys_.push_back(std::move(key));
        }
    }
}

bool Piano::isBlackKey(Note::NoteName name) {
    return name == Note::NoteName::CSharp ||
           name == Note::NoteName::DSharp ||
           name == Note::NoteName::FSharp ||
           name == Note::NoteName::GSharp ||
           name == Note::NoteName::ASharp;
}

PianoKey* Piano::getPianoKey(const Note::NoteName& name, int octave) {
    Note note(name, octave, Note::calculateFrequency(name, octave));
    std::string label = note.getLabel();
    
    auto it = keyLookup_.find(label);
    return (it != keyLookup_.end()) ? it->second : nullptr;
}

void Piano::pressKey(const Note::NoteName& name, int octave) {
    PianoKey* key = getPianoKey(name, octave);
    if (key) {
        key->setPressed(true);
    }
}

void Piano::releaseKey(const Note::NoteName& name, int octave) {
    PianoKey* key = getPianoKey(name, octave);
    if (key) {
        key->setPressed(false);
    }
}

void Piano::releaseAllKeys() {
    for (auto& key : keys_) {
        key->setPressed(false);
    }
}

void Piano::layoutKeys(int windowWidth, int windowHeight) {
    // Calculate layout: white keys at bottom, black keys overlaid
    int keyboardWidth = windowWidth - 40;  // Leave margins
    int keyboardHeight = windowHeight - 100;  // Leave space for UI
    int startX = 20;
    int startY = 20;
    
    // Count white keys
    int whiteKeyCount = 0;
    for (const auto& key : keys_) {
        if (key->getColor() == PianoKey::KeyColor::White) {
            whiteKeyCount++;
        }
    }
    
    int whiteKeyWidth = keyboardWidth / whiteKeyCount;
    int whiteKeyHeight = keyboardHeight;
    int blackKeyWidth = whiteKeyWidth * 0.6;
    int blackKeyHeight = whiteKeyHeight * 0.65;
    
    int whiteKeyX = startX;
    
    for (auto& key : keys_) {
        if (key->getColor() == PianoKey::KeyColor::White) {
            key->setScreenPosition(whiteKeyX, startY);
            key->setDimensions(whiteKeyWidth, whiteKeyHeight);
            whiteKeyX += whiteKeyWidth;
        }
    }
    
    // Position black keys
    whiteKeyX = startX;
    int whiteIdx = 0;
    
    for (const auto& whiteKey : keys_) {
        if (whiteKey->getColor() != PianoKey::KeyColor::White) continue;
        
        // Check if next white key should have a black key between
        int nextWhiteIdx = whiteIdx + 1;
        
        // Position black keys based on white key patterns
        // Between C-D, D-E, F-G, G-A, A-B are black keys
        Note::NoteName noteName = whiteKey->getNote().getName();
        
        if (noteName == Note::NoteName::C || noteName == Note::NoteName::D ||
            noteName == Note::NoteName::F || noteName == Note::NoteName::G ||
            noteName == Note::NoteName::A) {
            
            // Find corresponding black key
            for (auto& blackKey : keys_) {
                if (blackKey->getColor() != PianoKey::KeyColor::Black) continue;
                
                // Check if black key should be positioned here
                // (This is simplified; full implementation would track position better)
            }
        }
        
        whiteKeyX += whiteKeyWidth;
        whiteIdx++;
    }
}
