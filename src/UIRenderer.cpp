#include "UIRenderer.h"
#include <iostream>
#include <cstring>

UIRenderer::UIRenderer(int windowWidth, int windowHeight, const std::string& title)
    : windowWidth_(windowWidth), windowHeight_(windowHeight) {
    
    display_ = XOpenDisplay(nullptr);
    if (!display_) {
        throw std::runtime_error("Failed to open X11 display");
    }
    
    screenNumber_ = DefaultScreen(display_);
    Window rootWindow = RootWindow(display_, screenNumber_);
    
    // Create window
    XSetWindowAttributes attributes;
    attributes.background_pixel = WhitePixel(display_, screenNumber_);
    attributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | 
                            ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
    
    window_ = XCreateWindow(
        display_, rootWindow,
        0, 0, windowWidth_, windowHeight_, 0,
        DefaultDepth(display_, screenNumber_),
        InputOutput,
        DefaultVisual(display_, screenNumber_),
        CWBackPixel | CWEventMask,
        &attributes
    );
    
    if (!window_) {
        throw std::runtime_error("Failed to create X11 window");
    }
    
    // Set window properties
    XStoreName(display_, window_, title.c_str());
    
    // Create graphics context
    gc_ = XCreateGC(display_, window_, 0, nullptr);
    if (!gc_) {
        throw std::runtime_error("Failed to create graphics context");
    }
    
    // Setup colors
    colormap_ = DefaultColormap(display_, screenNumber_);
    colors_ = new XColor[256];
    
    // Load font
    font_ = XLoadQueryFont(display_, "-*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
    if (!font_) {
        font_ = XLoadQueryFont(display_, "fixed");
    }
    if (font_) {
        XSetFont(display_, gc_, font_->fid);
    }
    
    // Map window
    XMapWindow(display_, window_);
    
    // Flush and process events
    XFlush(display_);
    XSync(display_, False);
}

UIRenderer::~UIRenderer() {
    if (font_) {
        XFreeFont(display_, font_);
    }
    if (gc_) {
        XFreeGC(display_, gc_);
    }
    if (window_) {
        XDestroyWindow(display_, window_);
    }
    if (colors_) {
        delete[] colors_;
    }
    if (display_) {
        XCloseDisplay(display_);
    }
}

bool UIRenderer::isWindowOpen() const {
    return window_ != 0 && display_ != nullptr;
}

void UIRenderer::clear() {
    XSetForeground(display_, gc_, WhitePixel(display_, screenNumber_));
    XFillRectangle(display_, window_, gc_, 0, 0, windowWidth_, windowHeight_);
}

void UIRenderer::renderPiano(const Piano& piano) {
    // Render white keys first
    for (const auto& key : piano.getKeys()) {
        if (key->getColor() == PianoKey::KeyColor::White) {
            drawKey(*key);
        }
    }
    
    // Render black keys on top
    for (const auto& key : piano.getKeys()) {
        if (key->getColor() == PianoKey::KeyColor::Black) {
            drawKey(*key);
        }
    }
}

void UIRenderer::drawKey(const PianoKey& key) {
    int x = key.getScreenX();
    int y = key.getScreenY();
    int w = key.getWidth();
    int h = key.getHeight();
    
    Color keyColor = (key.getColor() == PianoKey::KeyColor::White) ? Color::White() : Color::Black();
    Color borderColor = Color::Black();
    Color pressedColor = Color::Gray();
    
    // Draw key
    if (key.isPressed()) {
        XSetForeground(display_, gc_, allocateColor(pressedColor));
    } else {
        XSetForeground(display_, gc_, allocateColor(keyColor));
    }
    
    XFillRectangle(display_, window_, gc_, x, y, w, h);
    
    // Draw border
    XSetForeground(display_, gc_, allocateColor(borderColor));
    XSetLineAttributes(display_, gc_, 2, LineSolid, CapButt, JoinBevel);
    XDrawRectangle(display_, window_, gc_, x, y, w, h);
    
    // Draw note label (simplified)
    if (key.getColor() == PianoKey::KeyColor::White && font_) {
        std::string label = key.getNote().getLabel();
        int textX = x + w / 2 - label.length() * 3;
        int textY = y + h - 10;
        
        XSetForeground(display_, gc_, allocateColor(Color::Black()));
        XDrawString(display_, window_, gc_, textX, textY, label.c_str(), label.length());
    }
}

void UIRenderer::renderText(int x, int y, const std::string& text, Color color) {
    if (!font_) return;
    
    XSetForeground(display_, gc_, allocateColor(color));
    XDrawString(display_, window_, gc_, x, y, text.c_str(), text.length());
}

void UIRenderer::renderRectangle(int x, int y, int width, int height, Color color, bool filled) {
    XSetForeground(display_, gc_, allocateColor(color));
    
    if (filled) {
        XFillRectangle(display_, window_, gc_, x, y, width, height);
    } else {
        XDrawRectangle(display_, window_, gc_, x, y, width, height);
    }
}

void UIRenderer::renderCircle(int x, int y, int radius, Color color, bool filled) {
    XSetForeground(display_, gc_, allocateColor(color));
    
    if (filled) {
        XFillArc(display_, window_, gc_, x - radius, y - radius, radius * 2, radius * 2, 0, 360 * 64);
    } else {
        XDrawArc(display_, window_, gc_, x - radius, y - radius, radius * 2, radius * 2, 0, 360 * 64);
    }
}

void UIRenderer::drawLine(int x1, int y1, int x2, int y2, Color color) {
    XSetForeground(display_, gc_, allocateColor(color));
    XDrawLine(display_, window_, gc_, x1, y1, x2, y2);
}

void UIRenderer::flush() {
    XFlush(display_);
}

void UIRenderer::setWindowSize(int width, int height) {
    windowWidth_ = width;
    windowHeight_ = height;
    XResizeWindow(display_, window_, width, height);
}

unsigned long UIRenderer::allocateColor(const Color& color) {
    XColor col;
    col.red = color.r;
    col.green = color.g;
    col.blue = color.b;
    col.flags = DoRed | DoGreen | DoBlue;
    
    if (XAllocColor(display_, colormap_, &col)) {
        return col.pixel;
    }
    
    return WhitePixel(display_, screenNumber_);
}

unsigned long UIRenderer::getXColor(const Color& color) {
    return allocateColor(color);
}

PianoKey* UIRenderer::getKeyAt(int x, int y, const Piano& piano) {
    for (const auto& key : piano.getKeys()) {
        if (x >= key->getScreenX() && x <= key->getScreenX() + key->getWidth() &&
            y >= key->getScreenY() && y <= key->getScreenY() + key->getHeight()) {
            return key.get();
        }
    }
    return nullptr;
}

void UIRenderer::renderStatus(const std::string& status) {
    if (!font_) return;
    
    int statusY = windowHeight_ - 20;
    XSetForeground(display_, gc_, allocateColor(Color::Black()));
    XDrawString(display_, window_, gc_, 10, statusY, status.c_str(), status.length());
}

void UIRenderer::renderRecordingStatus(bool isRecording, double duration) {
    std::string text = isRecording ? "● Recording: " : "○ Ready";
    text += std::to_string(static_cast<int>(duration)) + "s";
    renderStatus(text);
}
