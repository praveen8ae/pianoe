#include "Application.h"
#include <iostream>
#include <thread>
#include <chrono>

PianoApplication::PianoApplication(int windowWidth, int windowHeight)
    : windowWidth_(windowWidth), windowHeight_(windowHeight), display_(nullptr) {
}

PianoApplication::~PianoApplication() {
    shutdown();
}

bool PianoApplication::initialize() {
    try {
        // Initialize display
        display_ = XOpenDisplay(nullptr);
        if (!display_) {
            std::cerr << "Failed to open X11 display" << std::endl;
            return false;
        }
        
        // Create components
        piano_ = std::make_unique<Piano>();
        renderer_ = std::make_unique<UIRenderer>(windowWidth_, windowHeight_, "Professional Piano - C++");
        inputHandler_ = std::make_unique<InputHandler>(piano_.get());
        recorder_ = std::make_unique<Recorder>(piano_.get());
        audioEngine_ = std::make_unique<AudioEngine>();
        
        // Initialize audio engine with silent synthesizer
        audioEngine_->initialize(std::make_unique<SilentSynthesizer>());
        
        // Layout the piano keys on screen
        piano_->layoutKeys(windowWidth_, windowHeight_);
        
        // Setup event callback
        inputHandler_->setEventCallback([this](const InputHandler::InputEvent& event, Piano& piano) {
            // Handle input events (can be extended for recording, etc.)
            if (event.type == InputHandler::EventType::KeyDown) {
                statusMessage_ = "Key pressed";
            }
        });
        
        isRunning_ = true;
        statusMessage_ = "Piano Ready - Press any key or click to play";
        
        std::cout << "✓ Piano application initialized" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "✗ Initialization failed: " << e.what() << std::endl;
        return false;
    }
}

void PianoApplication::run() {
    if (!isRunning_) {
        std::cerr << "Application not initialized" << std::endl;
        return;
    }
    
    std::cout << "🎹 Piano Application Running" << std::endl;
    std::cout << "   Press 'R' to record, 'P' to playback, 'Q' to quit" << std::endl;
    
    while (isRunning_) {
        handleEvents();
        update();
        render();
        
        // Small sleep to avoid busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
}

void PianoApplication::handleEvents() {
    Display* display = renderer_->getDisplay();
    Window window = renderer_->getWindow();
    
    XEvent event;
    while (XPending(display) > 0) {
        XNextEvent(display, &event);
        
        switch (event.type) {
            case 2: { // KeyPress
                KeySym keySym = XLookupKeysym(&event.xkey, 0);
                
                // Handle special global commands
                if (keySym == XK_q || keySym == XK_Q || keySym == XK_Escape) {
                    isRunning_ = false;
                    return;
                }
                if (keySym == XK_r || keySym == XK_R) {
                    if (!recorder_->isRecording()) {
                        recorder_->startRecording();
                        statusMessage_ = "Recording started...";
                        std::cout << "Recording started" << std::endl;
                    } else {
                        recorder_->stopRecording();
                        statusMessage_ = "Recording stopped. " + std::to_string(recorder_->getRecordedEvents().size()) + " events";
                        std::cout << "Recording stopped" << std::endl;
                    }
                    return;
                }
                if (keySym == XK_p || keySym == XK_P) {
                    if (!recorder_->isPlaying() && !recorder_->getRecordedEvents().empty()) {
                        recorder_->startPlayback();
                        statusMessage_ = "Playing back recording...";
                        std::cout << "Playback started" << std::endl;
                    } else {
                        recorder_->stopPlayback();
                        statusMessage_ = "Playback stopped";
                    }
                    return;
                }
                if (keySym == XK_h || keySym == XK_H) {
                    showHelp_ = !showHelp_;
                    return;
                }
                if (keySym == XK_c || keySym == XK_C) {
                    piano_->releaseAllKeys();
                    recorder_->clearRecording();
                    statusMessage_ = "Cleared";
                    return;
                }
                
                // Pass to input handler for note playing
                inputHandler_->handleXEvent(event);
                
                // Record if recording
                if (recorder_->isRecording()) {
                    KeySym sym = XLookupKeysym(&event.xkey, 0);
                    Note::NoteName note = inputHandler_->getKeyNote(sym);
                    int octave = inputHandler_->getKeyOctave(sym);
                    recorder_->recordNoteOn(note, octave);
                }
                break;
            }
            
            case 3: { // KeyRelease
                inputHandler_->handleXEvent(event);
                
                if (recorder_->isRecording()) {
                    KeySym sym = XLookupKeysym(&event.xkey, 0);
                    Note::NoteName note = inputHandler_->getKeyNote(sym);
                    int octave = inputHandler_->getKeyOctave(sym);
                    recorder_->recordNoteOff(note, octave);
                }
                break;
            }
            
            case ButtonPress: {
                // Handle mouse click on keys
                PianoKey* key = renderer_->getKeyAt(event.xbutton.x, event.xbutton.y, *piano_);
                if (key) {
                    piano_->pressKey(key->getNote().getName(), key->getNote().getOctave());
                    if (recorder_->isRecording()) {
                        recorder_->recordNoteOn(key->getNote().getName(), key->getNote().getOctave());
                    }
                }
                break;
            }
            
            case ButtonRelease: {
                // Handle mouse release
                PianoKey* key = renderer_->getKeyAt(event.xbutton.x, event.xbutton.y, *piano_);
                if (key) {
                    piano_->releaseKey(key->getNote().getName(), key->getNote().getOctave());
                    if (recorder_->isRecording()) {
                        recorder_->recordNoteOff(key->getNote().getName(), key->getNote().getOctave());
                    }
                }
                break;
            }
            
            case ClientMessage: {
                // Window close button
                isRunning_ = false;
                break;
            }
            
            case Expose: {
                // Window needs redraw
                render();
                break;
            }
            
            default:
                break;
        }
    }
}

void PianoApplication::update() {
    // Update playback if playing
    if (recorder_->isPlaying()) {
        recorder_->updatePlayback();
    }
}

void PianoApplication::render() {
    renderer_->clear();
    renderer_->renderPiano(*piano_);
    
    // Render UI info
    renderer_->renderStatus(statusMessage_);
    renderer_->renderRecordingStatus(recorder_->isRecording(), recorder_->getRecordingDuration());
    
    renderer_->flush();
}

void PianoApplication::shutdown() {
    if (isRunning_) {
        isRunning_ = false;
    }
    
    // Release resources
    if (recorder_) {
        recorder_->clearRecording();
    }
    if (audioEngine_) {
        audioEngine_->shutdown();
    }
    
    piano_.reset();
    inputHandler_.reset();
    recorder_.reset();
    audioEngine_.reset();
    renderer_.reset();
    
    if (display_) {
        XCloseDisplay(display_);
        display_ = nullptr;
    }
}
