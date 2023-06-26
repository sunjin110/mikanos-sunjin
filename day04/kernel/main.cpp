#include <cstdint>
#include <cstddef>
#include "frame_buffer_config.hpp"

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

struct PixelColor {
    uint8_t red, green, blue;
};

class PixelWriter {
    public:
        PixelWriter(const FrameBufferConfig &config) : config_{config} {}
        virtual ~PixelWriter() = default; // デストラクター
        virtual void Write(int x, int y, const PixelColor &pixel_color) = 0;
    protected:
        // PixelAt 書き込む先のPixelDrawPointのindexを取得します
        uint8_t *PixelAt(int x, int y) {
            return config_.frame_buffer + 4 * (config_.pixels_per_scan_line * y + x);
        }
    private:
        const FrameBufferConfig &config_;
};

class RGBResv8BitPerColorPixelWriter : public PixelWriter {
    public:
        using PixelWriter::PixelWriter;
    
        virtual void Write(int x, int y, const PixelColor &pixel_color) override {
            uint8_t *pixel_draw_point = PixelAt(x, y);
            pixel_draw_point[0] = pixel_color.red;
            pixel_draw_point[1] = pixel_color.green;
            pixel_draw_point[2] = pixel_color.blue;
        }
};

class BGRResv8BitPerColorPixelWriter : public PixelWriter {
    public:
        using PixelWriter::PixelWriter;

        virtual void Write(int x, int y, const PixelColor &pixel_color) override {
            uint8_t *pixel_draw_point = PixelAt(x, y);
            pixel_draw_point[0] = pixel_color.blue;
            pixel_draw_point[1] = pixel_color.green;
            pixel_draw_point[2] = pixel_color.red;
        }
};

void Halt() {
    while (1) __asm__("hlt");
}

// 配置newの定義
void *operator new(size_t size, void *buf) {
    return buf;
}

void operator delete(void *obj) noexcept {}

PixelWriter *NewPixelWriter(const FrameBufferConfig &config, char *buf) {
    switch (config.pixel_format) {
        case kPixelRGBResv8BitPerColor:
            return new(buf)RGBResv8BitPerColorPixelWriter(config);
        case kPixelBGRResv8BitPerColor:
            return new(buf)BGRResv8BitPerColorPixelWriter(config);
        default:
            Halt();
    }
}

void WriteAscii(PixelWriter &writer, int x, int y, char c, const PixelColor &color) {
    if (c != 'A') {
        return;
    }

    for (int dy = 0; dy < 16; dy++) {
        for (int dx = 0; dx < 8; dx++) {
            if ((kFontA[dy] << dx) & 0x80u) {
                writer.Write(x + dx, y + dy, color);
            }
        }
    }
}

extern "C" void KernelMain(const FrameBufferConfig &frame_buffer_config) {
    // RGBもGBRも同じsizeなのでこれで問題ない
    char pixel_writer_class_buf[sizeof(RGBResv8BitPerColorPixelWriter)];
    PixelWriter *pixel_writer = NewPixelWriter(frame_buffer_config, pixel_writer_class_buf);
    
    // 白で塗りつぶす
    for (int x = 0; x < frame_buffer_config.horizontal_resolution; x++) {
        for (int y = 0; y < frame_buffer_config.vertical_resolution; y++) {
            pixel_writer->Write(x, y, PixelColor{.red = 255, .green = 255, .blue = 255});
        }
    }

    // 一箇所をgreenにする
    for (int x = 0; x < 200; x++) {
        for (int y = 0; y < 100; y++) {
            pixel_writer->Write(x + 100, y + 100, PixelColor{.red = 0, .green = 255, .blue = 0});
        }
    }

    // Aを出力する
    WriteAscii(*pixel_writer, 50, 50, 'A', {0, 0, 0});
    WriteAscii(*pixel_writer, 58, 50, 'A', {0, 0, 0});

    Halt();
}
