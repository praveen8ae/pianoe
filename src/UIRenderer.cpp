#include "UIRenderer.h"
#include <iostream>
#include <cstring>

UIRenderer::UIRenderer(int windowWidth, int windowHeight, const std::string& title)
    : windowWidth_(windowWidth), windowHeight_(windowHeight), window_(nullptr), renderer_(nullptr) {
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error(std::string("SDL initialization failed: ") + SDL_GetError());
    }
    
    // Create window
    window_ = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth_,
        windowHeight_,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    
    if (!window_) {
        SDL_Quit();
        throw std::runtime_error(std::string("Failed to create SDL window: ") + SDL_GetError());
    }
    
    // Create renderer
    renderer_ = SDL_CreateRenderer(
        window_,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    if (!renderer_) {
        SDL_DestroyWindow(window_);
        SDL_Quit();
        throw std::runtime_error(std::string("Failed to create SDL renderer: ") + SDL_GetError());
    }
    
    // Set render draw color to white
    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
    SDL_RenderClear(renderer_);
    SDL_RenderPresent(renderer_);
}

UIRenderer::~UIRenderer() {
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
    }
    if (window_) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

bool UIRenderer::isWindowOpen() const {
    return window_ != nullptr && renderer_ != nullptr;
}

void UIRenderer::clear() {
    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
    SDL_RenderClear(renderer_);
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
    
    // Draw key background
    if (key.isPressed()) {
        renderSDLColor(pressedColor);
    } else {
        renderSDLColor(keyColor);
    }
    
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer_, &rect);
    
    // Draw border
    renderSDLColor(borderColor);
    SDL_RenderDrawRect(renderer_, &rect);
    
    // Draw note label (simplified - text rendering would need SDL_ttf)
    if (key.getColor() == PianoKey::KeyColor::White) {
        std::string label = key.getNote().getLabel();
        // Text rendering would require SDL_ttf library
        // For now, we'll skip text rendering to keep it simple
    }
}

void UIRenderer::renderText(int x, int y, const std::string& text, Color color) {
    // Text rendering with SDL requires SDL_ttf library
    // For now, this is a placeholder
    // To implement: load font with TTF_OpenFont and render with TTF_RenderText_Solid
}

void UIRenderer::renderRectangle(int x, int y, int width, int height, Color color, bool filled) {
    renderSDLColor(color);
    
    SDL_Rect rect = {x, y, width, height};
    if (filled) {
        SDL_RenderFillRect(renderer_, &rect);
    } else {
        SDL_RenderDrawRect(renderer_, &rect);
    }
}

void UIRenderer::renderCircle(int x, int y, int radius, Color color, bool filled) {
    renderSDLColor(color);
    
    // SDL doesn't have a native circle drawing, so we'll draw it with lines (simplified)
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer_, x + dx, y + dy);
            }
        }
    }
}

void UIRenderer::drawLine(int x1, int y1, int x2, int y2, Color color) {
    renderSDLColor(color);
    SDL_RenderDrawLine(renderer_, x1, y1, x2, y2);
}

void UIRenderer::flush() {
    SDL_RenderPresent(renderer_);
}

void UIRenderer::setWindowSize(int width, int height) {
    windowWidth_ = width;
    windowHeight_ = height;
    SDL_SetWindowSize(window_, width, height);
}

void UIRenderer::renderSDLColor(const Color& color) {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, 255);
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
    // Status rendering would require SDL_ttf for text
    // For now, this is a placeholder
}

void UIRenderer::renderRecordingStatus(bool isRecording, double duration) {
    // Recording status rendering would require SDL_ttf for text
    // For now, this is a placeholder
}
