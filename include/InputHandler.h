#pragma once

#include "Piano.h"
#include <SDL2/SDL.h>
#include <functional>
#include <map>
#include <set>
#include <memory>

/**
 * @class InputHandler
 * @brief Manages keyboard and mouse input, converts them to piano key presses (cross-platform with SDL2)
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
        SDL_Keysym sdlKeySym;
    };
    
    using EventCallback = std::function<void(const InputEvent&, Piano&)>;
    
    InputHandler(Piano* piano);
    ~InputHandler();
    
    // Process SDL events
    void handleSDLEvent(const SDL_Event& event);
    
    // Register callback for custom event handling
    void setEventCallback(EventCallback callback) {
        eventCallback_ = callback;
    }
    
    // Get currently active keys (keyboard mapping)
    Note::NoteName getKeyNote(SDL_Keycode keycode) const;
    int getKeyOctave(SDL_Keycode keycode) const;
    
    // Setup keyboard map (customizable)
    void setupDefaultKeyboardMap();
    void mapKeyToNote(SDL_Keycode keycode, Note::NoteName note, int octave);
    
private:
    Piano* piano_;
    EventCallback eventCallback_;
    
    // Keyboard mapping: SDL keycode -> (note, octave)
    std::map<SDL_Keycode, std::pair<Note::NoteName, int>> keyMap_;
    
    // Track pressed keys to avoid repeats
    std::set<SDL_Keycode> pressedKeys_;
    
    // Handle specific events
    void handleKeyPress(SDL_Keycode keycode);
    void handleKeyRelease(SDL_Keycode keycode);
    void handleMouseClick(int x, int y);
    void handleMouseRelease(int x, int y);
};
