#include "Recorder.h"
#include <iostream>
#include <iomanip>

Recorder::Recorder(Piano* piano)
    : piano_(piano) {
}

Recorder::~Recorder() = default;

void Recorder::startRecording() {
    if (isRecording_) return;
    
    isRecording_ = true;
    recordedEvents_.clear();
    recordingDuration_ = 0.0;
    recordStartTime_ = std::chrono::high_resolution_clock::now();
}

void Recorder::stopRecording() {
    if (!isRecording_) return;
    
    isRecording_ = false;
    recordingDuration_ = getRecordingTime();
}

double Recorder::getRecordingTime() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - recordStartTime_
    );
    return duration.count() / 1000.0;
}

void Recorder::recordNoteOn(Note::NoteName name, int octave, double velocity) {
    if (!isRecording_) return;
    
    double timestamp = getRecordingTime();
    recordedEvents_.emplace_back(NoteEvent::Type::NoteOn, name, octave, timestamp, velocity);
}

void Recorder::recordNoteOff(Note::NoteName name, int octave) {
    if (!isRecording_) return;
    
    double timestamp = getRecordingTime();
    recordedEvents_.emplace_back(NoteEvent::Type::NoteOff, name, octave, timestamp);
}

void Recorder::startPlayback() {
    if (recordedEvents_.empty()) return;
    if (isPlaying_) return;
    
    isPlaying_ = true;
    playbackPosition_ = 0.0;
    nextEventIndex_ = 0;
    playbackStartTime_ = std::chrono::high_resolution_clock::now();
}

void Recorder::stopPlayback() {
    if (!isPlaying_) return;
    
    isPlaying_ = false;
    piano_->releaseAllKeys();  // Release all playing notes
}

double Recorder::getPlaybackTime() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - playbackStartTime_
    );
    return duration.count() / 1000.0;
}

void Recorder::updatePlayback() {
    if (!isPlaying_ || recordedEvents_.empty()) return;
    
    playbackPosition_ = getPlaybackTime();
    
    // Process all events up to current playback position
    while (nextEventIndex_ < recordedEvents_.size()) {
        const NoteEvent& event = recordedEvents_[nextEventIndex_];
        
        if (event.timestamp <= playbackPosition_) {
            if (event.type == NoteEvent::Type::NoteOn) {
                piano_->pressKey(event.noteName, event.octave);
            } else {
                piano_->releaseKey(event.noteName, event.octave);
            }
            nextEventIndex_++;
        } else {
            break;  // Wait for more time to pass
        }
    }
    
    // Stop playback if we've reached the end
    if (nextEventIndex_ >= recordedEvents_.size()) {
        stopPlayback();
    }
}

void Recorder::clearRecording() {
    stopRecording();
    stopPlayback();
    recordedEvents_.clear();
    recordingDuration_ = 0.0;
}

bool Recorder::saveToFile(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    
    // Write header with metadata
    file << "PIANO_REC\n";
    file << "Title: " << title_ << "\n";
    file << "Author: " << author_ << "\n";
    file << "Tempo: " << tempo_ << "\n";
    file << "Duration: " << recordingDuration_ << "\n";
    file << "Events: " << recordedEvents_.size() << "\n";
    file << "---\n";
    
    // Write events
    for (const auto& event : recordedEvents_) {
        file << (event.type == NoteEvent::Type::NoteOn ? "ON" : "OFF");
        file << " " << static_cast<int>(event.noteName);
        file << " " << event.octave;
        file << " " << std::fixed << std::setprecision(3) << event.timestamp;
        file << " " << event.velocity << "\n";
    }
    
    file.close();
    return true;
}

bool Recorder::loadFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    
    clearRecording();
    
    std::string line;
    
    // Read header
    if (!std::getline(file, line) || line != "PIANO_REC") {
        std::cerr << "Invalid file format" << std::endl;
        return false;
    }
    
    // Read metadata
    while (std::getline(file, line)) {
        if (line == "---") break;
        
        if (line.substr(0, 7) == "Title: ") {
            title_ = line.substr(7);
        } else if (line.substr(0, 8) == "Author: ") {
            author_ = line.substr(8);
        } else if (line.substr(0, 7) == "Tempo: ") {
            tempo_ = std::stod(line.substr(7));
        }
    }
    
    // Read events
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        std::string typeStr;
        int noteIdx, octave;
        double timestamp, velocity;
        
        iss >> typeStr >> noteIdx >> octave >> timestamp >> velocity;
        
        NoteEvent::Type type = (typeStr == "ON") ? 
            NoteEvent::Type::NoteOn : NoteEvent::Type::NoteOff;
        
        recordedEvents_.emplace_back(
            type,
            static_cast<Note::NoteName>(noteIdx),
            octave,
            timestamp,
            velocity
        );
    }
    
    file.close();
    return true;
}
