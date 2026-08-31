#include "InputHandler.h"
#include <set>

InputHandler::InputHandler(Piano* piano)
    : piano_(piano), eventCallback_(nullptr) {
    setupDefaultKeyboardMap();
}

InputHandler::~InputHandler() = default;

void InputHandler::setupDefaultKeyboardMap() {
    // Row 1: C through B (lower octave) - QWERTY layout
    mapKeyToNote(SDLK_a, Note::NoteName::C, 4);
    mapKeyToNote(SDLK_w, Note::NoteName::CSharp, 4);
    mapKeyToNote(SDLK_s, Note::NoteName::D, 4);
    mapKeyToNote(SDLK_e, Note::NoteName::DSharp, 4);
    mapKeyToNote(SDLK_d, Note::NoteName::E, 4);
    mapKeyToNote(SDLK_f, Note::NoteName::F, 4);
    mapKeyToNote(SDLK_t, Note::NoteName::FSharp, 4);
    mapKeyToNote(SDLK_g, Note::NoteName::G, 4);
    mapKeyToNote(SDLK_y, Note::NoteName::GSharp, 4);
    mapKeyToNote(SDLK_h, Note::NoteName::A, 4);
    mapKeyToNote(SDLK_u, Note::NoteName::ASharp, 4);
    mapKeyToNote(SDLK_j, Note::NoteName::B, 4);
    
    // Row 2: C through B (higher octave)
    mapKeyToNote(SDLK_k, Note::NoteName::C, 5);
    mapKeyToNote(SDLK_o, Note::NoteName::CSharp, 5);
    mapKeyToNote(SDLK_l, Note::NoteName::D, 5);
}

void InputHandler::mapKeyToNote(SDL_Keycode keycode, Note::NoteName note, int octave) {
    keyMap_[keycode] = std::make_pair(note, octave);
}

Note::NoteName InputHandler::getKeyNote(SDL_Keycode keycode) const {
    auto it = keyMap_.find(keycode);
    return (it != keyMap_.end()) ? it->second.first : Note::NoteName::C;
}

int InputHandler::getKeyOctave(SDL_Keycode keycode) const {
    auto it = keyMap_.find(keycode);
    return (it != keyMap_.end()) ? it->second.second : 4;
}

void InputHandler::handleSDLEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            handleKeyPress(event.key.keysym.sym);
            break;
        case SDL_KEYUP:
            handleKeyRelease(event.key.keysym.sym);
            break;
        case SDL_MOUSEBUTTONDOWN:
            handleMouseClick(event.button.x, event.button.y);
            break;
        case SDL_MOUSEBUTTONUP:
            handleMouseRelease(event.button.x, event.button.y);
            break;
        default:
            break;
    }
}

void InputHandler::handleKeyPress(SDL_Keycode keycode) {
    // Check if key already pressed (ignore key repeat)
    if (pressedKeys_.count(keycode) > 0) {
        return;
    }
    
    pressedKeys_.insert(keycode);
    
    // Special keys
    if (keycode == SDLK_z) {
        // Octave down
        return;
    }
    if (keycode == SDLK_x) {
        // Octave up
        return;
    }
    if (keycode == SDLK_SPACE) {
        // Sustain pedal
        return;
    }
    
    // Check if it's a note key
    auto it = keyMap_.find(keycode);
    if (it != keyMap_.end()) {
        Note::NoteName note = it->second.first;
        int octave = it->second.second;
        
        piano_->pressKey(note, octave);
        
        // Call event callback if registered
        if (eventCallback_) {
            InputEvent event;
            event.type = EventType::KeyDown;
            event.sdlKeySym.sym = keycode;
            eventCallback_(event, *piano_);
        }
    }
}

void InputHandler::handleKeyRelease(SDL_Keycode keycode) {
    pressedKeys_.erase(keycode);
    
    // Check if it's a note key
    auto it = keyMap_.find(keycode);
    if (it != keyMap_.end()) {
        Note::NoteName note = it->second.first;
        int octave = it->second.second;
        
        piano_->releaseKey(note, octave);
        
        // Call event callback if registered
        if (eventCallback_) {
            InputEvent event;
            event.type = EventType::KeyUp;
            event.sdlKeySym.sym = keycode;
            eventCallback_(event, *piano_);
        }
    }
}

void InputHandler::handleMouseClick(int x, int y) {
    InputEvent event;
    event.type = EventType::MouseClick;
    event.mouseX = x;
    event.mouseY = y;
    
    if (eventCallback_) {
        eventCallback_(event, *piano_);
    }
}

void InputHandler::handleMouseRelease(int x, int y) {
    InputEvent event;
    event.type = EventType::MouseRelease;
    event.mouseX = x;
    event.mouseY = y;
    
    if (eventCallback_) {
        eventCallback_(event, *piano_);
    }
}
