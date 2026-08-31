#pragma once

#include "Piano.h"
#include "InputHandler.h"
#include "Recorder.h"
#include "AudioEngine.h"
#include "UIRenderer.h"
#include <memory>

/**
 * @class PianoApplication
 * @brief Main application class coordinating all components (cross-platform with SDL2)
 */
class PianoApplication {
public:
    PianoApplication(int windowWidth = 1400, int windowHeight = 500);
    ~PianoApplication();
    
    /**
     * Initialize the application
     * @return true if successful
     */
    bool initialize();
    
    /**
     * Run the main application loop
     */
    void run();
    
    /**
     * Request application shutdown
     */
    void shutdown();
    
    // Accessors
    Piano& getPiano() { return *piano_; }
    InputHandler& getInputHandler() { return *inputHandler_; }
    Recorder& getRecorder() { return *recorder_; }
    AudioEngine& getAudioEngine() { return *audioEngine_; }
    UIRenderer& getRenderer() { return *renderer_; }
    
    bool isRunning() const { return isRunning_; }
    
private:
    // Core components
    std::unique_ptr<Piano> piano_;
    std::unique_ptr<InputHandler> inputHandler_;
    std::unique_ptr<Recorder> recorder_;
    std::unique_ptr<AudioEngine> audioEngine_;
    std::unique_ptr<UIRenderer> renderer_;
    
    // Application state
    bool isRunning_ = false;
    int windowWidth_;
    int windowHeight_;
    
    // Main loop
    void update();
    void render();
    void handleEvents();
    
    // UI state
    bool showHelp_ = false;
    std::string statusMessage_;
};
