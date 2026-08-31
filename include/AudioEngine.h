#pragma once

#include "Piano.h"
#include <memory>

/**
 * @class AudioSynthesizer
 * @brief Abstract base class for audio synthesis
 * This allows for different audio backends to be plugged in
 */
class AudioSynthesizer {
public:
    virtual ~AudioSynthesizer() = default;
    
    /**
     * Initialize the audio system
     * @return true if successful
     */
    virtual bool initialize() = 0;
    
    /**
     * Shutdown the audio system
     */
    virtual void shutdown() = 0;
    
    /**
     * Start playing a note
     * @param frequency Note frequency in Hz
     * @param velocity Volume level (0.0 to 1.0)
     */
    virtual void noteOn(double frequency, double velocity = 1.0) = 0;
    
    /**
     * Stop playing a note
     * @param frequency Note frequency in Hz
     */
    virtual void noteOff(double frequency) = 0;
    
    /**
     * Stop all playing notes
     */
    virtual void noteOffAll() = 0;
    
    /**
     * Set the main volume
     * @param volume Level from 0.0 to 1.0
     */
    virtual void setVolume(double volume) = 0;
    
    /**
     * Set the waveform type
     * @param waveform 0=sine, 1=square, 2=sawtooth, 3=triangle
     */
    virtual void setWaveform(int waveform) = 0;
    
    /**
     * Get current status
     */
    virtual std::string getStatus() const = 0;
    
    /**
     * Check if audio is initialized
     */
    virtual bool isInitialized() const = 0;
};

/**
 * @class SilentSynthesizer
 * @brief No-op audio synthesizer (placeholder until real audio is implemented)
 */
class SilentSynthesizer : public AudioSynthesizer {
public:
    bool initialize() override { return true; }
    void shutdown() override {}
    void noteOn(double frequency, double velocity = 1.0) override {}
    void noteOff(double frequency) override {}
    void noteOffAll() override {}
    void setVolume(double volume) override { volume_ = volume; }
    void setWaveform(int waveform) override { waveform_ = waveform; }
    std::string getStatus() const override { return "Silent (audio not yet implemented)"; }
    bool isInitialized() const override { return true; }
    
private:
    double volume_ = 1.0;
    int waveform_ = 0;
};

/**
 * @class AudioEngine
 * @brief Manages audio synthesis and note playback
 */
class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();
    
    // Initialize with a specific synthesizer
    bool initialize(std::unique_ptr<AudioSynthesizer> synthesizer);
    void shutdown();
    
    // Play a note
    void playNote(const Note& note, double velocity = 1.0);
    void stopNote(double frequency);
    void stopAllNotes();
    
    // Settings
    void setVolume(double volume);
    void setWaveform(int waveform);
    
    // Status
    bool isInitialized() const;
    std::string getStatus() const;
    
private:
    std::unique_ptr<AudioSynthesizer> synthesizer_;
};
