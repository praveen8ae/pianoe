#pragma once

#include "Piano.h"
#include <vector>
#include <chrono>
#include <fstream>
#include <memory>

/**
 * @struct NoteEvent
 * @brief Represents a recorded note event with timing
 */
struct NoteEvent {
    enum class Type {
        NoteOn,
        NoteOff
    };
    
    Type type;
    Note::NoteName noteName;
    int octave;
    double timestamp; // seconds from start of recording
    double velocity;  // 0.0 to 1.0
    
    NoteEvent(Type t, Note::NoteName n, int o, double ts, double v = 1.0)
        : type(t), noteName(n), octave(o), timestamp(ts), velocity(v) {}
};

/**
 * @class Recorder
 * @brief Records and plays back piano performances
 */
class Recorder {
public:
    Recorder(Piano* piano);
    ~Recorder();
    
    // Recording control
    void startRecording();
    void stopRecording();
    bool isRecording() const { return isRecording_; }
    
    // Record events
    void recordNoteOn(Note::NoteName name, int octave, double velocity = 1.0);
    void recordNoteOff(Note::NoteName name, int octave);
    
    // Playback control
    void startPlayback();
    void stopPlayback();
    bool isPlaying() const { return isPlaying_; }
    
    // Update playback (call this regularly, typically in update loop)
    void updatePlayback();
    
    // File I/O
    bool saveToFile(const std::string& filename);
    bool loadFromFile(const std::string& filename);
    
    // Access recorded data
    const std::vector<NoteEvent>& getRecordedEvents() const { return recordedEvents_; }
    double getRecordingDuration() const { return recordingDuration_; }
    void clearRecording();
    
    // Metadata
    void setTitle(const std::string& title) { title_ = title; }
    void setAuthor(const std::string& author) { author_ = author; }
    void setTempo(double bpm) { tempo_ = bpm; }
    
    std::string getTitle() const { return title_; }
    std::string getAuthor() const { return author_; }
    double getTempo() const { return tempo_; }
    
private:
    Piano* piano_;
    
    // Recording state
    std::vector<NoteEvent> recordedEvents_;
    bool isRecording_ = false;
    std::chrono::high_resolution_clock::time_point recordStartTime_;
    double recordingDuration_ = 0.0;
    
    // Playback state
    bool isPlaying_ = false;
    double playbackPosition_ = 0.0;
    std::chrono::high_resolution_clock::time_point playbackStartTime_;
    size_t nextEventIndex_ = 0;
    
    // Metadata
    std::string title_;
    std::string author_;
    double tempo_ = 120.0; // BPM
    
    double getRecordingTime();
    double getPlaybackTime();
};
