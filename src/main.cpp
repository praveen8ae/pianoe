#include "Application.h"
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[]) {
    try {
        // Create and initialize application
        PianoApplication app(1400, 500);
        
        if (!app.initialize()) {
            std::cerr << "Failed to initialize piano application" << std::endl;
            return 1;
        }
        
        // Run the application
        app.run();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
