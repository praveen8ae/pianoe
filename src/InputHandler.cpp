#include "InputHandler.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <set>

InputHandler::InputHandler(Piano* piano)
    : piano_(piano), eventCallback_(nullptr) {
    setupDefaultKeyboardMap();
}

InputHandler::~InputHandler() = default;

void InputHandler::setupDefaultKeyboardMap() {
    // Row 1: C through B (lower octave)
    mapKeyToNote(XK_a, Note::NoteName::C, 4);
    mapKeyToNote(XK_w, Note::NoteName::CSharp, 4);
    mapKeyToNote(XK_s, Note::NoteName::D, 4);
    mapKeyToNote(XK_e, Note::NoteName::DSharp, 4);
    mapKeyToNote(XK_d, Note::NoteName::E, 4);
    mapKeyToNote(XK_f, Note::NoteName::F, 4);
    mapKeyToNote(XK_t, Note::NoteName::FSharp, 4);
    mapKeyToNote(XK_g, Note::NoteName::G, 4);
    mapKeyToNote(XK_y, Note::NoteName::GSharp, 4);
    mapKeyToNote(XK_h, Note::NoteName::A, 4);
    mapKeyToNote(XK_u, Note::NoteName::ASharp, 4);
    mapKeyToNote(XK_j, Note::NoteName::B, 4);
    
    // Row 2: C through B (higher octave)
    mapKeyToNote(XK_k, Note::NoteName::C, 5);
    mapKeyToNote(XK_o, Note::NoteName::CSharp, 5);
    mapKeyToNote(XK_l, Note::NoteName::D, 5);
}

void InputHandler::mapKeyToNote(KeySym keySym, Note::NoteName note, int octave) {
    keyMap_[keySym] = std::make_pair(note, octave);
}

Note::NoteName InputHandler::getKeyNote(KeySym keySym) const {
    auto it = keyMap_.find(keySym);
    return (it != keyMap_.end()) ? it->second.first : Note::NoteName::C;
}

int InputHandler::getKeyOctave(KeySym keySym) const {
    auto it = keyMap_.find(keySym);
    return (it != keyMap_.end()) ? it->second.second : 4;
}

void InputHandler::handleXEvent(XEvent& event) {
    switch (event.type) {
        case 2: { // KeyPress
            KeySym keySym = XLookupKeysym(&event.xkey, 0);
            handleKeyPress(keySym);
            break;
        }
        case 3: { // KeyRelease
            KeySym keySym = XLookupKeysym(&event.xkey, 0);
            handleKeyRelease(keySym);
            break;
        }
        case ButtonPress: {
            handleMouseClick(event.xbutton.x, event.xbutton.y);
            break;
        }
        case ButtonRelease: {
            handleMouseRelease(event.xbutton.x, event.xbutton.y);
            break;
        }
        default:
            break;
    }
}

void InputHandler::handleKeyPress(KeySym keySym) {
    // Check if key already pressed (ignore key repeat)
    if (pressedKeys_.count(keySym) > 0) {
        return;
    }
    
    pressedKeys_.insert(keySym);
    
    // Special keys
    if (keySym == XK_z || keySym == XK_Z) {
        // Octave down
        return;
    }
    if (keySym == XK_x || keySym == XK_X) {
        // Octave up
        return;
    }
    if (keySym == XK_space) {
        // Sustain pedal
        return;
    }
    
    // Check if it's a note key
    auto it = keyMap_.find(keySym);
    if (it != keyMap_.end()) {
        Note::NoteName note = it->second.first;
        int octave = it->second.second;
        
        piano_->pressKey(note, octave);
        
        // Call event callback if registered
        if (eventCallback_) {
            InputEvent event;
            event.type = EventType::KeyDown;
            event.keySym = keySym;
            eventCallback_(event, *piano_);
        }
    }
}

void InputHandler::handleKeyRelease(KeySym keySym) {
    pressedKeys_.erase(keySym);
    
    // Check if it's a note key
    auto it = keyMap_.find(keySym);
    if (it != keyMap_.end()) {
        Note::NoteName note = it->second.first;
        int octave = it->second.second;
        
        piano_->releaseKey(note, octave);
        
        // Call event callback if registered
        if (eventCallback_) {
            InputEvent event;
            event.type = EventType::KeyUp;
            event.keySym = keySym;
            eventCallback_(event, *piano_);
        }
    }
}

void InputHandler::handleMouseClick(int x, int y) {
    // Find which key was clicked (would need renderer to determine this)
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
