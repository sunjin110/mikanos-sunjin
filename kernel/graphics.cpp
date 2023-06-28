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

void FillRectangle(PixelWriter &writer, const Vector2D<int> &pos, const Vector2D<int> &size, const PixelColor &c) {
    for (int dy = 0; dy < size.y; dy++) {
        for (int dx = 0; dx < size.x; dx++) {
            writer.Write(pos.x + dx, pos.y + dy, c);
        }
    }
}

void DrawRectangle(PixelWriter &writer, const Vector2D<int> &pos, const Vector2D<int> &size, const PixelColor &c) {
    for (int dx = 0; dx < size.x; dx++) {
        writer.Write(pos.x + dx, pos.y, c);
        writer.Write(pos.x + dx, pos.y + size.y -1, c);
    }

    for (int dy = 0; dy < size.y -1; dy++) {
        writer.Write(pos.x, pos.y + dy, c);
        writer.Write(pos.x + size.x - 1, pos.y + dy, c);
    }
}
