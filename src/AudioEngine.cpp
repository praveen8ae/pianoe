#include "AudioEngine.h"

AudioEngine::AudioEngine() = default;

AudioEngine::~AudioEngine() {
    shutdown();
}

bool AudioEngine::initialize(std::unique_ptr<AudioSynthesizer> synthesizer) {
    synthesizer_ = std::move(synthesizer);
    if (!synthesizer_) {
        synthesizer_ = std::make_unique<SilentSynthesizer>();
    }
    return synthesizer_->initialize();
}

void AudioEngine::shutdown() {
    if (synthesizer_) {
        synthesizer_->shutdown();
        synthesizer_.reset();
    }
}

void AudioEngine::playNote(const Note& note, double velocity) {
    if (synthesizer_) {
        synthesizer_->noteOn(note.getFrequency(), velocity);
    }
}

void AudioEngine::stopNote(double frequency) {
    if (synthesizer_) {
        synthesizer_->noteOff(frequency);
    }
}

void AudioEngine::stopAllNotes() {
    if (synthesizer_) {
        synthesizer_->noteOffAll();
    }
}

void AudioEngine::setVolume(double volume) {
    if (synthesizer_) {
        synthesizer_->setVolume(volume);
    }
}

void AudioEngine::setWaveform(int waveform) {
    if (synthesizer_) {
        synthesizer_->setWaveform(waveform);
    }
}

bool AudioEngine::isInitialized() const {
    return synthesizer_ && synthesizer_->isInitialized();
}

std::string AudioEngine::getStatus() const {
    if (!synthesizer_) {
        return "No audio engine";
    }
    return synthesizer_->getStatus();
}
