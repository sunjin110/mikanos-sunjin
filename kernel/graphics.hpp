#pragma once

#include "frame_buffer_config.hpp"
#include "common.hpp"

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
        virtual void Write(int x, int y, const PixelColor &pixel_color) override;
};

class BGRResv8BitPerColorPixelWriter : public PixelWriter {
    public:
        using PixelWriter::PixelWriter;
        virtual void Write(int x, int y, const PixelColor &pixel_color) override;
};

PixelWriter *NewPixelWriter(const FrameBufferConfig &config, char *buf);

template <typename T> 
struct Vector2D {
    T x, y;

    template <typename U>
    Vector2D<T> &operator += (const Vector2D<U> &rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
};

void FillRectangle(PixelWriter &writer, const Vector2D<int> &pos, const Vector2D<int> &size, const PixelColor &c);
void DrawRectangle(PixelWriter &writer, const Vector2D<int> &pos, const Vector2D<int> &size, const PixelColor &c);
