#pragma once

#include "Piano.h"
#include <SDL2/SDL.h>
#include <memory>
#include <string>

/**
 * @class UIRenderer
 * @brief Handles all rendering using SDL2 graphics (cross-platform)
 */
class UIRenderer {
public:
    struct Color {
        uint8_t r, g, b;
        
        Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0)
            : r(r), g(g), b(b) {}
        
        static Color White() { return Color(255, 255, 255); }
        static Color Black() { return Color(0, 0, 0); }
        static Color Gray() { return Color(128, 128, 128); }
        static Color DarkGray() { return Color(64, 64, 64); }
        static Color Red() { return Color(255, 0, 0); }
        static Color Blue() { return Color(0, 0, 255); }
        static Color Green() { return Color(0, 255, 0); }
    };
    
    UIRenderer(int windowWidth, int windowHeight, const std::string& title);
    ~UIRenderer();
    
    // Window management
    bool isWindowOpen() const;
    SDL_Window* getWindow() const { return window_; }
    SDL_Renderer* getRenderer() const { return renderer_; }
    
    // Rendering
    void clear();
    void renderPiano(const Piano& piano);
    void renderText(int x, int y, const std::string& text, Color color = Color::Black());
    void renderRectangle(int x, int y, int width, int height, Color color, bool filled = true);
    void renderCircle(int x, int y, int radius, Color color, bool filled = true);
    void flush();
    
    // Drawing primitives
    void drawLine(int x1, int y1, int x2, int y2, Color color);
    void drawKey(const PianoKey& key);
    
    // Window properties
    int getWindowWidth() const { return windowWidth_; }
    int getWindowHeight() const { return windowHeight_; }
    void setWindowSize(int width, int height);
    
    // Coordinate utilities
    PianoKey* getKeyAt(int x, int y, const Piano& piano);
    
    // Status display
    void renderStatus(const std::string& status);
    void renderRecordingStatus(bool isRecording, double duration);
    
private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    
    int windowWidth_;
    int windowHeight_;
    
    // Helper methods
    void renderSDLColor(const Color& color);
};

