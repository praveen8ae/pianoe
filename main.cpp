#include <algorithm>
#include <array>
#include <cmath>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

constexpr double PI = 3.14159265358979323846;
constexpr int SAMPLE_RATE = 44100;

struct PianoKey {
    char key;
    std::string label;
    double frequency;
    bool black;
};

const std::array<PianoKey, 14> KEYBOARD = {{
    {'a', "C4", 261.63, false},
    {'w', "C#4", 277.18, true},
    {'s', "D4", 293.66, false},
    {'e', "D#4", 311.13, true},
    {'d', "E4", 329.63, false},
    {'f', "F4", 349.23, false},
    {'t', "F#4", 369.99, true},
    {'g', "G4", 392.00, false},
    {'y', "G#4", 415.30, true},
    {'h', "A4", 440.00, false},
    {'u', "A#4", 466.16, true},
    {'j', "B4", 493.88, false},
    {'k', "C5", 523.25, false},
    {'o', "C#5", 554.37, true}
}};

std::unordered_map<char, PianoKey> makeKeyMap() {
    std::unordered_map<char, PianoKey> map;
    for (const auto& key : KEYBOARD) {
        map[key.key] = key;
    }
    return map;
}

std::vector<double> generateTone(double frequency, double durationSeconds, int sampleRate = SAMPLE_RATE) {
    const size_t samples = static_cast<size_t>(durationSeconds * sampleRate);
    std::vector<double> tone(samples);

    for (size_t i = 0; i < samples; ++i) {
        const double t = static_cast<double>(i) / sampleRate;
        const double envelope = 0.35 * std::sin((PI * i) / samples);
        tone[i] = std::sin(2.0 * PI * frequency * t) * envelope;
    }

    return tone;
}

void appendTone(std::vector<double>& output, double frequency, double durationSeconds) {
    const auto tone = generateTone(frequency, durationSeconds);
    output.insert(output.end(), tone.begin(), tone.end());
}

void writeWav(const std::string& filename, const std::vector<double>& samples, int sampleRate = SAMPLE_RATE) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open output file: " + filename);
    }

    constexpr int bitsPerSample = 16;
    constexpr int channels = 1;
    const int bytesPerSample = bitsPerSample / 8;
    const int dataSize = static_cast<int>(samples.size() * bytesPerSample);
    const int riffSize = 36 + dataSize;

    file << "RIFF";
    file.write(reinterpret_cast<const char*>(&riffSize), 4);
    file << "WAVE";
    file << "fmt ";
    const int fmtChunkSize = 16;
    file.write(reinterpret_cast<const char*>(&fmtChunkSize), 4);

    const short audioFormat = 1;
    const short numChannels = channels;
    const int byteRate = sampleRate * channels * bytesPerSample;
    const short blockAlign = channels * bytesPerSample;
    const short sampleBits = bitsPerSample;

    file.write(reinterpret_cast<const char*>(&audioFormat), 2);
    file.write(reinterpret_cast<const char*>(&numChannels), 2);
    file.write(reinterpret_cast<const char*>(&sampleRate), 4);
    file.write(reinterpret_cast<const char*>(&byteRate), 4);
    file.write(reinterpret_cast<const char*>(&blockAlign), 2);
    file.write(reinterpret_cast<const char*>(&sampleBits), 2);

    file << "data";
    file.write(reinterpret_cast<const char*>(&dataSize), 4);

    for (double sample : samples) {
        const short value = static_cast<short>(std::max(-1.0, std::min(1.0, sample)) * 32767.0);
        file.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
}

void printKeyboard() {
    std::cout << "\nPiano keys:\n";
    std::cout << "White: A S D F G H J K\n";
    std::cout << "Black: W E T Y U O\n";
    std::cout << "Press a key, type 'demo', 'sequence', or 'quit'\n";
}

std::vector<double> buildDemoMelody() {
    std::vector<double> melody;
    const std::array<double, 12> demoNotes = {
        261.63, 329.63, 392.00, 523.25,
        392.00, 329.63, 293.66, 329.63,
        392.00, 440.00, 392.00, 329.63
    };

    for (double note : demoNotes) {
        appendTone(melody, note, 0.25);
    }
    return melody;
}

void playInteractive() {
    const auto keyMap = makeKeyMap();
    std::cout << "Interactive piano mode. Type keys and press Enter.\n";
    std::cout << "Examples: a, s, d, 'demo', 'sequence', 'quit'\n";

    while (true) {
        printKeyboard();
        std::cout << "> ";

        std::string input;
        std::getline(std::cin, input);

        if (input == "quit" || input == "exit") {
            std::cout << "Goodbye!\n";
            return;
        }

        if (input == "demo") {
            std::vector<double> melody = buildDemoMelody();
            writeWav("demo_piano.wav", melody);
            std::cout << "Saved demo to demo_piano.wav\n";
            continue;
        }

        if (input == "sequence") {
            std::vector<double> sequence;
            const std::array<char, 8> pattern = {'a', 's', 'd', 'f', 'g', 'f', 'd', 's'};
            for (char c : pattern) {
                auto it = keyMap.find(c);
                if (it != keyMap.end()) {
                    appendTone(sequence, it->second.frequency, 0.18);
                }
            }
            writeWav("sequence_piano.wav", sequence);
            std::cout << "Saved sequence to sequence_piano.wav\n";
            continue;
        }

        if (input.empty()) {
            continue;
        }

        std::vector<double> noteData;
        bool played = false;
        for (char c : input) {
            char lower = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            auto it = keyMap.find(lower);
            if (it != keyMap.end()) {
                const auto& key = it->second;
                appendTone(noteData, key.frequency, 0.2);
                std::cout << "Playing " << key.label << " (" << key.key << ")\n";
                played = true;
            }
        }

        if (played) {
            const std::string filename = "note_piano.wav";
            writeWav(filename, noteData);
            std::cout << "Saved current notes to " << filename << "\n";
        } else {
            std::cout << "Unknown key. Try A, W, S, E, D, F, T, G, Y, H, U, J, K or O.\n";
        }
    }
}

int main(int argc, char* argv[]) {
    try {
        std::string mode = "interactive";
        if (argc > 1) {
            mode = argv[1];
        }

        if (mode == "demo") {
            const auto melody = buildDemoMelody();
            writeWav("demo_piano.wav", melody);
            std::cout << "Created demo_piano.wav\n";
            return 0;
        }

        if (mode == "sequence") {
            std::vector<double> sequence;
            const std::array<char, 8> pattern = {'a', 's', 'd', 'f', 'g', 'f', 'd', 's'};
            auto keyMap = makeKeyMap();
            for (char c : pattern) {
                auto it = keyMap.find(c);
                if (it != keyMap.end()) {
                    appendTone(sequence, it->second.frequency, 0.18);
                }
            }
            writeWav("sequence_piano.wav", sequence);
            std::cout << "Created sequence_piano.wav\n";
            return 0;
        }

        playInteractive();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
