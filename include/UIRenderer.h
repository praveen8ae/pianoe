#pragma once

#include "Piano.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <memory>
#include <string>

/**
 * @class UIRenderer
 * @brief Handles all rendering using X11 graphics
 */
class UIRenderer {
public:
    struct Color {
        unsigned short r, g, b;
        
        Color(unsigned short r = 0, unsigned short g = 0, unsigned short b = 0)
            : r(r), g(g), b(b) {}
        
        static Color White() { return Color(65535, 65535, 65535); }
        static Color Black() { return Color(0, 0, 0); }
        static Color Gray() { return Color(32768, 32768, 32768); }
        static Color DarkGray() { return Color(16384, 16384, 16384); }
        static Color Red() { return Color(65535, 0, 0); }
        static Color Blue() { return Color(0, 0, 65535); }
        static Color Green() { return Color(0, 65535, 0); }
    };
    
    UIRenderer(int windowWidth, int windowHeight, const std::string& title);
    ~UIRenderer();
    
    // Window management
    bool isWindowOpen() const;
    Window getWindow() const { return window_; }
    Display* getDisplay() const { return display_; }
    
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
    Display* display_;
    Window window_;
    GC gc_;  // Graphics Context
    Colormap colormap_;
    XColor* colors_;
    
    int windowWidth_;
    int windowHeight_;
    int screenNumber_;
    
    // Font support
    XFontStruct* font_;
    
    // Color mapping
    unsigned long allocateColor(const Color& color);
    unsigned long getXColor(const Color& color);
    
    void setupGraphics();
};
