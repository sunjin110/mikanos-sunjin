#include "graphics.hpp"
#include "common.hpp"

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

void RGBResv8BitPerColorPixelWriter::Write(int x, int y, const PixelColor& pixel_color) {
    uint8_t *pixel_draw_point = PixelAt(x, y);
    pixel_draw_point[0] = pixel_color.red;
    pixel_draw_point[1] = pixel_color.green;
    pixel_draw_point[2] = pixel_color.blue;
}

void BGRResv8BitPerColorPixelWriter::Write(int x, int y, const PixelColor& pixel_color) {
    uint8_t *pixel_draw_point = PixelAt(x, y);
    pixel_draw_point[0] = pixel_color.blue;
    pixel_draw_point[1] = pixel_color.green;
    pixel_draw_point[2] = pixel_color.red;
}
