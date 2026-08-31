#pragma once

#include "Piano.h"
#include <functional>
#include <map>
#include <set>
#include <memory>
#include <X11/Xlib.h>
#include <X11/keysym.h>

/**
 * @class InputHandler
 * @brief Manages keyboard and mouse input, converts them to piano key presses
 */
class InputHandler {
public:
    enum class EventType {
        KeyDown,
        KeyUp,
        MouseClick,
        MouseRelease,
        MouseMove
    };
    
    struct InputEvent {
        EventType type;
        int keyCode;
        int mouseX;
        int mouseY;
        KeySym keySym;
    };
    
    using EventCallback = std::function<void(const InputEvent&, Piano&)>;
    
    InputHandler(Piano* piano);
    ~InputHandler();
    
    // Process X11 events
    void handleXEvent(XEvent& event);
    
    // Register callback for custom event handling
    void setEventCallback(EventCallback callback) {
        eventCallback_ = callback;
    }
    
    // Get currently active keys (keyboard mapping)
    Note::NoteName getKeyNote(KeySym keySym) const;
    int getKeyOctave(KeySym keySym) const;
    
    // Setup keyboard map (customizable)
    void setupDefaultKeyboardMap();
    void mapKeyToNote(KeySym keySym, Note::NoteName note, int octave);
    
private:
    Piano* piano_;
    EventCallback eventCallback_;
    
    // Keyboard mapping: key symbol -> (note, octave)
    std::map<KeySym, std::pair<Note::NoteName, int>> keyMap_;
    
    // Track pressed keys to avoid repeats
    std::set<KeySym> pressedKeys_;
    
    // Handle specific events
    void handleKeyPress(KeySym keySym);
    void handleKeyRelease(KeySym keySym);
    void handleMouseClick(int x, int y);
    void handleMouseRelease(int x, int y);
};
