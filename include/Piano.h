#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cmath>

/**
 * @class Note
 * @brief Represents a musical note with frequency and duration
 */
class Note {
public:
    enum class NoteName {
        C, CSharp, D, DSharp, E, F, FSharp, G, GSharp, A, ASharp, B
    };

    Note(NoteName name, int octave, double frequency, double duration = 0.2);
    
    NoteName getName() const { return name_; }
    int getOctave() const { return octave_; }
    double getFrequency() const { return frequency_; }
    double getDuration() const { return duration_; }
    std::string getLabel() const;
    
    static double calculateFrequency(NoteName name, int octave);
    
private:
    NoteName name_;
    int octave_;
    double frequency_;
    double duration_;
};

/**
 * @class PianoKey
 * @brief Represents a single piano key
 */
class PianoKey {
public:
    enum class KeyColor { White, Black };
    
    PianoKey(const Note& note, KeyColor color, int visualIndex);
    
    Note& getNote() { return note_; }
    const Note& getNote() const { return note_; }
    KeyColor getColor() const { return color_; }
    int getVisualIndex() const { return visualIndex_; }
    
    bool isPressed() const { return isPressed_; }
    void setPressed(bool pressed) { isPressed_ = pressed; }
    
    // UI positioning
    int getScreenX() const { return screenX_; }
    int getScreenY() const { return screenY_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    
    void setScreenPosition(int x, int y);
    void setDimensions(int width, int height);
    
private:
    Note note_;
    KeyColor color_;
    int visualIndex_;
    bool isPressed_;
    
    int screenX_ = 0;
    int screenY_ = 0;
    int width_ = 60;
    int height_ = 300;
};

/**
 * @class Piano
 * @brief Main piano engine managing all 88 keys
 */
class Piano {
public:
    Piano();
    ~Piano();
    
    // Key access
    PianoKey* getPianoKey(const Note::NoteName& name, int octave);
    const std::vector<std::unique_ptr<PianoKey>>& getKeys() const { return keys_; }
    
    // Key state management
    void pressKey(const Note::NoteName& name, int octave);
    void releaseKey(const Note::NoteName& name, int octave);
    void releaseAllKeys();
    
    // Utility
    int getKeyCount() const { return keys_.size(); }
    int getWhiteKeyCount() const { return whiteKeyCount_; }
    int getBlackKeyCount() const { return blackKeyCount_; }
    
    // UI layout
    void layoutKeys(int windowWidth, int windowHeight);
    
private:
    std::vector<std::unique_ptr<PianoKey>> keys_;
    std::map<std::string, PianoKey*> keyLookup_;
    
    int whiteKeyCount_ = 52;
    int blackKeyCount_ = 36;
    
    void initializeKeys();
    static bool isBlackKey(Note::NoteName name);
};
