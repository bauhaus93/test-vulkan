#include "File.h"


std::vector<char> ReadFile(const std::string& filename) {
    std::ifstream f {filename, std::ios::ate | std::ios::binary };
    std::vector<char> buffer;

    if (!f.is_open()) {
        throw std::runtime_error("Could not open file " + filename);
    }

    size_t fSize = f.tellg();
    buffer.resize(fSize);
    f.seekg(0);
    f.read(buffer.data(), fSize);
    f.close();
    return buffer;
}
