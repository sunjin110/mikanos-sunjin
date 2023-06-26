
#include "font.hpp"

extern const uint8_t _binary_hankaku_bin_start;
extern const uint8_t _binary_hankaku_bin_end;
extern const uint8_t _binary_hankaku_bin_size;

const uint8_t *GetFont(char c) {
    unsigned int index = 16 * static_cast<unsigned int>(c);
    if (index >= reinterpret_cast<uintptr_t>(&_binary_hankaku_bin_size)) {
        return nullptr;
    }
    return &_binary_hankaku_bin_start + index;
}

const uint8_t kFontA[16] = {
    0b00000000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00100100,
    0b00100100,
    0b00100100,
    0b00100100,
    0b01111110,
    0b01000010,
    0b01000010,
    0b01000010,
    0b11100111,
    0b00000000,
    0b00000000,
};

void WriteAscii(PixelWriter &writer, int x, int y, char c, const PixelColor &color) {
    const uint8_t *font = GetFont(c);
    if (font == nullptr) {
        return;
    }
    for (int dy = 0; dy < 16; dy++) {
        for (int dx = 0; dx < 8; dx++) {
            if ((font[dy] << dx) & 0x80u) {
                writer.Write(x + dx, y + dy, color);
            }
        }
    }
}

void WriteString(PixelWriter &writer, int x, int y, const char *s, const PixelColor &color) {
    for (int i = 0; s[i] != '\0'; i++) {
        WriteAscii(writer, x + 8 * i, y, s[i], color);
    }
}
